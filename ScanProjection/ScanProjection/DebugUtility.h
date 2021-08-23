#ifndef _DEBUG_H
#define _DEBUG_H

#include <png.h>

#include "../utility/primitive.h"
#include "../utility/feature.h"

extern
void outputPointPair(const DPoint3 &p0, const DPoint3 &p1, int id, 
					 const char *path = NULL);

extern
bool SaveFltBmp(const char *path, float values[], int width, int height, 
			int bitWidth);

extern
void bin2png(float *buff, int imageWidth, int imageHeight, const char *dstPath);

extern
void Float3ToPng(float *values, int imageWidth, int imageHeight,
				 const char *dstPath);

extern
void FloatToPng(float *values, int imageWidth, int imageHeight,
	const char *dstPath);

extern
void WriteVS(const char *path, const char *name, DPoint3 *point, int nPoints);

extern void WriteLog(const char *msg);

template <int K>
void SaveImage(const char *path, Feature<K, float> *features,
	int width, int height, int k)
{
	int size = width * height;
	FPoint3 minv(FLT_MAX, FLT_MAX, FLT_MAX);
	FPoint3 maxv(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < size; i++) {
		const Feature<K, float> &curFtr = features[i];
		for (int j = 0; j < 3; j++) {
			float curVal = curFtr[k + j];
			if (curVal < minv[j])
				minv[j] = curVal;
			if (curVal > maxv[j])
				maxv[j] = curVal;
		}
	}
	FPoint3 range = (maxv - minv) / 255.0f;

	int bytesWidth = MakeByteWidth(width, 24);
	unsigned char *data = new unsigned char[bytesWidth * height];
	memset(data, 0, bytesWidth *height);
	for (int j = 0, l = 0, lineOffset = 0; j < height;
		j++, lineOffset += bytesWidth) {

		for (int i = 0, p = 0; i < width; i++, p += 3, l++) {
			const Feature<K, float> &curFtr = features[l];

			for (int c = 0; c < 3; c++) {
				float curVal = curFtr[k + c];
				data[lineOffset + p + c] = (curVal - minv[c]) / range[c];
				//data[lineOffset + p + c] = curVal * 255;
			}
		}
	}

	SaveBmp(path, data, width, height);

	delete[] data;
}

template <int K, typename T>
bool IsNormal(Feature<K, T> *ftrs, int imageWidth, int imageHeight,
	T initVal = 0.0, T propThresh = 0.1)
{
	int count = 0;
	int size = imageWidth * imageHeight;
	for (int i = 0; i < size; i++) {
		const Feature<K, T> &curFeature = ftrs[i];

		if (curFeature[0] == initVal)
			count++;
	}

	double proportion = (double)count / (double)size;
	return proportion > propThresh;
}

extern
double minDepth, maxDepth;

template <int K, typename T>
void VisInPng(Feature<K, T> *ftrs, int loc[], int imageWidth, int imageHeight,
	const char *dstPath)
{
	png_bytep *row_pointers
		= (png_bytep*)malloc(sizeof(png_bytep) * imageHeight);
	int bytesWidth = 3 * imageWidth;
	for (int i = 0; i < imageHeight; i++)
		row_pointers[i] = (png_byte*)malloc(bytesWidth);

	const T ceil = std::numeric_limits<T>::max();
	const T floor = std::numeric_limits<T>::min();;
	T minVal[] = { ceil, ceil, ceil };
	T maxVal[] = { floor, floor, floor };
	int size = imageWidth * imageHeight;
	for (int i = 0; i < size; i++) {
		const Feature<K, T> &curFeature = ftrs[i];

		for (int j = 0; j < 3; j++) {
			T curVal = curFeature[loc[j]];

			if (curVal < minVal[j])
				minVal[j] = curVal;
			if (curVal > maxVal[j])
				maxVal[j] = curVal;
		}
	}
	for (int j = imageHeight - 1, index = 0; j >= 0; j--) {
		png_byte *ri = row_pointers[j];

		for (int i = 0, offset = 0; i < imageWidth; i++, index++, offset += 3){
			const Feature<K, T> &curFeature = ftrs[index];

			for (int k = 0; k < 3; k++) {
				T curVal = curFeature[loc[k]];

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


#endif // !_DEBUG_H
