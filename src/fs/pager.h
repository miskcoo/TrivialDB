#ifndef __TRIVIALDB_PAGER__
#define __TRIVIALDB_PAGER__

#include "page_fs.h"

class pager
{
	int fid;
public:
	pager() : fid(0) {}
	pager(const char* filename) : fid(0) { open(filename); }
	~pager() { close(); }
	
	bool open(const char* filename)
	{
		page_fs *fs = page_fs::get_instance();
		if(fid) fs->close(fid);
		fid = fs->open(filename);
		return fid;
	}

	void close()
	{
		if(fid) 
			page_fs::get_instance()->close(fid);
		fid = 0;
	}

	int new_page()
	{
		return page_fs::get_instance()->allocate(fid);
	}

	void free_page(int page_id)
	{
		page_fs::get_instance()->deallocate(fid, page_id);
	}

	const char* read(int page_id)
	{
		return page_fs::get_instance()->read(fid, page_id);
	}

	char* read_for_write(int page_id)
	{
		return page_fs::get_instance()->read_for_write(fid, page_id);
	}

	void mark_dirty(int page_id)
	{
		page_fs::get_instance()->mark_dirty(fid, page_id);
	}

};

#endif
