#ifndef __TRIVIALDB_DBMS_TYPECAST__
#define __TRIVIALDB_DBMS_TYPECAST__
#include <cassert>
#include "../expression/expression.h"
#include "../defs.h"
#include "../parser/defs.h"

namespace typecast
{
	inline term_type_t column_to_term(int type)
	{
		switch(type)
		{
			case COL_TYPE_INT:
				return TERM_INT;
			case COL_TYPE_FLOAT:
				return TERM_FLOAT;
			case COL_TYPE_VARCHAR:
				return TERM_STRING;
			default:
				throw "[Error] wrong datatype.";
		}
	}

	inline char *expr_to_db(expression &expr, term_type_t desired)
	{
		char *ret = nullptr;
		switch(expr.type)
		{
			case TERM_INT:
				if(desired == TERM_FLOAT)
				{
					expr.val_f = expr.val_i;
					ret = (char*)&expr.val_f;
				} else {
					ret = (char*)&expr.val_i;
				}
				break;
			case TERM_FLOAT:
				if(desired == TERM_INT)
				{
					expr.val_i = expr.val_f;
					ret = (char*)&expr.val_i;
				} else {
					ret = (char*)&expr.val_f;
				}
				break;
			case TERM_STRING:
				ret = expr.val_s;
				break;
			case TERM_NULL:
				ret = nullptr;
				break;
			default:
				throw "[Error] unknown type.";
		}

		return ret;
	}

	inline bool type_compatible(int col_type, const expression &val)
	{
		switch(val.type)
		{
			case TERM_NULL:
				return true;
			case TERM_INT:
				return col_type == COL_TYPE_INT || col_type == COL_TYPE_FLOAT;
			case TERM_FLOAT:
				return col_type == COL_TYPE_FLOAT;
			case TERM_STRING:
				return col_type == COL_TYPE_VARCHAR;
			default:
				return false;
		}
	}
}

#endif
