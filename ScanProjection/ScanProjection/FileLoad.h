#ifndef _FILE_LOAD_H
#define _FILE_LOAD_H

#include <vector>
#include <fstream>

#include "../Utility/primitive.h"
#include "../Utility/Feature.h"
#include "params.h"

using namespace std;

int getWords(char* str, char* words[], const char delim)
{
	int count = 0;
	char* pch = str;

	while (true) {
		words[count] = pch;
		while (*pch != delim && *pch != '\0')
			pch++;

		if (*pch == '\0')
			break;

		*pch = '\0';
		pch++;
		count++;
	}

	return ++count;
}

bool LoadParams(const char* path)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	const int strSize = 4096;
	char str[strSize];
	const int maxNumWords = 1024;
	char* words[maxNumWords];

	while (ifs.getline(str, strSize)) {
		getWords(str, words, ' ');

		if (strcmp(words[0], "[imageWidth]") == 0) {
			ifs.getline(str, strSize);
			imageWidth = atoi(str);
		}

		if (strcmp(words[0], "[GridRes]") == 0) {
			ifs.getline(str, strSize);
			GridRes = atof(str);
		}

		if (strcmp(words[0], "[FrustumDepth]") == 0) {
			ifs.getline(str, strSize);
			FrustumDepth = atof(str);
		}

		if (strcmp(words[0], "[FrustumNear]") == 0) {
			ifs.getline(str, strSize);
			FrustumNear = atof(str);
		}

		if (strcmp(words[0], "[DepthOffset]") == 0) {
			ifs.getline(str, strSize);
			DepthOffset = atof(str);
		}

		if (strcmp(words[0], "[fovRatio]") == 0) {
			ifs.getline(str, strSize);
			fovRatio = atof(str);
		}

		if (strcmp(words[0], "[nanPropValue]") == 0) {
			ifs.getline(str, strSize);
			nanPropValue = atof(str);
		}

		if (strcmp(words[0], "[nanDepthValue]") == 0) {
			ifs.getline(str, strSize);
			nanDepthValue = atof(str);
		}

		if (strcmp(words[0], "[distanceScl]") == 0) {
			ifs.getline(str, strSize);
			distanceScl = atof(str);
		}

		if (strcmp(words[0], "[FrustumNear]") == 0) {
			ifs.getline(str, strSize);
			FrustumNear = atof(str);
		}

		if (strcmp(words[0], "[withPng]") == 0) {
			ifs.getline(str, strSize);
			withPng = atoi(str);
		}

		if (strcmp(words[0], "[meshPath]") == 0) {
			ifs.getline(meshPath, MAX_PATH);
		}

		if (strcmp(words[0], "[propPath]") == 0) {
			ifs.getline(propPath, MAX_PATH);
		}

		if (strcmp(words[0], "[voxelPath]") == 0) {
			ifs.getline(voxelPath, MAX_PATH);
		}

		if (strcmp(words[0], "[binDir]") == 0) {
			ifs.getline(binDir, MAX_PATH);
		}

		if (strcmp(words[0], "[pngDir]") == 0) {
			ifs.getline(pngDir, MAX_PATH);
		}

	}

	return true;
}

template <int K, class T>
bool readPro(const char* path, vector<Feature<K, T>>& pro)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	int count = 0;
	int buffSize = 1024;
	char* buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;

	pro.reserve(count);

	ifs.close();
	ifs.clear();

	ifs.open(path);

	char* words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		Feature<K, T> ftr;
		for (int k = 0; k < K; k++)
			ftr[k] = atof(words[k]);

		pro.push_back(ftr);
	}
	ifs.close();

	return true;
}


// Hao's version
bool readVox(const char* path, vector<DPoint3>& voxel)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	string strline;
	int count = 0;

	int buffSize = 1024;
	char* buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;

	ifs.close();
	ifs.clear();

	ifs.open(path);

	voxel.reserve(count);

	char* words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		double xx = atof(words[0]);
		double yy = atof(words[1]);
		double zz = atof(words[2]);
		voxel.push_back(DPoint3(xx, yy, zz));
	}
	ifs.close();

	return true;
}


#endif // _FILE_LOAD_H