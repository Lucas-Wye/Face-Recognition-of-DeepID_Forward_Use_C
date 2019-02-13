// usage: ./cnn.app image_bin_filename
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cnn.h"
int main(int argc, char* argv[])
{
	char *filename;
	// cnn initialization		
	CNN* cnn=(CNN*)malloc(sizeof(CNN));		

	//input image data:112 * 96(r * c)(h * w)
	float ***input_data;
	input_data = read_image_rgb(argv[1]);
	int i,j,k;

	//input layer 
	// w  -> c
	// h  -> r	
	// 112 * 96 (r * c)(h * w)
	cnn->conv1 = initCovLayer(96,112,4,3,20);//w(20,3,4,4)
	filename = "net_data/conv1.bin";
	read_file_conv(filename,20,3,4,4,cnn->conv1);
	cnn->pool1 = initPoolLayer(93,109,2,20,20,MaxPool,2); //stride = 2
	
	cnn->conv2 = initCovLayer(47,55,3,20,40);//(40,20,3,3)
	// w(40,20,3,3)
	filename = "net_data/conv2.bin";	
	read_file_conv(filename,40,20,3,3,cnn->conv2);
	cnn->pool2 = initPoolLayer(45,53,2,40,40,MaxPool,1); //stride = 1
		
	cnn->conv3 = initCovLayer(44,52,3,40,60);//(60,40,3,3)
	// w(60,40,3,3)
	filename = "net_data/conv3.bin";	
	read_file_conv(filename,60,40,3,3,cnn->conv3);

	cnn->pool3 = initPoolLayer(42,50,2,60,60,MaxPool,2); //stride = 2
	
	cnn->conv4 = initCovLayer(21,25,2,60,80);//(80,60,2,2)
	// w(80,60,2,2)
	filename = "net_data/conv4.bin";	
	read_file_conv(filename,80,60,2,2,cnn->conv4);
	
	cnn->fc160_1 = initFcLayer(31500,160);
	// w(160,31500)
	filename ="net_data/fc160_1.bin";
	read_file_fc(filename,160,31500,cnn->fc160_1);
	
	cnn->fc160_2 = initFcLayer(38400,160);	
	// w(160,38400)
	filename = "net_data/fc160_2.bin";
	read_file_fc(filename,160,38400,cnn->fc160_2);

	cnn->fc160 = initOutLayer(160,160);	

	cnnff(cnn,input_data);		

	return 0;
}
