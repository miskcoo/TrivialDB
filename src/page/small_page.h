#ifndef __TRIVIALDB_SMALL_PAGE__
#define __TRIVIALDB_SMALL_PAGE__

#include <cstring>
#include <cassert>
#include <utility>
#include "page_defs.h"

template<typename T>
class small_page : public general_page
{
public:
	typedef T* iterator;
public:
	using general_page::general_page;
	PAGE_FIELD_REF(magic,       uint16_t, 0);   // page type
	PAGE_FIELD_REF(flags,       uint8_t,  2);   // flags
	PAGE_FIELD_REF(field_size,  uint8_t,  3);   // log2(size of keywords)
	PAGE_FIELD_REF(size,        int,      4);   // number of items
	PAGE_FIELD_PTR(children,    int,      8);   // pointer to child pages
	int capacity() { return (PAGE_SIZE - 8) / (sizeof(T) + 4); }
	bool full() { return capacity() == size(); }
	bool empty() { return size() == 0; }
	void init()
	{
		magic_ref() = PAGE_SMALL;
		flags_ref() = 0;
		field_size_ref() = sizeof(T);
		size_ref() = 0;
	}

	T* begin() { return end(); }
	T* end() { return reinterpret_cast<T*>(buf + PAGE_SIZE); }

	/* insert element before `pos` */
	bool insert(int pos, int child, const T& key)
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
		*key_ptr = key;

		++size_ref();
		return true;
	}

	/* erase element */
	void erase(int pos)
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
};

#endif
