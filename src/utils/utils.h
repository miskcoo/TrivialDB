#ifndef __TRIVIALDB_UTILS__
#define __TRIVIALDB_UTILS__

#include <cstring>

inline int integer_comparer(int x, int y)
{
	if(x < y) return -1;
	return x == y ? 0 : 1;
}

inline int string_comparer(const char *x, const char *y)
{
	return std::strcmp(x, y);
}

#endif
