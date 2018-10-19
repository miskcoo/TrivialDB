#include "data_page.h"

#define LOAD_FREEBLK(offset) \
	reinterpret_cast<free_block_header*>(buf + (offset))

void data_page::erase(int pos)
{
	assert(0 <= pos && pos < size());

	uint16_t* slots_ptr = slots();
	uint16_t slot = slots_ptr[pos];
	for(int i = pos, s = size(); i < s; ++i)
		slots_ptr[i] = slots_ptr[i + 1];

	block_header *header = (block_header*)(buf + slot);
	if(header->ov_page)
		pg->free_overflow_block(header->ov_page, header->ov_id);

	free_block_header *fb_header = LOAD_FREEBLK(slot);
	fb_header->size = header->size;
	fb_header->next = free_block();
	free_block_ref() = slot;
	--size_ref();
}

/* TODO
bool data_page::insert(int pos, const char *data, int sz)
{
	assert(0 <= pos && pos <= size());
	bool ov = (sz > PAGE_BLOCK_MAX_SIZE);
	int size_req = ov ? PAGE_OV_KEEP_SIZE : sz;
}
*/

char* data_page::allocate(int sz)
{
	int unallocated = PAGE_SIZE - (header_size + size() * 2 + bottom_used());
	if(unallocated < 2) return nullptr;  // no space for slot

	if(unallocated - 2 >= sz)  // minus one slot size for this item
	{
		bottom_used_ref() += sz;
		free_size_ref() -= sz;
		return buf + PAGE_SIZE - bottom_used();
	}

	auto free_blk = LOAD_FREEBLK(free_block());
	if(free_blk->size >= sz)
	{
		if(free_blk->size - sz < PAGE_FREE_BLOCK_MIN_SIZE)
		{
			free_block_ref() = free_blk->next;
		} else {
			auto free_blk_new = LOAD_FREEBLK(free_block() + sz);
			free_blk_new->size = free_blk->size - sz;
			free_blk_new->next = free_blk->next;
			free_block_ref() += sz;
		}

		free_size_ref() -= sz;
		return buf + free_block();
	} else if(free_size() - 2 >= sz) {
		defragment();
		return allocate(sz);
	} else return nullptr;
}

void data_page::defragment()
{
}
