#ifndef __TRIVIALDB_BTREE__
#define __TRIVIALDB_BTREE__

#include "../page/pager.h"
#include "../page/fixed_page.h"
#include "../page/data_page.h"

/* Each node of the b-tree is a page.
 * For an interior node, the key of a page element is the largest
 * element of its children. */

class btree
{
	pager *pg;
	int root_page_id;
public:
	typedef int key_t;
	typedef fixed_page<key_t> interior_page;
	typedef data_page<key_t>  leaf_page;
	typedef std::pair<int, int> search_result;  // (page_id, pos)
public:
	/* create/load a btree
	 * If root_page_id = 0, create a new btree.
	 * If root_page_id != 0, load an existed btree */
	btree(pager *pg, int root_page_id = 0);

	void insert(key_t key, const char* data, int data_size);
	// the first element x for which x >= key
	search_result lower_bound(key_t key);
	// the last element x for which x >= key
	search_result upper_bound(key_t key);

	int get_root_page_id() { return root_page_id; }

private:
	struct insert_ret
	{
		bool split;
		int upper_pid;
		char *lower_half, *upper_half;
	};

	template<typename Page, typename ChPage>
	insert_ret insert_post_process(int, int, int, insert_ret);
	template<typename Page>
	void insert_split_root(insert_ret);
	insert_ret insert_interior(int, char*, key_t, const char*, int);
	insert_ret insert_leaf(int, char*, key_t, const char*, int);
	search_result lower_bound(int now, key_t key);
	search_result upper_bound(int now, key_t key);

};

#endif
