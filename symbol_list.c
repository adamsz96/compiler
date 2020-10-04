#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_list.h"
#include <stdbool.h>

symbol_list_t* new_empty_list(){
	return (symbol_list_t*) NULL;
}

symbol_list_t* add(symbol_list_t* list, char* name, int idx, char* reg, int offset, short is_var, int type, int depth)
{
	symbol_list_t *cur,*cur2;
	symbol_list_t *node;
	if(list == NULL) {
	    node = (symbol_list_t*) malloc(sizeof(symbol_list_t));
	    node->name = strdup(name);
	    node->idx = idx;
	    node->reg = reg;
	    node->offset=offset;
		node->is_var=is_var;
	    node->type = type;
	    node->depth = depth;
	    node->next = NULL;
	    list = node;
	    return list;
	}

	
	cur = list;
	cur2 = list;
	cur2 = cur->next;
		
	while(cur2 != NULL){
		cur = cur->next;
		cur2 = cur2->next;
	}
	node = (symbol_list_t*) malloc(sizeof(symbol_list_t));
	node->name = strdup(name);
	node->idx = idx;
	node->reg = reg;
    node->offset=offset;
	node->is_var=is_var;
       node->depth = depth;
	node->next = NULL;
	node->type = type;
	cur->next = node; 
	return list;
}


int check_if_contains(symbol_list_t* list, char* name)
{
	symbol_list_t * node = list;
	int contains = 0;
	if(node == NULL){
		return 0;
	}
	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			contains = 1;
		}
		node = node->next;
	}
	return contains;
}

void check_lab_usage_validity(symbol_list_t* list, char* name)
{
	symbol_list_t * node = list;
	int contains = 0;

	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			if(node->type == 3){
				contains = 1;
			} else {
				contains = 2;
			}
		}
		node = node->next;
	}
	if(contains == 0) {
		printf("AG Error: Undeclared Variable\n");
		exit(3);
	}
	if(contains == 2) {
		printf("AG Error: Wrong Type\n");
		exit(3);
	}
}

void check_var_usage_validity(symbol_list_t* list, char* name)
{
	symbol_list_t * node = list;
	int contains = 0;

	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			if(node->type != 3){
				contains = 1;
			} else {
				contains = 2;
			}
		}
		node = node->next;
	}
	if(contains == 0) {
		printf("AG Error: Undeclared Variable\n");
		exit(3);
	}
	if(contains == 2) {
		printf("AG Error: Wrong Type\n");
		exit(3);
	}
}

symbol_list_t* get_element(symbol_list_t* list, char* name)
{
	symbol_list_t * node = list;
	if(node == NULL){
		return NULL;
	}
	while(node != NULL){
		if(strcmp(node->name, name) == 0){
			return node;
		}
		node = node->next;
	}
	return NULL;
}

void check_declaration_validity(symbol_list_t* list, char* name)
{
	if(check_if_contains(list, name) == 1)
	{
		printf("AG Error: Double Declaration\n");
		exit(3);
	}
}



symbol_list_t* copy_list(symbol_list_t* list){
	symbol_list_t* new_list = new_empty_list();
	symbol_list_t* cur = list;
	if(list == NULL)
		return new_list;
	while(cur != NULL){
		new_list = add(new_list, cur->name, cur->idx, cur->reg, cur->offset, cur->is_var, cur->type, cur->depth);
		cur = cur->next;
	}
	return new_list;
}

void print_list(symbol_list_t* list)
{
	symbol_list_t * node = list;
	while(node != NULL){
		print_element(node);
		node = node->next;
	}
	printf("\n");
}

void print_element(symbol_list_t* node)
{
	char* type = "none";
	if(node->type == 1){
	   	type="par";
	}
	else if(node->type == 2){
		type="var";
	}
	else if(node->type == 3){
		type="lab";
	}	
	printf("<idx:%d name:%s reg:%s type:%s> ",node -> idx,node -> name, node -> reg, type);
}
