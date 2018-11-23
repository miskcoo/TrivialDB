#include <cstring>
#include <cstdio>
#include "table_header.h"
#include "../parser/defs.h"

void fill_table_header(table_header_t *header, const table_def_t *table)
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
				header->col_length[index] = field->width;
				break;
			default:
				// TODO: report not supported type.
				break;
		}

		header->col_offset[index] = offset;
		offset += header->col_length[index];
		// TODO: load constraint infos
	}

	/* add '__rowid__' column */
	int index = header->col_num++;
	std::strcpy(header->col_name[index], "__rowid__");
	header->col_type[index]   = COL_TYPE_INT;
	header->col_length[index] = 4;
	header->col_offset[index] = 0;
	header->main_index = index;
	header->is_main_index_additional = 1;
	header->flag_indexed |= 1 << index;
	header->flag_primary |= 1 << index;

}
