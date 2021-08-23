#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <limits>
#include <fstream>
#include <iomanip>

#include "DebugUtility.h"
#include "GrayBmp.h"

#define MakeByteWidth(width, bitwidth) (((width)*(bitwidth) + 31) / 32 * 4)

void  WriteLog(char const *msg)
{
	FILE *fp = fopen("debug.log", "a");
	fprintf(fp, "%s\n", msg);
	fclose(fp);
}

void outputPointPair(const DPoint3 &p0, const DPoint3 &p1, int id, 
					 const char *path)
{
#define MAX_PATH 260
	char defaultPath[MAX_PATH];
	const char *fileName = path;

	if (path == NULL) {
		sprintf(defaultPath, "data\\debug\\%d.vs", id);
		fileName = defaultPath;
	}

	FILE *fp = fopen(fileName, "w");
	fprintf(fp, "GOCAD VSet 1\n"
		"HEADER{\n"
		"name:%d\n"
		"*atoms*size: 10\n"
		"* atoms*color: 0.000000 1.000000 0.000000 1\n"
		"}\n", id);
	fprintf(fp, "VRTX 0 %lf %lf %lf\n", p0.x, p0.y, p0.z);
	fprintf(fp, "VRTX 1 %lf %lf %lf\n", p1.x, p1.y, p1.z);
	fclose(fp);
}

