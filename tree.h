#ifndef CODE
typedef struct burm_state *STATEPTR_TYPE; 
#endif

typedef enum {
    OP_NOT = 1,
    OP_ADD,
    OP_MULT,
    OP_AND,
    OP_SM_EQ,
    OP_NEQ,
    OP_NUM,
    OP_ID,
    OP_EXP,
    OP_PT,
    OP_RET,
    OP_EMPTY,
    OP_SUB,
    OP_POINTER,
    OP_ASS,
    OP_IF,
    OP_ELSE_IF,
    OP_LOOP,
    OP_BREAK,
    OP_CONT
} operator_t;

typedef struct tree_t{
	operator_t opr;
	struct tree_t *left_child;
	struct tree_t *right_child;
	STATEPTR_TYPE state_label;
	char *name;
	long value;
	char *reg;
	int offset;
	int is_var;
	int par_idx;
	int depth;
}tree_t;

typedef tree_t *treepointer;

#define NODEPTR_TYPE    	treepointer
#define OP_LABEL(p)     	((p)->opr)
#define LEFT_CHILD(p)   	((p)->left_child)
#define RIGHT_CHILD(p)  	((p)->right_child)
#define STATE_LABEL(p)  	((p)->state_label)
#define PANIC			printf
void print_tree(tree_t* tree);
tree_t* new_id_leaf(char* name, char* reg, int offset, short is_var);
tree_t* new_num_leaf(long value, char* reg, int offset, short is_var);
tree_t* new_var_leaf(char* name, int offset);
tree_t* new_var_leaf_r(char* name, char* reg, int offset, short is_valid);
tree_t* new_node(operator_t opr, tree_t* left, tree_t* right);
tree_t* new_node_r(operator_t opr, tree_t* left, tree_t* right, char*reg, int offset, short is_var);
tree_t* new_control_node(operator_t opr, tree_t* left, tree_t* right, int depth);
