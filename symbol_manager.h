#include "symbol_list.h"

typedef struct symbol_manager_t {
	symbol_list_t* symbol_list;
	int param_count;
	int var_count;
} symbol_manager_t;

symbol_manager_t* new_symbol_manager();
char *next_register(char *name);
symbol_manager_t* copy_manager(symbol_manager_t* symbol_manager);
symbol_manager_t* add_param(symbol_manager_t* symbol_manager, char* name);
symbol_manager_t* add_var(symbol_manager_t* symbol_manager, char* name, int offset);
symbol_manager_t* add_lab(symbol_manager_t* symbol_manager, char* name);
symbol_manager_t* add_lab_with_copy(symbol_manager_t* symbol_manager, char* name, int depth) ;
symbol_list_t* get_symbol(symbol_manager_t* symbol_manager, char* name);
char* get_reg(symbol_manager_t* symbol_manager, char* name);
void check_usage_validity(symbol_manager_t* symbol_manager, char* name, bool lab);
symbol_manager_t* join(symbol_manager_t* sm1, symbol_manager_t* sm2);
