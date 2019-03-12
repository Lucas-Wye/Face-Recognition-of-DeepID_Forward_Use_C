
#ifndef JPEGDECODER__H
#define JPEGDECODER__H

#include "Config.h"
#include "Stl.h"
#include <string>

namespace JpegCodec
{
	class Matrix
	{
	public:
		Matrix(){}
		void Create(int _rows, int _cols, int _channal)
		{
		    rows = _rows;
		    cols = _cols;
		    channal = _channal;
		    data = new uint8_t[_rows * _cols * _channal];
		}

		int rows;
		int cols;
		int channal;
		uint8_t *data;
	};

	class JpegDecoder
	{
	public:
		JpegDecoder(const char *fileName);
		~JpegDecoder();

		void Decoder(Matrix &mat);
	protected:
        //------------------  0.helper function   ----------------------------------

        /* @brief 获取压缩数据的下一个有效位
         */
        int NextBit();

        /* @brief 计算标准哈夫曼表的真实值
         */
        int ComputeRealValue(int length);

        /* @brief 反采样, 图像放大
         * @CbCr:   返回值， 16 x 16 block
         * @blk:    8x8 block
         */
        void UpSample(int32_ptr CbCr, int32_ptr blk);

        /* @brief Reset DC to 0x0
         */
        void ResetDC();


        /* @brief 查找对应的标记段的索引
         * @mark: 段标记
         */
        int MarkIndex(uint8_t mark);

        /* @brief 从二进制数据流中查找一个有效的编码值
         * @table:    用于解码的哈夫曼表
         */
        int FindKeyValue(tinyStl::tinyMap &table);

        /* @brief 将一个 MacroBlock 填充到 YCbCr 缓冲区
         */
        void FillYCbCr();

        /* @brief 构建哈夫曼表
         * @base:  从DHT_Segment 的表头开始，到表类型字段后一个字节的偏移量
         * @table: 要构建的哈夫曼表
         */
        void ReBuildTable(int base, tinyStl::tinyMap &table);

		//------------------  1.解码准备工作   -------------------------------------

		/* 读取量化表 */
		void ReadQuantTable();

		/* 读取图像的宽高 */
		void ReadImageSize();

		/* 构建 DC AC 哈夫曼表 */
		void ComputeDHT();

		/* 定位到数据头 */
		void ToStartOfData();


		//------------------  2.解码一个block   ------------------------------------

        /* @brief 反编码一个 8 x 8 的数据块
         * @out:     返回值，一个 8 x 8 的数据块
         * @dcTable: DC哈夫曼表
         * @actable: AC哈夫曼表
         * @dc:      前一个block的 DC 值
         */
        void DecoderBlock(int32_ptr out, tinyStl::tinyMap &dcTable, tinyStl::tinyMap &acTable, int &dc);


		//------------------  3.反量化   -------------------------------------------

        /* @brief 反量化
         * @out:    待量化数据，返回值（8x8）
         * @quant:  量化表（8x8）
         */
        void Dequant(int32_ptr out, int8_ptr quant);

		//------------------  4.反 ZigZag 编码   -----------------------------------

        /* @brief 反 ZigZag 编码
         * @out:  返回值（8x8）
         * @out:  原始数据（8x8）
         */
        void UnZigZag(int32_ptr out, int32_ptr source);

		//------------------  5.反离散余弦变换（IDCT）   ---------------------------

        /* @brief 反离散余弦变换
         * @out:    返回值， 8 x 8 block
         */
        void Transform(int32_ptr out);

        //------------------  6.构建一个 MacroBlock   ------------------------------

		/* 构建一个 MacroBlock */
		void DecoderMCU();

		//------------------  7.颜色空间转换（from YCbCr to RGB）   ----------------

		/* 颜色空间转换
		 * @outBgr:  16x16x3 block ,返回值，BGR格式像素序列
		 * @row:  当前数据块的 row index , 用于越界检查
		 * @col:  当前数据块的 col index， 用于越界检查*/
		void ConvertClrSpace(int8_ptr outBgr, int row, int col);

	private:
		uint8_t       *stream;      //Jpeg文件数据流

		bool           endOfDecoder; // 数据流结束标记
		int	       curIndex;     // 解码到当前数据位
		uint8_t        readCount;    // 已经解码的当前字节的位数

		uint8_t        quantY[64];       // Y 分量量化表
		uint8_t        quantCbCr[64];    // CbCr 分量量化表

		int            m_width;          // 图像宽度
		int            m_height;         // 图像高度

		int            yBlk[4][64];
		int            cbBlk[64];
		int            crBlk[64];

		int            yBuf[256], cbBuf[256], crBuf[256];

		int            yDC, cbDC, crDC;

		/* huffman table */
		tinyStl::tinyMap DC[2], AC[2];
	};

}


#endif
