#ifndef __TRIVIALDB_PARSER_DEF__
#define __TRIVIALDB_PARSER_DEF__

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	FIELD_TYPE_INT,
	FIELD_TYPE_FLOAT,
	FIELD_TYPE_CHAR,
	FIELD_TYPE_VARCHAR
} field_type_t;

typedef enum {
	FIELD_FLAG_NOTNULL = 1
} field_flag_t;

struct field_item_t {
	char *name;
	int type, width, flags;
	field_item_t *next;
};

struct table_def_t {
	char *name;
	field_item_t *fields;
};

#ifdef __cplusplus
};
#endif

#endif
