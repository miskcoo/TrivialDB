#include <cstring>
#include <cstdio>
#include "table_header.h"
#include "../parser/defs.h"

bool fill_table_header(table_header_t *header, const table_def_t *table)
{
	std::memset(header, 0, sizeof(table_header_t));
	std::strncpy(header->table_name, table->name, MAX_NAME_LEN);
	int offset = 8;  // 4 bytes for __rowid__, and 4 bytes for not null
	for(field_item_t *field = table->fields; field; field = field->next)
	{
		int index = header->col_num++;
		std::strncpy(header->col_name[index], field->name, MAX_NAME_LEN);
		switch(field->type)
		{
			case FIELD_TYPE_INT:
				header->col_type[index]   = COL_TYPE_INT;
				header->col_length[index] = 4;
				break;
			case FIELD_TYPE_FLOAT:
				header->col_type[index]   = COL_TYPE_FLOAT;
				header->col_length[index] = 4;
				break;
			case FIELD_TYPE_CHAR:
			case FIELD_TYPE_VARCHAR:
				header->col_type[index]   = COL_TYPE_VARCHAR;
				header->col_length[index] = field->width + 1;
				break;
			default:
				std::fprintf(stderr, "[Error] Unsupported type.\n");
				break;
		}

		header->col_offset[index] = offset;
		offset += header->col_length[index];
		if(field->flags & FIELD_FLAG_NOTNULL)
			header->flag_notnull |= 1 << index;
		if(field->flags & FIELD_FLAG_PRIMARY)
			header->flag_primary |= 1 << index;
		if(field->flags & FIELD_FLAG_UNIQUE)
			header->flag_unique |= 1 << index;
	}

	auto lookup_column = [&](const char *name) -> int {
		for(int i = 0; i != header->col_num; ++i)
		{
			if(std::strcmp(name, header->col_name[i]) == 0)
				return i;
		}

		std::fprintf(stderr, "[Error] Column `%s` not found.\n", name);
		return -1;
	};

	/* resolve constraint field */
	for(linked_list_t *link_ptr = table->constraints; link_ptr; link_ptr = link_ptr->next)
	{
		int cid;
		table_constraint_t *constraint = (table_constraint_t*)link_ptr->data;
		switch(constraint->type)
		{
			case TABLE_CONSTRAINT_UNIQUE:
				cid = lookup_column(constraint->column_ref->column);
				if(cid < 0) return false;
				header->flag_unique |= 1 << cid;
				break;
			case TABLE_CONSTRAINT_PRIMARY_KEY:
				cid = lookup_column(constraint->column_ref->column);
				if(cid < 0) return false;
				header->flag_primary |= 1 << cid;
				break;
			default:
				std::fprintf(stderr, "[Error] Unsupported constraint.\n");
				return false;
		}
	}

	/* add '__rowid__' column (with highest index) */
	int index = header->col_num++;
	std::strcpy(header->col_name[index], "__rowid__");
	header->col_type[index]   = COL_TYPE_INT;
	header->col_length[index] = 4;
	header->col_offset[index] = 0;
	header->main_index = index;
	header->is_main_index_additional = 1;
	header->flag_indexed |= 1 << index;
	if(!header->flag_primary)
		header->flag_primary |= 1 << index;
	header->flag_unique |= 1 << index;
	header->flag_unique |= header->flag_primary;
	header->flag_indexed |= header->flag_primary;
	header->auto_inc = 1;

	/* check constraint availability */
	if(header->flag_primary != (header->flag_primary & -header->flag_primary))
	{
		std::fprintf(stderr, "[Error] Find two or more primary key.\n");
		return false;
	}

	return true;
}

void table_header_t::dump()
{
	std::printf("======== Table Info Begin ========\n");
	std::printf("Table name  = %s\n", table_name);
	std::printf("Column size = %d\n", col_num);
	std::printf("Record size = %d\n", records_num);
	for(int i = 0; i != col_num; ++i)
	{
		std::printf("  [column] name = %s, length = %d, flag = ", col_name[i], col_length[i]);
		if(flag_notnull & (1 << i))
			std::printf("NOT_NULL ");
		if(flag_primary & (1 << i))
			std::printf("PRIMARY ");
		if(flag_unique & (1 << i))
			std::printf("UNIQUE ");
		if(flag_indexed & (1 << i))
			std::printf("INDEXED ");
		std::puts("");

	}
	std::printf("======== Table Info End   ========\n");
}
