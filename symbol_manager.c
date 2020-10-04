#include "symbol_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

char* get_param_register(int index) {
	char *registers[]={"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
	return registers[index];
}

char *next_register(char *name) {
	char *registers[]={"rax","r11", "r10", "r9", "r8", "rcx", "rdx", "rsi", "rdi"};
	int index, i;
	if(name==(char *)NULL) {
		index=0;
	}
	else {
		for(i=0;i<9;i++) {
			if(!strcmp(name,registers[i])) {
				index=i+1;
				break;
			}
		}
	}
	return registers[index];
}



symbol_manager_t* new_symbol_manager() {
	symbol_manager_t* new = (symbol_manager_t*) malloc(sizeof(symbol_manager_t));

	new->symbol_list = new_empty_list();
	new->param_count = 0;
	new->var_count = 0;

	return new;
}

symbol_manager_t* add_param(symbol_manager_t* symbol_manager, char* name) {
	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	char* reg = get_param_register(symbol_manager->param_count);

	check_declaration_validity(symbol_list, name);
	symbol_list = add(symbol_list, name, symbol_manager->param_count, reg, 0, 0, 1, 0);

	symbol_manager->symbol_list = symbol_list;
	symbol_manager->param_count++;

	return symbol_manager;
}

symbol_manager_t* join(symbol_manager_t* sm1, symbol_manager_t* sm2) {
	symbol_list_t* sl1 = sm1 -> symbol_list;
	symbol_list_t* sl2 = sm2 -> symbol_list;

	symbol_list_t * node = sl2;
	while(node != NULL){
		add_param(sm1, node->name);
		node = node->next;
	}
	/*printf("\n");*/
	return sm1;

}

void check_usage_validity(symbol_manager_t* symbol_manager, char* name, bool lab) {
	/*printf("check_usage_validity\n");*/
	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	if(lab == true) {
		check_lab_usage_validity(symbol_list, name);
	}else {
		check_var_usage_validity(symbol_list, name);
	}
}

symbol_manager_t* add_var(symbol_manager_t* symbol_manager, char* name, int offset) {

	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	check_declaration_validity(symbol_list, name);
	symbol_list = add(symbol_list, name, 0, "rsp", offset, 1, 2, 0);
	symbol_manager->symbol_list = symbol_list;

	return symbol_manager;
}

symbol_manager_t* add_lab(symbol_manager_t* symbol_manager, char* name) {

	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	check_declaration_validity(symbol_list, name);
	symbol_list = add(symbol_list, name, 0, "reg", 0, 0, 3, 0);
	symbol_manager->symbol_list = symbol_list;

	return symbol_manager;
}

symbol_manager_t* add_lab_with_copy(symbol_manager_t* symbol_manager, char* name, int depth) {

	symbol_list_t* symbol_list = copy_list(symbol_manager->symbol_list);
	check_declaration_validity(symbol_list, name);
	symbol_list = add(symbol_list, name, 0, "reg", 0, 0, 3, depth);
	
	symbol_manager_t* new = new_symbol_manager();
	new->symbol_list = symbol_list;

	return new;
}

symbol_manager_t* copy_manager(symbol_manager_t* symbol_manager) {
	symbol_list_t* symbol_list = copy_list(symbol_manager->symbol_list);
	symbol_manager_t* new = new_symbol_manager();
	new->symbol_list = symbol_list;
	new->param_count = symbol_manager->param_count;
	new->var_count = symbol_manager->var_count;
}

void print_symbols(symbol_manager_t* symbol_manager) {
	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	print_list(symbol_list);

}

symbol_list_t* get_symbol(symbol_manager_t* symbol_manager, char* name) {
	symbol_list_t* symbol_list = symbol_manager->symbol_list;
	symbol_list_t* symbol = get_element(symbol_list, name);
	return symbol;
}

char* get_reg(symbol_manager_t* symbol_manager, char* name) {
	check_usage_validity(symbol_manager, name, false);
	symbol_list_t* symbol = get_symbol(symbol_manager, name);
	char* reg = symbol->reg;
	return reg;
}

int get_offset(symbol_manager_t* symbol_manager, char* name) {
	check_usage_validity(symbol_manager, name, false);
	symbol_list_t* symbol = get_symbol(symbol_manager, name);
	int offset = symbol->offset;
	return offset;
}

short get_is_var(symbol_manager_t* symbol_manager, char* name) {
	check_usage_validity(symbol_manager, name, false);
	symbol_list_t* symbol = get_symbol(symbol_manager, name);
	short is_var = symbol->is_var;
	return is_var;
}

int get_depth(symbol_manager_t* symbol_manager, char* name) {
	check_usage_validity(symbol_manager, name, true);
	symbol_list_t* symbol = get_symbol(symbol_manager, name);
	int depth = symbol->depth;
	return depth;
}
