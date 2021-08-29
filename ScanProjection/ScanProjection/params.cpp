#include "params.h"

#include <fstream>

using namespace std;

// global variables
int imageWidth = 227;   // the image size ���ͼƬ����ı߳�

GLfloat GridRes = 10.0f;    // the pixel size ͶӰ����ʵ�ʴ�С: 10.0f��1f,5f,15f,25f,50f������λ���ף�
GLfloat FrustumDepth = 10000.0f; // depth of the frustum
GLfloat FrustumNear = 100.0f; // the near clipping plane of the frustum
GLfloat DepthOffset = 1000.0f; //1K���ò���һ�㲻��
double fovRatio = 1.5 ; // �ﵽͶӰ����ʵ�ʴ�С���ڵ�ͶӰ��� = 1km*fovRatio

// the background values
double nanPropValue = -10.0;  // default null value for the properties ���Կ�ֵ
double nanDepthValue = FrustumDepth; // default null value for the depth ��ȿ�ֵ

// the distance scaling factors
double distanceScl = 1.0 / 500.0;

// flag for output png files
bool withPng = false;

// the paths and directionaries
char meshPath[MAX_PATH]; // path of your 3D models
char propPath[MAX_PATH]; // path of your properties in .csv file
char voxelPath[MAX_PATH]; // path of the voxels for projection in .csv file

char binDir[MAX_PATH]; // directionary for output the bin files
char pngDir[MAX_PATH]; // directionary for output the png files

