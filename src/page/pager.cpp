#include <cassert>

#include "pager.h"
#include "overflow_page.h"

bool pager::open(const char *filename)
{
	if(page_file::open(filename))
	{
		ov_info = reinterpret_cast<__overflow_info*>(get_header_buffer());
		return true;
	} else {
		ov_info = nullptr;
		return false;
	}
}

pager::ov_block pager::new_overflow_page(int type)
{
	assert(0 <= type && type < LOG_MAX_OVPAGE_BLOCK);

	ov_block ov;
	int &ov_free = ov_info->ov_free[type];
	if(ov_free)
	{
		// find unused block
		auto ov_page = overflow_page(read_for_write(ov_free), this);
		assert(ov_page.magic() == PAGE_OVERFLOW);

		for(ov.block_id = 0; ov.block_id < (1 << type); ++ov.block_id)
			if(!(ov_page.used() & (1 << ov.block_id)))
				break;
		assert(ov.block_id < (1 << type));
		ov_page.used_ref() |= 1 << ov.block_id;

		// setup
		auto ret = ov_page.get_block(ov.block_id);
		ov.page_id = ov_free;
		ov.header  = ret.first;
		ov.data    = ret.second;

		// page full
		if(ov_page.used() == (1 << type) - 1)
		{
			ov_free = ov_page.next_free();
			ov_page.next_free_ref() = 0;
		}
	} else {
		int page_id = new_page();
		auto ov_page = overflow_page(read_for_write(page_id), this);
		ov_page.init(type);
		ov_page.used_ref() = 1;

		auto ret = ov_page.get_block(0);
		ov.page_id  = page_id;
		ov.block_id = 0;
		ov.header   = ret.first;
		ov.data     = ret.second;

		if(type > 0)
		{
			ov_free = page_id;
			ov_page.next_free_ref() = ov_free;
		}
	}

	return ov;
}

void pager::free_overflow_block(int page_id, int block_id, bool recursive)
{
	auto ov_page = overflow_page(read_for_write(page_id), this);

	assert(ov_page.used() & (1 << block_id));
	assert(ov_page.magic() == PAGE_OVERFLOW);

	int& ov_free = ov_info->ov_free[ov_page.log_blk_num()];

	auto blk_header = *ov_page.get_block(block_id).first;

	ov_page.used_ref() ^= 1 << block_id;
	if(ov_page.used() == 0)
	{
		if(ov_free == page_id)
			ov_free = ov_page.next_free();
		free_page(page_id);
	} else if(ov_free != page_id) {
		ov_page.next_free_ref() = ov_free;
		ov_free = page_id;
	}

	if(recursive && blk_header.next_page)
		free_overflow_block(blk_header.next_page, blk_header.next_index, true);
}
