#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

#include "GrayBmp.h"

using namespace std;

#define FILE_TYPE_BMP	19778

bool LoadGrayBmp(const char *path, unsigned char* &intensity, int &width, int &height)
{
	FILE *fp;
	if( fp = fopen(path, "rb") )
	{
		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		if(bfh.bfType != FILE_TYPE_BMP)
			return false;

		fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

		int sizePalette = bfh.bfOffBits - sizeof(BITMAPFILEHEADER) -  sizeof(BITMAPINFOHEADER);
		int numRGBQuad = sizePalette / sizeof(RGBQUAD);

		RGBQUAD * palette = NULL;
		palette = new RGBQUAD[numRGBQuad];
		fread(palette, sizePalette, 1, fp);

		width = bih.biWidth;
		height = bih.biHeight;
		int sizePixel = (bih.biBitCount >> 3);
		int ByteWidth = MakeByteWidth(width, bih.biBitCount);
		int size = ByteWidth * height;
		unsigned char * data = new unsigned char[size];
		fread(data, size, 1, fp);

		intensity = new unsigned char[width * height];

		int p = 0;
		if(numRGBQuad)
		{
			for(int j = 0; j < height; j++) {
				for(int i = 0; i < width; i++) {
					int offset = j * ByteWidth + i*sizePixel;
					unsigned char val = data[offset];
					const RGBQUAD &color = palette[val];
					int gray = ( color.rgbBlue + color.rgbGreen 
						+ color.rgbRed ) / 3;
					intensity[p++] = (unsigned char)gray;
				}
			}
		}
		else
		{
			for(int j = 0; j < height; j++) {
				for(int i = 0; i < width; i++) {
					int offset = j * ByteWidth + i*sizePixel;
					int gray = 0;
					for(int c = 0; c < sizePixel; c++)
						gray += data[offset+c];
					intensity[p++] = gray / sizePixel;
				}
			}
		}

		delete [] data;
		delete [] palette;
		fclose(fp);
	}
	else
		return false;
	return true;
}

