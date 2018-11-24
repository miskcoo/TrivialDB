#ifndef __TRIVIALDB_UTILS__
#define __TRIVIALDB_UTILS__

#include <cstring>

template<typename T>
inline int basic_type_comparer(T x, T y)
{
	if(x < y) return -1;
	return x == y ? 0 : 1;
}

inline int integer_comparer(int x, int y)
{
	return basic_type_comparer(x, y);
}

inline int float_comparer(float x, float y)
{
	return basic_type_comparer(x, y);
}

inline int integer_bin_comparer(const char *x, const char *y)
{
	return integer_comparer(*(int*)x, *(int*)y);
}

inline int float_bin_comparer(const char *x, const char *y)
{
	return float_comparer(*(float*)x, *(float*)y);
}

inline int string_comparer(const char *x, const char *y)
{
	return std::strcmp(x, y);
}

#endif
