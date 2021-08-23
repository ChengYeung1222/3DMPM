
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <string.h>
#include <iomanip>

#include "FileConvert.h"
#include "../Utility/Feature.h"
#include "../MeshUtility/MeshUtility.h"
#include "MakeShader.h"
#include "params.h"
#include "CalcFocus.h"
#include "DebugUtility.h"

//#include <learnopengl/model.h>
using namespace std;


// global variables
const double FieldOfView = 2.0 * atan(imageWidth*GridRes*0.5 / (DepthOffset*fovRatio));
glm::mat4 matProjection;

const int MaxNumDrawBuffers = 8;
const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0,
							   GL_COLOR_ATTACHMENT1,
							   GL_COLOR_ATTACHMENT2,
					   		   GL_COLOR_ATTACHMENT3,
							   GL_COLOR_ATTACHMENT4,
							   GL_COLOR_ATTACHMENT5,
							   GL_COLOR_ATTACHMENT6,
							   GL_COLOR_ATTACHMENT7,
							  };
GLuint texCanvas[MaxNumDrawBuffers];
int nTexCanvas = 0;
GLuint VAO;
GLuint VBO;
GLuint EBO;
GLuint DepthBuff;
GLuint FrameBuff;
Shader *pShader = NULL;
Shader *pShaderSimple = NULL;
DPoint3 origin(DBL_MAX, DBL_MAX, DBL_MAX);
GLFWwindow* pWindow = NULL;
int locDistBuffer;
double signedNanDepthVal;

#define toVec3d(d) ( *(Vec3d *)&(d) )

void glErrorCheck()
{
	int errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		printf("Failure in OpenGL %d\n", errCode);
		//	exit(0);
	}
}


template<int K>
void SetupShader()
{
	// build and compile shaders
	// -------------------------
	const char *vertShaderPath = "vtshader";
	const char *fragShaderPath = "fgshader";

	MakeVertexShaderCode(vertShaderPath, K);
	MakeFragmentShaderCode(fragShaderPath, K);

	pShader = new Shader(vertShaderPath, fragShaderPath);
	pShader->use();

	// setup the frustum
	const GLfloat res = GridRes;
	GLfloat halfExtent = imageWidth * res * 0.5f;
	GLfloat depth = FrustumDepth;
	GLfloat zNear = FrustumNear;
	GLfloat zFar = zNear + depth;
	pShader->setFloat("near", zNear);
	pShader->setFloat("far", zFar);
	pShader->setFloat("depthPivot", DepthOffset);
	glm::mat4 proj = glm::perspective((float)FieldOfView, 1.0f, zNear, zFar);

	pShader->setMat4("projection", proj);
	glm::mat4 model;
	pShader->setMat4("model", model);

	// the simple shader
	char vertShaderSimplePath[MAX_PATH];
	char fragShaderSimplePath[MAX_PATH];
	sprintf(vertShaderSimplePath, "%s_simple", vertShaderPath);
	sprintf(fragShaderSimplePath, "%s_simple", fragShaderPath);
	pShaderSimple = new Shader(vertShaderSimplePath, fragShaderSimplePath);
	pShaderSimple->use();

	pShaderSimple->setMat4("projection", proj);
	pShaderSimple->setMat4("model", model);
	pShaderSimple->setFloat("depthPivot", DepthOffset);
	pShaderSimple->setFloat("winWidth", imageWidth);
	pShaderSimple->setFloat("winHeight", imageWidth);
	pShaderSimple->setFloat("frustumNear", FrustumNear);
	pShaderSimple->setFloat("halfFrustumWidth", zNear * tan(FieldOfView * 0.5));
	pShader->use();
}

