#include <algorithm>
#include "data_page.h"
#include "overflow_page.h"

#define LOAD_FREEBLK(offset) \
	reinterpret_cast<free_block_header*>(buf + (offset))

void data_page::init()
{
	magic_ref() = PAGE_DATA;
	flags_ref() = 0;
	free_block_ref() = 0;
	free_size_ref() = PAGE_SIZE - header_size();
	size_ref() = 0;
	bottom_used_ref() = 0;
}

void data_page::erase(int pos)
{
	assert(0 <= pos && pos < size());

	uint16_t* slots_ptr = slots();
	uint16_t slot = slots_ptr[pos];
	for(int i = pos, s = size(); i < s; ++i)
		slots_ptr[i] = slots_ptr[i + 1];

	block_header *header = (block_header*)(buf + slot);
	if(header->ov_page)
		pg->free_overflow_page(header->ov_page);

	free_block_header *fb_header = LOAD_FREEBLK(slot);
	fb_header->size = header->size;
	fb_header->next = free_block();
	free_block_ref() = slot;
	--size_ref();
}

bool data_page::insert(int pos, const char *data, int data_size)
{
	assert(0 <= pos && pos <= size());
	int real_size = data_size + sizeof(block_header);
	bool ov = (real_size > PAGE_BLOCK_MAX_SIZE);
	int size_required = ov ? PAGE_OV_KEEP_SIZE : real_size;
	char *dest = allocate(size_required);
	if(!dest) return false;

	// set slot
	uint16_t *slots_ptr = slots();
	for(int i = size(); i > pos; --i)
		slots_ptr[i] = slots_ptr[i - 1];
	slots_ptr[pos] = dest - buf;
	
	// set data
	block_header* header = (block_header*)dest;
	header->size = size_required;
	int copied_size = size_required - sizeof(block_header);
	std::memcpy(dest + sizeof(block_header), data, copied_size);

	if(!ov)
	{
		header->ov_page = 0;
	} else {
		auto create_and_copy = [&](const char *src, int size) {
			int pid = pg->new_page();
			overflow_page page = overflow_page(pg->read_for_write(pid), pg);
			page.init();
			page.size_ref() = size;
			std::memcpy(page.block(), src, size);
			return std::make_pair(pid, page);
		};

		data += copied_size;
		int remain = data_size - copied_size;
		int to_copy = std::min(overflow_page::block_size(), remain);

		auto ret = create_and_copy(data, to_copy);
		overflow_page ov_page = ret.second;
		data   += to_copy;
		remain -= to_copy;
		header->ov_page = ret.first;

		while(remain > 0)
		{
			to_copy = std::min(overflow_page::block_size(), remain);
			auto ret = create_and_copy(data, to_copy);
			ov_page.next_ref() = ret.first;
			ov_page = ret.second;
			data   += to_copy;
			remain -= to_copy;
		}
	}

	return true;
}

char* data_page::allocate(int sz)
{
	int unallocated = PAGE_SIZE - (header_size() + size() * 2 + bottom_used());
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
	debug_puts("Call defragment()");

	int sz = size();
	uint16_t *slots_ptr = slots();
	int *index = new int[sz];
	for(int i = 0; i != sz; ++i)
		index[i] = i;
	std::sort(index, index + size(), [=](int a, int b) {
		return slots_ptr[a] > slots_ptr[b];
	} );

	int total_blk_sz = 0;
	char *ptr = buf + PAGE_SIZE;
	for(int i = 0; i < sz; ++i)
	{
		char *blk = buf + slots_ptr[index[i]];
		int blk_sz = reinterpret_cast<block_header*>(blk)->size;
		total_blk_sz += blk_sz;
		ptr -= blk_sz;
		slots_ptr[index[i]] = ptr - buf;
		std::memmove(ptr, blk, blk_sz);
	}

	delete[] index;

	free_block_ref()  = 0;
	bottom_used_ref() = total_blk_sz;

	assert(total_blk_sz + header_size() + 2 * size() + free_size() == PAGE_SIZE);
}
