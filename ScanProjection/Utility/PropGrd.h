#ifndef _PROP_GRD_H
#define _PROP_GRD_H

#include <fstream>
#include "primitive.h"

using namespace std;

template <typename T>
class PropGrd
{
public:
	PropGrd();
	~PropGrd();
	PropGrd(double res, const DPoint2 &extMin, const DPoint2 &extRange) 
	{ init(res, extMin, extRange); }

	void init(int widthX, int widthY);
	void init(double res, const DPoint2 &extMin, const DPoint2 &extRange);
	void destroy();

	// access
	inline T& operator() (int i, int j)
	{ return m_grd[make_offset(i, j)]; }
	inline const T& operator() (const DPoint2 &pos) const
	{ return GetProperty(pos); }

	const T& GetProperty(int i, int j) const;
	const T& GetProperty(const DPoint2 &pos) const;
	void GetIndex(const DPoint2 &pos, int &i, int &j) const;

	// resolution
	double GetRes() const { return m_res; }
	int GetWidthX() const { return m_widthX; }
	int GetWidthY() const { return m_widthY; }

	// serialize
	friend ofstream& operator << (ofstream &os, const PropGrd &propgrd) {
		os.write((char *) &propgrd.m_res, sizeof(double));
		os.write((char *) &propgrd.m_ExtentsMin, sizeof(DPoint2));
		os.write((char *) &propgrd.m_ExtentsRange, sizeof(DPoint2));
		os.write((char *) propgrd.m_grd, sizeof(T) * propgrd.m_size);
		return os;
	}

	friend ifstream& operator >> (ifstream &is, PropGrd &propgrd) {
		is.read((char *) &propgrd.m_res, sizeof(double));
		is.read((char *) &propgrd.m_ExtentsMin, sizeof(DPoint2));
		is.read((char *) &propgrd.m_ExtentsRange, sizeof(DPoint2));
		propgrd.init(propgrd.m_res, propgrd.m_ExtentsMin, 
					 propgrd.m_ExtentsRange);
		is.read((char *) propgrd.m_grd, sizeof(T) * propgrd.m_size);
		return is;
	}

	// dangerous access
	T * get_grd_ptr() { return m_grd; } 

#ifdef _DEBUG
	void RenderGrd(const char* path);
#endif	// _DEBUG

protected:
	inline int make_offset(int i, int j) const { return m_widthX * j + i; }

	T *m_grd;
	int m_widthX;
	int m_widthY;
	double m_res;
	DPoint2 m_ExtentsMin;
	DPoint2 m_ExtentsRange;
	DPoint2 m_convertRatio;
	int m_size;
};

///////////////////////////////////////////////////////////////////////////////
// class PropGrd
template <typename T>
PropGrd<T>::PropGrd()
{
	m_grd = NULL;
	m_res = 0.0;
	m_widthX = 0;
	m_widthY = 0;
}

template <typename T>
PropGrd<T>::~PropGrd()
{
	destroy();
}

template <typename T>
void PropGrd<T>::init(int widthX, int widthY)
{
	m_widthX = widthX;
	m_widthY = widthY;

	m_size = widthX * widthY;
	m_grd = new T[m_size];
	memset(m_grd, 0, m_size * sizeof(T));
}

template <typename T>
void PropGrd<T>::init(double res, const DPoint2 &extMin, const DPoint2 &extRange)
{
	m_res = res;
	m_ExtentsMin = extMin;
	m_ExtentsRange = extRange;

	m_widthX = (int)(extRange.x / res + 0.5);
	m_widthY = (int)(extRange.y / res + 0.5);
	m_convertRatio = DPoint2((m_widthX-1.0) / extRange.x, 
		(m_widthY-1.0) / extRange.y);

	m_size = m_widthX * m_widthY;
	if (m_grd != NULL)
		delete[] m_grd;
	m_grd = new T[m_size];
	memset(m_grd, 0, m_size * sizeof(T));
}

template <typename T>
void PropGrd<T>::destroy()
{
	if(m_grd != NULL)
		delete[] m_grd;
	m_grd = NULL;
	m_res = 0.0;
	m_widthX = 0;
	m_widthY = 0;
}

template <typename T>
const T& PropGrd<T>::GetProperty(int i, int j) const
{
	int offset = make_offset(i, j);
	return m_grd[offset];
}

template <typename T>
const T& PropGrd<T>::GetProperty(const DPoint2 &pos) const
{
	DPoint2 localPos = pos - m_ExtentsMin; 
	int i = (int)(localPos.x * m_convertRatio.x);
	int j = (int)(localPos.y * m_convertRatio.y);

	int offset = make_offset(i, j);
	//assert(offset >= 0 && offset < m_widthX*m_widthY);
	return m_grd[offset];
}

template <typename T>
void PropGrd<T>::GetIndex(const DPoint2 &pos, int &i, int &j) const
{
	DPoint2 localPos = pos - m_ExtentsMin; 
	i = (int)(localPos.x * m_convertRatio.x);
	j = (int)(localPos.y * m_convertRatio.y);
}

#ifdef _DEBUG
template <typename T>
void PropGrd<T>::RenderGrd(const char* path)
{
	T max_val = m_grd[0];
	T min_val = m_grd[0];
	for (int i = 0; i < m_size; i++) {
		T val = m_grd[i];
		if (max_val < val)
			max_val = val;
		if (min_val > val)
			min_val = val;
	}
	double scale = 255.0 / (max_val-min_val);
	unsigned char* intensity = new unsigned char[m_size];
	for (int i = 0; i < m_size; i++) {
		intensity[i] = (unsigned char) (scale * (m_grd[i]-min_val));
	}

	SaveGrayBmp(path, intensity, m_widthX, m_widthY, 24);
	delete [] intensity;
}
#endif	// _DEBUG

#endif // _PROP_GRD_H