#include <algorithm>
#include <queue>
#include <assert.h>
#include <float.h>
#include <iostream>

#include "MeshInfo.h"

MeshInfo::MeshInfo(const Mesh &mesh) : m_mesh(mesh)
{
}

MeshInfo::~MeshInfo()
{}

class IndexedDPoint3 : public DPoint3
{
public:
	IndexedDPoint3() {}
	~IndexedDPoint3() {}
	IndexedDPoint3(const DPoint3 &point, uint ind)
	{
		x = point.x; y = point.y; z = point.z; index = ind;
	}
	uint index;
};

template<class T>
bool dpoint3_cmp(const T &lhs, const T &rhs)
{
	const double eps = 1.0e-7;
	if (fabs(lhs.x - rhs.x) < eps) {
		if (fabs(lhs.y - rhs.y) < eps)
			return lhs.z < rhs.z;
		else
			return lhs.y < rhs.y;
	}
	else
		return lhs.x < rhs.x;

}
const double eps = 0.5;
bool dpoint3_diff(const DPoint3 &lhs, DPoint3 &rhs)
{
	DPoint3 diff = lhs - rhs;
	return (diff.x < -eps || diff.x > eps
		|| diff.y < -eps || diff.y > eps
		|| diff.z < -eps || diff.z > eps);
}

bool operator < (Mesh::TRGL &lhs, Mesh::TRGL &rhs)
{
	unsigned int lind[3], rind[3];
	lind[0] = lhs.ind[0];
	lind[1] = lhs.ind[1];
	lind[2] = lhs.ind[2];
	rind[0] = rhs.ind[0];
	rind[1] = rhs.ind[1];
	rind[2] = rhs.ind[2];
	unsigned int temp;
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 2; j++) {
			if (lind[j] > lind[j + 1]) {
				temp = lind[j];
				lind[j] = lind[j + 1];
				lind[j + 1] = temp;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 2; j++) {
			if (rind[j] > rind[j + 1]) {
				temp = lind[j];
				rind[j] = rind[j + 1];
				rind[j + 1] = temp;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		if (lind[i] != rind[i])
			return lind[i] < rind[i];
	}

	return true;
}

bool operator == (const Mesh::TRGL &lhs, const Mesh::TRGL &rhs)
{
	unsigned int lind[3], rind[3];
	lind[0] = lhs.ind[0];
	lind[1] = lhs.ind[1];
	lind[2] = lhs.ind[2];
	rind[0] = rhs.ind[0];
	rind[1] = rhs.ind[1];
	rind[2] = rhs.ind[2];

	unsigned int temp;
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 3; j++) {
			if (lind[j] > lind[j + 1]) {
				temp = lind[j];
				lind[j] = lind[j + 1];
				lind[j + 1] = temp;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 2; j++) {
			if (rind[j] > rind[j + 1]) {
				temp = lind[j];
				rind[j] = rind[j + 1];
				rind[j + 1] = temp;
			}
		}
	}

	return (lind[0] == rind[0] 
		 && lind[1] == rind[1]
		 && lind[2] == rind[2]);
}

bool operator != (const Mesh::TRGL &lhs, const Mesh::TRGL &rhs)
{
	unsigned int lind[3], rind[3];
	lind[0] = lhs.ind[0];
	lind[1] = lhs.ind[1];
	lind[2] = lhs.ind[2];
	rind[0] = rhs.ind[0];
	rind[1] = rhs.ind[1];
	rind[2] = rhs.ind[2];

	unsigned int temp;
	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 2; j++) {
			if (lind[j] > lind[j + 1]) {
				temp = lind[j];
				lind[j] = lind[j + 1];
				lind[j + 1] = temp;
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = i; j < 2; j++) {
			if (rind[j] > rind[j + 1]) {
				temp = lind[j];
				rind[j] = rind[j + 1];
				rind[j + 1] = temp;
			}
		}
	}

	return (lind[0] != rind[0]
		|| lind[1] != rind[1]
		|| lind[2] != rind[2]);
}

