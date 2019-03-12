#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cnn.h"

CovLayer *initCovLayer(int inputWidth,int inputHeight,int kernel_size,int inChannels,int outChannels)
{
	CovLayer* covL = (CovLayer*)malloc(sizeof(CovLayer));
// (96,112,4,3,20)
// (47,55,3,20,40)
// (21,25,2,60,80)
	covL->inputWidth = inputWidth;//96,47,21
	covL->inputHeight = inputHeight;//112,55,25
	covL->kernel_size = kernel_size;//4,3,2
	covL->inChannels = inChannels;//3,20,60
	covL->outChannels = outChannels;//20,20,80

	// initaial W
	int i,j,c,r;	
	covL->mapData=(float ****)malloc(outChannels * sizeof(float ***));
	
	// for(i=0;i<outChannels;i++)
	for(i=(outChannels-1);i!=(-1);i--)
	{
		covL->mapData[i]=(float ***)malloc(inChannels * sizeof(float **));
		// for(j=0;j<inChannels;j++)
		for(j=(inChannels-1);j!=(-1);j--)
		{	
			covL->mapData[i][j]=(float **)malloc(kernel_size * sizeof(float *));
			// for(r=0;r<kernel_size;r++)
			for(r=(kernel_size-1);r!=(-1);r--)
			{
				covL->mapData[i][j][r]=(float *)malloc(kernel_size * sizeof(float));				
			}
		}
	}

	// b
	covL->basicData=(float*)calloc(outChannels,sizeof(float));	
	
	int outW=inputWidth - kernel_size + 1;//93,45,20
	int outH=inputHeight - kernel_size + 1;//109,52,24
	//v,y
	covL->v=(float***)malloc(outChannels*sizeof(float**));
	covL->y=(float***)malloc(outChannels*sizeof(float**));
	// for(j=0;j<outChannels;j++)
	for(j=(outChannels-1);j!=(-1);j--)
	{		
		covL->v[j]=(float**)malloc(outH*sizeof(float*));
		covL->y[j]=(float**)malloc(outH*sizeof(float*));
		// for(r=0;r<outH;r++)
		for(r=(outH-1);r!=(-1);r--)	
		{			
			covL->v[j][r]=(float*)calloc(outW,sizeof(float));
			covL->y[j][r]=(float*)calloc(outW,sizeof(float));
		}
	}
	return covL;

}

PoolLayer *initPoolLayer(int inputWidth,int inputHeight,int kernel_size,
	int inChannels,int outChannels,int poolType,int stride)
{   
// (93,109,2,20,20,MaxPool)
	PoolLayer* poolL = (PoolLayer*)malloc(sizeof(PoolLayer));
	poolL->inputWidth = inputWidth;//93
	poolL->inputHeight = inputHeight;//109
	poolL->kernel_size = kernel_size;//2
	poolL->inChannels = inChannels;//20
	poolL->outChannels = outChannels;//20
	poolL->poolType = poolType;//MaxPool

	// poolL->basicData = (float*)calloc(outChannels,sizeof(float));
	int outW,outH;
	if(stride==2)
	{
		outW = ((inputWidth%2==0)?0:1) + inputWidth/kernel_size;
		// printf("outW should be 47/21, here the value is %d\n",outW);
		outH = ((inputHeight%2==0)?0:1) + inputHeight/kernel_size;
		// printf("outW should be 55/25, here the value is %d\n",outH);
	}
	else
	{
		outW = inputWidth - kernel_size + 1;
		// printf("outW should be 44, here the value is %d\n",outW);
		outH = inputHeight - kernel_size + 1;
		// printf("outW should be 52, here the value is %d\n",outH);
	}

	int j,r;
	poolL->y = (float***)malloc(outChannels*sizeof(float**));
	// for(j=0;j<outChannels;j++)
	for(j=(outChannels-1);j!=(-1);j--)	
	{
		poolL->y[j] = (float**)malloc(outH*sizeof(float*));
		// for(r=0;r<outH;r++)
		for(r=(outH-1);r!=(-1);r--)	
		{
			poolL->y[j][r] = (float*)calloc(outW,sizeof(float));
		}
	}

	return poolL;
}

