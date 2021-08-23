#ifndef _GRAY_BMP_H
#define _GRAY_BMP_H

#include <windows.h>
#include "primitive.h"


#define MakeByteWidth(width, bitwidth) (((width)*(bitwidth) + 31) / 32 * 4);
#define MakeOffset(i, j) ( (j)*ByteWidth + (i)*sizePixel )

extern bool LoadGrayBmp(const char *path, unsigned char* &intensity, int &width, int &height);

extern bool SaveGrayBmp(const char *path, unsigned char intensity[], int width, int height, int bitWidth);

extern void HSV2RGB(float h, float s, float v, float &r, float &g, float &b);

extern void GrayToColor(unsigned char gray, unsigned char &r, 
						unsigned char &g, unsigned char &b);

typedef void (*G2C)(unsigned char, 
					unsigned char&, unsigned char &, unsigned char &);

typedef void (*D2C)(double, 
					unsigned char&, unsigned char &, unsigned char &);

//extern bool SaveColoredBmp(const char *path, unsigned char intensity[], 
//					int width, int height, int bitWidth = 24);
extern bool SaveColoredBmp(const char *path, unsigned char intensity[], 
						   int width, int height, int bitWidth = 24, 
						   G2C g2c = NULL);

extern bool SaveColoredBmp(const char *path, double intensity[], 
						   int width, int height, int bitWidth ,
						   D2C d2c, DPoint2 org[], DPoint2 dst[], int nLines);

#if 0
void CreateBmpData(unsigned char *intensity, int width, int height, int bitWidth, unsigned char* &data);

void DestroyBmpData(unsigned char* &data);

void bmpDrawDisk24(int x, int y, int radius, COLORREF color, 
			  unsigned char* &data, int width, int height);

void bmpDrawLine24(int x0, int y0, int x1, int y1, COLORREF color, 
			  unsigned char* &data, int width, int height);

void WriteBmpData(const char *path, unsigned char *data, int width, int height, int bitWidth);

#endif

#endif // _GRAY_BMP_H