#ifndef __TRIVIALDB_DBMS__
#define __TRIVIALDB_DBMS__
#include "database.h"
#include "../table/table.h"
#include "../parser/defs.h"
#include "../expression/expression.h"

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

	void create_index(const char *tb_name, const char *col_name);
	void drop_index(const char *tb_name, const char *col_name);

	void insert_rows(const insert_info_t *info);
	void delete_rows(const delete_info_t *info);
	void select_rows(const select_info_t *info);
	void update_rows(const update_info_t *info);

public:
	bool assert_db_open();
	void cache_record(table_manager *tm, record_manager *rm);
	

	template<typename Callback>
	void iterate(std::vector<table_manager*> required_tables, expr_node_t *cond, Callback callback);

	template<typename Callback>
	void iterate_one_table(table_manager* table, expr_node_t *cond, Callback callback);

public:
	static dbms* get_instance()
	{
		static dbms ms;
		return &ms;
	}
};

#endif
