#define MESH_UTILITY_EXPORTS

#include <iostream>
#include <vector>

#include "Mesh.h"

#define BLOCK_SIZE	1024*1024

Mesh::Mesh(void)
{
	m_vertices = NULL;
	m_triangles = NULL;
	m_patchBound = NULL;
	m_name[0] = '\0';

	m_nVertices = 0;
	m_nTriangles = 0;
	m_nPatches = 0;
}

Mesh::~Mesh(void)
{
	if (m_vertices != NULL)
		delete[] m_vertices;
	if (m_triangles != NULL)
		delete[] m_triangles;
	if (m_patchBound != NULL)
		delete[] m_patchBound;
	m_name[0] = '\0';
}

Mesh::Mesh(const Mesh &mesh)
{
	m_nVertices = mesh.m_nVertices;
	m_vertices = new DPoint3[m_nVertices];
	memcpy(m_vertices, mesh.m_vertices, sizeof(DPoint3)*m_nVertices);
	
	m_nTriangles = mesh.m_nTriangles;
	m_triangles = new TRGL[m_nTriangles];
	memcpy(m_triangles, mesh.m_triangles, sizeof(TRGL)*m_nTriangles);

	m_nPatches = mesh.m_nPatches;
	if (m_nPatches != 0) {
		m_patchBound = new pair<int, int>[m_nPatches+1];
		memcpy(m_patchBound, mesh.m_patchBound, 
				sizeof(pair<int, int>)*(m_nPatches+1));
	} else
		m_patchBound = NULL;

	strcpy(m_name, mesh.m_name);
}

void Mesh::SetNumPatches(int nPatches)
{
	pair<int, int> *addr = new pair<int, int>[nPatches+1];
	
	if (m_patchBound != NULL) {
		memcpy(addr, m_patchBound, sizeof(pair<int, int>) * (m_nPatches+1));
		delete[] m_patchBound;
	}

	m_patchBound = addr;
}

void Mesh::BeginPatch()
{
	m_patchBound[m_nPatches] = pair<int, int>(m_nVertices, m_nTriangles);
}

void Mesh::EndPatch()
{
	m_patchBound[m_nPatches+1] = pair<int, int>(m_nVertices, m_nTriangles);
	m_nPatches++;
}

void Mesh::Clear()
{
	if (m_vertices != NULL) {
		delete[] m_vertices;
		m_vertices = NULL;
	}
	if (m_triangles != NULL) {
		delete[] m_triangles;
		m_triangles = NULL;
	}
	if (m_patchBound != NULL) {
		delete[] m_patchBound;
		m_patchBound = NULL;
	}

	m_nVertices = 0;
	m_nTriangles = 0;
	m_nPatches = 0;
	memset(m_name, 0, MESH_NAME_SIZE);
}

void Mesh::SetVertices(const DPoint3 *vertices, int nVertices)
{
	if (m_vertices != NULL)
		delete[] m_vertices;

	m_vertices = new DPoint3[nVertices];
	memcpy(m_vertices, vertices, sizeof(DPoint3)*nVertices);
	m_nVertices = nVertices;
}

void Mesh::SetTriangles(const TRGL *triangles, int nTriangles)
{
	if (m_triangles != NULL)
		delete[] m_triangles;

	m_triangles = new TRGL[nTriangles];
	//memcpy(m_triangles, triangles, sizeof(TRGL)*nTriangles);
	unsigned int *src = (unsigned int *) triangles;
	unsigned int *dst = (unsigned int *) m_triangles;
	int size = nTriangles * 3;
	for (int i = 0; i < size; i++)
		dst[i] = src[i] - 1;

	m_nTriangles = nTriangles;
}

void Mesh::SetPatchBound(pair<int, int> *bound, int nPatches)
{
	if (m_patchBound != NULL)
		delete[] m_patchBound;

	m_patchBound = new pair<int, int>[nPatches+1];
	memcpy(m_patchBound, bound, sizeof(pair<int, int>)*(nPatches+1));
	m_nPatches = nPatches;
}

