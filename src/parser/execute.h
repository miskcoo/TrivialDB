#ifndef __TRIVIALDB_PARSER_EXECUTE__
#define __TRIVIALDB_PARSER_EXECUTE__

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

void execute_create_table(const table_def_t *table);
void execute_quit();

#ifdef __cplusplus
}
#endif

#endif
