
#define FALSE 0
#define TRUE 1
#define VARSBUTNOTLABELS 2

#define DEBUG
#include <stdbool.h>

typedef struct list_t{
	char *name;
	short label;
	struct list_t *next;
} list_t;

list_t* new_empty_list();
list_t* add(list_t* list, char* name, bool declaration);
int check_if_contains(list_t* list, char* name);
void check_declaration_validity(list_t* list, char* name);
void check_usage_validity(list_t* list, char* name);
list_t* copy_list(list_t* list);
void print_list(list_t*);

extern int error;