double tri_area3(const DPoint3 &v0, const DPoint3 &v1, const DPoint3 &v2)
{
	double a = (v2 - v1).Length();
	double b = (v0 - v2).Length();
	double c = (v1 - v0).Length();
	double s = 0.5 * (a + b + c);
	return sqrt(s * (s-a) * (s-b) * (s-c));
}

void MeshInfo::MakeRegular()
{
	int nVertices = m_mesh.GetNumVertices();
	vector<IndexedDPoint3> bucket;
	bucket.reserve(nVertices);
	for (int i = 0; i < nVertices; i++) {
		const DPoint3 &curVertex = m_mesh.GetVertexAt(i);

		bucket.push_back(IndexedDPoint3(curVertex, i));
	}

	sort(bucket.begin(), bucket.end(), dpoint3_cmp<IndexedDPoint3>);

	m_vertices.reserve(nVertices);
	uint *maptable = new uint[nVertices];
	DPoint3 curVal(DBL_MIN, DBL_MIN, DBL_MIN);
	uint dstInd = 0;
	for (int i = 0; i < nVertices; i++) {
		const IndexedDPoint3 &curVertex = bucket[i];
		const DPoint3 &curPos = *(DPoint3 *)&curVertex;

		//if (curPos != curVal) {
		if (dpoint3_diff(curPos, curVal)) {
			dstInd = m_vertices.size();
			m_vertices.push_back(curPos);
			curVal = curPos;
		}

		maptable[curVertex.index] = dstInd;
	}

	int nTriangles = m_mesh.GetNumTriangles();
	//m_indices.resize(nTriangles);
	Mesh::TRGL *indicesBucket = new Mesh::TRGL[nTriangles];
	for (int i = 0; i < nTriangles; i++) {
		const Mesh::TRGL &srcTri = m_mesh.GetTriangleAt(i);
		Mesh::TRGL &dstTri = indicesBucket[i];

		const uint *srcInd = srcTri.ind;
		uint *ind = dstTri.ind;

		for (int j = 0; j < 3; j++)
			ind[j] = maptable[srcInd[j]];
	}

	delete[] maptable;

	// check redundant triangles
	m_indices.reserve(nTriangles);
	Mesh::TRGL token;
	memset(token.ind, 0, 12);
	sort(indicesBucket, indicesBucket+nTriangles);
	for (int i = 0; i < nTriangles; i++) {
		const Mesh::TRGL &srcTri = indicesBucket[i];
		if (srcTri != token) {
			token = srcTri;
			
			DPoint3 *v3[3];
			for (int j = 0; j < 3; j ++)
				v3[j] = &m_vertices[srcTri.ind[j]];
			double area = tri_area3(*v3[0], *v3[1], *v3[2]);
			if (area > 1.0e-7)
				m_indices.push_back(srcTri);

#if 0
if (area > 1.0)
m_indices.push_back(srcTri);
else
int dummy=1;
#endif
		}
	}
}


void MeshInfo::SetAnEdge(TriInfo &info, const EdgeInfo &curEdge, int id)
{
	uint *ind = info.ind;
	for (int i = 0; i < 3; i++) {
		if (ind[i] != curEdge.i && ind[i] != curEdge.j) {
			info.iEdge[i] = id;
			break;
		}
	}
}

