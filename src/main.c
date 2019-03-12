// usage: ./cnn.app image_bin_filename
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cnn.h"

int main(int argc, char* argv[])
{	
	clock_t start_all,end_all;
	start_all = clock();
	
	clock_t start,end;	
	start = clock();	
	
	char *filename;
//-----------------------------------------------------------------------
	// cnn initialization
	CNN* cnn=(CNN*)malloc(sizeof(CNN));
	
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

	end = clock();
	fprintf(stderr,"initialization time =  %f seconds\n",(double)(end-start)/CLOCKS_PER_SEC);

	float ***inputData;
//image 1	
	fprintf(stderr, "forward_1:\n");		
	inputData = read_image_rgb(argv[1]);
	cnnff(cnn,inputData,argv[2]);	

//clear	
	conv_clear(cnn->conv1);
	conv_clear(cnn->conv2);
	conv_clear(cnn->conv3);
	conv_clear(cnn->conv4);
	
	pool_clear(cnn->pool1,2);
	pool_clear(cnn->pool2,1);
	pool_clear(cnn->pool3,2);

	fc_clear(cnn->fc160_1);
	fc_clear(cnn->fc160_2);

//image 2	
	fprintf(stderr, "forward_2:\n");	
	//input image data:112 * 96(r * c)(h * w)	
	inputData = read_image_rgb(argv[3]);
	cnnff(cnn,inputData,argv[4]);	

	end_all = clock();
	fprintf(stderr,"\n------------------------------------------\ntotal time          =  %f seconds\n------------------------------------------\n",(double)(end_all-start_all)/CLOCKS_PER_SEC);

	return 0;
}
