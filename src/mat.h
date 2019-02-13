#ifndef __MAT_
#define __MAT_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define full 0
#define same 1
#define valid 2

typedef struct Mat2DSize{
	int c; // w
	int r; // h
}nSize;

// add
void addmat(float **res, float **mat1, nSize matSize1, float **mat2, nSize matSize2);

// correlation
float **correlation(float **map,nSize kernel_size,float **inputData,nSize inSize,int type);

// conv
float **cov(float **map,nSize kernel_size,float **inputData,nSize inSize,int type); 

// 
float **matEdgeExpand(float **mat,nSize matSize,int addc,int addr);

// 
float **matEdgeShrink(float **mat,nSize matSize,int shrinkc,int shrinkr);

//
float** transpose_matrix(float **input,int c, int r);

#endif
