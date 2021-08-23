#include "params.h"

#include <fstream>

using namespace std;

// global variables
int imageWidth = 227;   //���ͼƬ����ı߳�

GLfloat GridRes = 10.0f;    //ͶӰ����ʵ�ʴ�С: 10.0f��1f,5f,15f,25f,50f������λ���ף�
GLfloat FrustumDepth = 10000.0f; // that is, 10 km
GLfloat FrustumNear = 100.0f;
GLfloat DepthOffset = 1000.0f; //1K���ò���һ�㲻��
double fovRatio = 1.5 ; //�ﵽͶӰ����ʵ�ʴ�С���ڵ�ͶӰ��� = 1km*fovRatio

// the background values
double nanPropValue = -10.0;  // ���Կ�ֵ
double nanDepthValue = FrustumDepth; // ��ȿ�ֵ

// the distance scaling factors
double distanceScl = 1.0 / 500.0;

char meshPath[MAX_PATH];
char propPath[MAX_PATH];
char voxelPath[MAX_PATH];

char binDir[MAX_PATH];
char pngDir[MAX_PATH];

bool LoadParams(const char* path)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	const int strSize = 1024;
	char str[strSize];
	while (ifs.getline(str, strSize)) {

		if (strcmp(str, "[imageWidth]") == 0) {
			ifs.getline(str, strSize);
			imageWidth = atoi(str);
		}

		if (strcmp(str, "[GridRes]") == 0) {
			ifs.getline(str, strSize);
			GridRes = atof(str);
		}

		if (strcmp(str, "[FrustumDepth]") == 0) {
			ifs.getline(str, strSize);
			FrustumDepth = atof(str);
		}

		if (strcmp(str, "[FrustumNear]") == 0) {
			ifs.getline(str, strSize);
			FrustumNear = atof(str);
		}

		if (strcmp(str, "[DepthOffset]") == 0) {
			ifs.getline(str, strSize);
			DepthOffset = atof(str);
		}

		if (strcmp(str, "[fovRatio]") == 0) {
			ifs.getline(str, strSize);
			fovRatio = atof(str);
		}

		if (strcmp(str, "[nanPropValue]") == 0) {
			ifs.getline(str, strSize);
			nanPropValue = atof(str);
		}

		if (strcmp(str, "[nanDepthValue]") == 0) {
			ifs.getline(str, strSize);
			nanDepthValue = atof(str);
		}

		if (strcmp(str, "[distanceScl]") == 0) {
			ifs.getline(str, strSize);
			distanceScl = atof(str);
		}

		if (strcmp(str, "[FrustumNear]") == 0) {
			ifs.getline(str, strSize);
			FrustumNear = atof(str);
		}

		if (strcmp(str, "[meshPath]") == 0) {
			ifs.getline(meshPath, MAX_PATH);
		}

		if (strcmp(str, "[propPath]") == 0) {
			ifs.getline(propPath, MAX_PATH);
		}

		if (strcmp(str, "[voxelPath]") == 0) {
			ifs.getline(voxelPath, MAX_PATH);
		}

		if (strcmp(str, "[binDir]") == 0) {
			ifs.getline(binDir, MAX_PATH);
		}

		if (strcmp(str, "[pngDir]") == 0) {
			ifs.getline(pngDir, MAX_PATH);
		}

	}

	return true;
}