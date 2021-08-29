#ifndef _PARAMS_H
#define _PARAMS_H

#include <glad/glad.h>
#include "../Utility/primitive.h"

#define WITHOUT_NORMAL 0 // with normals or not

#define KDims 16 // dimension of properties

extern int imageWidth;

extern GLfloat GridRes;    // 10.0f£¬1f,5f,15f,25f,50f
extern GLfloat FrustumDepth; // that is, 10 km
extern GLfloat FrustumNear;
extern GLfloat DepthOffset; //1K
extern double fovRatio;

extern double nanPropValue;
extern double nanDepthValue;

extern double distanceScl;
extern bool withPng;

#ifdef _MSC_VER
#include <Windows.h>
#else // _MSC_VER
#define MAX_PATH 260
#endif // _MSC_VER

extern char meshPath[MAX_PATH];
extern char propPath[MAX_PATH];
extern char voxelPath[MAX_PATH];

extern char binDir[MAX_PATH];
extern char pngDir[MAX_PATH];

#endif // _PARAMS_H