template <int K>
int SetupGL()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	pWindow = glfwCreateWindow(imageWidth, imageWidth, "ScanProjection", NULL, NULL);
	if (pWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(pWindow);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// gen renderbuffer
	glGenRenderbuffers(1, &DepthBuff);
	// initialize depth renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, DepthBuff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, imageWidth,
		imageWidth);

	int nFBOBuffers = K / 3 + (bool)(K % 3) + 1;
	if (nFBOBuffers > MaxNumDrawBuffers) {
		std::cout << "Number of FBO buffers exceeds"
			"the maximum number of supported buffers" << std::endl;
		return -1;
	}

	// make a texture
	nTexCanvas = nFBOBuffers;
	glGenTextures(nTexCanvas, texCanvas);
	glErrorCheck();

	// initialize texture that will store the framebuffer image (BGRA type)
	for (int i = 0; i < nFBOBuffers - 1; i++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, texCanvas[i]);

		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
			GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
			GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glErrorCheck();

		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, imageWidth,
			imageWidth, 0, GL_RGB, GL_FLOAT, NULL); // all of textures will use rgba mode
		glErrorCheck();

	}
	// initialize texture that will store the framebuffer image (single color type)
	glBindTexture(GL_TEXTURE_RECTANGLE, texCanvas[nFBOBuffers - 1]);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
#if WITHOUT_NORMAL
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R32F, imageWidth,
		imageWidth, 0, GL_RED, GL_FLOAT, NULL);
#else // WITHOUT_NORMAL
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F, imageWidth, imageWidth,
		0, GL_RGBA, GL_FLOAT, NULL);
#endif // WITHOUT_NORMAL
	glErrorCheck();

	// gen the framebuffer object
	glGenFramebuffers(1, &FrameBuff);
	glErrorCheck();

	// bind the framebuffer, FrameBuff, so operations will now occur on it
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuff);
	glErrorCheck();

	// bind this texture to the current framebuffer obj. as color_attachment_0
	for (int i = 0; i < nFBOBuffers; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[i],
			GL_TEXTURE_RECTANGLE,
			texCanvas[i], 0);
		glErrorCheck();
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, DepthBuff);
	glErrorCheck();

	//check framebuffer status
	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		printf("Failure in OpenGL Framebuffer %d", status);
		return 0;
	}

	glDrawBuffers(nFBOBuffers, drawBuffers);

	// configure OpenGL global state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	SetupShader<K>();

	return 0;
}

void GetExtent(const Mesh &mesh)
{
	int nVertices = mesh.GetNumVertices();
	int nTriangles = mesh.GetNumTriangles();
	const unsigned int *indices = (const unsigned int *)&mesh.GetTriangleAt(0);
	const DPoint3 *vertices = &mesh.GetVertexAt(0);

	for (int i = 0; i < nVertices; i++) {
		const DPoint3 &curVert = vertices[i];
		if (curVert.x < origin.x)
			origin.x = curVert.x;
		if (curVert.y < origin.y)
			origin.y = curVert.y;
		if (curVert.z < origin.z)
			origin.z = curVert.z;
	}
}


#if !WITHOUT_NORMAL
void CalcMeshNormal(const Mesh &mesh, FPoint3 *normals)
{
	int nVertices = mesh.GetNumVertices();
	const DPoint3 *vertices = &mesh.GetVertexAt(0);
	int nTriangles = mesh.GetNumTriangles();
	const Mesh::TRGL *triangles = &mesh.GetTriangleAt(0);
	
	DPoint3 *normalBuff = new DPoint3[nVertices];
	memset(normalBuff, 0, sizeof(DPoint3)*nVertices);
	double *area = new double[nVertices];
	memset(area, 0, sizeof(double)*nVertices);

	for (int i = 0; i < nTriangles; i++) {
		const unsigned int *ind = triangles[i].ind;

		const DPoint3 &pi = vertices[ind[0]];
		const DPoint3 &pj = vertices[ind[1]];
		const DPoint3 &pk = vertices[ind[2]];

		DPoint3 di = pj - pi;
		DPoint3 dj = pk - pi;

		DPoint3 weightedNormal = di.Cross(dj);
		double curArea = weightedNormal.Length();

		normalBuff[ind[0]] += weightedNormal;
		normalBuff[ind[1]] += weightedNormal;
		normalBuff[ind[2]] += weightedNormal;
		area[ind[0]] += curArea;
		area[ind[1]] += curArea;
		area[ind[2]] += curArea;
	}

	for (int i = 0; i < nVertices; i++)
		normalBuff[i] /= area[i];

	for (int i = 0; i < nVertices; i++) {
		normals[i] = normalBuff[i];
		normals[i].Normalize();
	}

	delete[] area;
}
#endif // !WITHOUT_NORMAL

