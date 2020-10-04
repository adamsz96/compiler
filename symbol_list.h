
#define FALSE 0
#define TRUE 1
#define VARSBUTNOTLABELS 2

#define DEBUG
#include <stdbool.h>


#ifndef _SYMBOL_LIST
#define _SYMBOL_LIST



typedef struct symbol_list_t{
	char *name;
	short label;
	char* reg;
	int offset;
	int is_var;
	int idx;
	int type;
	int depth;
	struct symbol_list_t *next;
} symbol_list_t;


symbol_list_t* new_empty_list();
symbol_list_t* add(symbol_list_t* list, char* name, int idx, char* reg, int offset, short is_var, int type, int depth);
int check_if_contains(symbol_list_t* list, char* name);
symbol_list_t* get_element(symbol_list_t* list, char* name);
void check_declaration_validity(symbol_list_t* list, char* name);
symbol_list_t* copy_list(symbol_list_t* list);
void print_list(symbol_list_t*);
void print_element(symbol_list_t* node);
void check_lab_usage_validity(symbol_list_t* list, char* name);
void check_var_usage_validity(symbol_list_t* list, char* name);

#endif

extern int error;