void Mesh::WriteAsDatamineCsv(const char *ptPath, const char *trPath) const
{
	FILE *fpPt = fopen(ptPath, "w");
	fprintf(fpPt, "XP,YP,ZP,PID\n");
	for (int i = 0; i < m_nVertices; i++) {
		const DPoint3 &vertex = m_vertices[i];
		fprintf(fpPt, "%lf,%lf,%lf,%d\n", vertex.x, vertex.y, vertex.z, i+1);
	}
	fclose(fpPt);

	FILE *fpTr = fopen(trPath, "w");
	fprintf(fpTr, "PID1,PID2,PID3,TRIANGLE,COLOUR,SOURCE\n");
	for (int i = 0; i < m_nTriangles; i++) {
		const unsigned int *trg = m_triangles[i].ind;
		fprintf(fpTr, "%d,%d,%d,%d,1,%s\n", 
				trg[0], trg[1], trg[2], i+1, m_name);
	}
	fclose(fpTr);
}

int get_words(char *str, char *words[], const char delim = ' ');

ifstream& operator >> (ifstream &ifs, Mesh &mesh)
{
	const int BuffSize = 512;
	char buff[BuffSize];

	char *words[32];
	int nWords = 0;

	// the tag
	ifs.getline(buff, BuffSize);
	nWords = get_words(buff, words);
	if (nWords == 0 || strcmp(words[1], "TSurf") != 0) {
		cout << "The input file is not a triangle mesh." << endl;
		return ifs;
	}

	// the header
	ifs.getline(buff, BuffSize);
	nWords = get_words(buff, words);
	if (nWords == 0 || strcmp(words[0], "HEADER") != 0) {
		cout << "The file format error." << endl;
		return ifs;
	}

	// get name
	while (ifs.getline(buff, BuffSize)) {
		nWords = get_words(buff, words, ':');

		if (strcmp(words[0], "}") == 0)
			break;

		if (strcmp(words[0], "name") == 0)
			mesh.SetName(words[1]);
	}

	// start 'TFACE'
	while (ifs.getline(buff, BuffSize)) {
		nWords = get_words(buff, words);

		if (nWords == 0 || strcmp(words[0], "TFACE") == 0)
			break;
	}

	// start reading the mesh
	vector<DPoint3> verts;
	verts.reserve(BLOCK_SIZE);
	vector<Mesh::TRGL> tris;
	tris.reserve(BLOCK_SIZE);
	vector<pair<int, int>> patchBound;
	patchBound.reserve(256);

	bool bStop = false;
	do {
		do {
			if (strcmp(words[0], "TFACE") == 0) {
				int vertBound = verts.size();
				int triBound = tris.size();
				patchBound.push_back(pair<int, int>(vertBound, triBound));
				break;
			}

			if (strcmp(words[0], "END") == 0) {
				bStop = true;
				break;
			}

			ifs.getline(buff, BuffSize);
			if (ifs.eof())
				break;

			get_words(buff, words);
		} while (true);

		if (bStop)
			break;

		while (ifs.getline(buff, BuffSize)) {
			get_words(buff, words);

			if (strcmp(words[0], "VRTX") == 0) {
				double xx = atof(words[2]);
				double yy = atof(words[3]);
				double zz = atof(words[4]);
				
				verts.push_back(DPoint3(xx, yy, zz));
			} else if (strcmp(words[0], "ATOM") == 0) {
				int ind = atoi(words[2]);

				verts.push_back(verts[ind-1]);
			} else
				break;
		}

		do {
			if (strcmp(words[0], "TRGL") != 0)
				break;

			Mesh::TRGL trgl;
			unsigned int *ind = trgl.ind;
			ind[0] = atoi(words[1]);
			ind[1] = atoi(words[2]);
			ind[2] = atoi(words[3]);
			tris.push_back(trgl);

			ifs.getline(buff, BuffSize);
			if (ifs.eof())
				break;

			get_words(buff, words);
		} while (true);

	} while (true);

	int nVertices = verts.size();
	int nTriangles = tris.size();
	int nPatches = patchBound.size();
	patchBound.push_back(pair<int, int>(nVertices, nTriangles));

	if (nVertices != 0)
		mesh.SetVertices(&verts[0], nVertices);
	if (nTriangles != 0)
		mesh.SetTriangles(&tris[0], nTriangles);
	if (nPatches != 0)
		mesh.SetPatchBound(&patchBound[0], nPatches);

	return ifs;
}