template <int K>
void NormalizeFeatures(Feature<K, float>* ftrs, int nVertices)
{
	// calculate the mean
	Feature<K, double> mean;
	for (int i = 0; i < K; i++)
		mean[i] = 0.0;
	for (int i = 0; i < nVertices; i++) {
		const Feature<K, float>& curFtr = ftrs[i];

		for (int j = 0; j < K; j++) {
			mean[j] += curFtr[j];
		}
	}
	for (int i = 0; i < K; i++)
		mean[i] /= nVertices;

	// calculate the standard deviation
	Feature<K, double> stddev;
	for (int i = 0; i < K; i++)
		stddev[i] = 0.0;
	for (int i = 0; i < nVertices; i++) {
		const Feature<K, float>& curFtr = ftrs[i];

		for (int j = 0; j < K; j++) {
			double diff = curFtr[j] - mean[j];
			stddev[j] += diff * diff;
		}
	}
	for (int i = 0; i < K; i++)
		stddev[i] = sqrt(stddev[i] / nVertices);

	// z-score normalization
	for (int i = 0; i < nVertices; i++) {
		Feature<K, float>& curFtr = ftrs[i];

		for (int j = 0; j < K; j++) {
			curFtr[j] = (curFtr[j] - mean[j]) / stddev[j];
		}
	}
}

template <int K>
void SetupMesh(const Mesh &mesh, Feature<K, float> *ftrs)
{
	int nVertices = mesh.GetNumVertices();
	const DPoint3 *vertices = &mesh.GetVertexAt(0);
	int nTriangles = mesh.GetNumTriangles();
	const unsigned int *indices = (const unsigned int *)&mesh.GetTriangleAt(0);

	for (int i = 0; i < nVertices; i++) {
		const DPoint3 &curVert = vertices[i];
		if (curVert.x < origin.x)
			origin.x = curVert.x;
		if (curVert.y < origin.y)
			origin.y = curVert.y;
		if (curVert.z < origin.z)
			origin.z = curVert.z;
	}

	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	// setup the mesh for scan conversion features
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	int bytesIndices = sizeof(unsigned int)*nTriangles * 3;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesIndices, indices, 
				 GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int bytesVertices = sizeof(FPoint3) * nVertices;
	bool bExtra = K % 3 != 0;
	int bytesFeatures = (K / 3 + (int)bExtra)*sizeof(FPoint3) * nVertices;
	int bytesNormals = bytesVertices;

#if WITHOUT_NORMAL
	glBufferData(GL_ARRAY_BUFFER, bytesVertices + bytesFeatures, NULL,
		GL_STATIC_DRAW);
#else
	glBufferData(GL_ARRAY_BUFFER, bytesVertices+bytesFeatures+bytesNormals,
				 NULL, GL_STATIC_DRAW);
#endif // WITHOUT_NORMAL

	// upload vertex positions
	FPoint3 *fVertexBuff = new FPoint3[nVertices];
	for (int i = 0; i < nVertices; i++) {
		const DPoint3 &curVertex = vertices[i];
		fVertexBuff[i] = curVertex - origin;
	}
	glBufferSubData(GL_ARRAY_BUFFER, 0, bytesVertices, fVertexBuff);

	// normalize features
	NormalizeFeatures(ftrs, nVertices);

	// upload the features
	glBufferSubData(GL_ARRAY_BUFFER, bytesVertices,
		sizeof(Feature<K, float>)*nVertices, ftrs);

#if !WITHOUT_NORMAL
	// upload the normals
	FPoint3 *normals = new FPoint3[nVertices];
	CalcMeshNormal(mesh, normals);
	glBufferSubData(GL_ARRAY_BUFFER, bytesVertices + bytesFeatures, bytesNormals,
					normals);
	delete[] normals;
#endif // !WITHOUT_NORMAL

	// set vertex pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FPoint3), (void*)0);
	int stride = sizeof(Fields<K, float>);
	int n = K / 3;
	for (int i = 0; i < n; i++) {
		glEnableVertexAttribArray(i + 1);
		glVertexAttribPointer(i + 1, 3, GL_FLOAT, GL_FALSE, stride,
			(void *)(bytesVertices + sizeof(float)*i * 3));
	}

	if (bExtra) {
		glEnableVertexAttribArray(n + 1);
		glVertexAttribPointer(n + 1, 3, GL_FLOAT, GL_FALSE, stride,
			(void *)(bytesVertices + sizeof(float)*n * 3));
	}

	int attribLoc = n + 1 + bExtra;
#if !WITHOUT_NORMAL
	glEnableVertexAttribArray(attribLoc);
	glVertexAttribPointer(attribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(FPoint3),
		(void *)(bytesVertices + bytesFeatures));
	attribLoc++;
#endif // !WITHOUT_NORMAL

	delete[] fVertexBuff;
}

