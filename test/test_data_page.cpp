#include <cstdio>
#include <fstream>
#include "page/pager.h"
#include "page/data_page.h"

void print_page(data_page page)
{
	for(int i = 0; i != page.size(); ++i)
	{
		char *p = page.buf + page.slots()[i];
		int size = *(uint16_t*)p;
		char *data = p + 8;
		for(int j = 0; j < size - 8; ++j)
			std::putchar(data[j]);
		std::puts("");
	}
}

int main()
{
	pager pg("test.db");
	int pid = pg.new_page();
	data_page page { pg.read_for_write(pid), &pg };
	page.init();
	page.insert(0, "123456789", 9);
	page.insert(0, "ABCDEFGHI", 9);
	page.insert(2, "abcdefghi", 9);
	page.erase(0);
	page.insert(2, "======", 6);
	page.erase(1);
	page.insert(0, "*************", 13);
	print_page(page);
	while(page.insert(0, "9999999999999999999999999999999999999999", 40));
	std::printf("%d %d\n", page.free_size(), page.bottom_used());
	page.insert(0, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", page.free_size() - 8);
	page.insert(0, "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz", page.free_size() - 10);

	std::printf("%d %d\n", page.free_size(), page.bottom_used());
	page.erase(2);
	std::printf("%d %d\n", page.free_size(), page.bottom_used());
	page.insert(0, "uuuuuuu", 6);
	std::printf("%d %d\n", page.free_size(), page.bottom_used());
	page.insert(0, "vvvvvvv", 6);
	std::printf("%d %d\n", page.free_size(), page.bottom_used());
	page.insert(0, "wwwwwww", 7);
	std::printf("%d %d\n", page.free_size(), page.bottom_used());

	for(int i = 4; i < 20; ++i)
		page.erase(i);

	char *large_data = new char[4096];
	for(int i = 0; i != 4096; ++i)
		large_data[i] = ('A' + i % 26);
	page.insert(0, large_data, 4096);
	delete[] large_data;

	print_page(page);

	page.erase(0);

	return 0;
}
