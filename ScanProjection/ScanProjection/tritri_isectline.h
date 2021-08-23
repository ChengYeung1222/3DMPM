#ifndef _TRI_TRI_ISECTLINE_H
#define _TRI_TRI_ISECTLINE_H

extern
int tri_tri_intersect_with_isectline(double V0[3], double V1[3], double V2[3],
	double U0[3], double U1[3], double U2[3], int *coplanar,
	double isectpt1[3], double isectpt2[3]);

#endif // _TRI_TRI_ISECTLINE_H