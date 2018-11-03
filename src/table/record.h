#ifndef __TRIVIALDB_RECORD__
#define __TRIVIALDB_RECORD__

#include "../page/pager.h"
#include "../page/data_page.h"

class record_manager
{
	pager *pg;
	int pid, pos, cur_pid;
	char *cur_buf;
	int remain, next_pid, offset;
	bool dirty;
public:
	record_manager(pager *pg) : pg(pg) {}
	void open(int pid, int pos, bool dirty);
	void seek(int offset);
	void write(const void* data, int size);
	void read(void* buf, int size);
	void forward(int size);
};

#endif
