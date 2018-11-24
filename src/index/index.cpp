#include "index.h"
#include "../utils/comparer.h"
#include <cstring>

index_manager::index_manager(pager *pg, int size, int root_pid, comparer_t comparer)
{
	this->size = size;
	buf = new char[size + sizeof(int)];
	btr = new index_btree(pg, root_pid, size + sizeof(int),
		[comparer](const char *a, const char *b) -> bool {
			int r = comparer(a + sizeof(int), b + sizeof(int));
			if(r != 0) return r;
			return integer_comparer(*(int*)a, *(int*)b);
		} );
}

index_manager::~index_manager()
{
	delete []buf;
	delete btr;
	buf = nullptr;
	btr = nullptr;
}

int index_manager::get_root_pid()
{
	return btr->get_root_page_id();
}

void index_manager::fill_buf(const char *key, int rid)
{
	*(int*)buf = rid;
	std::memcpy(buf + sizeof(int), key, size);
}

void index_manager::insert(const char *key, int rid)
{
	fill_buf(key, rid);
	btr->insert(buf, rid);
}

void index_manager::erase(const char *key, int rid)
{
	fill_buf(key, rid);
	btr->erase(buf);
}

index_btree::search_result index_manager::lower_bound(const char *key, int rid)
{
	fill_buf(key, rid);
	return btr->lower_bound(buf);
}
