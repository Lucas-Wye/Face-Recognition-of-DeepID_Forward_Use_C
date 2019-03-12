#include "JpegDecoder.h"
#include "Config.h"
#include "Stl.h"
#include <stdio.h>
#include <cmath>

using namespace JpegCodec;
using namespace tinyStl;

//------------------  Public Field   -----------------------------------------------------//

JpegDecoder::JpegDecoder(const char *fileName) : yDC(0), cbDC(0), crDC(0), endOfDecoder(false), readCount(0x80)
{
    FILE *fp = fopen(fileName, "rb+"); //打开文件
    /* 计算文件大小 */
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    /* 重定位到文件头 */
    fseek(fp, 0, SEEK_SET);

    stream = new uint8_t[length];
    fread(stream, 1, length, fp); // 读取整个文件数据

    fclose(fp); // 关闭文件
}


JpegDecoder::~JpegDecoder()
{
    delete []stream; //free 文件缓冲区
}


void JpegDecoder::Decoder(Matrix &mat)
{
    /* Preparation */
    ReadQuantTable();
    ReadImageSize();
    ComputeDHT();

    /* 创建Matrix */
    mat.Create(m_height, m_width, 3);

    int rowBlkNr = (m_height + 15) >> 4; // 垂直方向分块
    int colBlkNr = (m_width + 15) >> 4;  // 水平方向分块

    /* 解码 */
	ToStartOfData();
    for (int i = 0; i < rowBlkNr; i++)
    {
        for (int j = 0; j < colBlkNr; j++)
        {
			if (endOfDecoder) return;

            DecoderMCU(); // 解码一个 MacroBlock
            FillYCbCr();  // 填充 YCbCr 缓冲区

            /* 写入到 mat */
            ConvertClrSpace(mat.data,i * 16, j * 16);
        }
    }
}

//------------------  Proteced Field   ----------------------------------------------------//

//------------------  0.helper function   ----------------------------------

/* @brief 获取压缩数据的下一个有效位
 */
int JpegDecoder::NextBit()
{
    if (readCount == 0x0)
    {
        // reset
        readCount = 0x80;
        curIndex ++;

        // check
        if (stream[ curIndex ] == 0xFF) //标记值
        {
            curIndex ++;
            if (stream[ curIndex ] & 0xD7)       ResetDC();
            else if (stream[ curIndex ] == 0xD9) endOfDecoder = true;
            else if (stream[ curIndex ] == 0x00) stream[ curIndex ] = 0xFF;
        }
    }

    int retVal = stream[ curIndex ] & readCount; // 获取当前位的值 (1 or 0)
    readCount >>= 1;
    return (retVal > 0 ? 1 : 0);
}


/* @brief 计算标准哈夫曼表的真实值
 */
int JpegDecoder::ComputeRealValue(int length)
{
    int retVal = 0;
    for (int i = 0; i < length; i++)
    {
        retVal = (retVal << 1) + NextBit();
    }

    return (retVal >= pow(2, length - 1) ? retVal : retVal - pow(2, length) + 1);
}

/* @brief 反采样
 */
void JpegDecoder::UpSample(int32_ptr CbCr, int32_ptr blk)
{
    typedef int (*Ptr_CbCr)[16];
    Ptr_CbCr p = reinterpret_cast<Ptr_CbCr>(CbCr); // 一位数组转二维数组

    for (int i = 0; i < 16; i++)
    {
        for (int j =0; j< 16; j++)
        {
            p[i][j] = blk[(i >> 1) * 8 + (j >> 1)];
        }
    }


}

/* @brief Reset DC 值
 */
void JpegDecoder::ResetDC()
{
    yDC = cbDC = crDC = 0x0;
}


/* @brief 查找对应的标记段的索引
* @mark: 段标记
*/
int JpegDecoder::MarkIndex(uint8_t mark)
{
    int idx = 0;
    while(stream[idx] != 0xFF || stream[idx + 1] != mark) idx ++;
    return idx;
}


/* @brief 从二进制数据流中查找一个有效的编码值
 * @table:    用于解码的哈夫曼表
 */
int JpegDecoder::FindKeyValue(tinyMap &table)
{
    int key = 0;
    int keyLength = 0;
    while (table.find(key, keyLength) == table.end())
    {
        key = (key << 1) + NextBit(); // 1 or 0
        keyLength++;
    }
    //printf("%X\n", key);
    return table[key];
}