bool SaveGrayBmp(const char *path, unsigned char intensity[], int width, int height, int bitWidth)
{
	assert(bitWidth <= 32 && bitWidth >= 8);

	FILE *fp;
	if( fp = fopen(path, "wb") )
	{
		int ByteWidth = MakeByteWidth(width, bitWidth);
		int size = ByteWidth * height;
		int sizePixel = bitWidth >> 3;
		int numRGBQuads = bitWidth == 8 ? 256 : 0;
		int sizePalette = numRGBQuads * sizeof(RGBQUAD);

		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		bfh.bfType = FILE_TYPE_BMP;
		bfh.bfSize = size * sizePixel + 54 + sizePalette;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = 54 + sizePalette;
		bih.biSize = 40;
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = bitWidth;
		bih.biCompression = 0;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		unsigned char * data = new unsigned char[size];
		int p = 0;

		RGBQUAD *palette = NULL;
		if(sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for(int i = 0; i < numRGBQuads; i++)
			{	
				unsigned char *adrr = (unsigned char *)(palette + i);
				memset(adrr, i, 3);
				*(adrr+3) = 0;
			}
		}
		for(int j = 0; j < height; j++) {
			for(int i = 0; i < width; i++) {
				int offset = j * ByteWidth + i*sizePixel;
				unsigned char gray = intensity[p++];
				memset(data+offset, gray, sizePixel);
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if(sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		delete [] data;
		if(palette) delete [] palette;
	}
	else
		return false;
	return true;
}

bool SaveBmp(const char *path, unsigned char intensity[],
			 int width, int height, int bitWidth)
{
	FILE *fp;
	if (fp = fopen(path, "wb"))
	{
		int ByteWidth = MakeByteWidth(width, bitWidth);
		int size = ByteWidth * height;
		int sizePixel = bitWidth >> 3;
		int numRGBQuads = bitWidth == 8 ? 256 : 0;
		int sizePalette = numRGBQuads * sizeof(RGBQUAD);

		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		bfh.bfType = FILE_TYPE_BMP;
		bfh.bfSize = size * sizePixel + 54 + sizePalette;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = 54 + sizePalette;
		bih.biSize = 40;
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = bitWidth;
		bih.biCompression = 0;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;


		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		fwrite(intensity, size, 1, fp);

		fclose(fp);
	}
	else
		return false;
	return true;
}

void HSV2RGB(float h, float s, float v, float &r, float &g, float &b)
{
	//const float ratio = 360.0f / 255.0f;
	float k = h / 60.0f;
	int hi = ((int)k) % 6; 
	float f = k - hi;
	float p = v * (1.0 - s);
	float q = v * (1.0 - f*s);
	float t = v * (1.0 - (1.0-f)*s);

	//	float fR, fG, fB;
	switch (hi)
	{
	case 0:
		r = v;
		g = t;
		b = p;
		break;	
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}
}

void GrayToColor(unsigned char gray, 
				 unsigned char &r, unsigned char &g, unsigned char &b)
{
	const float ratio = 360.0f / 255.0f;

	float h = gray * ratio;
	float k = h / 60.0f;
	int hi = (int)k % 6; 
	float f = k - hi;
	float p = 0.0f;
	float q = 1.0 - f;
	float t = f;

	float fR, fG, fB;
	switch (hi)
	{
	case 0:
		fR = 1.0f;
		fG = t;
		fB = p;
		break;	
	case 1:
		fR = q;
		fG = 1.0f;
		fB = p;
		break;
	case 2:
		fR = p;
		fG = 1.0f;
		fB = t;
		break;
	case 3:
		fR = p;
		fG = q;
		fB = 1.0f;
		break;
	case 4:
		fR = t;
		fG = p;
		fB = 1.0f;
		break;
	case 5:
		fR = 1.0f;
		fG = p;
		fB = q;
		break;
	}

	r = (unsigned char) (fR * 255.0f);
	g = (unsigned char) (fG * 255.0f);
	b = (unsigned char) (fB * 255.0f);
}

void GrayToGR(unsigned char gray, 
			  unsigned char &r, unsigned char &g, unsigned char &b)
{
	const float ratio = 150.0f / 255.0f;
	float hue = (float) (255 - gray) * ratio;
	float fR, fG, fB;
	HSV2RGB(hue, 1.0f, 1.0f, fR, fG, fB);
	r = fR * 255.0f;
	g = fG * 255.0f;
	b = fB * 255.0f;
}

bool SaveColoredBmp(const char *path, unsigned char intensity[], 
					int width, int height, int bitWidth, G2C g2c)
{
	assert(bitWidth <= 32 && bitWidth >= 8);

	if (g2c == NULL)
		g2c = GrayToGR;

	FILE *fp;
	if( fp = fopen(path, "wb") )
	{
		int ByteWidth = MakeByteWidth(width, bitWidth);
		int size = ByteWidth * height;
		int sizePixel = bitWidth >> 3;
		int numRGBQuads = bitWidth == 8 ? 256 : 0;
		int sizePalette = numRGBQuads * sizeof(RGBQUAD);

		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		bfh.bfType = FILE_TYPE_BMP;
		bfh.bfSize = size * sizePixel + 54 + sizePalette;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = 54 + sizePalette;
		bih.biSize = 40;
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = bitWidth;
		bih.biCompression = 0;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		unsigned char * data = new unsigned char[size];
		int p = 0;

		RGBQUAD *palette = NULL;
		if(sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for(int i = 0; i < numRGBQuads; i++)
			{	
				unsigned char *adrr = (unsigned char *)(palette + i);
				memset(adrr, i, 3);
				*(adrr+3) = 0;
			}
		}
		for(int j = 0; j < height; j++) {
			for(int i = 0; i < width; i++) {
				int offset = j * ByteWidth + i*sizePixel;
				unsigned char gray = intensity[p++];
				//memset(data+offset, gray, sizePixel);
				unsigned char *color = data + offset;
#if 0
				GrayToColor(gray, color[0], color[1], color[2]);
#else
				//GrayToGR(gray, color[2], color[1], color[0]);
				g2c(gray, color[2], color[1], color[0]);
#endif			
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if(sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		delete [] data;
		if(palette) delete [] palette;
	}
	else
		return false;
	return true;
}



#if 0
void CreateBmpData(unsigned char *intensity, int width, int height, int bitWidth, unsigned char* &data)
{
	int ByteWidth = MakeByteWidth(width, bitWidth);
	int size = height * ByteWidth;
	data = new unsigned char[size];
	int sizePixel = bitWidth >> 3;
	int pos = 0;
	for(int j = 0; j < height; j++) {
		for(int i = 0; i < width; i++) {
			int offset = j * ByteWidth + i*sizePixel;
			unsigned char gray = intensity[pos++];
			memset(data+offset, gray, sizePixel);
		}
	}
}

#define MakeOffset(i, j) ( (j)*ByteWidth + (i)*sizePixel )

void bmpDrawDisk24(int x, int y, int radius, COLORREF color, 
				   unsigned char* &data, int width, int height)
{
	assert(data);
	int ByteWidth = MakeByteWidth(width, 24);
	int sizePixel = 3;
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);

	const int radius2 = radius * radius;
	for(int j = -radius; j <= radius; j++) {
		int jdist = j * j;
		for(int i = -radius; i <= radius; i++) {
			if(jdist + i*i < radius2)
			{
				int x0 = x + i;
				int y0 = y + j;

				if(x0 < 0 || x0 >= width)
					continue;
				if(y0 < 0 || y0 >= height)
					continue;

				int offset = MakeOffset(x0, y0);
				data[offset] = b;
				data[offset+1] = g;
				data[offset+2] = r;
			}
		}
	}
}

void bmpDrawLine24(int x0, int y0, int x1, int y1, COLORREF color, 
				   unsigned char* &data, int width, int height)
{
	assert(data);
	int ByteWidth = MakeByteWidth(width, 24);
	int sizePixel = 3;
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if(steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}
	int nStep = abs(x1 - x0);
	int x = x0;
	int n = x1-x0 > 0 ? 1 : -1; 
	double fy = y0;
	double m = double(y1-y0) / nStep;
	for(int i = 0; i < nStep; i++) {
		int y = fy + 0.5;
		int offset = steep ? MakeOffset(y, x) : MakeOffset(x, y);
		data[offset] = b;
		data[offset+1] = g;
		data[offset+2] = r;	
		x += n;
		fy += m;
	}
}

void WriteBmpData(const char *path, unsigned char *data, int width, int height, int bitWidth)
{
	FILE *fp;
	if( fp = fopen(path, "wb") )
	{
		int ByteWidth = MakeByteWidth(width, bitWidth);
		int size = ByteWidth * height;
		int sizePixel = bitWidth >> 3;
		int numRGBQuads = bitWidth == 8 ? 256 : 0;
		int sizePalette = numRGBQuads * sizeof(RGBQUAD);

		BITMAPFILEHEADER bfh;
		BITMAPINFOHEADER bih;

		bfh.bfType = FILE_TYPE_BMP;
		bfh.bfSize = size * sizePixel + 54 + sizePalette;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = 54 + sizePalette;
		bih.biSize = 40;
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = bitWidth;
		bih.biCompression = 0;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;


		RGBQUAD *palette = NULL;
		if(sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for(int i = 0; i < numRGBQuads; i++)
			{	
				unsigned char *adrr = (unsigned char *)(palette + i);
				memset(adrr, i, 3);
				*(adrr+3) = 0;
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if(sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		if(palette) delete [] palette;
	}
}

void DestroyBmpData(unsigned char* &data)
{
	assert(data);
	delete [] data;
	data = NULL;
}
#endif