FcLayer *initFcLayer(int inputNum,int outputNum)
{
	FcLayer* FcL = (FcLayer*)malloc(sizeof(FcLayer));
	FcL->inputNum = inputNum;
	FcL->outputNum = outputNum;			

	//temp
	FcL->v = (float*)calloc(inputNum,sizeof(float));

	// initial W
	FcL->wData = (float**)malloc(outputNum*sizeof(float*)); 
	int i,j;	
	// for(i=0;i<outputNum;i++)
	for(i=(outputNum-1);i!=(-1);i--)
	{
		FcL->wData[i] = (float*)malloc(inputNum*sizeof(float));		
	}
	//b
	FcL->basicData = (float*)calloc(outputNum,sizeof(float));
	//output
	FcL->y = (float*)calloc(outputNum,sizeof(float));
	
	return FcL;
}

OutLayer *initOutLayer(int inputNum,int outputNum)
{
	OutLayer* outL = (OutLayer*)malloc(sizeof(OutLayer));
	outL->inputNum = inputNum;
	outL->outputNum = outputNum;

	outL->outputdata = (float*)calloc(inputNum,sizeof(float));
	return outL; 
}

float activation_relu(float input,float bias)
{
	float temp=input+bias;
	return (temp>0)?temp:0;
}

void MaxPooling(float** output,nSize outputSize,float** input,nSize inputSize,int kernel_size,int stride)
{
	int i,j,m,n;	
	int t_m,t_n;
	t_m = t_n = 0;
	float sum;

	for(i=0;i<outputSize.r;i++)//55
	{
		for(j=0;j<outputSize.c;j++)//47
		{
			sum=0.0;
			
			for(m = t_m;m<(t_m+kernel_size);m++)			
			{
				for(n = t_n;n<(t_n+kernel_size);n++)				
				{											
					if(m > (inputSize.r-1) || n > (inputSize.c-1));					
					else
					{		
						if(sum < input[m][n])
						{														
							sum = input[m][n];						
						}
					}
				}
			}
			output[i][j]=sum;
			t_n = t_n + stride;
		}
		t_m = t_m + stride;
		t_n = 0;
	}
}



void read_file_conv(char *filename,int a, int b, int c, int d,CovLayer *conv)
{
//(20,3,4,4)
	FILE  *fp=fopen(filename,"rb");		
	int i,j,m;
	for(i=0;i<a;i++)//20
	{
		for(j=0;j<b;j++)//3
		{
			for(m=0;m<c;m++)//4
			{
				fread(conv->mapData[i][j][m],sizeof(float),d,fp);
			}
		}
	}
	fread(conv->basicData,sizeof(float),a,fp);
	fclose(fp);
}


void read_file_fc(char *filename,int a,int b,FcLayer *fc)
{
	FILE  *fp=fopen(filename,"rb");	
	float temp;
	int i,j;
	for(i=0;i<a;i++)//160
	{
		fread(fc->wData[i],sizeof(float),b,fp);		
	}
	fread(fc->basicData,sizeof(float),a,fp);
	fclose(fp);
}

float*** read_image_rgb(char *filename)
{
	FILE *file;
    file = fopen(filename, "rb");

    // int **r,**g,**b;
    int w,h;
    fread(&w,sizeof(int),1,file);
    fread(&h,sizeof(int),1,file);
    fprintf(stderr,"image size w = %d, image size h =  %d \n",w,h);

    int i,j,k;

    float ***image_data;
    image_data = (float ***)malloc(sizeof(float **) * 3);
    
	// B mean[0,:,:] = 92.1874 
	// G mean[1,:,:] = 108.306
	// R mean[2,:,:] = 141.596

    float mean[3];
    mean[0] = 141.596;
    mean[1] = 108.306;
    mean[2] = 92.1874;

    int temp[112];

    for(k=0;k<3;k++)
    {
    	image_data[k] = (float **)malloc(sizeof(float *) * w);
    	for(i=0;i<w;i++)
    	{
    		image_data[k][i] = (float *)malloc(sizeof(float) * h);
    		
    		fread(temp,sizeof(float),h,file);
    		for(j=0;j<h;j++)
            {                               
                image_data[k][i][j] = temp[j] - mean[k];
                // printf("%f ",image_data[k][i][j]);
            }
            // printf("\n");
    	}
    	// printf("\n");
    }
    fclose(file);

	float ***input_data = (float***)malloc(sizeof(float**)*3);	
	for(i=2;i>=0;i--)
	{
		input_data[2-i] = transpose_matrix(image_data[i],96,112);		
	}	
	return input_data;
}

