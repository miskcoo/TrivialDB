#ifndef __TRIVIALDB_TABLE__
#define __TRIVIALDB_TABLE__
#include <stdint.h>

#include "../defs.h"

struct table_header_t
{
	// the number of all columns
	uint8_t col_num;
	// the number of size-variant columns
	uint8_t variant_field_num;
	// main index for this table
	uint8_t main_index;

	uint8_t col_flags[MAX_COL_NUM];
	uint8_t col_type[MAX_COL_NUM];

	// the length of columns, 0 when the field is size-variant
	int col_length[MAX_COL_NUM];
	// the offset of columns, 0 when the field is size-variant
	int col_offset[MAX_COL_NUM];
	// root page of index
	int index_root[MAX_COL_NUM];
	// auto increment counter
	int64_t auto_inc[MAX_COL_NUM];

	char col_name[MAX_COL_NUM][MAX_NAME_LEN];
	char table_name[MAX_NAME_LEN];
};

#endif
