#ifndef _TRI_DIST_H
#define _TRI_DIST_H

#include "../Utility/primitive.h"

extern double 
	PointToTriangle(const DPoint3 &q, const DPoint3 &p0, const DPoint3 &p1,
		const DPoint3 &p2, DPoint3 &p);

extern double
	MaxNormToTriangle(const DPoint3 &q, const DPoint3 &p0,
		const DPoint3 &p1, const DPoint3 &p2);

#endif // _TRI_DIST_H