#define KENGDIE_DEBUG 0
void MeshInfo::Extract()
{
	MakeRegular();

	// the acceleration structure
	int nVertices = m_vertices.size();
	int nTriangles = m_indices.size();

	//vector<EdgeInfo> m_edgeinfo;
	m_edgeinfo.reserve(nTriangles * 3);
	m_triinfo.reserve(nTriangles);
	//vector<list<TriInfo>> vertexAngles;
	m_vertexTriangles.resize(nVertices);

	for (int i = 0; i < nTriangles; i++) {
		const Mesh::TRGL &curTri = m_indices[i];
		const uint *ind = curTri.ind;

		const DPoint3 &v0 = m_vertices[ind[0]];
		const DPoint3 &v1 = m_vertices[ind[1]];
		const DPoint3 &v2 = m_vertices[ind[2]];

		DPoint3 e0 = v2 - v1;
		e0.Normalize();
		DPoint3 e1 = v0 - v2;
		e1.Normalize();
		DPoint3 e2 = v1 - v0;
		e2.Normalize();
		DPoint3 curNorm = -e2.Cross(e1);
		curNorm.Normalize();

		double cos_a0 = -e1.Dot(e2);
		double a0 = acos(cos_a0);
		double cot_a0 = 1.0 / tan(a0);
		double cos_a1 = -e2.Dot(e0);
		double a1 = acos(cos_a1);
		double cot_a1 = 1.0 / tan(a1);
		double cos_a2 = -e0.Dot(e1);
		double a2 = acos(cos_a2);
		double cot_a2 = 1.0 / tan(a2);

#if 0
		if (i == 26 / 3)
			int dummy = 1;
#endif
		EdgeInfo edge0;
		edge0.i = ind[1];
		edge0.j = ind[2];
		edge0.itri = i;
		edge0.ialpha = ind[0];
		edge0.cot_alpha = cot_a0;
		edge0.state = USE_ALPHA;
		m_edgeinfo.push_back(edge0);

		EdgeInfo edge1;
		edge1.i = ind[2];
		edge1.j = ind[0];
		edge1.itri = i;
		edge1.ialpha = ind[1];
		edge1.cot_alpha = cot_a1;
		edge1.state = USE_ALPHA;
		m_edgeinfo.push_back(edge1);

		EdgeInfo edge2;
		edge2.i = ind[0];
		edge2.j = ind[1];
		edge2.itri = i;
		edge2.ialpha = ind[2];
		edge2.cot_alpha = cot_a2;
		edge2.state = USE_ALPHA;
		m_edgeinfo.push_back(edge2);

		int eBase = i * 3;
		TriInfo tri = { (uint*)ind, { eBase, eBase + 1, eBase + 2, },
			DPoint3(a0, a1, a2), curNorm };
		m_triinfo.push_back(tri);
		TriInfo *pTri = &m_triinfo.back();
		m_vertexTriangles[ind[0]].push_back(pTri);
		m_vertexTriangles[ind[1]].push_back(pTri);
		m_vertexTriangles[ind[2]].push_back(pTri);
	}
#if KENGDIE_DEBUG
DPoint3 *addr = &m_vertices[0];
FILE *fp = fopen("kengdie.vs", "w");
int kcount = 0;
#endif // KENGDIE_DEBUG
	// delete redundant edges
	sort(m_edgeinfo.begin(), m_edgeinfo.end());
	int nEdges = m_edgeinfo.size();
	EdgeInfo token;
	token.i = -1;
	token.j = -1;
	EdgeInfo *edgeBuff = new EdgeInfo[nEdges];
	int count = 0;
	int countSingleEdge = 0;
	for (int i = 0; i < nEdges; i++) {
		const EdgeInfo &curEdge = m_edgeinfo[i];

		if (curEdge != token) {
			edgeBuff[count++] = curEdge;
			token = curEdge;
			countSingleEdge = 0;
		} else {
#if KENGDIE_DEBUG
if (countSingleEdge != 0) {
const DPoint3 &kp1 = addr[curEdge.i];
const DPoint3 &kp2 = addr[curEdge.j];
fprintf(fp, "VRTX %d %lf %lf %lf \n", kcount, kp1.x, kp1.y, kp1.z);
fprintf(fp, "VRTX %d %lf %lf %lf \n", kcount + 1, kp2.y, kp2.y, kp2.z);
kcount += 2;
}
#endif // KENGDIE_DEBUG
			//assert(countSingleEdge == 0);// alert if real redundant edge is found
			EdgeInfo &info = edgeBuff[count - 1];
			info.jtri = curEdge.itri;
			info.cot_beta = curEdge.cot_alpha;
			info.ibeta = curEdge.ialpha;
			info.state = USE_ALPHA_AND_BETA;
			countSingleEdge++;
		}
	}
	m_edgeinfo.clear();
	m_edgeinfo.assign(edgeBuff, edgeBuff + count);
	delete[] edgeBuff;
	nEdges = count;

	// reset triangle information regarding the edges
	for (int i = 0; i < nEdges; i++) {
		const EdgeInfo &curEdge = m_edgeinfo[i];

		uint itri = curEdge.itri;

		TriInfo &itriInfo = m_triinfo[itri];
		SetAnEdge(itriInfo, curEdge, i);

		if (curEdge.state == USE_ALPHA_AND_BETA) {
			uint jtri = curEdge.jtri;
			TriInfo &jtriInfo = m_triinfo[jtri];
			SetAnEdge(jtriInfo, curEdge, i);
		}
	}


	// set edges for each vertex
	m_vertexEdges.resize(nVertices);
	for (int i = 0; i < nEdges; i++) {
		const EdgeInfo &curEdge = m_edgeinfo[i];

		m_vertexEdges[curEdge.i].push_back((EdgeInfo *)&curEdge);
		m_vertexEdges[curEdge.j].push_back((EdgeInfo *)&curEdge);
	}
#if KENGDIE_DEBUG
fclose(fp);
#endif // KENGDIE_DEBUG
	// flip the normals
	//FlipNormals();

#if 0	
	// delete redundant edges
	for (int i = 0; i < nVertices; i++) {
		list<EdgeInfo*> &edgeList = m_vertexEdges[i];

		edgeList.sort(edge_cmp);
		//sort(edgeList.begin(), edgeList.end(), cmp);
		EdgeInfo edge;
		edge.i = edge.j = edge.state = 0;
		edge.cot_alpha = edge.cot_beta = 0.0;
		list<EdgeInfo *>::iterator it = edgeList.begin();
		list<EdgeInfo *>::iterator pivot;
		for (; it != edgeList.end();) {
			if (!edge_equal(*it, &edge)) {
				edge = **it;
				pivot = it;
				it++;
			}
			else {
				EdgeInfo *pCur = *it;
				EdgeInfo *pPre = *pivot;
#if 0
				if ((int)(pPre - &m_edgeinfo[0]) == 26) {
					int dloc = (int)(pCur - &m_edgeinfo[0]);
					int dummy = 1;
				}
#endif

				if (pPre->state == USE_ALPHA) {
					pPre->jtri = pCur->itri;
					pPre->cot_beta = pCur->cot_alpha;
					pPre->ibeta = pCur->ialpha;
					pPre->state = USE_ALPHA_AND_BETA;
					pCur->state = USE_BETA;
				}
#if 0
				else if (pPre->state == USE_ALPHA_AND_BETA) {
					;
				}
				else if (pPre->state == USE_BETA) {
					pPre->itri = pCur->jtri;
					pPre->cot_alpha = pCur->cot_alpha;
					pPre->state = USE_ALPHA_AND_BETA;
				}
#endif // 0

				it = edgeList.erase(it);
			}
		}
	}
#endif


}