void RenderMesh(const Mesh &mesh, bool withSpecInitVal = true)
{
	if (withSpecInitVal)
		glClearColor(nanPropValue, nanPropValue, nanPropValue, 
			signedNanDepthVal);
	else
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int nIndices = mesh.GetNumTriangles() * 3;
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
//	glfwSwapBuffers(pWindow);
}

template <int K>
void FillFeatrues(float *buff, int stride, int nAttrib, int offset, 
				  Feature<K, float> *features)
{
	int size = imageWidth * imageWidth;

	for (int i = 0, loc = 0; i < size; i++, loc += stride) {
		Feature<K, float> &curFtr = features[i];
		float *src = buff + loc;

		for (int k = 0; k < nAttrib; k++)
			curFtr[offset+k] = src[k];
	}
}

#if WITHOUT_NORMAL
template <int K>
void ScanFeatures3(const Mesh &mesh, float *buff, Feature<K+1, float> *features)
{
	RenderMesh(mesh);

	if (K % 3 != 0) {
		// use 3 channels to obtain the scan conversion results
		int n = K / 3;
		for (int i = 0; i < n; i++) {
			glReadBuffer(drawBuffers[i]);
			glReadPixels(0, 0, imageWidth, imageWidth, GL_RGB, GL_FLOAT, buff);

			FillFeatrues(buff, 3, 3, i*3, features);
		}

		// the remaining attributes
		int r = K - 3*n;
		glReadBuffer(drawBuffers[n]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RGBA, GL_FLOAT, buff);
		FillFeatrues(buff, 4, r, n*3, features);
		FillFeatrues(buff+3, 4, 1, K, features);
	} else {
		int n = K / 3;
		for (int i = 0; i < n-1; i++) {
			glReadBuffer(drawBuffers[i]);
			glReadPixels(0, 0, imageWidth, imageWidth, GL_RGB, GL_FLOAT, buff);

			FillFeatrues(buff, 3, 3, i*3, features);
		}
		glReadBuffer(drawBuffers[n-1]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RGBA, GL_FLOAT, buff);
		FillFeatrues(buff, 4, 4, (n-1)*3, features);
	}


}
#else // WITHOUT_NORMAL
template <int K>
void ScanFeatures3(const Mesh &mesh, float *buff, Feature<K+4, float> *features)
{
	RenderMesh(mesh);

	// use 3 channels to obtain the scan conversion results
	int n = K / 3;
	for (int i = 0; i < n; i++) {
		glReadBuffer(drawBuffers[i]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RGB, GL_FLOAT, buff);

		FillFeatrues(buff, 3, 3, i*3, features);
	}

	// the remaining attributes
	int r = K - 3*n;
	int di = n;
	if (r > 0) {
		glReadBuffer(drawBuffers[n]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RGB, GL_FLOAT, buff);
		FillFeatrues(buff, 3, r, n*3, features);
		di++;
	}

	// the depth attributes
	glReadBuffer(drawBuffers[di]);
	glReadPixels(0, 0, imageWidth, imageWidth, GL_RGBA, GL_FLOAT, buff);
//SaveFltBmpSigleChannel("dist.bmp", buff, imageWidth, imageWidth, 24);
	FillFeatrues(buff, 4, 4, n*3+r, features);
}
#endif // WITHOUT_NORMAL

