#include "table.h"
#include <cstdio>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

bool table_manager::open(const char *table_name)
{
	if(is_open) return false;
	tname = table_name;
	std::string thead = tname + ".thead";
	std::string tdata = tname + ".tdata";

	std::ifstream ifs(thead, std::ios::binary);
	ifs.read((char*)&header, sizeof(header));
	pg = std::make_shared<pager>(tdata.c_str());
	btr = std::make_shared<btree>(
			pg.get(), header.index_root[header.main_index]);
	allocate_temp_record();

	return is_open = true;
}

bool table_manager::create(const char *table_name, table_header_t header)
{
	if(is_open) return false;
	tname = table_name;
	std::string tdata = tname + ".tdata";

	pg = std::make_shared<pager>(tdata.c_str());
	btr = std::make_shared<btree>(pg.get(), 0);

	this->header = header;
	this->header.index_root[header.main_index] = btr->get_root_page_id();
	allocate_temp_record();

	return is_open = true;
}

void table_manager::close()
{
	if(!is_open) return;
	std::string thead = tname + ".thead";
	std::string tdata = tname + ".tdata";

	header.index_root[header.main_index] = btr->get_root_page_id();
	std::ofstream ofs(thead, std::ios::binary);
	ofs.write((char*)&header, sizeof(header));
	btr = nullptr;
	pg->close();
	pg = nullptr;
	delete []tmp_record;
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
			if((int)strlen((const char*)data) > header.col_length[col])
			{
				error_msg = "[ERROR] string too long.";
				return false;
			}

			strcpy(tmp_record + header.col_offset[col], (const char*)data);
			break;
		default:
			assert(false);
	}

	return true;
}

void table_manager::init_record()
{
	// TODO: add default value
	std::memset(tmp_record, 0, tmp_record_size);
}

int table_manager::insert_record()
{
	int *rid = (int*)tmp_record + header.col_offset[header.main_index];
	if(header.is_main_index_additional)
		*rid = header.auto_inc;
	// TODO: validation record
	btr->insert(*rid, tmp_record, tmp_record_size);
	// TODO: update index
	if(header.is_main_index_additional)
	{
		++header.records_num;
		++header.auto_inc;
	}
	return *rid;
}

bool table_manager::remove_record(int rid)
{
	// TODO: remove index
	return btr->erase(rid);
}

record_manager table_manager::get_record_ptr(int rid)
{
	auto ret = btr->lower_bound(rid);
	assert(ret.first != 0);
	record_manager r(pg.get());
	r.open(ret.first, ret.second, false);
	// TODO: verify rid
	return r;
}

void table_manager::dump_record(int rid)
{
	record_manager rec = get_record_ptr(rid);

	for(int i = 0; i < header.col_num; ++i)
	{
		std::printf("%s = ", header.col_name[i]);
		rec.seek(header.col_offset[i]);
		char buf[1024];
		switch(header.col_type[i])
		{
			case COL_TYPE_INT:
				rec.read(buf, 4);
				std::printf("%d\n", *(int*)buf);
				break;
			case COL_TYPE_FLOAT:
				rec.read(buf, 4);
				std::printf("%f\n", *(float*)buf);
				break;
			case COL_TYPE_VARCHAR:
				rec.read(buf, header.col_length[i]);
				std::printf("%s\n", buf);
				break;
			default:
				debug_puts("[Error] Data type not supported!");
		}
	}
}
