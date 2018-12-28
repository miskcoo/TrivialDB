#include "dbms.h"
#include "database.h"
#include "../table/table.h"
#include "../index/index.h"
#include "../expression/expression.h"
#include "../utils/type_cast.h"
#include "../table/record.h"
#include <vector>

dbms::dbms()
	: cur_db(nullptr)
{
}

dbms::~dbms()
{
	close_database();
}

template<typename Callback>
void dbms::iterate(
	std::vector<table_manager*> required_tables,
	expr_node_t *cond,
	Callback callback)
{
	if(required_tables.size() == 1)
	{
		std::vector<record_manager*> rm_list(1);
		std::vector<int> rid_list(1);
		iterate_one_table(required_tables[0], cond, [&](table_manager *, record_manager *rm, int rid) -> bool {
			rm_list[0] = rm;
			rid_list[0] = rid;
			return callback(required_tables, rm_list, rid_list);
		} );
	} else if(required_tables.size() == 2) {
		if(iterate_two_tables_with_joint_cond_equal(required_tables[0], required_tables[1], cond, callback))
		{
			std::puts("[Info] Join two tables using index.");
		} else {
			iterate_many_tables(required_tables, cond, callback);
			std::puts("[Info] Join two tables by enumerating.");
		}
	} else {
		iterate_many_tables(required_tables, cond, callback);
		std::puts("[Info] Join many tables by enumerating.");
	}
}

template<typename Callback>
bool dbms::iterate_two_tables_with_joint_cond_equal(
		table_manager* tb1,
		table_manager* tb2,
		expr_node_t *cond,
		Callback callback)
{
	expr_node_t *join_cond = get_join_cond(cond);
	if(!join_cond) return false;

	if(join_cond->op != OPERATOR_EQ)
		return false;

	if(std::strcmp(join_cond->left->column_ref->table, tb1->get_table_name()) != 0)
		std::swap(tb1, tb2);

	int cid1 = tb1->lookup_column(join_cond->left->column_ref->column);
	int cid2 = tb2->lookup_column(join_cond->right->column_ref->column);
	if(cid1 < 0 || cid2 < 0)
		return false;

	index_manager *idx1 = tb1->get_index(cid1);
	index_manager *idx2 = tb2->get_index(cid2);

	if(!idx1 && !idx2)
	{
		std::printf("[Info] No index for %s.%s and %s.%s\n",
				join_cond->left->column_ref->table,
				join_cond->left->column_ref->column,
				join_cond->right->column_ref->table,
				join_cond->right->column_ref->column);
		return false;
	}

	if(idx2 == nullptr)
	{
		std::swap(tb1, tb2);
		std::swap(idx1, idx2);
		std::swap(cid1, cid2);
	}

	std::vector<table_manager*> table_list { tb1, tb2 };
	std::vector<record_manager*> record_list(2);
	std::vector<int> rid_list(2);

	// iterate table 1 first
	auto tb1_it = tb1->get_record_iterator_lower_bound(0);
	for(; !tb1_it.is_end(); tb1_it.next())
	{
		int tb1_rid;
		record_manager tb1_rm(tb1_it.get_pager());
		tb1_rm.open(tb1_it.get(), false);
		tb1_rm.read(&tb1_rid, 4);
		tb1->cache_record(&tb1_rm);

		// iterate table 2 by index
		const char *tb1_col = tb1->get_cached_column(cid1);
		if(tb1_col == nullptr) continue;
		auto tb2_it = idx2->get_iterator_lower_bound(tb1_col);
		for(; !tb2_it.is_end(); tb2_it.next())
		{
			int tb2_rid;
			record_manager tb2_rm = tb2->open_record_from_index_lower_bound(tb2_it.get(), &tb2_rid);
			tb2->cache_record(&tb2_rm);

			bool result = false;
			try {
				result = typecast::expr_to_bool(expression::eval(cond));
			} catch(const char *msg) {
				std::puts(msg);
				return true;
			}

			if(!result) break;

			record_list[0] = &tb1_rm;
			record_list[1] = &tb2_rm;
			rid_list[0] = tb1_rid;
			rid_list[1] = tb2_rid;
			if(!callback(table_list, record_list, rid_list))
				break;
		}
	}

	return true;
}

template<typename Callback>
void dbms::iterate_one_table(
		table_manager* table,
		expr_node_t *cond,
		Callback callback)
{
	auto bit = table->get_record_iterator_lower_bound(0);
	for(; !bit.is_end(); bit.next())
	{
		int rid;
		record_manager rm(bit.get_pager());
		rm.open(bit.get(), false);
		rm.read(&rid, 4);
		table->cache_record(&rm);
		if(cond)
		{
			bool result = false;
			try {
				result = typecast::expr_to_bool(expression::eval(cond));
			} catch(const char *msg) {
				std::puts(msg);
				return;
			}

			if(!result) continue;
		}

		if(!callback(table, &rm, rid))
			break;
	}
}

template<typename Callback>
void dbms::iterate_many_tables(
	const std::vector<table_manager*> &table_list,
	expr_node_t *cond, Callback callback)
{
	std::vector<record_manager*> record_list(table_list.size());
	std::vector<int> rid_list(table_list.size());
	iterate_many_tables_impl(table_list, record_list, rid_list, cond, callback, 0);
}

