#ifndef __TRIVIALDB_PAGER__
#define __TRIVIALDB_PAGER__

#include <utility>
#include "../fs/page_file.h"
#include "overflow_page.h"

class pager : public page_file
{
	struct __overflow_info
	{
		// pointer to the overflow page which has empty space
		int ov_free[LOG_MAX_OVPAGE_BLOCK];
	} *ov_info;
public:
	struct ov_block
	{
		int page_id, block_id;
		overflow_page::block_header *header;
		char *data;
	};
public:
	pager() : ov_info(nullptr) {}
	bool open(const char* filename);

	ov_block new_overflow_page(int type);
	void free_overflow_block(int page_id, int block_id, bool recursive = true);
};

#endif

