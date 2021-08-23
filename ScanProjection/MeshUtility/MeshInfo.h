#ifndef _MESH_INFO_H
#define _MESH_INFO_H

#include <vector>
#include <list>
#include "Mesh.h"

using namespace std;

typedef unsigned int uint;

const double HalfPi = PI * 0.5;

#define NO_VAL	0
#define USE_ALPHA 1
#define USE_BETA  2
#define USE_ALPHA_AND_BETA  3

struct MESH_UTILITY_API EdgeInfo
{
	int i;
	int j;

	int itri;
	int jtri;

	int state;

	int ialpha;
	int ibeta;

	double cot_alpha;
	double cot_beta;

	bool operator < (const EdgeInfo &rhs) const {
		uint lmin, lmax, rmin, rmax;
		if (i < j) {
			lmin = i;
			lmax = j;
		} else {
			lmin = j;
			lmax = i;
		}
		if (rhs.i < rhs.j) {
			rmin = rhs.i;
			rmax = rhs.j;
		} else {
			rmin = rhs.j;
			rmax = rhs.i;
		}

		if (lmin == rmin)
			return lmax < rmax;
		else
			return lmin < rmin;
	}

	bool operator == (const EdgeInfo &rhs) const {
		uint lmin, lmax, rmin, rmax;
		if (i < j) {
			lmin = i;
			lmax = j;
		} else {
			lmin = j;
			lmax = i;
		}
		if (rhs.i < rhs.j) {
			rmin = rhs.i;
			rmax = rhs.j;
		} else {
			rmin = rhs.j;
			rmax = rhs.i;
		}

		return (lmin == rmin && lmax == rmax);
	}

	bool operator != (const EdgeInfo &rhs) const {
		uint lmin, lmax, rmin, rmax;
		if (i < j) {
			lmin = i;
			lmax = j;
		} else {
			lmin = j;
			lmax = i;
		}
		if (rhs.i < rhs.j) {
			rmin = rhs.i;
			rmax = rhs.j;
		} else {
			rmin = rhs.j;
			rmax = rhs.i;
		}

		return (lmin != rmin || lmax != rmax);
	}
};

struct MESH_UTILITY_API TriInfo {
	uint *ind;
	int iEdge[3];
	DPoint3 angles;
	DPoint3 normal;
};

class MESH_UTILITY_API MeshInfo
{
public:
	MeshInfo(const Mesh &mesh);
	~MeshInfo();

	void Extract(uint *maptable);
	void Extract();
	const vector<DPoint3> &GetVertices() const { return m_vertices; }
	const vector<Mesh::TRGL> &GetIndices() const { return m_indices; }
	const vector<TriInfo> &GetTriInfo() const { return m_triinfo; }
	const vector<EdgeInfo> &GetEdgeInfo() const { return m_edgeinfo; }
	const list<TriInfo*> &GetVertTriInfo(int i) const 
	{ return m_vertexTriangles[i]; }
	const list<EdgeInfo*> &GetVertEdgeInfo(int i) const
	{ return m_vertexEdges[i]; }

	int GetTriLoc(TriInfo *info) const { return (int)(info - &m_triinfo[0]); }
	int GetEdgeLoc(EdgeInfo *info) const { return (int)(info - &m_edgeinfo[0]); }
	/////////////////////////////////////
private:
	void MakeRegular(uint *maptable);
	void MakeRegular();
	void FlipNormals();
	void OtherTwo(const TriInfo &info, int here, int &other0, int &other1);
	void SetAnEdge(TriInfo &info, const EdgeInfo &curEdge, int id);

private:
	const Mesh &m_mesh;
	vector<DPoint3> m_vertices;
	vector<Mesh::TRGL> m_indices;
	vector<TriInfo> m_triinfo;
	vector<EdgeInfo> m_edgeinfo;
	vector<list<TriInfo*>> m_vertexTriangles;
	vector<list<EdgeInfo*>> m_vertexEdges;
};

#endif // _MESH_INFO_H
