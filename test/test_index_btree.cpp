#include "../src/btree/btree.h"
#include "../src/utils/utils.h"
#include <cstring>
#include <algorithm>

int main()
{
	pager pg("test.db");
	index_btree bt(&pg, 0, 10, string_comparer);

	auto insert = [&](const char * str, int rid) {
		char tmp[10];
		std::memset(tmp, 0, sizeof(tmp));
		std::strncpy(tmp, str, 10);
		bt.insert(tmp, rid);
	};
	
	insert("hello", 23);
	insert("world", 233);
	insert("miskcoo", 2398);

	char ans[30];
	std::memset(ans, 0, sizeof(ans));
	std::strcpy(ans, "hello");
	std::strcpy(ans + 10, "miskcoo");
	std::strcpy(ans + 20, "world");

	char *buf = pg.read(bt.get_root_page_id()) + PAGE_SIZE - 30;
	int cmp_r = std::memcmp(buf, ans, 30);
	UNUSED(cmp_r);
	assert(cmp_r == 0);

	bt.erase("world");
	cmp_r = std::memcmp(buf + 10, ans, 20);
	UNUSED(cmp_r);
	assert(cmp_r == 0);

	insert("world", 233);
	cmp_r = std::memcmp(buf, ans, 30);
	UNUSED(cmp_r);
	assert(cmp_r == 0);

	const int MaxR = 10000;
	int *rnd = new int[MaxR];
	for(int i = 0; i != MaxR; ++i)
		rnd[i] = i;
	std::random_shuffle(rnd, rnd + MaxR);

	for(int i = 0; i < MaxR; ++i)
	{
		char c[10];
		std::sprintf(c, "%d", rnd[i]);
		insert(c, i);
	}

	puts("===== Pass! =====");
	
	return 0;
}
