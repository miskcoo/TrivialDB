#include <cstdio>
#include <fstream>
#include "page/pager.h"
#include "page/small_page.h"

int main()
{
	pager pg("test.db");
	int pid = pg.new_page();
	small_page<int> page { pg.read_for_write(pid), &pg };
	page.init();

	for(int i = 0; i < 30; ++i)
		page.insert(i, 0, 1 << i);

	for(int count = 30; !page.full(); ++count)
		page.insert(count, 0, count);

	for(auto x : page) std::printf("%d ", x);
	std::puts("");

	int new_pid = page.split();
	small_page<int> new_page { pg.read_for_write(new_pid), &pg };

	std::puts("===== LOWER PAGE ===== ");
	for(auto x : page) std::printf("%d ", x);
	std::puts("");
	std::puts("===== UPPER PAGE ===== ");
	for(auto x : new_page) std::printf("%d ", x);
	std::puts("");

	return 0;
}
