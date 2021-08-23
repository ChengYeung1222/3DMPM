#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

#include <stdio.h>
#include <math.h>

#define PI	3.141592653589793
#define DEG_TO_RAD	.0174532925199432958
#define RAD_TO_DEG	57.29577951308232

class DPoint2;
/**
* A 2-component vector class, single-precision (float).
*/
class FPoint2
{
public:
	FPoint2() { x = y = 0.0f; }
	FPoint2(int ix, int iy) { x=(float)ix; y=(float)iy; }
	FPoint2(float fx, float fy) { x=fx; y=fy; }
	FPoint2(double dx, double dy) { x=(float)dx; y=(float)dy; }
	FPoint2(const DPoint2 &d);

	float Length() const { return sqrtf(x*x+y*y); }
	float LengthSquared() const { return x*x+y*y; }
	FPoint2 &Normalize() { float s = 1.0f/Length(); x*=s; y*=s; return (*this); }
	FPoint2 &SetLength(float len) { float s = len/Length(); x*=s; y*=s; return (*this); }
	void Set(float fx, float fy) { x=fx; y=fy; }
	float Dot(const FPoint2 &rhs) const
	{
		return x*rhs.x+y*rhs.y;
	}
	void Mult(const FPoint2 &factor) { x *= factor.x; y *= factor.y; }
	void Mult(const float fx, const float fy) { x *= fx; y *= fy; }
	void Div(const FPoint2 &factor) { x /= factor.x; y /= factor.y; }
	void Div(const float fx, const float fy) { x /= fx; y /= fy; }
	void Rotate(double radians)
	{
		float tempx = x;
		x = x * (float) cos(radians) - y * (float) sin(radians);
		y = tempx * (float) sin(radians) + y * (float) cos(radians);
	}
	/** The so-called "2D cross product" is really the dot-product with the
	perpendicular vector */
	float Cross(const FPoint2 &rhs)
	{
		return (x*rhs.y - y*rhs.x);
	}

	// assignment
	FPoint2 &operator=(const FPoint2 &v) { x = v.x; y = v.y; return *this; }
	FPoint2 &operator=(const class DPoint2 &v);

	FPoint2 operator +(const FPoint2 &v) const { return FPoint2(x+v.x, y+v.y); }
	FPoint2 operator -(const FPoint2 &v) const { return FPoint2(x-v.x, y-v.y); }
	FPoint2 operator *(float s) const { return FPoint2(x*s, y*s); }
	FPoint2 operator /(float s) const { return FPoint2(x/s, y/s); }
	FPoint2 operator -() const { return FPoint2(-x, -y); }

	void operator +=(const FPoint2 &v) { x+=v.x; y+=v.y; }
	void operator -=(const FPoint2 &v) { x-=v.x; y-=v.y; }
	void operator *=(float s) { x*=s; y*=s; }
	void operator /=(float s) { x/=s; y/=s; }

	bool operator==(const FPoint2 &v) const { return (x == v.x && y == v.y); }
	bool operator!=(const FPoint2 &v) const { return (x != v.x || y != v.y); }

	float x, y;
};

class DPoint2
{
public:
	DPoint2() { x = y = 0.0; }
	DPoint2(int ix, int iy) { x=(double)ix; y=(double)iy; }
	DPoint2(double fx, double fy) { x=fx; y=fy; }

	double Length() const { return sqrt(x*x+y*y); }
	double LengthSquared() const { return x*x+y*y; }
	DPoint2 &Normalize() { double s = 1.0f/Length(); x*=s; y*=s; return (*this); }
	DPoint2 &SetLength(double len) { double s = len/Length(); x*=s; y*=s; return (*this); }
	void Set(double fx, double fy) { x=fx; y=fy; }
	double Dot(const DPoint2 &rhs) const
	{
		return x*rhs.x+y*rhs.y;
	}
	void Mult(const DPoint2 &factor) { x *= factor.x; y *= factor.y; }
	void Mult(const double fx, const double fy) { x *= fx; y *= fy; }
	void Div(const DPoint2 &factor) { x /= factor.x; y /= factor.y; }
	void Div(const double fx, const double fy) { x /= fx; y /= fy; }
	void Rotate(double radians)
	{
		double tempx = x;
		x = x * (double) cos(radians) - y * (double) sin(radians);
		y = tempx * (double) sin(radians) + y * (double) cos(radians);
	}
	/** The so-called "2D cross product" is really the dot-product with the
		perpendicular vector */
	double Cross(const DPoint2 &rhs)
	{
		return (x*rhs.y - y*rhs.x);
	}

