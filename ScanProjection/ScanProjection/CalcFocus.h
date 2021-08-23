#ifndef _CALC_FOCUS_H
#define _CALC_FOCUS_H

#include "../kdtree/kdtree.h"

class MeshInfo;
class DPoint3;

class FocusSpatialInfo
{
public:
	FocusSpatialInfo();
	~FocusSpatialInfo();

	void Make(MeshInfo *pMeshinfo, double range, double lowerBnd);
	double ClosestPoint(const DPoint3 &src, DPoint3 &dst) const;

private:
	void MakeKdTree();
	void CalcTriangleInfo();
	void CalcVertNormal(); // for foot wall judgement only

private:
	double m_lowerBnd;
	double m_range;
	MeshInfo *m_pMeshInfo;
	DPoint3 *m_triNormal;
	double *m_triArea;
	DPoint3 *m_normals;
	kdtree *m_kd;
	int *m_kdData;

};


#endif // _CALC_FOCUS_H