// save bitmap in float format
bool SaveFltBmp(const char *path, float values[], int width, int height, int bitWidth)
{
#define FILE_TYPE_BMP	19778

	assert(bitWidth <= 32 && bitWidth >= 8);

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

		BYTE * data = new BYTE[ByteWidth*height];
		memset(data, 0, ByteWidth*height);
		int originBytesWidth = width * 3;
		for (int j = 0, idx = 0, lineBase = 0; j < height; j++, lineBase += ByteWidth) {
			int lineOffset = 0;
			//int count = 0;
			for (int i = 0; i < originBytesWidth; i++, idx++) {
				//if (count++ == 3) {
				//	count = 0;
				//	continue;
				//}

				int dataOffset = lineBase + lineOffset;

				if (values[idx] <= 1.0f && values[idx] > 0.0f)
					data[dataOffset] = 255 * values[idx];
				else
					data[dataOffset] = values[idx];

				lineOffset++;
			}
		}

		RGBQUAD *palette = NULL;
		if (sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for (int i = 0; i < numRGBQuads; i++)
			{
				BYTE *adrr = (BYTE *)(palette + i);
				memset(adrr, i, 3);
				*(adrr + 3) = 0;
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if (sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		if (data != NULL)
			delete[] data;
		if (palette)
			delete[] palette;
	}
	else
		return false;
	return true;
}

// save bitmap in float format
bool SaveFltBmp4ch(const char *path, float values[], int width, int height, int bitWidth)
{
#define FILE_TYPE_BMP	19778

	assert(bitWidth <= 32 && bitWidth >= 8);

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


		BYTE * data = new BYTE[ByteWidth*height];
		memset(data, 0, ByteWidth*height);
		int originBytesWidth = width * 3;
		for (int j = 0, idx = 0, lineBase = 0; j < height; j++, lineBase += ByteWidth) {
			for (int i = 0; i < originBytesWidth; i++, idx++) {
				int dataOffset = lineBase + i;


				if (values[idx] <= 1.0f && values[idx] > 0.0f)
					data[dataOffset] = 255 * values[idx];
				else
					data[dataOffset] = values[idx];
			}
		}


		RGBQUAD *palette = NULL;
		if (sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for (int i = 0; i < numRGBQuads; i++)
			{
				BYTE *adrr = (BYTE *)(palette + i);
				memset(adrr, i, 3);
				*(adrr + 3) = 0;
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if (sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		if (data != NULL)
			delete[] data;
		if (palette)
			delete[] palette;
	}
	else
		return false;
	return true;
}

// save bitmap in float format
bool SaveFltBmpSigleChannel(const char *path, float values[], int width, int height, int bitWidth)
{
#define MakeByteWidth(width, bitwidth) (((width)*(bitwidth) + 31) / 32 * 4)
#define FILE_TYPE_BMP	19778

	assert(bitWidth <= 32 && bitWidth >= 8);

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

		float max = -FLT_MAX;
		float min = FLT_MAX;
		BYTE * data = new BYTE[ByteWidth*height];
		for (int j = 0, idx = 0; j < height; j++) {
			for (int i = 0; i < width; i++, idx++) {
				float val = values[idx];
				if (val > max)
					max = val;
				if (val < min)
					min = val;
			}
		}
		for (int j = 0, idx = 0, lineBase = 0; j < height; j++, lineBase += ByteWidth) {
			for (int i = 0, offset = 0; i < width; i++, idx++, offset += 3) {
				float val = (values[idx] - min) / (max - min) * 255.0f;

				data[lineBase + offset] = val;
				data[lineBase + offset + 1] = val;
				data[lineBase + offset + 2] = val;
			}
		}

		RGBQUAD *palette = NULL;
		if (sizePalette)
		{
			palette = new RGBQUAD[numRGBQuads];
			for (int i = 0; i < numRGBQuads; i++)
			{
				BYTE *adrr = (BYTE *)(palette + i);
				memset(adrr, i, 3);
				*(adrr + 3) = 0;
			}
		}

		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
		if (sizePalette)
			fwrite(palette, sizePalette, 1, fp);
		fwrite(data, size, 1, fp);

		fclose(fp);

		if (data != NULL)
			delete[] data;
		if (palette)
			delete[] palette;
	}
	else
		return false;
	return true;
}

void Float3ToPng(float *values, int imageWidth, int imageHeight,
				 const char *dstPath)
{
	png_bytep *row_pointers
		= (png_bytep*)malloc(sizeof(png_bytep) * imageHeight);
	int bytesWidth = 3 * imageWidth;
	for (int i = 0; i < imageHeight; i++)
		row_pointers[i] = (png_byte*)malloc(bytesWidth);

	float minVal[] = { FLT_MAX, FLT_MAX, FLT_MAX };
	float maxVal[] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	int size = imageWidth * imageHeight;
	for (int i = 0, offset = 0; i < size; i++, offset += 3) {
		float *curTuple = values + offset;

		for (int j = 0; j < 3; j++) {
			float curVal = curTuple[j];

			if (curVal < minVal[j])
				minVal[j] = curVal;
			if (curVal > maxVal[j])
				maxVal[j] = curVal;
		}
	}

	for (int j = imageHeight - 1, loc = 0; j >= 0; j--) {
		png_byte *ri = row_pointers[j];

		for (int i = 0, offset = 0; i < imageWidth; 
				i++, offset += 3, loc += 3) {
			float *curTuple = values + loc;

			for (int k = 0; k < 3; k++) {
				float curVal = curTuple[k];

				ri[offset + k] = (curVal - minVal[k]) / (maxVal[k] - minVal[k]) * 255;
			}
		}
	}

	FILE *fp = fopen(dstPath, "wb");
	if (!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		imageWidth, imageHeight,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);
	fclose(fp);

	for (int i = 0; i < imageHeight; i++) {
		free(row_pointers[i]);
	}

	free(row_pointers);
	png_destroy_write_struct(&png, &info);
}

void FloatToPng(float *values, int imageWidth, int imageHeight,
				const char *dstPath)
{
	png_bytep *row_pointers
		= (png_bytep*)malloc(sizeof(png_bytep) * imageHeight);
	int bytesWidth = 3 * imageWidth;
	for (int i = 0; i < imageHeight; i++)
		row_pointers[i] = (png_byte*)malloc(bytesWidth);

	float minVal = FLT_MAX;
	float maxVal = -FLT_MAX;
	int size = imageWidth * imageHeight;
	for (int i = 0, offset = 0; i < size; i++, offset++) {
		float curVal = values[offset];

		if (curVal < minVal)
			minVal = curVal;
		if (curVal > maxVal)
			maxVal = curVal;
	}

	for (int j = imageHeight - 1, loc = 0; j >= 0; j--) {
		png_byte *ri = row_pointers[j];

		for (int i = 0, offset = 0; i < imageWidth;
			i++, offset += 3, loc ++) {
			float curVal = values[loc];
			BYTE color = (curVal - minVal) / (maxVal - minVal) * 255;

			for (int k = 0; k < 3; k++)
				ri[offset + k] = color;
		}
	}

	FILE *fp = fopen(dstPath, "wb");
	if (!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		imageWidth, imageHeight,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);
	fclose(fp);

	for (int i = 0; i < imageHeight; i++) {
		free(row_pointers[i]);
	}

	free(row_pointers);
	png_destroy_write_struct(&png, &info);
}


void FloatToGRPng(float* values, int imageWidth, int imageHeight,
	const char* dstPath)
{
	png_bytep* row_pointers
		= (png_bytep*)malloc(sizeof(png_bytep) * imageHeight);
	int bytesWidth = 3 * imageWidth;
	for (int i = 0; i < imageHeight; i++)
		row_pointers[i] = (png_byte*)malloc(bytesWidth);

	float minVal = FLT_MAX;
	float maxVal = -FLT_MAX;
	int size = imageWidth * imageHeight;
	for (int i = 0, offset = 0; i < size; i++, offset++) {
		float curVal = values[offset];

		if (curVal < minVal)
			minVal = curVal;
		if (curVal > maxVal)
			maxVal = curVal;
	}


	unsigned char color[3];
	float fcolor[3];
	for (int j = imageHeight - 1, loc = 0; j >= 0; j--) {
		png_byte* ri = row_pointers[j];

		for (int i = 0, offset = 0; i < imageWidth;
			i++, offset += 3, loc++) {
			float curVal = values[loc];
			BYTE gray = (curVal - minVal) / (maxVal - minVal) * 255;

			if (gray == 0)
				fcolor[0] = fcolor[1] = fcolor[2] = 0;
			else
				HSV2RGB(255-gray, 1.0, 1.0, fcolor[2], fcolor[1], fcolor[0]);
			//	GrayToGR(gray, color[2], color[1], color[0]);


			for (int k = 0; k < 3; k++) {
				color[k] = fcolor[k]*255.0;
				ri[offset + k] = color[k];
			}
		}
	}

	FILE* fp = fopen(dstPath, "wb");
	if (!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
		NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		imageWidth, imageHeight,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);
	fclose(fp);

	for (int i = 0; i < imageHeight; i++) {
		free(row_pointers[i]);
	}

	free(row_pointers);
	png_destroy_write_struct(&png, &info);
}

void bin2png(float *buff, int imageWidth, int imageHeight, const char *dstPath)
{
	png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * imageWidth);
	int bytesWidth = 24 * imageWidth;
	for (int i = 0; i < imageWidth; i++) {
		row_pointers[i] = (png_byte*)malloc(bytesWidth);
	}

	for (int j = 0, index = 0; j < imageWidth; j++) {
		png_byte *ri = row_pointers[imageWidth - j - 1];
		float *pi = &buff[j*imageWidth * 3];

		for (int i = 0, offset = 0; i < imageWidth; i++, index++, offset += 3) {
			float *curFeature = &pi[offset];

			ri[offset] = 255 * curFeature[0];
			ri[offset + 1] = 255 * curFeature[1];
			ri[offset + 2] = 255 * curFeature[2];
		}
	}


	FILE *fp = fopen(dstPath, "wb");
	if (!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		imageWidth, imageWidth,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
		);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	for (int i = 0; i < imageWidth; i++) {
		free(row_pointers[i]);
	}
	free(row_pointers);

	fclose(fp);
}

void WriteVS(const char *path, const char *name, DPoint3 *point, int nPoints)
{
	std::ofstream ofs(path);
	if (!ofs) {
		printf("outpath not found.\n");
		return;
	}

	const char *sFmt = "GOCAD VSet 1\n"
		"HEADER{\n"
		"name:%s\n"
		"}\n"
		;
	char sHeader[1024];
	sprintf(sHeader, sFmt, name);

	ofs << sHeader << std::endl;
	for (int i = 0; i < nPoints; i++) {
		const DPoint3 &curPos = point[i];


		ofs << "VRTX" << ' ';
		ofs << i + 1 << ' ';
		ofs << std::setprecision(12) << curPos.x << ' ';
		ofs << std::setprecision(12) << curPos.y << ' ';
		ofs << std::setprecision(12) << curPos.z << ' ';


		ofs << std::endl;
	}

	ofs << "END\n";
	ofs.close();
}

double minDepth, maxDepth;
