#ifndef _MESH_H
#define _MESH_H

#include <fstream>
#include <string.h>

#include "MeshUtilityDef.h"
#include "../Utility/primitive.h"

#define MESH_NAME_SIZE	64

using namespace std;

class MESH_UTILITY_API Mesh
{
public:
	typedef struct {
		unsigned int ind[3];
	} TRGL;

	Mesh(void);
	Mesh(const Mesh &mesh);
	~Mesh(void);

	void SetNumPatches(int nPatches);
	void BeginPatch();
	void EndPatch();
	void Clear();

	const char* GetName() const { return m_name; }
	void SetName(const char *name) { strcpy_s(m_name, MESH_NAME_SIZE, name); }

	void SetVertices(const DPoint3 *vertices, int nVertices);
	void SetTriangles(const TRGL *triangles, int nTriangles);
	void SetPatchBound(pair<int, int> *bound, int nPatches);
	inline void SetVertexAt(int i, const DPoint3 &vertex) 
	{ m_vertices[i] = vertex; }

	int GetNumVertices() const { return m_nVertices; }
	int GetNumTriangles() const { return m_nTriangles; }

	int GetNumPatches() const { return m_nPatches; }  

	void GetPatchVertBound(int i, int &begin, int &end) const
	{ begin = m_patchBound[i].first; end = m_patchBound[i+1].first; }
	void GetPatchTriBound(int i, int &begin, int &end) const
	{ begin = m_patchBound[i].second; end = m_patchBound[i+1].second; }

	inline const DPoint3& GetVertexAt(int i) const { return m_vertices[i]; }
	inline const TRGL& GetTriangleAt(int i) const { return m_triangles[i]; }

	void WriteAsDatamineCsv(const char *ptPath, const char *trPath) const;

	friend MESH_UTILITY_API ifstream& operator >> (ifstream &ifs, Mesh &mesh);
	friend MESH_UTILITY_API ofstream& 
		operator << (ofstream &ofs, const Mesh &mesh);
	friend MESH_UTILITY_API ifstream& PassOneMesh(ifstream &ifs);

private:
	DPoint3 *m_vertices;
	int m_nVertices;
	TRGL *m_triangles;
	int m_nTriangles;
	pair<int, int> *m_patchBound;
	int m_nPatches;
	char m_name[MESH_NAME_SIZE];
};


#endif	// _MESH_H
