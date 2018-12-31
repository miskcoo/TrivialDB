#ifndef __TRIVIALDB_PARSER_EXECUTE__
#define __TRIVIALDB_PARSER_EXECUTE__

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

void execute_create_database(const char *db_name);
void execute_use_database(const char *db_name);
void execute_drop_database(const char *db_name);
void execute_show_database(const char *db_name);
void execute_create_table(const table_def_t *table);
void execute_drop_table(const char *table_name);
void execute_show_table(const char *table_name);
void execute_insert(const insert_info_t *insert_info);
void execute_delete(const delete_info_t *delete_info);
void execute_select(const select_info_t *select_info);
void execute_update(const update_info_t *update_info);
void execute_create_index(const char *table_name, const char *col_name);
void execute_drop_index(const char *table_name, const char *col_name);
void execute_switch_output(const char *output_filename);
void execute_quit();

#ifdef __cplusplus
}
#endif

#endif
