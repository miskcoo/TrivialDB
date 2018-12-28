#include "table.h"
#include "../index/index.h"
#include "../expression/expression.h"
#include "../utils/type_cast.h"
#include <cstdio>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

index_manager::comparer_t get_index_comparer(int type)
{
	switch(type)
	{
		case COL_TYPE_INT:
			return integer_bin_comparer;
		case COL_TYPE_FLOAT:
			return float_bin_comparer;
		case COL_TYPE_VARCHAR:
			return string_comparer;
		default:
			assert(0);
			return string_comparer;
	}
}

record_manager table_manager::open_record_from_index_lower_bound(
	std::pair<int, int> idx_pos, int *rid)
{
	index_btree::leaf_page page { pg->read(idx_pos.first), pg.get() };
	int r = page.get_child(idx_pos.second);
	record_manager rm = get_record_ptr_lower_bound(r, false);
	if(rid != nullptr) rm.read(rid, 4);
	return rm;
}

void table_manager::cache_record(record_manager *rm)
{
	expression::cache_clear(header.table_name);
	int null_mark;
	rm->seek(4);
	rm->read(&null_mark, 4);
	((int*)tmp_cache)[1] = null_mark;
	for(int i = 0; i < header.col_num; ++i)
	{
		if(i == header.main_index && header.is_main_index_additional)
			continue;

		char *buf = nullptr;
		if(!((null_mark >> i) & 1))
		{
			buf = tmp_cache + header.col_offset[i];
			rm->seek(header.col_offset[i]);
			rm->read(buf, header.col_length[i]);
		}

		expression::cache_column(
			header.table_name,
			header.col_name[i],
			typecast::column_to_expr(buf, header.col_type[i])
		);
	}
}

const char* table_manager::get_cached_column(int cid)
{
	assert(cid >= 0 && cid < header.col_num);
	int null_mark = ((int*)tmp_cache)[1];
	if(!((null_mark >> cid) & 1))
		return tmp_cache + header.col_offset[cid];
	else return nullptr;
}

void table_manager::load_indices()
{
	std::memset(indices, 0, sizeof(indices));
	for(int i = 0; i < header.col_num; ++i)
	{
		if(i != header.main_index && ((1u << i) & header.flag_indexed))
		{
			indices[i] = new index_manager(pg.get(),
				header.col_length[i],
				header.index_root[i],
				get_index_comparer(header.col_type[i])
			);
		}
	}
}

void table_manager::free_indices()
{
	for(int i = 0; i < header.col_num; ++i)
	{
		if(i != header.main_index && ((1u << i) & header.flag_indexed))
		{
			assert(indices[i]);
			header.index_root[i] = indices[i]->get_root_pid();
			delete indices[i];
			indices[i] = nullptr;
		}
	}
}

bool table_manager::open(const char *table_name)
{
	if(is_open) return false;
	tname = table_name;
	std::string thead = tname + ".thead";
	std::string tdata = tname + ".tdata";

	std::ifstream ifs(thead, std::ios::binary);
	ifs.read((char*)&header, sizeof(header));
	pg = std::make_shared<pager>(tdata.c_str());
	btr = std::make_shared<int_btree>(
			pg.get(), header.index_root[header.main_index]);
	allocate_temp_record();
	load_indices();

	return is_open = true;
}

bool table_manager::create(const char *table_name, const table_header_t *header)
{
	if(is_open) return false;
	tname = table_name;
	std::string tdata = tname + ".tdata";

	pg = std::make_shared<pager>(tdata.c_str());
	btr = std::make_shared<int_btree>(pg.get(), 0);

	this->header = *header;
	this->header.index_root[header->main_index] = btr->get_root_page_id();
	allocate_temp_record();
	load_indices();

	return is_open = true;
}

void table_manager::close()
{
	if(!is_open) return;
	std::string thead = tname + ".thead";
	std::string tdata = tname + ".tdata";

	header.index_root[header.main_index] = btr->get_root_page_id();
	free_indices();

	std::ofstream ofs(thead, std::ios::binary);
	ofs.write((char*)&header, sizeof(header));
	btr = nullptr;
	pg->close();
	pg = nullptr;
	delete []tmp_record;
	delete []tmp_cache;
	tmp_cache = nullptr;
	tmp_record = nullptr;
	is_open = false;
}

int table_manager::lookup_column(const char *col_name)
{
	for(int i = 0; i < header.col_num; ++i)
	{
		if(std::strcmp(col_name, header.col_name[i]) == 0)
			return i;
	}

	return -1;
}

void table_manager::allocate_temp_record()
{
	if(tmp_record) delete[] tmp_record;
	int tot_len = 4; // 4 bytes for not_null
	for(int i = 0; i < header.col_num; ++i)
		tot_len += header.col_length[i];
	tmp_record = new char[tmp_record_size = tot_len];
	tmp_cache = new char[tot_len];
	tmp_null_mark = reinterpret_cast<int*>(tmp_record + 4);
}

