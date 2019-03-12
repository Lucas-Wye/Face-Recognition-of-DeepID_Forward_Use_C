#pragma once

/* Bitmap Header, 54 Bytes  */
static 
unsigned char BmpHeader[54] = 
{
	0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x01, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x60, 0xCD, 0x04, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x23, 0x2E, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void SetBitmapInfo(unsigned int size, int height, int width)
{
	for (int i = 0; i < 4; i++)
	{
		// size of image ( header + data )
		BmpHeader[2 + i] = size & 0xff;
		size >>= 8;

		// width of image
		BmpHeader[18 + i] = width & 0xff;
		width >>= 8;

		// height of image
		BmpHeader[22  + i] = height & 0xff;
		height >>= 8;
	}
}


/* BGR format */
unsigned char *Encoder(unsigned char *buf, int height, int width, int &size,const char *fileName)
{
	unsigned char *bitmap = NULL;
	int rowSize = (24 * width + 31) / 32 * 4;
	
	// compute the size of total bytes of image
	size = rowSize * height + 54; // data size + header size
	bitmap = new unsigned char [ size ];
	
	// set the header info
	SetBitmapInfo(size, height, width);

	// fill the header area
	for (int i = 0; i < 54; i++)
	{
		bitmap[i] = BmpHeader[i];
	}

	int **p1= new int*[height];
	for(int i=0;i<height;i++)
	{
 		p1[i]=new int[width];
	}
	int **p2= new int*[height];
	for(int i=0;i<height;i++)
	{
 		p2[i]=new int[width];
	}
	int **p3= new int*[height];
	for(int i=0;i<height;i++)
	{
 		p3[i]=new int[width];
	}

	// fill the data area
	for (int i = 0; i < height; i++)
	{
		// compute the offset of destination bitmap and source image
		int idx = height - 1 - i;
		int offsetDst = idx * rowSize + 54; // 54 means the header length
		int offsetSrc = i * width * 3;
		
		// fill data
		for (int j = 0; j < width * 3; j++)
		{
			bitmap[offsetDst + j] = buf[offsetSrc + j];//B,G,R
			// printf("%d ",buf[offsetSrc + j]);
		}
		// printf("\n");

		int count = 0;
		
		//B
		for(int j = 0 ;j < width * 3;j=j+3)
		{
			p1[i][count] = buf[offsetSrc + j]; 
			count++;
			// printf("%d\n",buf[offsetSrc + j]);
		}
		
		// G
		count = 0;
		for(int j = 1 ;j < width * 3;j=j+3)
		{
			p2[i][count] = buf[offsetSrc + j]; 
			count++;
			// printf("%d\n",buf[offsetSrc + j]);
		}		
					
		//R
		count = 0;
		for(int j = 2 ;j < width * 3;j=j+3)
		{
			p3[i][count] = buf[offsetSrc + j]; 
			count++;
		// 	printf("%d\n",buf[offsetSrc + j]);
		}
		
		// fill 0x0, this part can be ignored
		for (int j = width * 3; j < rowSize; j++)
		{
			bitmap[offsetDst +j] = 0x0;
		}
	}

	FILE *fp = fopen(fileName,"wb");
	
	int temp;
	temp = width;
	fwrite(&temp,sizeof(int),1,fp);
	temp = height;
	fwrite(&temp,sizeof(int),1,fp);
	//R
	for(int i=0;i<width;i++)		
	{
		for(int j=0;j<height;j++)
		{
			// printf("%d\n", );
			temp = p3[j][i];
			fwrite(&temp,sizeof(int),1,fp);
		}
	}	

	//G
	for(int i=0;i<width;i++)		
	{
		for(int j=0;j<height;j++)
		{
			// printf("%d\n", p2[j][i]);
			temp = p2[j][i];
			fwrite(&temp,sizeof(int),1,fp);
		}
	}

	//B	
	for(int i=0;i<width;i++)		
	{
		for(int j=0;j<height;j++)
		{
			// printf("%d\n", p1[j][i]);
			temp = p1[j][i];
			fwrite(&temp,sizeof(int),1,fp);
		}
	}

	return bitmap;
}


/* Save to file */
void Write(const char *fileName, unsigned char *buf, int &size)
{
	FILE *fp = fopen(fileName, "wb+");
	fwrite(buf, 1, size, fp);
	fclose(fp);
}

