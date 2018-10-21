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
	PAGE_FIELD_PTR(children,    int,      8);   // pointer to child pages
	PAGE_FIELD_ACCESSER(T,   key,   begin() + id);
	PAGE_FIELD_ACCESSER(int, child, children() + id);
	static constexpr int header_size() { return 8; }
	int capacity() { return (PAGE_SIZE - 8) / (sizeof(T) + 4); }
	bool full() { return capacity() == size(); }
	bool empty() { return size() == 0; }
	void init()
	{
		magic_ref() = PAGE_FIXED;
		flags_ref() = 0;
		field_size_ref() = sizeof(T);
		size_ref() = 0;
	}

	T* begin() { return end() - size(); }
	T* end() { return reinterpret_cast<T*>(buf + PAGE_SIZE); }

	bool insert(int pos, int child, const T& key);
	void erase(int pos);
	std::pair<int, fixed_page> split();
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
std::pair<int, fixed_page<T>> fixed_page<T>::split()
{
	if(size() < PAGE_BLOCK_MIN_NUM)
		return { 0, { nullptr, nullptr } };

	int page_id = pg->new_page();
	if(!page_id) return { 0, { nullptr, nullptr } };
	fixed_page upper_page { pg->read_for_write(page_id), pg };
	upper_page.init();
	upper_page.flags_ref() = flags();

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

#endif
