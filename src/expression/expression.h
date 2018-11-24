#ifndef __TRIVIALDB_EXPRESSION__
#define __TRIVIALDB_EXPRESSION__

#include "../parser/defs.h"

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
};

#endif
