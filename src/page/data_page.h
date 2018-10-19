#ifndef __TRIVIALDB_DATA_PAGE__
#define __TRIVIALDB_DATA_PAGE__

#include <cstring>
#include <cassert>
#include <utility>
#include "page_defs.h"
#include "pager.h"

/* For each item, let S be the size of it. If S > PAGE_BLOCK_MAX_SIZE,
 * then part of it will be stored in overflow pages and the first
 * PAGE_OV_KEEP_SIZE will stay in the data page. If S <= PAGE_BLOCK_MAX_SIZE,
 * all the data will be stored in the data page.
 * */

class data_page : public general_page
{
	struct free_block_header
	{
		uint16_t size;
		uint16_t next;
	};

	struct block_header
	{
		uint16_t size;  // block size (overflow is not included)
		uint16_t ov_id; // block id in the overflow page
		int ov_page;    // overflow page
	};
private:
	char *allocate(int sz);
public:
	using general_page::general_page;
	PAGE_FIELD_REF(magic,       uint16_t, 0);   // page type
	PAGE_FIELD_REF(flags,       uint8_t,  2);   // flags
	PAGE_FIELD_REF(free_block,  uint16_t, 4);   // pointer to the first freeslot
	PAGE_FIELD_REF(free_size,   uint16_t, 6);   // size of free space
	PAGE_FIELD_REF(size,        uint16_t, 8);   // number of items
	PAGE_FIELD_REF(bottom_used, uint16_t, 10);
	PAGE_FIELD_PTR(slots,       uint16_t, 12);  // slots
	static const int header_size = 12;

	void erase(int pos);
	bool insert(int pos, const char *data, int sz);
	void defragment();
};

#endif
