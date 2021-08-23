#include "tri_dist.h"

#include <assert.h>
#include <float.h>
#include <algorithm>

#include "tritri_isectline.h"

using namespace std;

double PointToTriangle(const DPoint3 &q, const DPoint3 &p0, 
	const DPoint3 &p1, const DPoint3 &p2, DPoint3 &p)
{
	DPoint3 v0 = p1 - p0;
	DPoint3 v1 = p2 - p0;

	DPoint3 l = p0 - q;

	double a = v0.x * v0.x + v0.y * v0.y + v0.z * v0.z;
	double b = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
	double c = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
	double d = l.x * v0.x + l.y * v0.y + l.z * v0.z;
	double e = l.x * v1.x + l.y * v1.y + l.z * v1.z;

	double det = a*c - b*b;
	double s = b*e - c*d;
	double t = b*d - a*e;

	if (s + t < det) {
		if (s < 0) {
			if (t < 0) {
				// region 4
				// Q'(s, t) = (as + bt + d, bs + ct + e)
				// Q'(0, 0) = (d, e)
				// (1, 0)^T \cdot Q'(0, 0) = d
				// (0, 1)^T \cdot Q'(0, 0) = e
				if (d < 0) {
					t = 0;
					s = d >= 0 ? 0.0 : (-d >= a ? 1.0 : -d / a);
				}
				else {
					s = 0.0;
					t = e >= 0 ? 0 : (-e >= c ? 1.0 : -e / c);
				}

			}
			else {
				// region 3
				s = 0.0;
				t = e >= 0 ? 0 : (-e >= c ? 1.0 : -e / c);
			}
		}
		else if (t < 0) {
			// region 5
			// F(s) = 2Q(s, 0) = as^2 + 2ds + f
			// F'(s) = as + d
			t = 0.0;
			s = d >= 0 ? 0.0 : (-d >= a ? 1.0 : -d / a);
		}
		else {
			// region 0
			double invDet = 1.0 / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else {
		if (s < 0) {
			// region 2
			double tmp0 = b + d;
			double tmp1 = c + e;
			if (tmp1 > tmp0) {
				double nume = tmp1 - tmp0;
				double deno = a - 2 * b + c;
				s = nume > deno ? 1 : nume / deno;
				t = 1.0 - s;
			}
			else {
				s = 0;
				t = tmp1 < 0.0 ? 1 : (e >= 0 ? 0 : -e / c);
			}

		}
		else if (t < 0) {
			// region 6
			// Q'(s, t) = (as + bt + d, bs + ct + e)
			// Q'(1, 0) = (a + d, b + e)
			// (-1, 0)^T \cdot Q'(1, 0) = -(a + d)
			// (-1, 1)^T \cdot Q'(1, 0) = -(a + d) + (b + e)
			// minimum on edge s + t = 1 if (-1, 1)^T \cdot Q'(1, 0) < 0
			double tmp0 = a + d;
			double tmp1 = b + e;
			if (tmp1 < tmp0) {
				double nume = c + e - b - d;

				if (nume <= 0.0)
					s = 0.0;
				else {
					double deno = a - 2.0 * b + c;
					s = nume >= deno ? 1 : nume / deno;
				}

				t = 1.0 - s;
			}
			else {
				t = 0;
				s = d >= 0 ? 0.0 : (-d >= a ? 1.0 : -d / a);
			}


		}
		else {
			// region 1
			double nume = c + e - b - d;

			if (nume < 0) {
				s = 0;
			}
			else {
				double deno = a - 2.0*b + c;
				s = (nume >= deno ? 1 : nume / deno);
			}

			t = 1 - s;
		}
	}

	assert(s >= 0 && s <= 1 && t >= 0 && t <= 1 && s + t <= 1);

	p = p0 + v0 * s + v1 * t;
	return (q - p).Length();
}


#define SAFE_DIST 1024
namespace MaxNormTriDist {
	const double v = SAFE_DIST;
	const DPoint3 dirs[12][2] = { {DPoint3(v, v, v), DPoint3(-v, v, v)},
								  {DPoint3(-v, v, v), DPoint3(-v, -v, v)},
								  {DPoint3(-v, -v, v), DPoint3(v, -v, v)},
								  {DPoint3(v, v, v), DPoint3(v, v, v)},
								  {DPoint3(v, v, -v), DPoint3(v, -v, -v)},
								  {DPoint3(v, -v, -v), DPoint3(-v, -v, -v)},
								  {DPoint3(-v, -v, -v), DPoint3(-v, v, -v)},
								  {DPoint3(-v, v, -v), DPoint3(v, v, -v)},
								  {DPoint3(v, v, v), DPoint3(v, v, -v)},
								  {DPoint3(-v, v, v), DPoint3(-v, v, -v)},
								  {DPoint3(-v, -v, v), DPoint3(-v, -v, -v)},
								  {DPoint3(v, -v, v), DPoint3(v, -v, -v)},
								};
};

#define dp2dlp(p) ((double *)&(p))

#define MaxNorm(v) ( max(max(fabs((v).x), fabs((v).y)), fabs((v).z)) )

#define UpdateMinDist(p, q, m) {	\
	DPoint3 diff = (p) - (q);		\
	double distSqr = MaxNorm(diff);	\
	if (distSqr < m)				\
		m = distSqr;				\
}									\

double MaxNormToTriangle(const DPoint3 &q, const DPoint3 &p0,
	const DPoint3 &p1, const DPoint3 &p2)
{
	double minDist = DBL_MAX;

	const DPoint3 *dirs = &MaxNormTriDist::dirs[0][0];
	for (int i = 0, j = 0; i < 12; i++, j += 2) {
		DPoint3 q1 = q + dirs[j];
		DPoint3 q2 = q + dirs[j + 1];

		DPoint3 endPoint[2];
		int coplanar;
		int bIntersect = tri_tri_intersect_with_isectline(dp2dlp(p0),
			dp2dlp(p1), dp2dlp(p2), dp2dlp(q), dp2dlp(q1), dp2dlp(q2),
			&coplanar, dp2dlp(endPoint[0]), dp2dlp(endPoint[1]));

		if (bIntersect)
			for (int k = 0; k < 2; k++)
				UpdateMinDist(endPoint[0], q, minDist);
	}

	UpdateMinDist(p0, q, minDist);
	UpdateMinDist(p1, q, minDist);
	UpdateMinDist(p2, q, minDist);

	return minDist;
}