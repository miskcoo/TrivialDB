#ifndef __TRIVIALDB_DATABASE__
#define __TRIVIALDB_DATABASE__
#include "../defs.h"
#include "../table/table.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

class database
{
	struct database_info
	{
		int table_size;
		char db_name[MAX_NAME_LEN];
		char table_name[MAX_TABLE_NUM][MAX_NAME_LEN];
	} info;

	bool opened;
public:
	database() : opened(false) { }
	~database() { close(); }
	bool is_opened() { return opened; }
	void open(const char *db_name);
	void create(const char *db_name);
	void close();
};

#endif