/* @brief 将一个 MacroBlock 填充到 YCbCr 缓冲区
 */
void JpegDecoder::FillYCbCr()
{
    Ptr16 yPtrDst = (Ptr16)yBuf;
    for (int k = 0; k < 4; k++)
    {
        int xOffset = (k >> 1) << 3;  // equals: (k / 2) * 8;
        int yOffset = (k & 0x1) << 3; // equals: (k % 2) * 8;
        Ptr8 yPtrSrc = (Ptr8)yBlk[k];
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                yPtrDst[xOffset + i][yOffset + j] = yPtrSrc[i][j];
            }
        }
    }

    // 反采样
    UpSample(cbBuf, cbBlk);
    UpSample(crBuf, crBlk);
}

//------------------  1.解码准备工作   -------------------------------------

/* 读取量化表 */
void JpegDecoder::ReadQuantTable()
{
    int base = MarkIndex(DQT);

    int baseY = base + 5;
    int baseCbCr = base + 74;
    for (int i = 0; i < 64; i++)
    {
        quantY[i] = stream[i + baseY];
        quantCbCr[i] = stream[i + baseCbCr];
    }
}

/* 读取图像的宽高 */
void JpegDecoder::ReadImageSize()
{
    int base = MarkIndex(SOF);

    m_height = (stream[base + 5] << 8) + stream[base + 6];	
    m_width  = (stream[base + 7] << 8) + stream[base + 8];
	// printf("height: %d\nwidth: %d\n",m_height,m_width );
}

/* @brief 构建哈夫曼表
 * @base:  从DHT_Segment 的表头开始，到表类型字段后一个字节的偏移量
 * @table: 要构建的哈夫曼表
 */
void JpegDecoder::ReBuildTable(int base, tinyMap &table)
{
    int offset = 16;  // offset
	int key = 0x0;
	int keyLength = 0;
	for (int i = 0; i < 16; i++) // length of key (i.e. i = 2 means key = 000 , 001 , 010 , 011 or ...)
	{
		int cnt = DHT_Segment[base + i]; // number of key, which length is (i+1)

		/* alignment */
		if (keyLength <= i)
        {
            key <<= (i - keyLength + 1);
            keyLength = i + 1;
        }

		while (cnt > 0)
		{
			/* value of key */
			table.insert(key, keyLength, DHT_Segment[base + offset]);
			offset++;

			/* increment */
			key++;
			cnt = cnt - 1;
		}
	}
}

/* 构建 DC AC 哈夫曼表 */
void JpegDecoder::ComputeDHT()
{
    ReBuildTable(5, DC[0]);
    ReBuildTable(34, DC[1]);
    ReBuildTable(63, AC[0]);
    ReBuildTable(242, AC[1]);
}

/* 定位到数据头 */
void JpegDecoder::ToStartOfData()
{
    curIndex = MarkIndex(SOS) + 14;
}

//------------------  2.解码一个block   ------------------------------------

/* @brief 反编码一个 8 x 8 的数据块
 * @out:     返回值，一个 8 x 8 的数据块
 * @dcTable: DC哈夫曼表
 * @actable: AC哈夫曼表
 * @dc:      前一个block的 DC 值
 */
void JpegDecoder::DecoderBlock(int32_ptr out, tinyMap &dcTable, tinyMap &acTable, int &dc)
{
    // reset matrix
    for (int i = 0; i < 64; i++) out[i] = 0x0;

    // decoder DC of matrix
    int length = FindKeyValue(dcTable);
    int value = ComputeRealValue(length);
    dc += value; // DC
    out[0] = dc;

    // decoder AC of matrix
    for (int i = 1; i < 64; i++)
    {
        length = FindKeyValue(acTable);
        if (length == 0x0) break; // 结束条件

        value = ComputeRealValue(length & 0xf); // 右边 4位，实际值长度
        i += (length >> 4);          // 左边 4位，行程长度
        out[i] = value; // AC
    }
}


//------------------  3.反量化   -------------------------------------------

/* @brief 反量化
 * @out:    待量化数据，返回值（8x8）
 * @quant:  量化表（8x8）
 */