#if 1
ofstream& operator << (ofstream &ofs, const Mesh &mesh)
{
	const char *sHeaderFmt = "GOCAD TSurf 1\n"
							 "HEADER {\n"
							 "name:%s\n"
							 "}\n";
	char sLine[1024];
	sprintf(sLine, sHeaderFmt, mesh.GetName());
	ofs << sLine;

	int nPatches = mesh.m_nPatches;
	const pair<int, int> *patchBound = mesh.m_patchBound;
	for (int k = 0; k < nPatches; k++) {
		const pair<int, int> &begin = patchBound[k];
		const pair<int, int> &end = patchBound[k+1];

		ofs << "TFACE\n";

		int nVertices = mesh.GetNumVertices();
		for (int i = begin.first; i < end.first; i++) {
			const DPoint3 &vertex = mesh.GetVertexAt(i);

#if 0
			sprintf(sLine, "VRTX %d %7.8f %7.8f %7.8f\n", i+1, 
					vertex.x, vertex.y, vertex.z);
#else
			sprintf(sLine, "VRTX %d %lf %lf %lf\n", i+1, 
				vertex.x, vertex.y, vertex.z);
#endif
			ofs << sLine;
		}

		int nTriangles = mesh.GetNumTriangles();
		for (int i = begin.second; i < end.second; i++) {
			const Mesh::TRGL &trgl = mesh.GetTriangleAt(i);
			const unsigned int *ind = trgl.ind;

			sprintf(sLine, "TRGL %d %d %d\n", ind[0]+1, ind[1]+1, ind[2]+1);
			ofs << sLine;
		}
	}

	ofs << "END" << endl;

	return ofs;
}
#else
ofstream& operator << (ofstream &ofs, const Mesh &mesh)
{
	const char *sHeaderFmt = "GOCAD TSurf 1\n"
		"HEADER {\n"
		"name:\%d\n"
		"}\n";
	char sLine[1024];

	int nPatches = mesh.m_nPatches;
	const pair<int, int> *patchBound = mesh.m_patchBound;
	for (int k = 0; k < nPatches; k++) {
		const pair<int, int> &begin = patchBound[k];
		const pair<int, int> &end = patchBound[k+1];

		sprintf(sLine, sHeaderFmt, k);
		ofs << sLine;
		ofs << "TFACE\n";

		int nVertices = mesh.GetNumVertices();
		for (int i = begin.first; i < end.first; i++) {
			const DPoint3 &vertex = mesh.GetVertexAt(i);

			sprintf(sLine, "VRTX %d %f %f %f\n", i+1, vertex.x, vertex.y, vertex.z);
			ofs << sLine;
		}

		int nTriangles = mesh.GetNumTriangles();
		for (int i = begin.second; i < end.second; i++) {
			const Mesh::TRGL &trgl = mesh.GetTriangleAt(i);
			const int *ind = trgl.ind;

			sprintf(sLine, "TRGL %d %d %d\n", ind[0], ind[1], ind[2]);
			ofs << sLine;
		}

		ofs << "END" << endl;

	}
	return ofs;
}
#endif

ifstream& PassOneMesh(ifstream &ifs)
{
	const int buffSize = 1024;
	char buff[buffSize];

	while (ifs.getline(buff, buffSize)) {
		if (strcmp(buff, "END") == 0)
			break;
	}

	return ifs;
}

int get_words(char *str, char *words[], const char delim)
{
	int count = 0;
	char *pch = str;

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

	return count;
}