	// assignment
	DPoint2 &operator=(const DPoint2 &v) { x = v.x; y = v.y; return *this; }
//	DPoint2 &operator=(const class DPoint2 &v);

	DPoint2 operator +(const DPoint2 &v) const { return DPoint2(x+v.x, y+v.y); }
	DPoint2 operator -(const DPoint2 &v) const { return DPoint2(x-v.x, y-v.y); }
	DPoint2 operator *(double s) const { return DPoint2(x*s, y*s); }
	DPoint2 operator /(double s) const { return DPoint2(x/s, y/s); }

	void operator +=(const DPoint2 &v) { x+=v.x; y+=v.y; }
	void operator -=(const DPoint2 &v) { x-=v.x; y-=v.y; }
	void operator *=(double s) { x*=s; y*=s; }
	void operator /=(double s) { x/=s; y/=s; }

	bool operator==(const DPoint2 &v) const { return (x == v.x && y == v.y); }
	bool operator!=(const DPoint2 &v) const { return (x != v.x || y != v.y); }

	DPoint2 operator -() const { return DPoint2(-x, -y); }
	inline double operator[] (int i) const { return *(&x + i); }

	bool operator < (const DPoint2 &p) {
		if(x < p.x)
			return true;
		else {
			if(x > p.x)
				return false;
			else
				return (y < p.y);
		}
	}

	double x, y;
};

inline FPoint2::FPoint2(const DPoint2 &d) { x = (float)d.x; y = (float)d.y; }
/**
 * A 2-component vector class, integer.  Useful for representing discrete
 * coordinates and sizes such as pixels and grids.
 */
class IPoint2 {
public:
	IPoint2() {}
	IPoint2(int ix, int iy) { x=ix; y=iy; }

	float Length() const { return sqrtf((float)x*x + (float)y*y); }
	int LengthSqr() const { return (x*x + y*y); } 
	void Set(int ix, int iy) { x=ix; y=iy; }
	IPoint2 &operator=(const IPoint2 &v) { x = v.x; y = v.y; return *this; }

	IPoint2 operator +(const IPoint2 &v) const { return IPoint2(x+v.x, y+v.y); }
	IPoint2 operator -(const IPoint2 &v) const { return IPoint2(x-v.x, y-v.y); }
	IPoint2 operator *(int s) const { return IPoint2(x*s, y*s); }
	IPoint2 operator *(float f) const { return IPoint2((int)(x*f), (int)(y*f)); }

	void operator +=(const IPoint2 &v) { x+=v.x; y+=v.y; }
	void operator -=(const IPoint2 &v) { x-=v.x; y-=v.y; }
	void operator *=(int s) { x*=s; y*=s; }
	void operator *=(float f) { x=(int)(x*f); y=(int)(y*f); }

	bool operator==(const IPoint2 &v) const { return (x == v.x && y == v.y); }
	bool operator!=(const IPoint2 &v) const { return (x != v.x || y != v.y); }

	int x, y;
};

class DPoint3;

/**
* A 3-component vector class, single-precision (float).
*/
class FPoint3
{
public:
	FPoint3() { x = y = z = 0.0f; }
	FPoint3(float fx, float fy, float fz) { x=fx; y=fy; z=fz; }
	FPoint3(const DPoint3 &p);

