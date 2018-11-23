#include "dbms.h"
#include "database.h"
#include "../table/table.h"

dbms::dbms()
	: cur_db(nullptr)
{
}

dbms::~dbms()
{
	if(cur_db) 
	{
		cur_db->close();
		delete cur_db;
	}
}

void dbms::switch_database(const char *db_name)
{
	if(cur_db)
	{
		cur_db->close();
		delete cur_db;
		cur_db = nullptr;
	}

	cur_db = new database();
	cur_db->open(db_name);
}

void dbms::create_database(const char *db_name)
{
	database db;
	db.create(db_name);
	db.close();
}

void dbms::drop_database(const char *db_name)
{
}

void dbms::show_database(const char *db_name)
{
}

void dbms::create_table(const table_header_t *header)
{
	if(cur_db)
	{
		cur_db->create_table(header);
	} else {
		// TODO: report error
	}
}
