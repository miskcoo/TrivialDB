#ifndef __TRIVIALDB_EXPRESSION__
#define __TRIVIALDB_EXPRESSION__

#include "../parser/defs.h"
#include <string>

struct expression
{
	union {
		char *val_s;
		int   val_i;
		float val_f;
		bool  val_b;
	};

	term_type_t type;

	static expression eval(const expr_node_t *expr);
	static void cache_clear();
	static void cache_clear(const char *table);
	static void cache_column(const char *table, const char *col, const expression &expr);
};

#endif