	float Length() const { return sqrtf(x*x+y*y+z*z); }
	float LengthSquared() const { return x*x+y*y+z*z; }
	FPoint3 &Normalize() { float s = 1.0f/Length(); x*=s; y*=s; z*=s; return (*this); }
	FPoint3 &SetLength(float len) { float s = len/Length(); x*=s; y*=s; z*=s; return (*this); }
	void Set(float fx, float fy, float fz) { x=fx; y=fy; z=fz; }
	float Dot(const FPoint3 &rhs) const
	{
		return x*rhs.x+y*rhs.y+z*rhs.z;
	}
	float Dot(const float *fp) const
	{
		return x*fp[0]+y*fp[1]+z*fp[2];
	}
	FPoint3 Cross(const FPoint3 &v) const
	{
		return FPoint3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	/**
	* Determine the unit cross product (normal vector) to the triangle
	*  made up of the three given points.
	*/
	void UnitNormal(const FPoint3 &p0, const FPoint3 &p1, const FPoint3 &p2)
	{
		FPoint3 edge0 = p1 - p0;
		FPoint3 edge1 = p2 - p0;
		*this = edge0.Cross(edge1);
		this->Normalize();
	}

	// assignment
	FPoint3 &operator=(const FPoint3 &v) { x = v.x; y = v.y; z = v.z; return *this; }
	FPoint3 &operator=(const DPoint3 &v);

	// operators
	FPoint3 operator +(const FPoint3 &v) const { return FPoint3(x+v.x, y+v.y, z+v.z); }
	FPoint3 operator -(const FPoint3 &v) const { return FPoint3(x-v.x, y-v.y, z-v.z); }
	FPoint3 operator *(float s) const { return FPoint3(x*s, y*s, z*s); }
	FPoint3 operator *(double s) const { return FPoint3((float)(x*s), (float)(y*s), (float)(z*s)); }
	FPoint3 operator /(float s) const { return FPoint3(x/s, y/s, z/s); }
	FPoint3 operator -() { return FPoint3(-x, -y, -z); }
	bool operator==(const FPoint3 &v2) const
	{ return (x == v2.x && y == v2.y && z == v2.z); }
	bool operator!=(const FPoint3 &v2) const
	{ return (x != v2.x || y != v2.y || z != v2.z); }

	void operator +=(const FPoint3 &v) { x+=v.x; y+=v.y; z+=v.z; }
	void operator -=(const FPoint3 &v) { x-=v.x; y-=v.y; z-=v.z; }
	void operator *=(float s) { x*=s; y*=s; z*=s; }
	void operator /=(float s) { x/=s; y/=s; z/=s; }
	FPoint3 operator -() const { return FPoint3 (-x, -y, -z); }

	// also allow array-like access, such that x,y,z components are 0,1,2
	float &operator[](int nIndex) { return *(&x+nIndex); }
	const float &operator[](int nIndex) const { return *(&x+nIndex); }

	// add by Hao
	operator DPoint3() const;

	float x, y, z;
};

/**
* A 3-component vector class, double-precision.
*/
class DPoint3
{
public:
	DPoint3() { x = y = z = 0.0f; }
	DPoint3(double fx, double fy, double fz) { x=fx; y=fy; z=fz; }
	DPoint3(const FPoint3 &p);

	double Length() const { return sqrt(x*x+y*y+z*z); }
	double LengthSquared() const { return x*x+y*y+z*z; }
	DPoint3 &Normalize()
	{
		double s = 1.0/Length();
		x*=s; y*=s; z*=s;
		return (*this);
	}
	DPoint3 &SetLength(double len) { double s = len/Length(); x*=s; y*=s; z*=s; return (*this); }
	void Set(double fx, double fy, double fz) { x=fx; y=fy; z=fz; }
	double Dot(const DPoint3 &rhs) const
	{
		return x*rhs.x+y*rhs.y+z*rhs.z;
	}
	DPoint3 Cross(const DPoint3 &v) const
	{
		return DPoint3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}
	// assignment
	DPoint3 &operator=(const DPoint3 &v) { x = v.x; y = v.y; z = v.z; return *this; }
	DPoint3 &operator=(const FPoint3 &v);

	// operators
	DPoint3 operator +(const DPoint3 &v) const { return DPoint3(x+v.x, y+v.y, z+v.z); }
	DPoint3 operator -(const DPoint3 &v) const { return DPoint3(x-v.x, y-v.y, z-v.z); }
	DPoint3 operator *(double s) const { return DPoint3(x*s, y*s, z*s); }
	DPoint3 operator /(double s) const { return DPoint3(x/s, y/s, z/s); }
	bool operator==(const DPoint3 &v2) const
	{ return (x == v2.x && y == v2.y && z == v2.z); }
	bool operator!=(const DPoint3 &v2) const
	{ return (x != v2.x || y != v2.y || z != v2.z); }
	DPoint3 operator -() const { return DPoint3(-x, -y, -z); }

