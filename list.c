#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <stdbool.h>

list_t* new_empty_list(){
	return (list_t*) NULL;
}

list_t* add(list_t* list, char* name, bool declaration)
{
	
	if(declaration == true) {
	    check_declaration_validity(list, name);
	}
	
	list_t *cur,*cur2;
	list_t *node;
	if(list == NULL) {
	    node = (list_t*) malloc(sizeof(list_t));
	    node->name = strdup(name);
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
	node = (list_t*) malloc(sizeof(list_t));
	node->name = strdup(name);
	node->next = NULL;
	cur->next = node; 
	return list;
}


int check_if_contains(list_t* list, char* name)
{
	list_t * node = list;
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

void check_declaration_validity(list_t* list, char* name)
{
	printf("Declaration testing\n");
	if(check_if_contains(list, name) == 1)
	{
		printf("AG Error: Double Declaration\n");
		exit(3);
	}
}

void check_usage_validity(list_t* list, char* name)
{
	if(check_if_contains(list, name) == 0)
	{
		printf("AG Error: Undeclared Variable\n");
		exit(3);
	}
}

list_t* copy_list(list_t* list){
	list_t* new_list = new_empty_list();
	list_t* cur = list;
	if(list == NULL)
		return new_list;
	while(cur != NULL){
		new_list = add(new_list, cur->name, false);
		cur = cur->next;
	}
	return new_list;
}

void print_list(list_t* list)
{
	list_t * node = list;
	while(node != NULL){
		printf("%s ",node -> name);
		node = node->next;
	}
	printf("\n");
}
