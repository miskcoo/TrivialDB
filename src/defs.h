#ifndef __TRIVIALDB_DEFS__
#define __TRIVIALDB_DEFS__

/* filesystem */
#ifdef SMALL_PAGE_FOR_DEBUG
#define PAGE_SIZE 0x10
#define PAGE_CACHE_CAPACITY 8
#else
#define PAGE_SIZE 4096
#define PAGE_CACHE_CAPACITY 1024
#endif

#define MAX_FILE_ID 1024

/* page info */
#define PAGE_FREEBLOCK  0x45455246
#define PAGE_BLOCK_MIN_NUM   4
#define PAGE_BLOCK_MAX_SIZE  ((PAGE_SIZE - 12) / PAGE_BLOCK_MIN_NUM - 2)
#define PAGE_OV_KEEP_SIZE    64
#define PAGE_FREE_BLOCK_MIN_SIZE 16

/* page type (2 bytes) */
#define PAGE_SMALL      0x4c53
#define PAGE_LARGE      0x474c
#define PAGE_LARGE_OV   0x4f4c
#define PAGE_VARIANT    0x5456
#define PAGE_OVERFLOW   0x564f

/* table info */
#define MAX_COL_NUM     32
#define MAX_NAME_LEN    64

#define COL_FLAG_PRIMARY   1
#define COL_FLAG_INDEX     2
#define COL_FLAG_NOTNULL   4
#define COL_FLAG_AUTOINC   8

#define COL_TYPE_INT       1
#define COL_TYPE_BIGINT    2
#define COL_TYPE_FLOAT     3
#define COL_TYPE_DOUBLE    4
#define COL_TYPE_CHAR      5
#define COL_TYPE_VARCHAR   30

/* debug */
#ifndef NDEBUG
#include <cstdio>
#define debug_printf(s, ...)  std::fprintf(stderr, s, __VA_ARGS__)
#else
#define debug_printf(s, ...)  
#endif

#endif
