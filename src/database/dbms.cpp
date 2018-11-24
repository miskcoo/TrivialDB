#include "dbms.h"
#include "database.h"
#include "../table/table.h"
#include "../expression/expression.h"
#include "../utils/type_cast.h"
#include <vector>

dbms::dbms()
	: cur_db(nullptr)
{
}

dbms::~dbms()
{
	close_database();
}

void dbms::close_database()
{
	if(cur_db) 
	{
		cur_db->close();
		delete cur_db;
		cur_db = nullptr;
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
	if(assert_db_open())
		cur_db->create_table(header);
}

void dbms::insert_rows(const insert_info_t *info)
{
	if(!assert_db_open())
		return;
	table_manager *tb = cur_db->get_table(info->table);
	if(tb == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` not found.\n", info->table);
		return;
	}

	std::vector<int> cols_id;
	if(info->columns == nullptr)
	{
		// exclude __rowid__, which has the largest index
		for(int i = 0; i < tb->get_column_num() - 1; ++i)
			cols_id.push_back(i);
	} else {
		// TODO: parse column infos
	}

	tb->init_temp_record();
	int count_succ = 0, count_fail = 0;
	for(linked_list_t *list = info->values; list; list = list->next)
	{
		linked_list_t *expr_list = (linked_list_t*)list->data;
		unsigned val_num = 0;
		for(linked_list_t *i = expr_list; i; i = i->next, ++val_num);
		if(val_num != cols_id.size())
		{
			std::fprintf(stderr, "[Error] column size not equal.");
			continue;
		}

		bool succ = true;
		for(auto it = cols_id.begin(); expr_list; expr_list = expr_list->next, ++it)
		{
			expression v;
			try {
				v = expression::eval((expr_node_t*)expr_list->data);
			} catch (const char *e) {
				std::fprintf(stderr, "%s\n", e);
				return;
			}

			auto col_type = tb->get_column_type(*it);
			// TODO: check column type
			
			term_type_t desired_type = typecast::column_to_term(col_type);
			char *db_val = typecast::expr_to_db(v, desired_type);
			if(!tb->set_temp_record(*it, db_val))
			{
				succ = false;
				break;
			}
		}

		if(succ) succ = (tb->insert_record() > 0);
		count_succ += succ;
		count_fail += 1 - succ;
	}

	std::printf("[Info] %d row(s) inserted, %d row(s) failed.\n", count_succ, count_fail);
}

bool dbms::assert_db_open()
{
	if(cur_db && cur_db->is_opened())
		return true;
	std::fprintf(stderr, "[Error] database is not opened.\n");
	return false;
}