bool table_manager::set_temp_record(int col, const void *data)
{
	if(data == nullptr)
	{
		*tmp_null_mark |= header.flag_notnull & (1u << col);
		return true;
	}

	*tmp_null_mark &= ~(1u << col);
	switch(header.col_type[col])
	{
		case COL_TYPE_INT:
		case COL_TYPE_FLOAT:
			memcpy(tmp_record + header.col_offset[col], data, 4);
			break;
		case COL_TYPE_VARCHAR:
			strncpy(tmp_record + header.col_offset[col], (const char*)data, header.col_length[col]);
			break;
		default:
			assert(false);
	}

	return true;
}

void table_manager::init_temp_record()
{
	// TODO: add default values
	std::memset(tmp_record, 0, tmp_record_size);
}

int table_manager::insert_record()
{
	assert(header.col_offset[header.main_index] == 0);
	int *rid = (int*)tmp_record;
	if(header.is_main_index_additional)
		*rid = header.auto_inc;
	// TODO: validate record
	btr->insert(*rid, tmp_record, tmp_record_size);

	for(int i = 0; i < header.col_num; ++i)
	{
		if(i != header.main_index && ((1u << i) & header.flag_indexed))
		{
			assert(indices[i]);
			indices[i]->insert(tmp_record + header.col_offset[i], *rid);
		}
	}

	if(header.is_main_index_additional)
	{
		++header.records_num;
		++header.auto_inc;
	}
	return *rid;
}

bool table_manager::remove_record(int rid)
{
	// TODO: remove corresponding index
	return btr->erase(rid);
}

btree_iterator<int_btree::leaf_page> table_manager::get_record_iterator_lower_bound(int rid)
{
	auto ret = btr->lower_bound(rid);
	return { pg.get(), ret.first, ret.second };
}

record_manager table_manager::get_record_ptr_lower_bound(int rid, bool dirty)
{
	auto ret = btr->lower_bound(rid);
	record_manager r(pg.get());
	r.open(ret.first, ret.second, dirty);
	return r;
}

record_manager table_manager::get_record_ptr(int rid, bool dirty)
{
	auto r = get_record_ptr_lower_bound(rid, dirty);
	if(r.valid() && *(int*)r.ptr().first == rid)
		return r;
	else return record_manager(pg.get());
}

void table_manager::dump_record(int rid)
{
	record_manager rec = get_record_ptr(rid);
	dump_record(&rec);
}

void table_manager::dump_record(record_manager *rm)
{
	char *buf = tmp_cache;
	for(int i = 0; i < header.col_num; ++i)
	{
		std::printf("%s.%s\t= ", header.table_name, header.col_name[i]);
		rm->seek(header.col_offset[i]);
		switch(header.col_type[i])
		{
			case COL_TYPE_INT:
				rm->read(buf, 4);
				std::printf("%d\n", *(int*)buf);
				break;
			case COL_TYPE_FLOAT:
				rm->read(buf, 4);
				std::printf("%f\n", *(float*)buf);
				break;
			case COL_TYPE_VARCHAR:
				rm->read(buf, header.col_length[i]);
				std::printf("%s\n", buf);
				break;
			default:
				debug_puts("[Error] Data type not supported!");
		}
	}
}

bool table_manager::modify_record(int rid, int col, const void* data)
{
	record_manager rec = get_record_ptr(rid, true);
	if(!rec.valid()) return false;
	assert(col >= 0 && col < header.col_num);
	// TODO: check validation
	rec.seek(header.col_offset[col]);
	rec.write(data, header.col_length[col]);
	// TODO: update index
	return true;
}

index_manager* table_manager::get_index(int cid)
{
	assert(cid >= 0 && cid < header.col_num);
	return indices[cid];
}

bool table_manager::has_index(const char *col_name)
{
	int cid = lookup_column(col_name);
	return has_index(cid);
}

bool table_manager::has_index(int cid)
{
	assert(cid >= 0 && cid < header.col_num);
	return (header.flag_indexed >> cid) & 1u;
}

void table_manager::create_index(const char *col_name)
{
	int cid = lookup_column(col_name);
	if(cid < 0)
	{
		std::fprintf(stderr, "[Error] column `%s' not exists.\n", col_name);
	} else if(has_index(cid)) {
		std::fprintf(stderr, "[Error] index for column `%s' already exists.\n", col_name);
	} else {
		header.flag_indexed |= 1u << cid;
		indices[cid] = new index_manager(pg.get(),
			header.col_length[cid],
			header.index_root[cid],
			get_index_comparer(header.col_type[cid])
		);

		// TODO: add existed data.
	}
}