void cnnff(CNN* cnn,float*** inputData,char * filename)
{

	clock_t start,end;	

	start = clock();

	// conv1
	int i,j,r,c;
	//(4,4)
	nSize kernel_size = {cnn->conv1->kernel_size,cnn->conv1->kernel_size};
	//(96,112)
	nSize inSize = {cnn->conv1->inputWidth,cnn->conv1->inputHeight};
	//(93,109)
	nSize outSize = {cnn->pool1->inputWidth,cnn->pool1->inputHeight};
	
	float **mapout;

	for(i=0;i<(cnn->conv1->outChannels);i++)//20
	{
		for(j=0;j<(cnn->conv1->inChannels);j++)//3
		{
			mapout=cov(cnn->conv1->mapData[i][j],kernel_size,inputData[j],inSize,valid);//(96,112)
			//conv result is saved to v
			addmat(cnn->conv1->v[i],cnn->conv1->v[i],outSize,mapout,outSize);//(93,102)						
		}
		for(r=0;r<outSize.r;r++)//109
		{
			for(c=0;c<outSize.c;c++)//93
			{
				cnn->conv1->y[i][r][c]=activation_relu(cnn->conv1->v[i][r][c],cnn->conv1->basicData[i]);
			}
		}
	}
	// pool1	
	inSize.c = cnn->pool1->inputWidth;//93
	inSize.r = cnn->pool1->inputHeight;//109
	outSize.c = cnn->conv2->inputWidth;//47;
	outSize.r = cnn->conv2->inputHeight;//55;
	for(i=0;i<(cnn->pool1->outChannels);i++)
	{		
		MaxPooling(cnn->pool1->y[i],outSize,cnn->conv1->y[i],inSize,cnn->pool1->kernel_size,2);		
	}

	end = clock();
	fprintf(stderr,"conv1 and pool1 time = %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

	start = clock();

	// conv2	
	//(3,3)	
	kernel_size.r = kernel_size.c = cnn->conv2->kernel_size;
	//(47,55)
	inSize.c = cnn->conv2->inputWidth;
	inSize.r = cnn->conv2->inputHeight;
	//(45,53)	
	outSize.c = cnn->pool2->inputWidth;
	outSize.r = cnn->pool2->inputHeight;


	for(i=0;i<(cnn->conv2->outChannels);i++)//40
	{
		for(j=0;j<(cnn->conv2->inChannels);j++)//20
		{														
			mapout=cov(cnn->conv2->mapData[i][j],kernel_size,cnn->pool1->y[j],inSize,valid);//(47,55)
			//conv result is saved to v
			addmat(cnn->conv2->v[i],cnn->conv2->v[i],outSize,mapout,outSize);//(45,53)

			// for(r=0;r<outSize.r;r++)
			// {
			// 	free(mapout[r]);
			// }
			// free(mapout);
		}		

		for(r=0;r<outSize.r;r++)//53
		{
			for(c=0;c<outSize.c;c++)//45
			{
				cnn->conv2->y[i][r][c]=activation_relu(cnn->conv2->v[i][r][c],cnn->conv2->basicData[i]);
			}
		}
	}
	
	// pool2	
	inSize.c = cnn->pool2->inputWidth;//45
	inSize.r = cnn->pool2->inputHeight;//53
	outSize.c = 44;//cnn->conv3->inputWidth;
	outSize.r = 52;//cnn->conv3->inputHeight;
	for(i=0;i<(cnn->pool2->outChannels);i++)
	{
		MaxPooling(cnn->pool2->y[i],outSize,cnn->conv2->y[i],inSize,cnn->pool2->kernel_size,1);
	}

	end = clock();
	fprintf(stderr,"conv2 and pool2 time = %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);	

	start = clock();

	// conv3	
	//(3,3)	
	kernel_size.r = kernel_size.c = cnn->conv3->kernel_size;
	//(44,52)
	inSize.c = cnn->conv3->inputWidth;
	inSize.r = cnn->conv3->inputHeight;
	//(42,50)
	outSize.c = cnn->pool3->inputWidth;
	outSize.r = cnn->pool3->inputHeight;
	
	for(i=0;i<(cnn->conv3->outChannels);i++)//60
	{
		for(j=0;j<(cnn->conv3->inChannels);j++)//40
		{			
			mapout=cov(cnn->conv3->mapData[i][j],kernel_size,cnn->pool2->y[j],inSize,valid);//(44,52)
			//conv result is saved to v
			addmat(cnn->conv3->v[i],cnn->conv3->v[i],outSize,mapout,outSize);//(42,50)

			// for(r=0;r<outSize.r;r++)
			// {
			// 	free(mapout[r]);
			// }
			// free(mapout);
		}		

		for(r=0;r<outSize.r;r++)//50
		{
			for(c=0;c<outSize.c;c++)//42
			{
				cnn->conv3->y[i][r][c]=activation_relu(cnn->conv3->v[i][r][c],cnn->conv3->basicData[i]);
				/*
				printf("%f\n", cnn->conv3->y[i][r][c]);
				*/
			}
		}
	}
	
	// pool3	
	inSize.c = cnn->pool3->inputWidth;//42
	inSize.r = cnn->pool3->inputHeight;//50
	outSize.c = cnn->conv4->inputWidth;//21
	outSize.r = cnn->conv4->inputHeight;//25
	for(i=0;i<(cnn->pool3->outChannels);i++)
	{
		MaxPooling(cnn->pool3->y[i],outSize,cnn->conv3->y[i],inSize,cnn->pool3->kernel_size,2);		
	}

	end = clock();
	fprintf(stderr,"conv3 and pool3 time = %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);
	

	start = clock();

	// conv4	
	//(2,2)	
	kernel_size.r = kernel_size.c = cnn->conv4->kernel_size;
	//(21,25)
	inSize.c = cnn->conv4->inputWidth;
	inSize.r = cnn->conv4->inputHeight;	
	//(20,24)
	outSize.c = 20;
	outSize.r = 24;
	

	for(i=0;i<(cnn->conv4->outChannels);i++)//80
	{
		for(j=0;j<(cnn->conv4->inChannels);j++)//60
		{														
			mapout=cov(cnn->conv4->mapData[i][j],kernel_size,cnn->pool3->y[j],inSize,valid);//(44,52)			
			//conv result is saved to v
			addmat(cnn->conv4->v[i],cnn->conv4->v[i],outSize,mapout,outSize);//(20,24)
		}		

		for(r=0;r<outSize.r;r++)//24
		{
			for(c=0;c<outSize.c;c++)//20
			{
				cnn->conv4->y[i][r][c]=activation_relu(cnn->conv4->v[i][r][c],cnn->conv4->basicData[i]);
			}
		}
	}		

	end = clock();	
	fprintf(stderr,"conv4 time =           %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);
	
	start = clock();

	//fc160_1
	//(21,25)
	inSize.c = cnn->conv4->inputWidth;
	inSize.r = cnn->conv4->inputHeight;
	//60
	int input_channel = cnn->pool3->outChannels;
	//
	int out_channel = 160;
	
	int k = 0;
	for(j=0;j<input_channel;j++)//60
	{
		for(r=0;r<inSize.r;r++)	//25
		{
			for(c=0;c<inSize.c;c++)//21
			{
				cnn->fc160_1->v[k] = cnn->pool3->y[j][r][c];
				k++;
			}
		}
	}
	
	for(i=(out_channel-1);i!=(-1);i--) //160
	{			
		for(k=(cnn->fc160_1->inputNum-1);k!=(-1);k--)
		{
			cnn->fc160_1->y[i] += cnn->fc160_1->v[k] * cnn->fc160_1->wData[i][k];
		}
		cnn->fc160_1->y[i] += cnn->fc160_1->basicData[i];		
	}

	
	end = clock();	
	fprintf(stderr,"fc160_1 time =         %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);
	
	start = clock();

	//fc160_2
	//(20,24)
	inSize.c = cnn->conv4->inputWidth - cnn->conv4->kernel_size + 1;	
	inSize.r = cnn->conv4->inputHeight - cnn->conv4->kernel_size + 1;	
	//80
	input_channel = cnn->conv4->outChannels;
	//160
	out_channel = 160;
	
	k = 0;
	for(j=0;j<input_channel;j++)//80
	{
		for(r=0;r<inSize.r;r++)	//24
		{
			for(c=0;c<inSize.c;c++)//20
			{
				cnn->fc160_2->v[k] = cnn->conv4->y[j][r][c];
				k++;
			}
		}
	}

	
	for(i=(out_channel-1);i!=(-1);i--) //160
	// for(i=0;i<out_channel;i++) //160
	{		
		for(k=(cnn->fc160_2->inputNum-1);k!=(-1);k--)
		// for(k=0;k<(cnn->fc160_1->inputNum);k++)
		{
			cnn->fc160_2->y[i] += cnn->fc160_2->v[k] * cnn->fc160_2->wData[i][k];
		}
		cnn->fc160_2->y[i] += cnn->fc160_2->basicData[i];		
	}

	end = clock();
	fprintf(stderr,"fc160_2 time =         %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

	start = clock();

	FILE *output = fopen(filename,"w");

	// fc160	
	for(i=(160-1);i!=(-1);i--)
	{
		cnn->fc160->outputdata[i] = cnn->fc160_1->y[i] + cnn->fc160_2->y[i];
		fprintf(output,"%f\n", cnn->fc160->outputdata[i]);
	}
	fclose(output);

	end = clock();
	fprintf(stderr,"fc160 time =           %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

}


void conv_clear(CovLayer* covL)
{
	int j,c,r;
	int outH,outW;
//conv
	outW = covL->inputWidth - covL->kernel_size + 1;
	outH = covL->inputHeight - covL->kernel_size + 1;
	for(j=(covL->outChannels-1);j!=(-1);j--)
	{
		for(r=(outH-1);r!=(-1);r--)	
		{			
			for(c=(outW-1);c!=(-1);c--)
			{
				covL->v[j][r][c] = 0;
				covL->y[j][r][c] = 0;
			}
		}
	}
}

void pool_clear(PoolLayer* pool,int stride)
{
	int j,r,c;
	int outW,outH;

	if(stride==2)
	{
		outW = ((pool->inputWidth%2==0)?0:1) + pool->inputWidth/pool->kernel_size;
		// printf("outW should be 47/21, here the value is %d\n",outW);
		outH = ((pool->inputHeight%2==0)?0:1) + pool->inputHeight/pool->kernel_size;
		// printf("outW should be 55/25, here the value is %d\n",outH);
	}
	else
	{
		outW = pool->inputWidth - pool->kernel_size + 1;
		// printf("outW should be 44, here the value is %d\n",outW);
		outH = pool->inputHeight - pool->kernel_size + 1;
		// printf("outW should be 52, here the value is %d\n",outH);
	}

	for(j=(pool->outChannels-1);j!=(-1);j--)	
	{		
		for(r=(outH-1);r!=(-1);r--)	
		{
			for(c=(outW-1);c!=(-1);c--)
			{
				pool->y[j][r][c] = 0;
			}
		}
	}
}

void fc_clear(FcLayer* fc)
{
	int i;
	for(i=(fc->inputNum-1);i!=(-1);i--)
	{
		fc->v[i] = 0;
	}
	for(i=(fc->outputNum-1);i!=(-1);i--)
	{
		fc->y[i] = 0;
	}

}