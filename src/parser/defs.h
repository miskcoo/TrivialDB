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

typedef enum {
	TABLE_CONSTRAINT_PRIMARY_KEY,
	TABLE_CONSTRAINT_FOREIGN_KEY,
	TABLE_CONSTRAINT_CHECK
} table_constraint_type_t;

typedef enum {
	OPERATOR_NONE = 0,
	OPERATOR_ADD,
	OPERATOR_MINUS,
	OPERATOR_DIV,
	OPERATOR_MUL,
	OPERATOR_NEGATE
} operator_type_t;

typedef enum {
	TERM_NONE = 0,
	TERM_COLUMN_REF,
	TERM_INT,
	TERM_STRING,
	TERM_FLOAT,
	TERM_NULL
} term_type_t;

typedef struct field_item_t {
	char *name;
	int type, width, flags;
	struct field_item_t *next;
} field_item_t;

typedef struct table_constraint_t {
	int type;
	void *values;
} table_constraint_t;

typedef struct linked_list_t {
	void *data;
	struct linked_list_t *next;
} linked_list_t;

typedef struct column_ref_t {
	char *table;
	char *column;
} column_ref_t;

typedef struct table_def_t {
	char *name;
	struct field_item_t *fields;
	struct linked_list_t *constraints;
} table_def_t;

typedef struct insert_info_t {
	char *table;
	linked_list_t *columns, *values;
} insert_info_t;

typedef struct expr_node_t {
	union {
		int    val_i;
		float  val_f;
		char  *val_s;
		struct column_ref_t *column_ref;
		struct expr_node_t  *left;
	};
	struct expr_node_t *right;
	operator_type_t op;
	term_type_t term_type;
} expr_node_t;

#ifdef __cplusplus
};
#endif

#endif
