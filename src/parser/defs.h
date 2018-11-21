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

typedef struct field_item_t {
	char *name;
	int type, width, flags;
	struct field_item_t *next;
} field_item_t;

typedef struct table_def_t {
	char *name;
	struct field_item_t *fields;
} table_def_t;

#ifdef __cplusplus
};
#endif

#endif