template <int K>
#if WITHOUT_NORMAL

void NormalizeDistance(Feature<K+1, float> *features, int size)
{
	for (int i = 0; i < size; i++)
		features[i][K] *= distanceScl;
}
#else // WITHOUT_NORMAL
void NormalizeDistance(Feature<K+4, float> *features, int size)
{
	for (int i = 0; i < size; i++)
		features[i][K+3] *= distanceScl;
}
#endif // WITHOUT_NORMAL



void set_view(const DPoint3& src, DPoint3& dst, glm::mat4& view,
	DPoint3& viewpoint)
{
	DPoint3 dir = dst - src;
	dir.Normalize();

	bool bCoincide
		= fabs(fabs(dir.z) - 1.0) > numeric_limits<double>::epsilon();
	DPoint3 up = DPoint3(0.0, 0.0, 1.0);
	viewpoint = src - dir * DepthOffset;
	view = glm::lookAt(glm::vec3(viewpoint.x, viewpoint.y, viewpoint.z),
		glm::vec3(dst.x, dst.y, dst.z),
		glm::vec3(up.x, up.y, up.z));

}

void SetDistBuff(const DPoint3& src, const Mesh& mesh, float* buff)
{
	// calculate the distance to the mesh vertices
	int nVertices = mesh.GetNumVertices();
	for (int i = 0; i < nVertices; i++) {
		const DPoint3& curVert = mesh.GetVertexAt(i) - origin;
		DPoint3 diff = curVert - src;
		buff[i] = diff.Length();
	}
	glBufferSubData(GL_ARRAY_BUFFER, locDistBuffer, sizeof(float) * nVertices,
		buff);
}

void calc_centroid(float* buff, int width, DPoint2& centroid)
{
	assert(width % 2 != 0);
	int halfWidth = width >> 1;

	DPoint2 mean(0.0, 0.0);
	double deno = 0.0;

	int offset = 0;
	for (int j = -halfWidth; j <= halfWidth; j++) {
		for (int i = -halfWidth; i <= halfWidth; i++) {
			float curVal = buff[offset];
			double curWt = curVal != 0.0 ? 1.0 / curVal : 0.0;
			mean += DPoint2(i, j) * curWt;
			deno += curWt;

			offset++;
		}
	}

	mean /= deno * halfWidth;

	centroid = mean;
}

void AdjustFocus(const DPoint3& focus, const DPoint3& lastFocus,
	const DPoint3& src, DPoint3& adjustedFocus, double tol)
{
	const double degRad = PI / 180.0;
	double thresh = tol * degRad;

	DPoint3 d1 = lastFocus - src;
	glm::dvec4 da(d1.x, d1.y, d1.z, 1.0);
	d1.Normalize();
	DPoint3 d2 = focus - src;
	d2.Normalize();

	double dotProd = d1.Dot(d2);
	double angle = acos(dotProd);

	if (angle > thresh) {
		double adjustedAngle = thresh;

		DPoint3 cross = d1.Cross(d2);

		glm::highp_dmat4 M;
		glm::dvec3 axis(cross.x, cross.y, cross.z);
		glm::highp_dmat4 R = glm::rotate(M, adjustedAngle, axis);

		glm::dvec4 dir = R * da;

		adjustedFocus = src + DPoint3(dir.x, dir.y, dir.z);
	}
	else {
		adjustedFocus = focus;
	}
}

