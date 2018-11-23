#include "database.h"
#include <fstream>
#include <string>

void database::open(const char *db_name)
{
	if(is_opened()) return;
	std::string filename = db_name;
	filename += ".database";
	std::ifstream ifs(filename, std::ios::binary);
	ifs.read((char*)&info, sizeof(info));
	opened = true;
}

void database::create(const char *db_name)
{
	if(is_opened()) return;
	std::memset(&info, 0, sizeof(info));
	std::strncpy(info.db_name, db_name, MAX_NAME_LEN);
	opened = true;
}

void database::close()
{
	if(!is_opened()) return;
	std::string filename = info.db_name;
	filename += ".database";
	std::ofstream ofs(filename, std::ios::binary);
	ofs.write((char*)&info, sizeof(info));
	opened = false;
}

void database::create_table(const table_header_t *header)
{
	if(!is_opened())
	{
		// TODO: report error
	} else {
		table_manager tb;
		std::strncpy(info.table_name[info.table_num++], header->table_name, MAX_NAME_LEN);
		tb.create(header->table_name, header);
		tb.close();
	}
}
