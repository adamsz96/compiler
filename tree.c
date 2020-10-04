#include <stdlib.h>
#include <stdio.h>
#include "tree.h"

tree_t* new_id_leaf(char* name, char* reg, int offset, short is_var) {
	
	/*printf("new_leaf\n");*/
	tree_t *new=(tree_t *)malloc(sizeof(tree_t));
	
	new->left_child=(tree_t *)NULL;
	new->right_child=(tree_t *)NULL;
	new->opr=OP_ID;
	new->name=name;
	new->reg=reg;
	new->offset=offset;
	new->is_var=is_var;

	return new;
}

tree_t* new_var_leaf(char* name, int offset) {
	
	/*printf("new_leaf\n");*/
	tree_t *new=(tree_t *)malloc(sizeof(tree_t));
	
	new->left_child=(tree_t *)NULL;
	new->right_child=(tree_t *)NULL;
	new->opr=OP_ID;
	new->name=name;
	new->reg="rsp";
	new->offset=offset;
	new->is_var=1;

	return new;
}

tree_t* new_var_leaf_r(char* name, char* reg, int offset, short is_var) {
	
	/*printf("new_leaf\n");*/
	tree_t *new=(tree_t *)malloc(sizeof(tree_t));
	
	new->left_child=(tree_t *)NULL;
	new->right_child=(tree_t *)NULL;
	new->opr=OP_ID;
	new->name=name;
	new->reg=reg;
	new->offset=offset;
	new->is_var=is_var;


	return new;
}

void print_tree(tree_t* tree) {
	if(tree != NULL) {
		printf("<");
		switch(tree->opr) {
			case 1: 
				printf("not");
				break;
			case 2: 
				printf("add");
				break;
			case 3:
				printf("mul");
				break;
			case 4:
				printf("and");
				break;
			case 5:
				printf("sm_eq");
				break;
			case 6:
				printf("neq");
				break;
			case 7:
				printf("num");
				break;
			case 8:
				printf("id ");
				break;
			case 9:
				printf("expr");
				break;
			case 10:
				printf("pt ");
				break;
			case 11:
				printf("ret");
				break;
			case 12:
				printf("emp");
				break;
			case 13:
				printf("sub");
				break;
			case 14:
				printf("ptr");
				break;
			case 15:
				printf("ass");
				break;
		}
		printf(", ");
		if(tree->reg != NULL) {
			printf("%s", tree->reg);
		}else {
			printf("%s", "null");
		}
		printf(", ");
		printf("%i", tree->offset);
		printf(", ");

		printf(", ");
		printf("%i", tree->is_var);
		printf(", ");

		printf(", ");
		printf("%i", tree->value);
		printf(", ");
		if(tree->name != NULL) {
			printf("%s", tree->name);
		}else {
			printf("%s", "null");
		}
		printf(">\n");

		print_tree(tree->left_child);
		print_tree(tree->right_child);
	}

}

tree_t* new_num_leaf(long value, char* reg, int offset, short is_var) {
	
	/*printf("new_leaf\n");*/
	tree_t *new=(tree_t *)malloc(sizeof(tree_t));
	
	new->left_child=(tree_t *)NULL;
	new->right_child=(tree_t *)NULL;
	new->opr=OP_NUM;
	new->value=value;
	new->reg=reg;
	new->offset=offset;
	new->is_var=is_var;

	return new;
}

tree_t* new_node(operator_t opr, tree_t* left, tree_t* right) {

	tree_t *new=(tree_t *)malloc(sizeof(tree_t));

	new->left_child=left;
	new->right_child=right;
	new->opr=opr;

	if(opr == OP_RET) {
		new->reg = left->reg;
	}

	return new;
}

tree_t* new_node_r(operator_t opr, tree_t* left, tree_t* right, char*reg, int offset, short is_var) {

	tree_t *new=(tree_t *)malloc(sizeof(tree_t));

	new->left_child=left;
	new->right_child=right;
	new->opr=opr;
	new->reg=reg;
	new->offset=offset;
	new->is_var=is_var;

	if(opr == OP_RET) {
		new->reg = left->reg;
		new->offset=offset;
		new->is_var=is_var;
	}

	return new;
}

tree_t* new_control_node(operator_t opr, tree_t* left, tree_t* right, int depth) {

	tree_t *new=(tree_t *)malloc(sizeof(tree_t));

	new->left_child=left;
	new->right_child=right;
	new->opr=opr;

	new->depth=depth;


	return new;
}
