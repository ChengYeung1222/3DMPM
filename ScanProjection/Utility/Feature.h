#ifndef _FEATURE_H
#define _FEATURE_H

// for properties
template <int K, class T>
class Feature
{
public:
	Feature() {}

	Feature(const Feature<K, T> &ftr) { *this = ftr; }
	template <class S> Feature(S v) { *this = v; }

	inline T& operator[] (int i) { return values[i]; }
	inline const T& operator[] (int i) const { return values[i]; }

	//Feature<K, T> &operator = (const Feature<K, T> &rhs);

	template <class S>
	Feature<K, T> &operator = (S v);

	T values[K];
};

template <int K, class T>
template <class S>
Feature<K, T> & Feature<K, T>::operator = (S v)
{
	for (int i = 0; i < K; i++)
		values[i] = v;
	return *this;
}

#if 0
template <int K, class T>
Feature<K, T> & Feature<K, T>::operator = (const Feature<K, T> &rhs)
{
	if (this != &rhs)
		memcpy(this, &rhs, sizeof(T)*K);
	return *this;
}
#endif

#define ArithmeticOp(op)													\
template <int K, class T>													\
Feature<K, T> operator op(const Feature<K, T> &l, const Feature<K, T> &r)	\
{																			\
	Feature<K, T> ret;														\
	for (int i = 0; i < K; i++)												\
		ret[i] = l[i] op r[i];												\
	return ret;																\
}													
																		
ArithmeticOp(+)
ArithmeticOp(-)
ArithmeticOp(*)
ArithmeticOp(/ )

#define ArithmeticOpV(op)													\
template <int K, class T>													\
Feature<K, T> operator op(const Feature<K, T> &l, T v)						\
{																			\
	Feature<K, T> ret;														\
	for (int i = 0; i < K; i++)												\
		ret[i] = l[i] op v;													\
	return ret;																\
}

ArithmeticOpV(+)
ArithmeticOpV(-)
ArithmeticOpV(*)
ArithmeticOpV(/)

#define CompoundAssignOp(op)												\
template <int K, class T>													\
Feature<K, T>& operator op(Feature<K, T> &l, const Feature<K, T> &r)		\
{																			\
	for (int i = 0; i < K; i++)												\
		l[i] op r[i];														\
	return l;																\
}

CompoundAssignOp(+=)
CompoundAssignOp(-=)
CompoundAssignOp(*=)
CompoundAssignOp(/=)

#define CompoundAssignOpV(op)												\
template <int K, class T, class S>											\
Feature<K, T>& operator op(Feature<K, T> &l, S v)							\
{																			\
	for (int i = 0; i < K; i++)												\
		l[i] op v;															\
	return l;																\
}

CompoundAssignOpV(+=)
CompoundAssignOpV(-=)
CompoundAssignOpV(*=)
CompoundAssignOpV(/=)

#endif // _FEATURE_H
