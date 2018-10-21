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
			auto upper = page.split(pid);
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

	int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
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
	int now, char* addr, key_t key, const char *data, int data_size)
{
	leaf_page page { addr, pg };

	int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
		return page.get_key(id) < key;
	} );

	insert_ret ret;
	ret.split = false;

	bool succ_ins = page.insert(ch_pos, data, data_size);
	if(!succ_ins)
	{
		auto upper = page.split(now);

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

btree::search_result btree::lower_bound(key_t key)
{
	return lower_bound(root_page_id, key);
}

btree::search_result btree::lower_bound(int now, key_t key)
{
	char *addr = pg->read_for_write(now);
	uint16_t magic = general_page::get_magic_number(addr);
	if(magic == PAGE_FIXED)
	{
		interior_page page { addr, pg };
		int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
			return page.get_key(id) < key;
		} );

		ch_pos = std::min(page.size() - 1, ch_pos);
		return lower_bound(page.get_child(ch_pos), key);
	} else {
		assert(magic == PAGE_VARIANT);
		leaf_page page { addr, pg };
		int pos = ::lower_bound(0, page.size(), [&](int id) {
			return page.get_key(id) < key;
		} );

		if(pos == page.size())
			return { 0, 0 };
		else return { now, pos };
	}
}

template<typename Page>
btree::merge_ret btree::erase_try_merge(int pid, char *addr)
{
	Page page { addr, pg };

	if(page.underflow())
	{
		char *next_addr = nullptr, *prev_addr = nullptr;
		if(page.next_page())
		{
			next_addr = pg->read(page.next_page());
			Page next_page { next_addr, pg };
			if(!next_page.underflow_if_remove(0))
			{
				pg->mark_dirty(page.next_page());
				page.move_from(next_page, 0, page.size());
				return { false, false, 0 };
			}
		}

		if(page.prev_page())
		{
			prev_addr = pg->read(page.prev_page());
			Page prev_page { prev_addr, pg };
			if(!prev_page.underflow_if_remove(prev_page.size() - 1))
			{
				pg->mark_dirty(page.prev_page());
				page.move_from(prev_page, prev_page.size() - 1, 0);
				return { false, false, 0 };
			}
		}

		if(next_addr)
		{
			int next_pid = page.next_page();
			bool succ_merge = page.merge( { next_addr, pg }, pid);
			UNUSED(succ_merge);
			assert(succ_merge);
			pg->free_page(next_pid);
			return { false, true, pid };
		} else if(prev_addr) {
			int prev_pid = page.prev_page();
			Page prev_page { prev_addr, pg };
			bool succ_merge = prev_page.merge(page, prev_pid);
			UNUSED(succ_merge);
			assert(succ_merge);
			pg->free_page(pid);
			return { true, false, prev_pid };
		}
	} 

	return { false, false, 0 };
}

btree::erase_ret btree::erase(int now, key_t key)
{
	char *addr = pg->read_for_write(now);
	uint16_t magic = general_page::get_magic_number(addr);
	if(magic == PAGE_FIXED)
	{
		interior_page page { addr, pg };
		int ch_pos = ::lower_bound(0, page.size(), [&](int id) {
			return page.get_key(id) < key;
		} );

		ch_pos = std::min(page.size() - 1, ch_pos);
		erase_ret ret = erase(page.get_child(ch_pos), key);

		if(!ret.found) return ret;

		addr = pg->read_for_write(now);
		page = interior_page { addr, pg };
		if(ret.merged_right)
		{
			page.erase(ch_pos + 1);
			page.set_key(ch_pos, ret.largest);
			page.set_child(ch_pos, ret.merged_pid);
		} else if(ret.merged_left) {
			page.erase(ch_pos);
			page.set_key(ch_pos - 1, ret.largest);
			page.set_child(ch_pos - 1, ret.merged_pid);
		} else {
			page.set_key(ch_pos, ret.largest);
		}

		merge_ret mret = erase_try_merge<interior_page>(now, addr);
		return { true, mret.merged_left, mret.merged_right,
			mret.merged_pid, page.get_key(page.size() - 1) };
	} else {
		assert(magic == PAGE_VARIANT);
		leaf_page page { addr, pg };
		int pos = ::lower_bound(0, page.size(), [&](int id) {
			return page.get_key(id) < key;
		} );

		if(pos == page.size() || page.get_key(pos) != key)
			return { false, false, false, 0, 0};

		page.erase(pos);
		auto ret = erase_try_merge<leaf_page>(now, addr);
		return { true, ret.merged_left, ret.merged_right,
			ret.merged_pid, page.get_key(page.size() - 1) };
	}
}

bool btree::erase(key_t key)
{
	erase_ret ret = erase(root_page_id, key);

	char *addr = pg->read_for_write(root_page_id);
	uint16_t magic = general_page::get_magic_number(addr);
	if(magic == PAGE_FIXED)
	{
		interior_page page { addr, pg };
		if(page.size() == 1 && page.get_child(0))
		{
			debug_puts("B-tree merge root.");
			pg->free_page(root_page_id);
			root_page_id = page.get_child(0);
		}
	}

	return ret.found;
}