template<typename Callback>
bool dbms::iterate_many_tables_impl(
	const std::vector<table_manager*> &table_list,
	std::vector<record_manager*> &record_list,
	std::vector<int> &rid_list,
	expr_node_t *cond, Callback callback, int now)
{
	if(now == (int)table_list.size())
	{
		if(cond)
		{
			bool result = false;
			try {
				result = typecast::expr_to_bool(expression::eval(cond));
			} catch(const char *msg) {
				std::puts(msg);
				return false; // stop
			}

			if(!result)
				return true; // continue
		}

		if(!callback(table_list, record_list, rid_list))
			return false;  // stop
		return true;  // continue
	} else {
		auto it = table_list[now]->get_record_iterator_lower_bound(0);
		for(; !it.is_end(); it.next())
		{
			record_manager rm(it.get_pager());
			rm.open(it.get(), false);
			rm.read(&rid_list[now], 4);
			table_list[now]->cache_record(&rm);
			record_list[now] = &rm;
			bool ret = iterate_many_tables_impl(
				table_list, record_list, rid_list,
				cond, callback, now + 1
			);

			if(!ret) return false;
		}
	}

	return true;
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

void dbms::update_rows(const update_info_t *info)
{
	if(!assert_db_open())
		return;

	table_manager *tm = cur_db->get_table(info->table);
	if(tm == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", info->table);
		return;
	}

	int col_id = tm->lookup_column(info->column_ref->column);
	if(col_id < 0)
	{
		std::fprintf(stderr, "[Error] column `%s' not exists.\n", info->column_ref->column);
		return;
	}

	int succ_count = 0;
	try {
		iterate_one_table(tm, info->where, [&](table_manager *tm, record_manager *, int rid) -> bool {
			expression val = expression::eval(info->value);
			int col_type = tm->get_column_type(col_id);
			if(!typecast::type_compatible(col_type, val))
				throw "[Error] Incompatible data type.";
			auto term_type = typecast::column_to_term(col_type);
			bool ret = tm->modify_record(rid, col_id, typecast::expr_to_db(val, term_type));
			if(!ret) return false;
			succ_count++;
			return true;
		} );
	} catch(const char *msg) {
		std::puts(msg);
		return;
	} catch(...) {
	}

	expression::cache_clear();
	std::printf("[Info] %d row(s) updated.\n", succ_count);
}

void dbms::select_rows(const select_info_t *info)
{
	if(!assert_db_open())
		return;
	
	// get required tables
	std::vector<table_manager*> required_tables;
	for(linked_list_t *table_l = info->tables; table_l; table_l = table_l->next)
	{
		table_join_info_t *table_info = (table_join_info_t*)table_l->data;
		table_manager *tm = cur_db->get_table(table_info->table);
		if(tm == nullptr)
		{
			std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", table_info->table);
			return;
		} else required_tables.push_back(tm);
	}

	// iterate records
	int counter = 0;
	iterate(required_tables, info->where,
		[&](const std::vector<table_manager*> &table,
			const std::vector<record_manager*> &record,
			const std::vector<int>& )
		{
			for(size_t i = 0; i < table.size(); ++i)
				table[i]->dump_record(record[i]);
			std::puts("----------------");
			++counter;
			return true;
		}
	);

	expression::cache_clear();
	std::printf("[Info] %d row(s) selected.\n", counter);
}

void dbms::delete_rows(const delete_info_t *info)
{
	if(!assert_db_open())
		return;

	std::vector<int> delete_list;
	table_manager *tm = cur_db->get_table(info->table);
	if(tm == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` doesn't exists.\n", info->table);
		return;
	}

	iterate_one_table(tm, info->where,
		[&delete_list](table_manager*, record_manager*, int rid) -> bool {
			delete_list.push_back(rid);
			return true;
		} );

	int counter = 0;
	for(int rid : delete_list)
		counter += tm->remove_record(rid);
	expression::cache_clear();
	std::printf("[Info] %d row(s) deleted.\n", counter);
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
			if(!typecast::type_compatible(col_type, v))
			{
				std::fprintf(stderr, "[Error] incompatible type.\n");
				return;
			}
			
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

void dbms::drop_index(const char *tb_name, const char *col_name)
{
}

void dbms::create_index(const char *tb_name, const char *col_name)
{
	if(!assert_db_open())
		return;
	table_manager *tb = cur_db->get_table(tb_name);
	if(tb == nullptr)
	{
		std::fprintf(stderr, "[Error] table `%s` not exists.\n", tb_name);
	} else {
		tb->create_index(col_name);
	}
}

bool dbms::assert_db_open()
{
	if(cur_db && cur_db->is_opened())
		return true;
	std::fprintf(stderr, "[Error] database is not opened.\n");
	return false;
}

expr_node_t *dbms::get_join_cond(expr_node_t *cond)
{
	if(!cond) return nullptr;
	if(cond->left->term_type == TERM_COLUMN_REF && cond->right->term_type == TERM_COLUMN_REF)
	{
		return cond;
	} else if(cond->left->term_type == TERM_NONE) {
		return get_join_cond(cond->right);
	} else if(cond->right->term_type == TERM_NONE) {
		return get_join_cond(cond->left);
	} else {
		return nullptr;
	}
}