	// dot product
	double operator *(const DPoint3 &v) const { return x*v.x + y*v.y + z*v.z; }

	void operator +=(const DPoint3 &v) { x+=v.x; y+=v.y; z+=v.z; }
	void operator -=(const DPoint3 &v) { x-=v.x; y-=v.y; z-=v.z; }
	void operator *=(double s) { x*=s; y*=s; z*=s; }
	void operator /=(double s) { x/=s; y/=s; z/=s; }
	// also allow array-like access, such that x,y,z components are 0,1,2
	double &operator[](int nIndex) { return *(&x+nIndex); }
	const double &operator[](int nIndex) const { return *(&x+nIndex); }

	// add by Hao
	operator DPoint2() { return DPoint2(x, y); }

	double x, y, z;
};

// Conversion
inline FPoint3::FPoint3(const DPoint3 &v) { x = (float) v.x; y = (float) v.y; z = (float) v.z; }
inline DPoint3::DPoint3(const FPoint3 &v) { x = v.x; y = v.y; z = v.z; }
inline FPoint3 &FPoint3::operator=(const DPoint3 &v) { x = (float) v.x; y = (float) v.y; z = (float) v.z; return *this; }
inline DPoint3 &DPoint3::operator=(const FPoint3 &v) { x = v.x; y = v.y; z = v.z; return *this; }
inline FPoint3::operator DPoint3() const { return DPoint3(x, y, z); }
////////////////////////////////////////////////////////////////

/**
* A class representing an infinite plane, interface single-precision (float).
*/
class FPlane : public FPoint3
{
public:
	typedef enum { COLINEAR, COPLANAR, PARALLEL, FACING_AWAY, INTERSECTING } IntersectionType;

	// Default constructor
	FPlane()
	{
		w = 0.0;
	}
	/// Construct from parametric coefficients
	FPlane(float a, float b, float c, float d) { x = a; y = b; z = c; w = d; }
	/// Construct from three points
	FPlane(const FPoint3& p, const FPoint3& q, const FPoint3& r) {
		float fpx = p.x - r.x;
		float fpy = p.y - r.y;
		float fpz = p.z - r.z;
		float fqx = q.x - r.x;
		float fqy = q.y - r.y;
		float fqz = q.z - r.z;

		x = fpy * fqz - fqy * fpz;
		y = fpz * fqx - fqz * fpx;
		z = fpx * fqy - fqx * fpy;
		w = - x * r.x - y * r.y - z * r.z;
	}
	/// Construct from point and normal vector
	FPlane(const FPoint3& Point, const FPoint3& Normal) {
		x = Normal.x;
		y = Normal.y;
		z = Normal.z;
		w = - x * Normal.x - y * Normal.y - z * Normal.z;
	}

	const FPlane &operator=(const FPlane &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
		return *this;
	}
	void Set(const FPoint3 &p, const FPoint3 &n)
	{
		x = n.x;
		y = n.y;
		z = n.z;
		w = -n.Dot(p);
	}
	void Set(float a, float b, float c, float d) { x = a; y = b; z = c; w = d; }
	float Distance(const FPoint3 &v) const
	{
		return Dot(v) + w;
	}

