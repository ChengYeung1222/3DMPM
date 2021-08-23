#include <assert.h>
#include <limits>

#include "MinDist2Mesh.h"

MinDist2Mesh::MinDist2Mesh()
{
	m_triBucket = NULL;
	m_fpDf = NULL;
}

MinDist2Mesh::~MinDist2Mesh()
{
	Destroy();
}

void MinDist2Mesh::Destroy()
{
	if (m_triBucket != NULL)
		DestroyTriangleBucket(m_triBucket, *m_pExt);
	if (m_fpDf != NULL)
		fclose(m_fpDf);
}

bool MinDist2Mesh::Init(MeshInfo *pMeshinfo, const char *distPath,
						GridExtent<double> *pExt)
{
	Destroy();

	m_pMeshInfo = pMeshinfo;
	m_pExt = pExt;
	MeshVox(*m_pMeshInfo, *m_pExt, m_triBucket);
	m_fpDf = fopen(distPath, "rb");
	return (m_fpDf != NULL);
}

void MinDist2Mesh::SearchClosestVoxel(const DPoint3 &curVoxel, 
									  DPoint3 &dstVoxel)  const
{
	int ii, jj, kk;
	m_pExt->GetCoord(curVoxel.x, curVoxel.y, curVoxel.z, ii, jj, kk);

	int layerSize = m_pExt->GetLayerSize();
	int dim[3];
	m_pExt->GetDim(dim);

	DPoint3 org;
	m_pExt->GetOrg((double *)&org);
	short distVec[3];
	const int vecSize = sizeof(distVec);

	double VoxelSize = m_pExt->GetRes();
	double halfVoxelSize = 0.5 * VoxelSize;

	const int offset = sizeof(int) * 3;
	int loc = (kk * layerSize + jj * dim[0] + ii) * vecSize;

	fseek(m_fpDf, offset + loc, SEEK_SET);
	fread(distVec, vecSize, 1, m_fpDf);

	int ni = ii + distVec[1];
	int nj = jj + distVec[2];
	int nk = kk + distVec[0];

	double nx = org.x + VoxelSize * ni + halfVoxelSize;
	double ny = org.y + VoxelSize * nj + halfVoxelSize;
	double nz = org.z + VoxelSize * nk + halfVoxelSize;

	dstVoxel = DPoint3(nx, ny, nz);
}


double MinDist2Mesh::MinDist(const DPoint3 &curVoxel, DPoint3 &dstPoint) const
{
	DPoint3 dstVoxel;
	SearchClosestVoxel(curVoxel, dstVoxel);

	int loc = m_pExt->GetIndex(dstVoxel.x, dstVoxel.y, dstVoxel.z);
	set<int> *pTriLoc = m_triBucket[loc];

	assert(pTriLoc != NULL);

	const vector<DPoint3> &vertices = m_pMeshInfo->GetVertices();
	const vector<Mesh::TRGL> &indices = m_pMeshInfo->GetIndices();

	double closestDist = numeric_limits<double>::max();
	set<int>::const_iterator it = pTriLoc->begin();
	for (; it != pTriLoc->end(); it++) {
		const unsigned int *ind = indices[*it].ind;

		const DPoint3 &p0 = vertices[ind[0]];
		const DPoint3 &p1 = vertices[ind[1]];
		const DPoint3 &p2 = vertices[ind[2]];

		DPoint3 p;
		double dist = PointToTriangle(curVoxel, p0, p1, p2, p);
		if (dist < closestDist) {
			closestDist = dist;
			dstVoxel = p;
		}
	}

	return closestDist;
}