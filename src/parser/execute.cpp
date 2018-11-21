#include <stdio.h>
#include <stdlib.h>
#include "execute.h"

void execute_create_table(const table_def_t *table)
{
	printf("[create] table name = %s\n", table->name);
	for(field_item_t *field = table->fields; field; field = field->next)
	{
		printf("  [field] name = %s, type = %d, width = %d, flags = %d\n",
				field->name, field->type, field->width, field->flags);
	}
}

void execute_quit()
{
	exit(0);
}