void ImageCent2Focus(const DPoint3& viewPos, const glm::mat4& view,
					float* depthBuff, const DPoint2& centroid,
					DPoint3& focus)
{
	// search for the depth value at the centroid
	DPoint2 imageCoord = (centroid - DPoint2(-1.0, -1.0)) * imageWidth / 2.0;
	int winX = (int)imageCoord.x;
	int winY = (int)imageCoord.y;
	int imageIndex = winY * imageWidth + winX;
	float d00 = depthBuff[imageIndex] + DepthOffset;
	float d01 = depthBuff[imageIndex + imageWidth] + DepthOffset;
	float d10 = depthBuff[imageIndex + 1] + DepthOffset;
	float d11 = depthBuff[imageIndex + imageWidth + 1] + DepthOffset;
	DPoint2 interloc(imageCoord.x - winX, imageCoord.y - winY);
	//double depthValue = BiLinearInterp(interloc, d00, d01, d10, d11);
	double depthValue = depthBuff[imageIndex] + DepthOffset;

	// convert the mean point into the viewing space
	double ratio = tan(FieldOfView * 0.5) * depthValue;
	DPoint2 viewCentroid = centroid * ratio;
	DPoint3 viewDiff(viewCentroid.x, viewCentroid.y, -depthValue);

	DPoint3 nx(view[0][0], view[1][0], view[2][0]);
	DPoint3 ny(view[0][1], view[1][1], view[2][1]);
	DPoint3 nz(view[0][2], view[1][2], view[2][2]);

	DPoint3 worldDiff = nx * viewDiff.x + ny * viewDiff.y + nz * viewDiff.z;
	focus = viewPos + worldDiff;

}

double get_target(const DPoint3& src, DPoint3& dst, void** params)
{
	const int iMaxSteps = 32;
	const double tol = 4.0;

	const FocusSpatialInfo& focusInfo
		= *reinterpret_cast<FocusSpatialInfo*>(params[0]);
	const Mesh& mesh = *reinterpret_cast<Mesh*>(params[1]);
	float* buff = reinterpret_cast<float*>(params[2]);

	DPoint3 cur = src + origin;
	DPoint3 target;

	pShaderSimple->use();

	// guess the initial focus
	double dist = focusInfo.ClosestPoint(cur, target);
	DPoint3 focus = target - origin;

	DPoint3 lastFocus = focus;
	DPoint3 adjustedFocus;

	// search for the optimal focus
	const double thresh = 1.0e-3 * 1.0e-3;
	glm::mat4 view;
	int iStep = 0;
	double etaDecay = 0.1;
	while (true) {
		DPoint3 viewPos;
		set_view(src, focus, view, viewPos);

		RenderMesh(mesh, false);

		// depth acquirement
		int layerSize = imageWidth * imageWidth;
		float* zBuff = buff + layerSize;
		glReadBuffer(drawBuffers[0]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RED, GL_FLOAT, zBuff);

		float* rBuff = buff;
		glReadBuffer(drawBuffers[1]);
		glReadPixels(0, 0, imageWidth, imageWidth, GL_RED, GL_FLOAT, rBuff);


		DPoint2 centroid;
		calc_centroid(rBuff, imageWidth, centroid);
		double eta = exp(-etaDecay * iStep);
		centroid *= eta;

		// stop the iterations if the centroid converges
		if (centroid.LengthSquared() < thresh)
			break;

		if (iStep == iMaxSteps)
			break;

		// calculate the world coordinate of the focus
		ImageCent2Focus(viewPos, view, zBuff, centroid, focus);

		AdjustFocus(focus, lastFocus, src, adjustedFocus, tol);
		lastFocus = adjustedFocus;
		focus = adjustedFocus;

		iStep++;
	}

	pShader->use();

	dst = focus;
	return (dst - src).Length();
}

