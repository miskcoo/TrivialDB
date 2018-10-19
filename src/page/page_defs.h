#ifndef __TRIVIALDB_PAGE_DEFS__
#define __TRIVIALDB_PAGE_DEFS__

#include <stdint.h>
#include "../defs.h"

#define PAGE_FIELD_REF(name, type, offset) \
public: \
	type name() { return *reinterpret_cast<type*>(buf + offset); } \
private: \
	type& name##_ref() { return *reinterpret_cast<type*>(buf + offset); }

#define PAGE_FIELD_PTR(name, type, offset) \
	type* name() { return reinterpret_cast<type*>(buf + offset); }

class pager;

struct general_page
{
	char *const buf;
	pager *const pg;
	general_page(char *buf, pager *pg)
		: buf(buf), pg(pg) {}
};

#endif
