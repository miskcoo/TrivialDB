#ifndef __TRIVIALDB_BTREE__
#define __TRIVIALDB_BTREE__

#include "../page/pager.h"
#include "../page/fixed_page.h"
#include "../page/data_page.h"

/* Each node of the b-tree is a page.
 * For an interior node, the key of a page element is the largest
 * element of its children. */

template<typename KeyType, typename Comparer>
class btree
{
	pager *pg;
	int root_page_id;
	Comparer compare;
public:
	typedef KeyType key_t;
	typedef fixed_page<key_t> interior_page;
	typedef data_page<key_t>  leaf_page;
	typedef std::pair<int, int> search_result;  // (page_id, pos)
public:
	/* create/load a btree
	 * If root_page_id = 0, create a new btree.
	 * If root_page_id != 0, load an existed btree */
	btree(pager *pg, int root_page_id, Comparer compare);

	void insert(key_t key, const char* data, int data_size);
	// erase one of the elements with specified key randomly
	bool erase(key_t key);
	// the first element x for which x >= key
	search_result lower_bound(key_t key);

	int get_root_page_id() { return root_page_id; }

private:
	struct insert_ret
	{
		bool split;
		int upper_pid;
		char *lower_half, *upper_half;
	};

	struct erase_ret
	{
		bool found;
		bool merged_left, merged_right;
		int merged_pid;
		key_t largest;
	};

	struct merge_ret
	{
		bool merged_left, merged_right;
		int merged_pid;
	};

	template<typename Page, typename ChPage>
	insert_ret insert_post_process(int, int, int, insert_ret);
	template<typename Page>
	void insert_split_root(insert_ret);
	insert_ret insert_interior(int, char*, key_t, const char*, int);
	insert_ret insert_leaf(int, char*, key_t, const char*, int);
	search_result lower_bound(int now, key_t key);
	erase_ret erase(int, key_t);
	template<typename Page>
	merge_ret erase_try_merge(int pid, char *addr);
};

class int_btree : public btree<int, bool(*)(int, int)>
{
	static bool integer_comparer(int x, int y) { return x < y; }
public:
	int_btree(pager *pg, int root_page_id = 0)
		: btree(pg, root_page_id, &int_btree::integer_comparer) {}
};

#endif
