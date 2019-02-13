#ifndef __CNN_
#define __CNN_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "mat.h"
#define MaxPool 1

// conv layer
typedef struct convolutional_layer{
	int inputWidth;  
	int inputHeight;	
	int inChannels;  
	int outChannels;
	int kernel_size;
	//w is a 4d matrix, inChannels*outChannels*kernel_size*kernel_size	
	float**** mapData;	  	
	//b
	float* basicData;	
	// the input of active function
	float*** v; 
	// the output of active function
	float*** y; 
}CovLayer;

// pooling layer
typedef struct pooling_layer{
	int inputWidth;   
	int inputHeight; 
	int kernel_size;
	int inChannels;
	int outChannels; 
	int poolType;
	//the output data
	float*** y; 
}PoolLayer;

// full connect layer
typedef struct fc_layer{
	int inputNum;
	int outputNum;

	float** wData; // w(inputNum*outputNum)
	float* basicData;   //b(outputNum)
	float* v;
	float* y;
	
}FcLayer;

// output layer
typedef struct out_layer{
	int inputNum;
	int outputNum;
	float *outputdata;
}OutLayer;

// the whole network
typedef struct cnn_network{
	int layerNum;

	CovLayer *conv1; // 20,3,4,4
	PoolLayer *pool1; // MAX 2,2

	CovLayer *conv2; // 40,20,3,3
	PoolLayer *pool2; // MAX 2,1

	CovLayer *conv3; // 60,40,3,3
	PoolLayer *pool3; // MAX 2,2

	CovLayer *conv4; // 80,60,2,2
	
	FcLayer *fc160_1; // 160,31500
	FcLayer *fc160_2; // 160,38400
	
	OutLayer *fc160; // 160

	float* e; // training error
	float* L; // error energy
}CNN;

CovLayer* initCovLayer(int inputWidth,int inputHeight,int kernel_size,int inChannels,int outChannels);
PoolLayer *initPoolLayer(int inputWidth,int inputHeight,int kernel_size,int inChannels,int outChannels,int poolType,int stride);
FcLayer* initFcLayer(int inputNum,int outputNum);
OutLayer *initOutLayer(int inputNum,int outputNum);

float activation_relu(float input,float bas);
void MaxPooling(float** output,nSize outputSize,float** input,nSize inputSize,int kernel_size,int stride); 

void read_file_fc(char *filename,int a,int b,FcLayer *fc);
void read_file_conv(char *filename,int a, int b, int c, int d,CovLayer *conv);

float*** read_image_rgb(char *filename);

void cnnff(CNN* cnn,float*** inputData);
#endif

