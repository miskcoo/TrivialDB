#ifndef __TRIVIALDB_DBMS__
#define __TRIVIALDB_DBMS__
#include "database.h"
#include "../table/table.h"
#include "../parser/defs.h"

class dbms
{
	database *cur_db;
private:
	dbms();

public:
	~dbms();

	void close_database();
	void show_database(const char *db_name);
	void switch_database(const char *db_name);
	void drop_database(const char *db_name);
	void create_database(const char *db_name);

	void create_table(const table_header_t *header);
	void insert_rows(const insert_info_t *info);

public:
	bool assert_db_open();
	

public:
	static dbms* get_instance()
	{
		static dbms ms;
		return &ms;
	}
};

#endif
