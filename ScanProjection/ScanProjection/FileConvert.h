#pragma once

#include<iostream>
#include<float.h>
#include <fstream>
#include <sstream>

#include "../Utility/kd_tree.h"
#include "../Utility/primitive.h"
#include "../Utility/Feature.h"

#define Fields Feature

using namespace std;

int getWords(char *str, char *words[], const char delim)
{
	int count = 0;
	char *pch = str;

	while (true) {
		words[count] = pch;
		while (*pch != delim && *pch != '\0')
			pch++;

		if (*pch == '\0')
			break;

		*pch = '\0';
		pch++;
		count++;
	}

	return ++count;
}

void replace(string &str, char src, char dest)
{
	int length = str.size();
	for (int i = 0; i < length; i++)
		if (str[i] == src)
			str[i] = dest;
}

template <int K, class T>
void readPro(string path, int nVertices, vector<Fields<K, T>> &pro)
{
	ifstream ifs(path);
	string strline;
	int count = 0;
	while (getline(ifs, strline) && count<nVertices)
	{
		replace(strline, ',', ' ');
		istringstream is(strline);
		Fields<K, T> tempF;
		string word;
		int i = 0;
		while (is >> word && i<K)
		{
			tempF[i++] = stod(word);
		}
		pro.push_back(tempF);
		count++;
	}
	ifs.close();
}

// Hao's version
template <int K, class T>
bool readPro(const char *path, vector<Fields<K, T>> &pro)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	int count = 0;
	int buffSize = 1024;
	char *buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;

	pro.reserve(count);

	ifs.close();
	ifs.clear();

	ifs.open(path);

	char *words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		Fields<K, T> ftr;
		for (int k = 0; k < K; k++)
			ftr[k] = atof(words[k]);

		pro.push_back(ftr);
	}
	ifs.close();

	return true;
}

template <int K, class T>
void readPro(string path, int nVertices, Fields<K, T> *prop)
{
	ifstream ifs(path);
	string strline;

	int count = 0;
	while (getline(ifs, strline) && count<nVertices)
	{
		replace(strline, ',', ' ');
		istringstream is(strline);
		Fields<K, T> tempF;
		string word;
		int i = 0;
		while (is >> word && i<K)
		{
			tempF[i] = stod(word);
			i = i + 1;
		}
		prop[count] = tempF;
		count++;
	}
	ifs.close();
}

template<int K, class T>
void readPandPro(string path, int nVertices, vector<DPoint3>&points, vector<Fields<K, T>>&pro)
{
	ifstream ifs(path);
	string strline;
	int count = 0;

	points.reserve(nVertices);
	pro.reserve(nVertices);

	while (getline(ifs, strline))
	{
		replace(strline, ',', ' ');
		istringstream is(strline);

		string word;
		DPoint3 tmpP;
		is >> word; tmpP.x = stod(word);
		is >> word; tmpP.y = stod(word);
		is >> word; tmpP.z = stod(word);
		points.push_back(tmpP);

		Fields<K, T> tempF;
		int i = 0;
		while (is >> word && i<K)
		{
			tempF[i++] = stod(word);
		}
		pro.push_back(tempF);
		count++;
	}
	ifs.close();
}

// Hao's version
template<int K, class T>
bool readPandPro(const char *path, vector<DPoint3>&points, vector<Fields<K, T>>&pro)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	string strline;
	int count = 0;

	int buffSize = 1024;
	char *buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;
	
	ifs.close();
	ifs.clear();

	ifs.open(path);

	points.reserve(count);
	pro.reserve(count);

	char *words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		double xx = atof(words[0]);
		double yy = atof(words[1]);
		double zz = atof(words[2]);
		points.push_back(DPoint3(xx, yy, zz));

		Fields<K, T> ftr;
		for (int k = 0; k < K; k++)
			ftr[k] = atof(words[2 + k]);

		pro.push_back(ftr);
	}
	ifs.close();

	return true;
}

#if 0
void readP(string path, vector<DPoint3>&points)
{
	ifstream ifs(path);
	string strline;
	int count = 0;
	while (getline(ifs, strline))
	{
		if (strline[0] == ' ')
			break;
		replace(strline, ',', ' ');
		istringstream is(strline);
		string word;
		DPoint3 tmpP;
		is >> word; tmpP.x = stod(word);
		is >> word; tmpP.y = stod(word);
		is >> word; tmpP.z = stod(word);
		points.push_back(tmpP);
	}
	ifs.close();
}
#endif

