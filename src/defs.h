#ifndef __TRIVIALDB_DEFS__
#define __TRIVIALDB_DEFS__

#ifdef SMALL_PAGE_FOR_DEBUG
#define PAGE_SIZE 0x10
#define PAGE_CACHE_CAPACITY 8
#else
#define PAGE_SIZE 4096
#define PAGE_CACHE_CAPACITY 1024
#endif

#define MAX_FILE_ID 1024

#define PAGE_FREEBLOCK  0x45455246

#ifndef NDEBUG
#include <cstdio>
#define debug_printf(s, ...)  std::fprintf(stderr, s, __VA_ARGS__)
#else
#define debug_printf(s, ...)  
#endif

#endif
