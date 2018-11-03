#ifndef __TRIVIALDB_TABLE__
#define __TRIVIALDB_TABLE__

#include <stdint.h>
#include <fstream>
#include <memory>

#include "../defs.h"
#include "../btree/btree.h"
#include "table_header.h"
#include "record.h"

/*    Data page structure for rows
 *  | main index | notnull | fixed col 1 | ... | fixed col n |
 */

class table_manager
{
	bool is_open;
	table_header_t header;
	std::shared_ptr<btree> btr;
	std::shared_ptr<pager> pg;
	std::string tname;
	const char *error_msg;

	int tmp_record_size;
	char *tmp_record;
	int *tmp_null_mark;
	void allocate_temp_record();
public:
	table_manager() : is_open(false), tmp_record(nullptr) {}
	~table_manager() { if(is_open) close(); }
	bool create(const char *table_name, table_header_t header);
	bool open(const char *table_name);
	void close();

	int lookup_column(const char *col_name);

	void init_temp_record();
	int insert_record();
	bool remove_record(int rid);
	bool modify_record(int rid, int col, const void* data);
	bool set_temp_record(int col, const void* data);

	// get the record R such that R.rid = min_{r.rid >= rid} r.rid
	record_manager get_record_ptr_lower_bound(int rid, bool dirty=false);
	// get the record R such that R.rid = rid
	record_manager get_record_ptr(int rid, bool dirty=false);
	void dump_record(int rid);
};

#endif
