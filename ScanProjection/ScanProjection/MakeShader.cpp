#include "MakeShader.h"
#include <stdio.h>

#include "params.h"

bool MakeVertexShaderCode(const char *path, int nFeatures)
{
	FILE *fp;
	if ((fp = fopen(path, "w")) == NULL)
		return false;

	fprintf(fp, "#version 330 core\n");

	fprintf(fp, "layout(location = 0) in vec3 aPos;\n"); 
	bool bExtra = nFeatures % 3 != 0;
	int nAttributes = nFeatures / 3 + bExtra;
	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "layout(location = %d) in vec3 a%d;\n", i+1, i);
#if !WITHOUT_NORMAL
	fprintf(fp, "layout(location = %d) in vec3 a%d;\n", nAttributes+1, 
			nAttributes);
#endif // !WITHOUT_NORMAL

	fprintf(fp, "\n");

	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "out vec3 v%d;\n", i);
#if !WITHOUT_NORMAL
	fprintf(fp, "out vec3 v%d;\n", nAttributes);
#endif // !WITHOUT_NORMAL

	fprintf(fp, "\n");


	fprintf(fp, "uniform mat4 model;\n");
	fprintf(fp, "uniform mat4 view;\n");
	fprintf(fp, "uniform mat4 projection;\n");
	fprintf(fp, "\n");
	
	fprintf(fp, "varying float vz;\n");
	fprintf(fp, "\n");

	fprintf(fp, "void main()\n");
	fprintf(fp, "{\n");

#if 1
	fprintf(fp, "\tvec4 viewPos = "
				"view * model * vec4(aPos, 1.0);\n");
	fprintf(fp, "\tgl_Position = "
				"\tprojection * viewPos;\n");
	fprintf(fp, "\tvz = viewPos.z;\n");
#else
	fprintf(fp, "\tvec4 pos = projection * view * model * vec4(aPos, 1.0);\n");
	fprintf(fp, "\tgl_Position = "
				"vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w, 1.0);\n");
#endif
	fprintf(fp, "\n");

	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "\tv%d = a%d;\n", i, i);
#if !WITHOUT_NORMAL
	fprintf(fp, "\tv%d = a%d;\n", nAttributes, nAttributes);
#endif // !WITHOUT_NORMAL
	fprintf(fp, "}\n");

	fclose(fp);

	return true;
}

void PutLinearizeDepthFunc(FILE *fp)
{
	fprintf(fp, "uniform float near;\n");
	fprintf(fp, "uniform float far;\n");
	fprintf(fp, "\n");

	fprintf(fp, "float LinearizeDepth(float depth)\n");
	fprintf(fp, "{\n");
	fprintf(fp, "\tfloat z = depth * 2.0 - 1.0;\n");
#if 0	// for perspective projection
	fprintf(fp, "\treturn (2.0 * near * far)");
	fprintf(fp, " / (far + near - z * (far - near));\n");
#else // for ortho projection
	fprintf(fp, "\treturn (z*(far - near) + (far + near))*0.5;\n");
#endif
	fprintf(fp, "}\n");
	fprintf(fp, "\n");
}

bool MakeFragmentShaderCode(const char *path, int nFeatures)
{
	FILE *fp;
	if ((fp = fopen(path, "w")) == NULL)
		return false;

	fprintf(fp, "#version 330 core\n");
	fprintf(fp, "\n");

	int nAttributes = nFeatures / 3 + (bool) (nFeatures % 3);
	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "in vec3 v%d;\n", i);
#if !WITHOUT_NORMAL
	fprintf(fp, "in vec3 v%d;\n", nAttributes);
#endif // !WITHOUT_NORMAL

	fprintf(fp, "\n");
	
	fprintf(fp, "uniform float depthPivot;\n");

#if !WITHOUT_NORMAL
	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "layout (location = %d) out vec3 c%d;\n", i, i);
	fprintf(fp, "layout (location = %d) out vec4 endChannel;\n", nAttributes);
#else // WITHOUT_NORMAL
	for (int i = 0; i < nAttributes-1; i++)
		fprintf(fp, "layout (location = %d) out vec3 c%d;\n", i, i);
	fprintf(fp, "layout (location = %d) out vec4 c%d;\n", nAttributes-1, 
		nAttributes-1);
#endif // WITHOUT_NORMAL

	fprintf(fp, "\n");

	fprintf(fp, "varying float vz;\n");
	fprintf(fp, "\n");

	fprintf(fp, "void main()\n");
	fprintf(fp, "{\n");

	fprintf(fp, "\tfloat depth = -vz;\n");
	fprintf(fp, "\tfloat distance = depth - depthPivot;\n");
	fprintf(fp, "\tif (gl_FrontFacing)\n");
	fprintf(fp, "\t\tdistance = -distance;\n");

#if !WITHOUT_NORMAL
	for (int i = 0; i < nAttributes; i++)
		fprintf(fp, "\tc%d = v%d;\n", i, i);
	fprintf(fp, "\tendChannel = vec4(vec3(normalize(v%d)), distance);\n",
		nAttributes);
	fprintf(fp, "\n");
#else // WITHOUT_NORMAL
	for (int i = 0; i < nAttributes-1; i++)
		fprintf(fp, "\tc%d = v%d;\n", i, i);
	fprintf(fp, "\tc%d = vec4(vec3(v%d), distance);\n", nAttributes-1, nAttributes-1);
	fprintf(fp, "\n");
#endif // WITHOUT_NORMAL

	fprintf(fp, "}\n");

	fclose(fp);

	return true;
}