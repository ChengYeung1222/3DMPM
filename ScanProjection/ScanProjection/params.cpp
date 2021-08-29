#include "params.h"

#include <fstream>

using namespace std;

// global variables
int imageWidth = 227;   // the image size 输出图片矩阵的边长

GLfloat GridRes = 10.0f;    // the pixel size 投影像素实际大小: 10.0f，1f,5f,15f,25f,50f，（单位：米）
GLfloat FrustumDepth = 10000.0f; // depth of the frustum
GLfloat FrustumNear = 100.0f; // the near clipping plane of the frustum
GLfloat DepthOffset = 1000.0f; //1K，该参数一般不动
double fovRatio = 1.5 ; // 达到投影像素实际大小所在的投影深度 = 1km*fovRatio

// the background values
double nanPropValue = -10.0;  // default null value for the properties 属性空值
double nanDepthValue = FrustumDepth; // default null value for the depth 深度空值

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