	/**
	* Find the intersection of two planes. which in the general case is a line.
	* The line is provided as a ray (origin and direction).
	*
	* \return status, which is either INTERSECTING, COPLANAR, or PARALLEL.  In
	*		the case of INTERSECTING, then the Origin and Direction parameters
	*		describe the line of intersection.
	*/
	const IntersectionType Intersection(const FPlane &Plane, FPoint3 &Origin, 
					FPoint3 &Direction, float fEpsilon = 0.0) const {
		double a = x;
		double b = y;
		double c = z;
		double d = w;
		double p = Plane.x;
		double q = Plane.y;
		double r = Plane.z;
		double s = Plane.w;
		double Determinant;

		Determinant = a * q - p * b;
		if (fabs(Determinant - 0.0) > fEpsilon) {
			Origin.Set((float)((b * s - d * q) / Determinant), (float)((p * d - a * s) / Determinant), 0.0f);
			Direction.Set((float)(b * r - c * q), (float)(p * c - a * r), (float)Determinant);
			return INTERSECTING;
		}

		Determinant = a * r - p * c;
		if (fabs(Determinant - 0.0) > fEpsilon) {
			Origin.Set((float)((c * s - d * r) / Determinant), 0.0f, (float)((p * d - a * s) / Determinant));
			Direction.Set((float)(c * q - b * r), (float)Determinant, (float)(p * b - a * q));
			return INTERSECTING;
		}

		Determinant = b * r - c * q;
		if (fabs(Determinant - 0.0) > fEpsilon) {
			Origin.Set(0.0f, (float)((c * s - d * r) / Determinant), (float)((d * q - b * s) / Determinant));
			Direction.Set((float)Determinant, (float)(c * p - a * r), (float)(a * q - b * p));
			return INTERSECTING;
		}

		if (a != 0.0 || p != 0.0) {
			if (fabs(a * s - p * d) <= fEpsilon)
				return COPLANAR;
			else
				return PARALLEL;
		}
		if (b != 0.0 || q != 0.0) {
			if (fabs(b * s - q * d) <= fEpsilon)
				return COPLANAR;
			else
				return PARALLEL;
		}
		if (c != 0.0 || r != 0.0) {
			if (fabs(c * s - r * d) <= fEpsilon)
				return COPLANAR;
			else
				return PARALLEL;
		}
		return PARALLEL;
	}
	
	/**
	* Compute Ray-Plane intersection.
	*
	* \param Origin, Direction	The position and direction that define the ray.
	* \param fDistance			The distance along the ray to the intersection point.
	* \param Intersection		The intersection point.
	* \param fEpsilon			Small value to test for numeric equivalenccy.
	*
	* \return The intersection result (PARALLEL, FACING_AWAY, or INTERSECTING)
	*/
	const IntersectionType RayIntersection(const FPoint3 &Origin, 
					const FPoint3 &Direction, float &fDistance, 
					FPoint3 &Intersection, float fEpsilon = 0.0) const {
		// Intersection of ray with plane
		float NdotV = Dot(Direction);

		if (fabs(0.0 - NdotV) <= fEpsilon)
			return PARALLEL;

		fDistance = -(Dot(Origin) + w) / NdotV;

		if (fDistance < 0.0)
			return FACING_AWAY;

		Intersection = Origin + Direction * fDistance;

		return INTERSECTING;
	}

	/// Intersection of line with plane
	const IntersectionType LineIntersection(const FPoint3 &Origin, const FPoint3 &Direction, 
							FPoint3 &Intersection, float fEpsilon = 0.0) const {
		float Numerator = x * Origin.x + y * Origin.y + z * Origin.z + w;
		float Denominator = x * Direction.x + y * Direction.y + z * Direction.z;

		if (fabs(0.0 - Denominator) <= fEpsilon)
		{
			if (fabs(0.0 - Numerator) <= fEpsilon)
				return COPLANAR;
			else
				return PARALLEL;
		}
		Intersection.x = (Denominator * Origin.x - Numerator * Direction.x) / Denominator;
		Intersection.y = (Denominator * Origin.y - Numerator * Direction.y) / Denominator;
		Intersection.z = (Denominator * Origin.z - Numerator * Direction.z) / Denominator;
		return INTERSECTING;
	}

	/// Intersection of three planes
	const IntersectionType ThreePlanesIntersection(const FPlane &Plane1, 
							const FPlane &Plane2, FPoint3 &Intersection, 
							float fEpsilon = 0.0) const {
		FPoint3 Origin;
		FPoint3 Direction;

		IntersectionType Type = Plane1.Intersection(Plane2, Origin, Direction, fEpsilon);

		if (INTERSECTING == Type) {
			// Planes have an intersection line
			Type = LineIntersection(Origin, Direction, Intersection, fEpsilon);
			return Type;
		}
		else
			return Type;
	}

	float w;
};

class IPoint3
{
public:
	IPoint3() {}
	IPoint3(int i, int j, int k) { x = i; y = j; z = k; }
	~IPoint3() {}

	int x;
	int y;
	int z;
};

#endif // _PRIMITIVE_H