template <int K>
void project(const Mesh &mesh, const vector<DPoint3> &voxels, 
			 double (*minDist)(const DPoint3 &, DPoint3 &, void **), 
			 void **params)
{
	char fileName[MAX_PATH];
	int visLoc[] = { 1, 2, 3 };

	const int layerSize = imageWidth * imageWidth;
#if WITHOUT_NORMAL
	Feature<K+1, float> *features 
		= new Feature<K+1, float>[layerSize];
#else
	Feature<K+4, float> *features
		= new Feature<K+4, float>[layerSize];
#endif
	int buffSize = layerSize * 4;
	float *buff = new float[buffSize];

	int nVoxels = voxels.size();
	int nIncre = nVoxels / 100;
	int iSteps = nIncre;
	int percent = 0;
	printf("start projection...");
	for (int i = 0; i < nVoxels; i++) {
		if (iSteps++ == nIncre) {
			iSteps = 0;
			printf("\b\b\b%3d", percent++);
		}

		const DPoint3 &curVoxel = voxels[i];

		DPoint3 closest;
		double len = minDist(curVoxel, closest, params);
		DPoint3 dir = closest - curVoxel;
		dir /= len;

		glm::mat4 view;
		if (fabs(len) < numeric_limits<double>::epsilon())
			continue;

		DPoint3 up = DPoint3(0.0, 0.0, 1.0); 
		DPoint3 viewpoint = curVoxel - dir * DepthOffset;
		view = glm::lookAt(glm::vec3(viewpoint.x, viewpoint.y, viewpoint.z),
							glm::vec3(closest.x, closest.y, closest.z),
							glm::vec3(up.x, up.y, up.z));

		pShader->use();
		pShader->setMat4("view", view);

		ScanFeatures3<K>(mesh, buff, features);
		NormalizeDistance<K>(features, layerSize);

		// output binary file
		sprintf(fileName, "%s%d.bin", binDir, i);
		ofstream output(fileName, ios::out | ios::binary);
#if WITHOUT_NORMAL
		output.write((char*)features, sizeof(Feature<K+1, float>)*layerSize);
#else // WITHOUT_NORMAL
		output.write((char*)features, sizeof(Feature<K+4, float>)*layerSize);
#endif // WITHOUT_NORMAL
		output.close();

		// out put png
		sprintf(fileName, "%s%d.png", pngDir, i);
		VisInPng(features, visLoc, imageWidth, imageWidth, fileName);
	}
	printf("\b\b\bdone.\n");

	delete[] buff;
	delete[] features;
}


void DestroyGL()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteTextures(nTexCanvas, texCanvas);
	glDeleteRenderbuffers(1, &DepthBuff);
	glDeleteFramebuffers(1, &FrameBuff);

	if (pShader != NULL)
		delete pShader;
	if (pShaderSimple != NULL)
		delete pShaderSimple;

	glfwTerminate();
}


template<int K>
void ReadFiles(Mesh &mesh,  vector<Fields<K, float>> &ori_pro,
			   vector<DPoint3> &known_p)
{
	ifstream ifs(meshPath);
	if (!ifs.is_open())
		return;
	// read mesh
	ifs >> mesh;
	ifs.close();
	GetExtent(mesh);

	// firstly we read the properties
	readPro(propPath, ori_pro);
	cout << "properties loaded" << endl;

	// read the voxels to generate the images
	readP(voxelPath, known_p);
	int nKnownVoxels = known_p.size();
	for (int i = 0; i < nKnownVoxels; i++)
		known_p[i] -= origin;
	cout << "voxels loaded" << endl;
}


void proc_with_gl()
{
	Mesh mesh;
	vector<Fields<KDims, float>> ori_pro; //original data
	vector<DPoint3> known_p;
	ReadFiles(mesh, ori_pro, known_p);

	// build the kd-tree for searching for closest voxels
	MeshInfo meshinfo(mesh);
	meshinfo.Extract();

	FocusSpatialInfo focusinfo;
	focusinfo.Make(&meshinfo, 800.0, 200.0);

	SetupGL<KDims>();
	SetupMesh(mesh, &ori_pro[0]);
		
	float *buff = new float[imageWidth*imageWidth*3];
	void *params[] = { &focusinfo, &mesh, buff };
	project<KDims>(mesh, known_p, get_target, params);
	delete[] buff;

	DestroyGL();
}

int main()
{
	proc_with_gl();

	return 0;
}