void CalcNormal(const DPoint3 &pi, const DPoint3 &pj, const DPoint3 &pk, DPoint3 &norm)
{
	DPoint3 v0 = pj - pi;
	DPoint3 v1 = pk - pi;
	norm = v0.Cross(v1);
	norm.Normalize();
}

void MeshInfo::OtherTwo(const TriInfo &info, int here, int &other0, int &other1)
{
	const uint *iEdge = info.iEdge;
	int dstLoc = -1;
	for (int i = 0; i < 3; i++) {
		if (iEdge[i] == here)
			dstLoc = i;
	}

	other0 = iEdge[(dstLoc + 1) % 3];
	other1 = iEdge[(dstLoc + 2) % 3];
}

void MeshInfo::FlipNormals()
{
	int nEdges = m_edgeinfo.size();
	double *realProd = new double[nEdges];

	int maxLoc = -1;
	double maxProd = DBL_MIN;
	for (int i = 0; i < nEdges; i++) {
		const EdgeInfo &info = m_edgeinfo[i];

		if (info.state == USE_ALPHA_AND_BETA) {
			const DPoint3 &vi = m_vertices[info.i];
			const DPoint3 &vj = m_vertices[info.j];
			const DPoint3 &valpha = m_vertices[info.ialpha];
			const DPoint3 &vbeta = m_vertices[info.ibeta];

			DPoint3 nalpha;
			CalcNormal(vi, vj, valpha, nalpha);
			DPoint3 nbeta;
			CalcNormal(vj, vi, vbeta, nbeta);

			double dotprod = nalpha.Dot(nbeta);
			realProd[i] = dotprod;

			if (dotprod > maxProd) {
				maxProd = dotprod;
				maxLoc = i;
			}
		}
		else
			realProd[i] = 0.0;
	}

	// traverse the mesh and do flipping
	int nTriangles = m_triinfo.size();
	bool *visited = new bool[nTriangles];
	memset(visited, 0, nTriangles);

	queue<int> eQueue;
	int srcTri = m_edgeinfo[maxLoc].itri;
	int other0, other1;
	OtherTwo(m_triinfo[srcTri], maxLoc, other0, other1);
	eQueue.push(maxLoc);
	eQueue.push(other0);
	eQueue.push(other1);
	visited[srcTri] = true;
	int count = 0;
	while (!eQueue.empty()) {
		int loc = eQueue.front();
		eQueue.pop();

		const EdgeInfo &info = m_edgeinfo[loc];
		int curTriLoc = visited[info.itri] ? info.jtri : info.itri;

		if (info.state == USE_ALPHA_AND_BETA && !visited[curTriLoc]) {
			TriInfo &curTriInfo = m_triinfo[curTriLoc];

			const TriInfo &itriInfo = m_triinfo[info.itri];
			const TriInfo &jtriInfo = m_triinfo[info.jtri];
			const DPoint3 &ni = itriInfo.normal;
			const DPoint3 &nj = jtriInfo.normal;
			double curProd = ni.Dot(nj);

			if (fabs(curProd - realProd[loc]) > 1.0e-6)
				curTriInfo.normal = -curTriInfo.normal;

			OtherTwo(curTriInfo, loc, other0, other1);
			eQueue.push(other0);
			eQueue.push(other1);

			visited[curTriLoc] = true;
			count++;
		}
	}

	delete[] visited;
	delete[] realProd;
}

#if 0
void OutputTriangleNormals(vector<DPoint3> &m_vertices, const vector<TriInfo> &m_triinfo, const char *path)
{
	int nTriangles = m_triinfo.size();

	DPoint3 *vertBuff = new DPoint3[nTriangles << 1];
	int *indi = new int[nTriangles << 2];
	for (int i = 0; i < nTriangles; i++) {
		const TriInfo &info = m_triinfo[i];
		uint *ind = info.ind;
		const DPoint3 &curNorm = info.normal;

		const DPoint3 &v0 = m_vertices[ind[0]];
		const DPoint3 &v1 = m_vertices[ind[1]];
		const DPoint3 &v2 = m_vertices[ind[2]];
		DPoint3 centroid = (v0 + v1 + v2) / 3.0;

		//	indi[base + 1] = base + 1;
		int base = i << 1;
		vertBuff[base] = centroid;
		vertBuff[base + 1] = centroid + curNorm * 25.0;
		indi[i] = base;
	}
	indi[nTriangles] = nTriangles << 1;
	FILE *fp = fopen(path, "w");
	plputs(vertBuff, indi, nTriangles, "trinorms", fp, false);
	fclose(fp);
	delete[] indi;
	delete[] vertBuff;
}
#endif
