#include "../src/table/table.h"
#include <cstring>
#include <algorithm>

int main()
{
	table_header_t header;
	std::memset(&header, 0, sizeof(header));
	header.auto_inc = 1;
	header.col_num = 2;
	header.main_index = 0;
	header.is_main_index_additional = 1;
	strcpy(header.table_name, "2333");
	strcpy(header.col_name[0], "rid");
	strcpy(header.col_name[1], "age");
	header.col_length[0] = 4;
	header.col_length[1] = 4;
	header.col_offset[0] = 0;
	header.col_offset[1] = 8;
	header.col_type[0] = COL_TYPE_INT;
	header.col_type[1] = COL_TYPE_INT;

	table_manager tb;
	tb.create("hello", &header);


	auto insert = [&](int x) {
		tb.set_temp_record(1, &x);
		tb.insert_record();
	};
	
	insert(43);
	insert(239);
	insert(2);

	tb.dump_record(1);
	tb.dump_record(2);
	tb.dump_record(3);

	tb.remove_record(2);
	tb.dump_record(1);
	tb.dump_record(3);

	assert((int)tb.get_record_ptr(1).valid());
	assert(!(int)tb.get_record_ptr(2).valid());
	assert((int)tb.get_record_ptr(3).valid());
	assert(!(int)tb.get_record_ptr(4).valid());

	int x = 100, y;
	tb.get_record_ptr(1).seek(8).read(&y, 4);
	assert(y == 43);
	tb.modify_record(1, 1, &x);
	tb.get_record_ptr(1).seek(8).read(&y, 4);
	assert(y == x);

	puts("===== Pass! =====");
	
	return 0;
}