void JpegDecoder::Dequant(int32_ptr out, int8_ptr quant)
{
    for (int i = 0; i < 64; i++)
    {
        out[i] = quant[i] * out[i];
    }
}

//------------------  4.反 ZigZag 编码   -----------------------------------

/* @brief 反 ZigZag 编码
 * @out:  返回值（8x8）
 * @out:  原始数据（8x8）
 */
void JpegDecoder::UnZigZag(int32_ptr out, int32_ptr source)
{
    for (int i = 0; i < 64; i++) out[i] = source[UnZigZagTable[i]];
}

//------------------  5.反离散余弦变换（IDCT）   ---------------------------

/* @brief 反离散余弦变换
 * @out:    返回值， 8 x 8 block
 */
void JpegDecoder::Transform(int32_ptr out)
{
    double tmp[8][8];
    Ptr8 pIn = (Ptr8)out; // 用二维数组方式访问一位数组
    double tmpVal;

    /* tmp = MtxIDCT * Matrix */
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tmpVal = 0;
            for (int k = 0; k < 8; k++)
            {
                tmpVal += MtxIDCT[i][k] * pIn[k][j];
            }
            tmp[i][j] = round(tmpVal);
        }
    }

    /* Matrix = tmp * MtxDCT */
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tmpVal =0;
            for (int k = 0; k < 8; k++)
            {
                tmpVal +=tmp[i][k] * MtxDCT[k][j];
            }
            pIn[i][j] = round(tmpVal);
        }
    }
}

//------------------  6.构建一个 MacroBlock   ------------------------------

/* 构建一个 MacroBlock */
void JpegDecoder::DecoderMCU()
{
    int tmp[64];

    /* 4 个 Y 分量 */
    for (int i = 0; i < 4; i++)
    {
        DecoderBlock(tmp, DC[0], AC[0], yDC); // 解码一个数据块
        Dequant(tmp, quantY);                         // 反量化
        UnZigZag(yBlk[i], tmp);                        // 反 ZigZag 编码
        Transform(yBlk[i]);                       // 反离散余弦变换 （IDCT）
    }
    /* 一个 Cb 分量 */
    DecoderBlock(tmp, DC[1], AC[1], cbDC);
    Dequant(tmp, quantCbCr);
    UnZigZag(cbBlk, tmp);
    Transform(cbBlk);

    /* 一个 Cr 分量 */
    DecoderBlock(tmp, DC[1], AC[1], crDC);
    Dequant(tmp, quantCbCr);
    UnZigZag(crBlk, tmp);
    Transform(crBlk);
}

//------------------  7.颜色空间转换（from YCbCr to RGB）   ----------------

/* 颜色空间转换
 * @outBgr:  16x16x3 block ,返回值，BGR格式像素序列
 * @row:  当前数据块的 row index , 用于越界检查
 * @col:  当前数据块的 col index， 用于越界检查
 */
void JpegDecoder::ConvertClrSpace(int8_ptr outBgr, int row, int col)
{
    Ptr16 Y  = (Ptr16) yBuf;
    Ptr16 Cb = (Ptr16) cbBuf;
    Ptr16 Cr = (Ptr16) crBuf;
	int R, G, B;

	outBgr += (row * m_width + col) * 3; // 偏移到 第 (x,y) 块
    int stride = m_width * 3; // 一次偏移一行
    for (int i = 0; i < 16; i++)
    {
		if (row + i >= m_height) break; // 越界检查
        // 计算一行
        for (int j = 0; j < 16; j++)
        {
            if (col + j >= m_width) break; // 越界检查
            int offset = j * 3;
            R = Y[i][j] + 1.402 * Cr[i][j] + 128;                         // R
            G = Y[i][j] - 0.34414 * Cb[i][j] - 0.71414 * Cr[i][j] + 128;  // G
            B = Y[i][j] + 1.772 * Cb[i][j] + 128;                          // B

			if (R > 255) R = 255;
			if (G > 255) G = 255;
			if (B > 255) B = 255;

			if (R < 0) R = 0;
			if (G < 0) G = 0;
			if (B < 0) B = 0;

			outBgr[offset + 0] = B;
			outBgr[offset + 1] = G;
			outBgr[offset + 2] = R;
        } // end of for_j

        outBgr += stride; // 偏移到下一行首
    } // end of for_i
}
