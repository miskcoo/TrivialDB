#ifndef __TRIVIALDB_OVERFLOW_PAGE__
#define __TRIVIALDB_OVERFLOW_PAGE__

#include <cassert>
#include <utility>
#include "page_defs.h"

class overflow_page : public general_page
{
	friend class pager;
public:
	struct block_header
	{
		uint16_t size;
		uint16_t next_index;
		int next_page;
	};
public:
	using general_page::general_page;

	PAGE_FIELD_REF(magic,       uint16_t, 0);
	PAGE_FIELD_REF(log_blk_num, uint16_t, 2);
	PAGE_FIELD_REF(next_free,   int,      4);   // next ov-page with free space
	PAGE_FIELD_REF(used,        int,      8);
	PAGE_FIELD_PTR(block_start, char,    12);

	void init(int log_blk_num_)
	{
		magic_ref() = PAGE_OVERFLOW;
		log_blk_num_ref() = log_blk_num_;
		next_free_ref() = 0;
		used_ref() = 0;
	}

	int block_size() { return (PAGE_SIZE - 12) >> log_blk_num(); }
	int data_size() { return block_size() - sizeof(block_header); }
	std::pair<block_header*, char*> get_block(int id)
	{
		assert(0 <= id && id < (1 << log_blk_num()));
		char *blk = block_start() + id * block_size();
		return {
			reinterpret_cast<block_header*>(blk),
			blk + sizeof(block_header)
		};
	}
};

#endif
