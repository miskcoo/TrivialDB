#ifndef __TRIVIALDB_DEFS__
#define __TRIVIALDB_DEFS__

/* filesystem */
#define PAGE_SIZE 4096
#define PAGE_CACHE_CAPACITY 1024
#define MAX_FILE_ID 1024

/* page info */
#define PAGE_FREEBLOCK  0x45455246
#define PAGE_BLOCK_MIN_NUM   4
#define PAGE_BLOCK_MAX_SIZE  ((PAGE_SIZE - 12) / PAGE_BLOCK_MIN_NUM - 2)
#define PAGE_OV_KEEP_SIZE    64
#define PAGE_FREE_BLOCK_MIN_SIZE 16
#define PAGE_FREE_SPACE_MAX  (PAGE_SIZE / 4 * 3)

/* page type (2 bytes) */
#define PAGE_FIXED      0x4946
#define PAGE_INDEX_LEAF 0x4947
#define PAGE_VARIANT    0x4156
#define PAGE_OVERFLOW   0x564f

/* table info */
#define MAX_COL_NUM     32
#define MAX_NAME_LEN    64

#define COL_FLAG_PRIMARY   1
#define COL_FLAG_INDEX     2
#define COL_FLAG_NOTNULL   4
#define COL_FLAG_AUTOINC   8

#define COL_TYPE_INT       1
#define COL_TYPE_FLOAT     3
// Note the actual data has one more zero byte
#define COL_TYPE_VARCHAR   5

/* debug */
#ifndef NDEBUG
#include <cstdio>
#define debug_printf(s, ...)  std::fprintf(stderr, s, __VA_ARGS__)
#define debug_puts(s) std::fprintf(stderr, "%s\n", s)
#else
#define debug_printf(s, ...)  
#define debug_puts(s) 
#endif

#define UNUSED(x) ((void)(x))

#endif
