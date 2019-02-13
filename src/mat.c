#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "mat.h"

void addmat(float **res, float **mat1, nSize matSize1, float **mat2, nSize matSize2)
{
	int i,j;
	if(matSize1.c!=matSize2.c || matSize1.r!=matSize2.r)
	{
		printf("ERROR: Size is not same!");
	}
	for(i=0;i<matSize1.r;i++)
	{
		for(j=0;j<matSize1.c;j++)
		{
			res[i][j]=mat1[i][j]+mat2[i][j];
		}
	}
}

/*
full:
5 * 5 conv 3 * 3 = (5 + 3 - 1) * (5 + 3 - 1)
inSize + (kernel_size - 1)

same:
5 * 5 conv 3 * 3 = 5 * 5
inSize

valid:
5 * 5 conv 3 * 3 = (5 - 3 + 1) * (5 - 3 + 1)
inSize - (kernel_size - 1)

*/

float **correlation(float **map,nSize kernel_size,float **inputData,nSize inSize,int type)
{
	int i,j,c,r;
	int halfkernel_sizew;
	int halfkernel_sizeh;
	if(kernel_size.r%2==0 && kernel_size.c%2==0)
	{ // even
		halfkernel_sizew=(kernel_size.c)/2; 
		halfkernel_sizeh=(kernel_size.r)/2;
	}
	else
	{ // odd
		halfkernel_sizew=(kernel_size.c-1)/2; 
		halfkernel_sizeh=(kernel_size.r-1)/2;
	}

	// full -> inSize+(kernel_size-1)
	int outSizeW=inSize.c+(kernel_size.c-1); 
	int outSizeH=inSize.r+(kernel_size.r-1);
	float **outputData=(float **)malloc(outSizeH * sizeof(float *)); 
	for(i=0;i<outSizeH;i++)
	{
		outputData[i]=(float*)calloc(outSizeW,sizeof(float));
	}

	float **exInputData=matEdgeExpand(inputData,inSize,kernel_size.c-1,kernel_size.r-1);	

	for(j=0;j<outSizeH;j++)
	{
		for(i=0;i<outSizeW;i++)
		{
			for(r=0;r<kernel_size.r;r++)
			{
				for(c=0;c<kernel_size.c;c++)
				{
					outputData[j][i]=outputData[j][i]+map[r][c]*exInputData[j+r][i+c];
				}
			}
		}
	}

	for(i=0;i<inSize.r+2*(kernel_size.r-1);i++)
	{
		free(exInputData[i]);
	}
	free(exInputData);
	nSize outSize={outSizeW,outSizeH};
	switch(type)
	{ 
		case full: 
			return outputData;
		case same:
		{
			float **sameres = matEdgeShrink(outputData,outSize,halfkernel_sizew,halfkernel_sizeh);
			for(i=0;i<outSize.r;i++)
			{
				free(outputData[i]);
			}
			free(outputData);
			return sameres;
		}
		case valid:
		{
			float **validres;
			if(kernel_size.r%2==0&&kernel_size.c%2==0)
			{
				validres=matEdgeShrink(outputData,outSize,halfkernel_sizew*2-1,halfkernel_sizeh*2-1);
			}
			else
			{
				validres=matEdgeShrink(outputData,outSize,halfkernel_sizew*2,halfkernel_sizeh*2);
			}
			for(i=0;i<outSize.r;i++)
			{
				free(outputData[i]);
			}
			free(outputData);
			return validres;
		}
		default:
			return outputData;
	}
}

float** cov(float** map,nSize kernel_size,float** inputData,nSize inSize,int type) 
{
	int i;	
	float **res=correlation(map,kernel_size,inputData,inSize,type);
	return res;
}


float **matEdgeExpand(float **mat,nSize matSize,int addc,int addr)
{
	int i,j;
	int c=matSize.c;
	int r=matSize.r;

	float **res=(float **)malloc((r + 2 * addr) * sizeof(float *));
	for(i=0;i<(r + 2 * addr);i++)
	{
		res[i]=(float*)malloc((c+2*addc)*sizeof(float));
	}

	for(j=0;j<r+2*addr;j++)
	{
		for(i=0;i<c+2*addc;i++)
		{
			if(j<addr || i<addc || j>=(r+addr) || i>=(c+addc))
				res[j][i]=(float)0.0;
			else
				res[j][i]=mat[j-addr][i-addc];
		}
	}
	return res;
}

float** matEdgeShrink(float** mat,nSize matSize,int shrinkc,int shrinkr)
{ 
	int i,j;
	int c = matSize.c;
	int r = matSize.r;
	float **res=(float **)malloc((r - 2 * shrinkr) * sizeof(float *)); 
	for(i=0;i<(r-2*shrinkr);i++)
	{
		res[i]=(float *)malloc((c-2*shrinkc)*sizeof(float));
	}
	
	for(j=0;j<r;j++)
	{
		for(i=0;i<c;i++)
		{
			if(j>=shrinkr&&i>=shrinkc&&j<(r-shrinkr)&&i<(c-shrinkc))
			{
				res[j-shrinkr][i-shrinkc]=mat[j][i]; 
			}
		}
	}
	return res;
}



float** transpose_matrix(float **input,int c, int r)
{
	float **output;
	output = (float**)malloc(sizeof(float*)*r);
	int i,j,k;
	for(i=0;i<r;i++)
	{
		output[i] = (float*)malloc(sizeof(float)*c);
		for(j=0;j<c;j++)			
		{
			output[i][j] = input[j][i];
		}
	}
	return output;
}