// Hao's version
bool readP(const char *path, vector<DPoint3> &voxel)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	string strline;
	int count = 0;

	int buffSize = 1024;
	char *buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;

	ifs.close();
	ifs.clear();

	ifs.open(path);

	voxel.reserve(count);

	char *words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		double xx = atof(words[0]);
		double yy = atof(words[1]);
		double zz = atof(words[2]);
		voxel.push_back(DPoint3(xx, yy, zz));
	}
	ifs.close();

	return true;
}

bool readP(const char *path, vector<DPoint3> &voxel, vector<DPoint3> &normal)
{
	ifstream ifs(path);
	if (!ifs)
		return false;

	string strline;
	int count = 0;

	int buffSize = 1024;
	char *buff = new char[buffSize];
	while (ifs.getline(buff, buffSize))
		count++;

	ifs.close();
	ifs.clear();

	ifs.open(path);

	voxel.reserve(count);
	normal.reserve(count);

	char *words[32];
	while (ifs.getline(buff, buffSize)) {
		getWords(buff, words, ',');

		double xx = atof(words[0]);
		double yy = atof(words[1]);
		double zz = atof(words[2]);
		voxel.push_back(DPoint3(xx, yy, zz));

		double nx = atof(words[4]);
		double ny = atof(words[5]);
		double nz = atof(words[6]);
		normal.push_back(DPoint3(nx, ny, nz));

	}
	ifs.close();

	return true;
}

template<int K, class T>
void normalizePro(vector<Fields<K, T>>&pro)
{
	int nVertices = pro.size();
	vector<double>minv(K, DBL_MAX);
	vector<double>maxv(K, 0);
	for (int i = 0; i < nVertices; i++)
	{
		for (int j = 0; j < K; j++)
		{
			if (pro[i][j] < minv[j])
				minv[j] = pro[i][j];
			if (pro[i][j] > maxv[j])
				maxv[j] = pro[i][j];
		}
	}
	for (int j = 0; j < K; j++)
	{
		double dif = maxv[j] - minv[j];
		for (int i = 0; i < nVertices; i++)
		{
			pro[i][j] = (pro[i][j] - minv[j]) / dif;
		}
	}
}

void readP(string path, DPoint3*points,int nVertices)
{
	ifstream ifs(path);
	string strline;
	int count = 0;
	while (getline(ifs, strline) && count<nVertices)
	{
		if (strline[0] == ' ')
			break;
		replace(strline, ',', ' ');
		istringstream is(strline);
		string word;
		DPoint3 tmpP;
		is >> word; tmpP.x = stod(word);
		is >> word; tmpP.y = stod(word);
		is >> word; tmpP.z = stod(word);
		points[count]=tmpP;
		count++;
	}
	ifs.close();
}

template <int K, class T>
void writePro(string path, vector<DPoint3> &points, vector<Fields<K, T>> &pro)
{
	int nVertices = points.size();
	ofstream ofs(path);
	for (int i = 0; i < nVertices; i++)
	{
		ofs << points[i].x << "," << points[i].y << "," << points[i].z;
		for (int j = 0; j < K; j++)
			ofs << "," << pro[i][j];
		ofs << endl;
	}
	ofs.close();
}

template <int K, class T>
void writePro(string path, int nVertices, Fields<K, T> *pro)
{

	ofstream ofs(path);
	for (int i = 0; i < nVertices; i++)
	{
		for (int j = 0; j < K - 1; j++)
			ofs << pro[i][j] << ",";
		ofs << pro[i][K - 1];
		ofs << endl;
	}
	ofs.close();
}

template <int K, class T>
void writePro(string path, vector<Fields<K, T>> &pro)
{
	int nVertices = pro.size();
	ofstream ofs(path);
	for (int i = 0; i < nVertices; i++)
	{
		for (int j = 0; j < K - 1; j++)
			ofs << pro[i][j] << ",";
		ofs << pro[i][K - 1];
		ofs << endl;
	}
	ofs.close();
}
