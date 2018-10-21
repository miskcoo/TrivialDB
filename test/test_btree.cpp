#include "page/pager.h"
#include "btree/btree.h"
#include <cstring>
#include <algorithm>

char data[1000];

void output_space(int level)
{
	for(int i = 0; i != level; ++i) std::printf("  ");
}

void dump(pager* pg, int pid, int parent = 0, int level = 0)
{
	char *addr = pg->read(pid);
	uint16_t magic = general_page::get_magic_number(addr);

	output_space(level);
	if(magic == PAGE_FIXED)
	{
		fixed_page<int> page { addr, pg };
		std::printf("[Interior page, parent = %d, size = %d]\n", parent, page.size());
		for(int i = 0; i != page.size(); ++i)
		{
			output_space(level);
			std::printf("Key = %d, Child = %d\n", page.get_key(i), page.get_child(i));
			dump(pg, page.get_child(i), pid, level + 1);
		}
	} else {
		data_page<int> page { addr, pg };
		std::printf("[Leaf page, parent = %d, size = %d]\n", parent, page.size());
		for(int i = 0; i != page.size(); ++i)
		{
			output_space(level);
			std::printf("Key = %d, Data = %s\n", page.get_key(i), page.get_block(i).second + 4);
		}
	}
}

int main()
{
	pager pg("test.db");
	btree bt(&pg);

	auto insert = [&](int key) {
		*(int*)data = key;
		std::sprintf(data + 4, "Key = %d", key);
		bt.insert(key, data, 4 + std::strlen(data + 4) + 1);
	};

	insert(1109);
	insert(1200);
	insert(1100);

	const int n = 5000;
	int tmp[n];
	for(int i = 0; i != n; ++i)
		tmp[i] = i + 1;
	std::random_shuffle(tmp, tmp + n);
	for(int x : tmp) insert(x);

	dump(&pg, bt.get_root_page_id());
	return 0;
}
