#include<stdio.h>
#include<stdlib.h>

typedef struct image
{
    int rows;//height
    int cols;//width
    int ***data;
}*Mat;

void MemAlloc(Mat input);
void DoubleLinearScale(Mat src, Mat dst);

int main(int argc, char* argv[])
{   
   	if(argc < 3)
   	{
   		printf("Usage: ./transform input_image_file output_bin_file\n");
   		return 1;
   	}

    Mat src=(Mat)malloc(sizeof(Mat));
    Mat dst=(Mat)malloc(sizeof(Mat));
    dst->rows = 112;
    dst->cols = 96;
    
    //read the src image w and h
    FILE *src_file;
    src_file = fopen(argv[1], "rb");
    // src_file = fopen("input.bin", "rb");
    int temp;    
    fread(&temp,sizeof(int),1,src_file);//w    
    src->cols = temp;
    printf("The input image size is %d(width)", temp);
    fread(&temp,sizeof(int),1,src_file);//h
    src->rows = temp;
    printf(" * %d(height). \n",temp);

    MemAlloc(src);
    MemAlloc(dst);

    //read the src image data by R,G,B
    int i,j,k;
    for(i=0;i<3;i++)
    {
        for(j=0;j<(src->cols);j++)
        {
            for(k=0;k<(src->rows);k++)
            {
                 fread(&temp,sizeof(int),1,src_file);
                 src->data[i][j][k] = temp;
            }
        }
    }
    fclose(src_file);

    DoubleLinearScale(src,dst);

    FILE *output = fopen(argv[2],"wb");
    temp = 96;
    fwrite(&temp,sizeof(int),1,output);
    temp = 112;
    fwrite(&temp,sizeof(int),1,output);

    for(i=0;i<3;i++)
    {
        for(j=0;j<(dst->cols);j++)
        {
            for(k=0;k<(dst->rows);k++)
            {
                // printf("%d ",dst->data[i][j][k] );
                temp = dst->data[i][j][k];
                fwrite(&temp,sizeof(int),1,output);
            }           
        }       
    }
    fclose(output);


    return 0;


}

void MemAlloc(Mat input)
{
    int channels = 3;
    int w = input->cols;
    int h = input->rows;
    int i,j;

    input->data = (int ***)malloc(channels * sizeof(int **));
    for(i=0;i<channels;i++)
    {
        input->data[i] = (int **)malloc(w * sizeof(int *));
        for(j=0;j<w;j++)
        {
            input->data[i][j] = (int *)malloc(h * sizeof(int));
        }
    }
}

void DoubleLinearScale(Mat src, Mat dst)
{
    double dstH = dst->rows;  //目标图片高度
    double dstW = dst->cols;  //目标图片宽度

    double srcW = src->cols;  //原始图片宽度，如果用int可能会导致(srcH - 1)/(dstH - 1)恒为零
    double srcH = src->rows;  //原始图片高度
    
    double xm = 0;      //映射的x
    double ym = 0;      //映射的y
    
    int xi = 0;         //映射x整数部分
    int yi = 0;         //映射y整数部分    
    int xl = 0;         //xi + 1
    int yl = 0;         //yi + 1
    
    double xs = 0;   
    double ys = 0;

    /* 为目标图片每个像素点赋值 */
    int i,j;
    for(i = 0; i < dstW; i ++)     
    {
        for(j = 0; j < dstH; j ++) 
        {
            //求出目标图像(i,j)点到原图像中的映射坐标(mapx,mapy)
            xm = (srcW - 1)/(dstW - 1) * i;
            ym = (srcH - 1)/(dstH - 1) * j;
            
            /* 取映射到原图的xm的整数部分 */
            xi = (int)xm;
            yi = (int)ym;
            xl = xi + 1;
            yl = yi + 1;
            
            /* 取偏移量 */
            xs = xm - xi;
            ys = ym - yi;
            
            //边缘点
            if((xi + 1) > (srcW - 1)) 
            {
                xl = xi - 1;
            }

            if((yi + 1) > (srcH - 1)) 
            {
                yl = yi - 1;
            }
            
            //R
            dst->data[0][i][j] = (int)
            (
                    src->data[0][xi][yi] * (1 - xs) * (1 - ys) +
                    src->data[0][xi][yl] * (1 - xs) * ys +
                    src->data[0][xl][yi] * xs * (1 - ys) +
                    src->data[0][xl][yl] * xs * ys
            );

            //G
            dst->data[1][i][j] = (int)
            (
                    src->data[1][xi][yi] * (1 - xs) * (1 - ys) +
                    src->data[1][xi][yl] * (1 - xs) * ys +
                    src->data[1][xl][yi] * xs * (1 - ys) +
                    src->data[1][xl][yl] * xs * ys
            );

            //B
            dst->data[2][i][j] = (int)(
                    src->data[2][xi][yi] * (1 - xs) * (1 - ys) +
                    src->data[2][xi][yl] * (1 - xs) * ys +
                    src->data[2][xl][yi] * xs * (1 - ys) +
                    src->data[2][xl][yl] * xs * ys
            );

        }
    }
}