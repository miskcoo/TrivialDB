#include "btree.h"
#include "../algo/search.h"

btree::btree(pager *pg, int root_page_id)
	: pg(pg), root_page_id(root_page_id)
{
	if(root_page_id == 0)
	{
		this->root_page_id = pg->new_page();
		leaf_page { pg->read_for_write(this->root_page_id), pg }.init();
	}
}

template<typename Page>
inline void btree::insert_split_root(insert_ret ret)
{
	if(ret.split)
	{
		debug_puts("B-tree split root.");
		int new_pid = pg->new_page();
		interior_page page { pg->read_for_write(new_pid), pg };
		page.init();

		Page lower { ret.lower_half, pg };
		Page upper { ret.upper_half, pg };
		page.insert(0, root_page_id, lower.get_key(lower.size() - 1));
		page.insert(1, ret.upper_pid, upper.get_key(upper.size() - 1));
		root_page_id = new_pid;
	}
}

void btree::insert(key_t key, const char *data, int data_size)
{
	char *addr = pg->read_for_write(root_page_id);
	uint16_t magic = general_page::get_magic_number(addr);
	if(magic == PAGE_FIXED)
	{
		insert_ret ret = insert_interior(
			root_page_id, addr, key, data, data_size);
		insert_split_root<interior_page>(ret);
	} else {
		assert(magic == PAGE_VARIANT);
		insert_ret ret = insert_leaf(
			root_page_id, addr, key, data, data_size);
		insert_split_root<leaf_page>(ret);
	}
}

template<typename Page, typename ChPage>
inline btree::insert_ret btree::insert_post_process(
	int pid, int ch_pid, int ch_pos, insert_ret ch_ret)
{
	insert_ret ret;
	ret.split = false;
	Page page { pg->read_for_write(pid), pg };
	if(ch_ret.split)
	{
		ChPage lower_ch { ch_ret.lower_half, pg };
		ChPage upper_ch { ch_ret.upper_half, pg };
		page.set_key(ch_pos, lower_ch.get_key(lower_ch.size() - 1));
		key_t ch_largest = upper_ch.get_key(upper_ch.size() - 1);
		bool succ_ins = page.insert(ch_pos + 1, ch_ret.upper_pid, ch_largest);
		if(!succ_ins)
		{
			auto upper = page.split();
			Page upper_page = upper.second;
			Page lower_page = page;
			if(ch_pos < lower_page.size())
			{
				succ_ins = lower_page.insert(
					ch_pos + 1, ch_ret.upper_pid, ch_largest);
				assert(succ_ins);
			} else {
				succ_ins = upper_page.insert(
					ch_pos - lower_page.size() + 1,
					ch_ret.upper_pid, ch_largest
				);
				assert(succ_ins);
			}

			ret.split = true;
			ret.lower_half = lower_page.buf;
			ret.upper_half = upper_page.buf;
			ret.upper_pid  = upper.first;
		}
	} else {
		ChPage ch_page { pg->read(ch_pid), pg };
		page.set_key(ch_pos, ch_page.get_key(ch_page.size() - 1));
	}

	return ret;
}

btree::insert_ret btree::insert_interior(
	int now, char* addr, key_t key, const char *data, int data_size)
{
	interior_page page { addr, pg };

	int ch_pos = lower_bound(0, page.size(), [&](int id) {
		return page.get_key(id) < key;
	} );

	ch_pos = std::min(page.size() - 1, ch_pos);

	int ch_pid = page.get_child(ch_pos);
	char *ch_addr = pg->read_for_write(ch_pid);
	uint16_t ch_magic = general_page::get_magic_number(ch_addr);

	if(ch_magic == PAGE_FIXED)
	{
		auto ch_ret = insert_interior(ch_pid, ch_addr, key, data, data_size);
		return insert_post_process<interior_page, interior_page>(
			now, ch_pid, ch_pos, ch_ret
		);
	} else {
		// leaf page
		assert(ch_magic == PAGE_VARIANT);
		auto ch_ret = insert_leaf(ch_pid, ch_addr, key, data, data_size);
		return insert_post_process<interior_page, leaf_page>(
			now, ch_pid, ch_pos, ch_ret
		);
	}
}

btree::insert_ret btree::insert_leaf(
	int, char* addr, key_t key, const char *data, int data_size)
{
	leaf_page page { addr, pg };

	int ch_pos = lower_bound(0, page.size(), [&](int id) {
		return page.get_key(id) < key;
	} );

	insert_ret ret;
	ret.split = false;

	bool succ_ins = page.insert(ch_pos, data, data_size);
	if(!succ_ins)
	{
		auto upper = page.split();

		leaf_page upper_page = upper.second;
		leaf_page lower_page = page;

		if(ch_pos < lower_page.size())
		{
			succ_ins = lower_page.insert(ch_pos, data, data_size);
			assert(succ_ins);
		} else {
			succ_ins = upper_page.insert(
				ch_pos - lower_page.size(), data, data_size);
			assert(succ_ins);
		}

		ret.split = true;
		ret.lower_half = lower_page.buf;
		ret.upper_half = upper_page.buf;
		ret.upper_pid  = upper.first;
	}

	return ret;
}
