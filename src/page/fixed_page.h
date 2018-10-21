#ifndef __TRIVIALDB_FIXED_PAGE__
#define __TRIVIALDB_FIXED_PAGE__

#include <cstring>
#include <cassert>
#include <utility>
#include "page_defs.h"
#include "pager.h"

template<typename T>
class fixed_page : public general_page
{
public:
	typedef T* iterator;
public:
	using general_page::general_page;
	PAGE_FIELD_REF(magic,       uint16_t, 0);   // page type
	PAGE_FIELD_REF(flags,       uint8_t,  2);   // flags
	PAGE_FIELD_REF(field_size,  uint8_t,  3);   // size of keywords
	PAGE_FIELD_REF(size,        int,      4);   // number of items
	PAGE_FIELD_REF(next_page,   int,      8);
	PAGE_FIELD_REF(prev_page,   int,      12);
	PAGE_FIELD_PTR(children,    int,      16);   // pointer to child pages
	PAGE_FIELD_ACCESSER(T,   key,   begin() + id);
	PAGE_FIELD_ACCESSER(int, child, children() + id);
	static constexpr int header_size() { return 16; }
	int capacity() { return (PAGE_SIZE - 8) / (sizeof(T) + 4); }
	bool full() { return capacity() == size(); }
	bool empty() { return size() == 0; }
	bool underflow() { return size() < capacity() / 2 - 1; }
	bool underflow_if_remove(int) { return size() < capacity() / 2; }
	void init()
	{
		magic_ref() = PAGE_FIXED;
		flags_ref() = 0;
		field_size_ref() = sizeof(T);
		size_ref() = 0;
		next_page_ref() = prev_page_ref() = 0;
	}

	T* begin() { return end() - size(); }
	T* end() { return reinterpret_cast<T*>(buf + PAGE_SIZE); }

	bool insert(int pos, int child, const T& key);
	void erase(int pos);
	std::pair<int, fixed_page> split(int cur_id);
	bool merge(fixed_page page, int cur_id);
	void move_from(fixed_page page, int src_pos, int dest_pos);
};

template<typename T>
bool fixed_page<T>::insert(int pos, int child, const T& key)
{
	assert(0 <= pos && pos <= size());
	if(full()) return false;

	int* ch_ptr = children();
	for(int i = size() - 1; i >= pos; --i)
		ch_ptr[i + 1] = ch_ptr[i];
	ch_ptr[pos] = child;

	T* key_ptr = begin();
	for(int i = 0; i < pos; ++i, ++key_ptr)
		*(key_ptr - 1) = *key_ptr;
	*(key_ptr - 1) = key;

	++size_ref();
	return true;
}

template<typename T>
void fixed_page<T>::erase(int pos)
{
	assert(0 <= pos && pos < size());

	int* ch_ptr = children() + pos;
	for(int i = size(); i > pos; --i, ++ch_ptr)
		*ch_ptr = *(ch_ptr + 1);
	*ch_ptr = 0;

	T* key_ptr = begin() + pos;
	for(int i = 0; i < pos; ++i, --key_ptr)
		*key_ptr = *(key_ptr - 1);
	*key_ptr = 0;

	--size_ref();
}

template<typename T>
std::pair<int, fixed_page<T>> fixed_page<T>::split(int cur_id)
{
	if(size() < PAGE_BLOCK_MIN_NUM)
		return { 0, { nullptr, nullptr } };

	int page_id = pg->new_page();
	if(!page_id) return { 0, { nullptr, nullptr } };
	fixed_page upper_page { pg->read_for_write(page_id), pg };
	upper_page.init();
	upper_page.flags_ref() = flags();

	if(next_page())
	{
		fixed_page page { pg->read_for_write(next_page()), pg };
		assert(page.magic() == magic());
		page.prev_page_ref() = page_id;
	}
	upper_page.next_page_ref() = next_page();
	upper_page.prev_page_ref() = cur_id;
	next_page_ref() = page_id;

	int lower_size = size() >> 1;
	int upper_size = size() - lower_size;
	std::memcpy(
		upper_page.children(),
		children() + upper_size,
		(size() - upper_size) * sizeof(int)
	);

	std::memcpy(
		upper_page.end() - upper_size, 
		end() - upper_size, 
		sizeof(T) * upper_size
	);

	std::memmove(end() - lower_size, begin(), sizeof(T) * lower_size);
	size_ref() = lower_size;
	upper_page.size_ref() = upper_size;
	return { page_id, upper_page };
}

template<typename T>
bool fixed_page<T>::merge(fixed_page page, int cur_id)
{
	if(size() + page.size() > capacity())
		return false;

	next_page_ref() = page.next_page_ref();
	if(next_page())
	{
		fixed_page page { pg->read_for_write(next_page()), pg };
		assert(page.magic() == magic());
		page.prev_page_ref() = cur_id;
	}

	std::memcpy(children() + size(), page.children(), 4 * page.size());
	std::memmove(begin() - page.size(), begin(), sizeof(T) * page.size());
	std::memcpy(end() - page.size(), page.begin(), sizeof(T) * page.size());
	size_ref() += page.size();

	return true;
}

template<typename T>
void fixed_page<T>::move_from(fixed_page page, int src_pos, int dest_pos)
{
	assert(page.magic() == magic());
	bool succ_ins = insert(dest_pos,
		page.get_child(src_pos), page.get_key(src_pos));
	page.erase(src_pos);
	assert(succ_ins);
	UNUSED(succ_ins);
}

#endif
