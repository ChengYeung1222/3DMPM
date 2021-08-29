#include <set>
#include <algorithm>

#include "CalcFocus.h"
#include "../kdtree/kdtree.h"
#include "../MeshUtility/MeshUtility.h"
#include "tri_dist.h"
#include "DebugUtility.h"

using namespace std;

FocusSpatialInfo::FocusSpatialInfo()
{
	m_pMeshInfo = NULL;
	m_triNormal = NULL;
	m_triArea = NULL;
	m_normals = NULL;
	m_kd = NULL;
	m_kdData = NULL;
}

FocusSpatialInfo::~FocusSpatialInfo()
{
	if (m_triNormal != NULL)
		delete[] m_triNormal;
	if (m_triArea != NULL)
		delete[] m_triArea;
	if (m_normals != NULL)
		delete[] m_normals;
	if (m_kd != NULL)
		kd_free(m_kd);
	if (m_kdData != NULL)
		delete[] m_kdData;
}

void FocusSpatialInfo::Make(MeshInfo *pMeshinfo, double range, double lowerBnd)
{
	m_pMeshInfo = pMeshinfo;
	m_range = range;
	m_lowerBnd = lowerBnd;
	MakeKdTree();
	CalcTriangleInfo();
	CalcVertNormal();
}

void FocusSpatialInfo::MakeKdTree()
{
	const vector<DPoint3> &vertices = m_pMeshInfo->GetVertices();
	int nVertices = vertices.size();

	m_kd = kd_create(3);
	m_kdData = new int[nVertices];

	for (int i = 0; i < nVertices; i++) {
		const DPoint3 &curVert = vertices[i];

		DPoint3 point = curVert;
		m_kdData[i] = i;
		kd_insert(m_kd, (double *)&point, m_kdData+i);
	}
}

double FocusSpatialInfo::ClosestPoint(const DPoint3 &src, DPoint3 &dst) const
{
	std::set<int> DstTris;

	DPoint3 closest;
	kdres* kdResult = kd_nearest(m_kd, (double*)&src);
	int idx = *(int*)kd_res_item(kdResult, (double*)&closest);
	const list<TriInfo*>& vtris = m_pMeshInfo->GetVertTriInfo(idx);
	list<TriInfo*>::const_iterator itTriInfo = vtris.begin();
	for (; itTriInfo != vtris.end(); itTriInfo++) {
		int triloc = m_pMeshInfo->GetTriLoc(*itTriInfo);
		DstTris.insert(triloc);
	}


	kd_res_free(kdResult);

	// calculate the centroid
	DPoint3 closestPoint;
	double minDist = numeric_limits<double>::max();

	const vector<DPoint3> &vertices = m_pMeshInfo->GetVertices();
	int nVertices = vertices.size();
	const vector<Mesh::TRGL> &triangles = m_pMeshInfo->GetIndices();
	int nTriangles = triangles.size();

	std::set<int>::const_iterator it = DstTris.begin();
	for (int ii = 1; it != DstTris.end(); it++, ii++) {
		int curid = *it;
		const Mesh::TRGL &curtri = triangles[curid];
		const unsigned int *trind = curtri.ind;

		const DPoint3 &p0 = vertices[trind[0]];
		const DPoint3 &p1 = vertices[trind[1]];
		const DPoint3 &p2 = vertices[trind[2]];

		DPoint3 p;
		double dist = PointToTriangle(src, p0, p1, p2, p);
		if (dist < minDist) {
			minDist = dist;
			closestPoint = p;
		}
	}

	dst = closestPoint;

	return minDist;
}


bool FocusSpatialInfo::IsFootWall(const DPoint3& src) const
{
	kdres* kdclosest = kd_nearest(m_kd, (const double*)&src);
	DPoint3 dummy;
	int idx = *(int*)kd_res_item(kdclosest, (double*)&dummy);
	kd_res_free(kdclosest);

	const vector<DPoint3>& vertices = m_pMeshInfo->GetVertices();
	const DPoint3* base = &vertices[0];

	const DPoint3& normal = m_normals[idx];
	const DPoint3& closest = vertices[idx];

	DPoint3 dir = src - closest;
	return dir.Dot(normal) >= 0.0;
}

void FocusSpatialInfo::CalcTriangleInfo()
{
	const vector<DPoint3> &vertices = m_pMeshInfo->GetVertices();
	const vector<TriInfo> &triangles = m_pMeshInfo->GetTriInfo();
	int nVertices = vertices.size();
	int nTriangles = triangles.size();

	m_triNormal = new DPoint3[nTriangles];
	m_triArea = new double[nTriangles];
	for (int i = 0; i < nTriangles; i++) {
		const TriInfo &curTri = triangles[i];
		const uint *triInd = curTri.ind;

		const DPoint3 &p0 = vertices[triInd[0]];
		const DPoint3 &p1 = vertices[triInd[1]];
		const DPoint3 &p2 = vertices[triInd[2]];
		DPoint3 v0 = p1 - p0;
		DPoint3 v1 = p2 - p0;

		DPoint3 curNormal = v0.Cross(v1);
		//curNormal.Normalize();
		double curNormLen = curNormal.Length();
		curNormal /= curNormLen;
		m_triNormal[i] = curNormal;
		m_triArea[i] = curNormLen * 0.5;
	}
}

void FocusSpatialInfo::CalcVertNormal()
{
	const vector<DPoint3>& vertices = m_pMeshInfo->GetVertices();
	int nVertices = vertices.size();
	if (m_normals != NULL)
		delete[] m_normals;
	m_normals = new DPoint3[nVertices];

	for (int i = 0; i < nVertices; i++) {
		const DPoint3& curVertex = vertices[i];

		DPoint3 mean(0.0, 0.0, 0.0);
		double voronoiArea = 0.0;

		const list<TriInfo*>& triinfo = m_pMeshInfo->GetVertTriInfo(i);
		list<TriInfo*>::const_iterator itTri = triinfo.begin();
		for (; itTri != triinfo.end(); itTri++) {
			TriInfo* curtri = *itTri;
			uint* ind = curtri->ind;
			const DPoint3 &angles = curtri->angles;

			int dstInd = -1;
			for (int j = 0; j < 3; j++) {
				if (curVertex == vertices[ind[j]]) {
					dstInd = j;
					break;
				}
			}

			double ap = angles[dstInd];
			DPoint3 curNorm = curtri->normal;
			mean += curNorm * ap;
		}

		m_normals[i] = mean.Normalize();
	}
}