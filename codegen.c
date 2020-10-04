#include <stdio.h>
#include <string.h>
#include "codegen.h"
#include "tree.h"

int variables = 0;

char *get_byte_register(char *name){
	char *registers[]  =  {"rax", "r11", "r10",  "r9",  "r8", "rcx","rdx", "rsi", "rdi"};
	char *byteRegisters[]={"al", "r11b", "r10b", "r9b", "r8b", "cl", "dl", "sil", "dil"};
	int index,i;
	if(name==(char *)NULL){
		index=0;
	}
	else{
		for(i=0;i<9;i++){
			if(!strcmp(name,registers[i])){
				index=i;
				break;
			}
		}
	}
	return byteRegisters[index];
}

char *get_32_register(char *name){
	char *registers[]=    {"rax", "r11",  "r10",  "r9",  "r8",  "rcx", "rdx", "rsi", "rdi"};
	char *byteRegisters[]={"eax", "r11d", "r10d", "r9d", "r8d", "ecx", "edx", "esi", "edi"};
	int index,i;
	if(name==(char *)NULL){
		index=0;
	}
	else{
		for(i=0;i<9;i++){
			if(!strcmp(name,registers[i])){
				index=i;
				break;
			}
		}
	}
	return byteRegisters[index];
}

void function(char *name, int vars) {
	printf("\t.globl %s\n", name);
	printf("\t.type %s, @function\n", name);
	printf("%s:\n",name);
	if(vars > 0){
		printf("\tenter $%li,$0\n", 8*vars);
		variables = 1;
	}
}

void start_if(void) {
	printf("\nstart_if\n");
}

void end_if(int depth) {
	printf("end_if%d:\n", depth);
}

void start_loop(int depth) {
	printf("loop%d:\n", depth);
}

void break_loop(int depth) {
	printf("\tjmp end_lp%d\n", depth);
}

void cont_loop(int depth) {
	printf("\tjmp loop%d\n", depth);
}


void end_loop(int depth) {
	printf("\tjmp loop%d\n", depth);
	printf("end_lp%d:\n", depth);
}

void if_var(tree_t* par) {
	tree_t* expr = par->left_child;
	/*if (strcmp(expr->reg, "rax") != 0) {
		if(expr->is_var == 0) {
    		printf("\tmovq %%%s, %%%s\n",expr->reg, "rax");
		} else if(expr->is_var == 1) {
			printf("\tmovq %d(%%%s), %%%s\n",expr->offset*8+8,expr->reg,"rax");
		}
    }*/

	if(expr->is_var == 0) {
    	printf("\ttest $1, %%%s\n",expr->reg, "rax");
	}
	printf("\tjz end_if%d\n", par->depth);
}

void loop_var(tree_t* expr) {
	printf("\tjpe end_lp%d\n", expr->depth);
}

void if_else_var(tree_t* par) {

	tree_t* expr = par->left_child;
	/*if (strcmp(expr->reg, "rax") != 0) {
		if(expr->is_var == 0) {
    		printf("\tmovq %%%s, %%%s\n",expr->reg, "rax");
		} else if(expr->is_var == 1) {
			printf("\tmovq %d(%%%s), %%%s\n",expr->offset*8+8,expr->reg,"rax");
		}
    }*/

	if(expr->is_var == 0) {
    	printf("\ttest $1, %%%s\n",expr->reg, "rax");
	}
	printf("\tjz else_if%d\n", par->depth);
}

void else_jump(int target) {
	if(target != -1) {
	    printf("\tjmp end_if%d\n", target);
		printf("else_if%d:\n", target);
	}
}

void ret(void) {
	if(variables == 1) {
		printf("\tleave\n");
	}
    printf("\tret\n");
}

void ret_var(tree_t* src) {

	if (strcmp(src->reg, "rax") != 0) {
		if(src->is_var == 0) {
    		printf("\tmovq %%%s, %%%s\n",src->reg, "rax");
		} else if(src->is_var == 1) {
			printf("\tmovq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,"rax");
		}
    }
	if(variables == 1) {
		printf("\tleave\n");
	}
    printf("\tret\n");
}

void move_var_rax(tree_t* src) {
	if (strcmp(src->reg, "rax") != 0) {
		if(src->is_var == 0) {
    		printf("\tmovq %%%s, %%%s\n",src->reg, "rax");
		} else if(src->is_var == 1) {
			printf("\tmovq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,"rax");
		}
    }
	printf("\taddq $0, %%rax\n");
}

void move(char *src, char *dst) {
    if (strcmp(src, dst) != 0) {
    	printf("\tmovq %%%s, %%%s\n",src,dst);
    }
}

void move_num(long value, char *dst) {
    printf("\tmovq $%li, %%%s\n",value,dst);

}

void move_var(tree_t *src, tree_t *dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
			if (strcmp(src->reg, dst->reg) != 0) {
   				printf("\tmovq %%%s, %%%s\n",src->reg,dst->reg);
			}
		} else if(dst->is_var == 1) {
			printf("\tmovq %%%s, %d(%%%s)\n",src->reg, dst->offset*8+8,dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\tmovq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			if (strcmp(src->reg, dst->reg) != 0 || src->offset != dst->offset) {
				printf("\tpush %rax\n");
				printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
				printf("\tmovq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);	
				printf("\tpop %rax\n");
			}
		}	
	}
}


void move_num_var(long value, tree_t *dst) {
	if(dst->is_var == 0) {
    	printf("\tmovq $%li, %%%s\n",value,dst->reg);
	} else if(dst->is_var == 1) {
		printf("\tmovq $%li, %d(%%%s)\n",value, dst->offset*8+8,dst->reg);
	}
}


void add_term(long value, char *dst) {
    if(value != 0) {
    	printf("\taddq $%li, %%%s\n", value, dst);
    }
}

void add_term_var(long value, tree_t *dst) {
	if(value != 0) {
    	if(dst->is_var == 0) {
    		printf("\taddq $%li, %%%s\n",value,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\taddq $%li, %d(%%%s)\n",value, dst->offset*8+8,dst->reg);
		}
	}
}

void add_expr(char* src, char *dst) {
    printf("\taddq %%%s, %%%s\n", src, dst);
}

void add_expr_var(tree_t *src, tree_t *dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
   			printf("\taddq %%%s, %%%s\n",src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\taddq %%%s, %d(%%%s)\n",src->reg, dst->offset*8+8,dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\taddq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tpush %rax\n");
			printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
			printf("\taddq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);
			printf("\tpop %rax\n");
		}	
	}
}

void mult_term(long value, char *dst) {
    if(value != 1) {
    	printf("\timulq $%li, %%%s\n", value, dst);
    }
}

void mult_term_var(long value, tree_t *dst) {
    if(value != 1) {
    	if(dst->is_var == 0) {
    		printf("\timulq $%li, %%%s\n",value,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\timulq $%li, %d(%%%s)\n",value, dst->offset*8+8,dst->reg);
		}
    }
}

void mult_expr(char* src, char *dst) {
    printf("\timulq %%%s, %%%s\n", src, dst);
}

void mult_expr_var(tree_t *src, tree_t *dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
   			printf("\timulq %%%s, %%%s\n",src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\timulq %%%s, %d(%%%s)\n",src->reg, dst->offset*8+8,dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\timulq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tpush %rax\n");
			printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
			printf("\timulq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tpop %rax\n");
		}	
	}
}

void and_term(long value, char *dst) {
    if (value != 9223372036854775807) {
    	printf("\tandq $%li, %%%s\n", value, dst);
    }
}

void and_term_var(long value, tree_t *dst) {
	if (value != 9223372036854775807) {
    	if(dst->is_var == 0) {
    		printf("\tandq $%li, %%%s\n",value,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tandq $%li, %d(%%%s)\n",value, dst->offset*8+8,dst->reg);
		}
	}
}

void and_expr(char* src, char *dst) {
    printf("\tandq %%%s, %%%s\n", src, dst); 
}

void and_expr_var(tree_t *src, tree_t *dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
   			printf("\tandq %%%s, %%%s\n",src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tandq %%%s, %d(%%%s)\n",src->reg, dst->offset*8+8,dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\tandq %d(%%%s), %%%s\n",src->offset*8+8,src->reg,dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tpush %rax\n");
			printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
			printf("\tandq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tpop %rax\n");
		}	
	}
}

void not_expr(char* src) {
    printf("\tnotq %%%s\n", src);
}

void not_expr_var(tree_t* src, tree_t* dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
   			printf("\tmovq %%%s, %%%s\n", src->reg, dst->reg);
    		printf("\tnotq %%%s\n", dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tmovq %%%s, %d(%%%s)\n", src->reg, dst->offset*8+8, dst->reg);
    		printf("\tnotq %d(%%%s)\n", dst->offset*8+8, dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\tmovq %%%s, %%%s\n", src->reg, dst->reg);
    		printf("\tnotq %%%s\n", dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tpush %rax\n");
			printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
			printf("\tmovq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tnotq %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tpop %rax\n");
		}	
	}
}


void not_term(long value, char* dst) {
    printf("\tmovq $%li, %%%s\n", value, dst);
    printf("\tnotq %%%s\n", dst);
}

void not_term_var(long value, tree_t* dst) {
	if(dst->is_var == 0) {
    	printf("\tmovq $%li, %%%s\n", value, dst->reg);
    	printf("\tnotq %%%s\n", dst->reg);
	} else if(dst->is_var == 1) {
    	printf("\tmovq $%li, %d(%%%s)\n", value, dst->offset, dst->reg);
    	printf("\tnotq %d(%%%s)\n", dst->offset, dst->reg);
	}
}

void neg_expr(char* src) {
    printf("\tnegq %%%s\n", src);
}

void neg_expr_var(tree_t* src, tree_t* dst) {
	if(src->is_var == 0) {
		if(dst->is_var == 0) {
   			printf("\tmovq %%%s, %%%s\n", src->reg, dst->reg);
    		printf("\tnegq %%%s\n", dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tmovq %%%s, %d(%%%s)\n", src->reg, dst->offset*8+8, dst->reg);
    		printf("\tnegq %d(%%%s)\n", dst->offset*8+8, dst->reg);
		}	
	} else if (src -> is_var == 1) {
		if(dst->is_var == 0) {
   			printf("\tmovq %%%s, %%%s\n", src->reg, dst->reg);
    		printf("\tnegq %%%s\n", dst->reg);
		} else if(dst->is_var == 1) {
			printf("\tpush %rax\n");
			printf("\tmovq %d(%%%s), %rax\n",src->offset*8+8,src->reg);
			printf("\tmovq %rax, %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tnegq %d(%%%s)\n",dst->offset*8+8,dst->reg);	
			printf("\tpop %rax\n");
		}	
	}
}
void pointer_expr(char* src) {
    printf("\tmovq (%%%s), %%%s\n", src, src);
}

void pointer_term(long value, char*dst) {
    printf("\tmovq ($%li), %%%s\n", value, dst);
}

void notequal_expr(char *fst, char *snd, char *dst){
	printf("\tcmpq %%%s, %%%s\n", fst, snd);
	printf("\tsetne %%%s\n",get_byte_register(dst));
	printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst), get_32_register(dst));
	printf("\tnegq %%%s\n",dst);
}

void notequal_expr_var(tree_t *fst, tree_t *snd, tree_t *dst){
	
	if(fst->is_var == 1) {
		if(snd->is_var == 1) {
			if(dst->is_var == 1) {
				printf("\tpush %%rax\n");
				printf("\tmovq %d(%%%s), %%rax\n", fst->offset*8+8, fst->reg);
				printf("\tcmpq %%rax, %d(%%%s)\n", snd->offset*8+8, snd->reg);
				printf("\tsetne %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%rax, %d(%%%s)\n", dst->offset*8+8, dst->reg);
				printf("\tpop %%rax\n");
			} else if (dst->is_var == 0) {
				printf("\tpush %%rax\n");
				printf("\tmovq %d(%%%s), %%rax\n", fst->offset*8+8, fst->reg);
				printf("\tcmpq %%rax, %d(%%%s)\n", snd->offset*8+8, snd->reg);
				printf("\tpop %%rax\n");
				printf("\tsetne %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		} else if (snd->is_var == 0) {
			if(dst->is_var == 1) {
				printf("\tcmpq %d(%%%s), %%%s\n", fst->offset*8+8, fst->reg, snd->reg);
				printf("\tpush %rax\n");
				printf("\tsetne %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %d(%%%s), %%%s\n", fst->offset*8+8, fst->reg, snd->reg);
				printf("\tsetne %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		}
	} else if (fst->is_var == 0) {
		if(snd->is_var == 1) {
			if(dst->is_var == 1) {
				printf("\tcmpq %%%s, %d(%%%s)\n", fst->reg, snd->offset*8+8, snd->reg);
				printf("\tpush %%rax\n");
				printf("\tsetne %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %%rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %%%s, %d(%%%s)\n", fst->reg, snd->offset*8+8, snd->reg);
				printf("\tsetne %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		} else if (snd->is_var == 0) {
			if(dst->is_var == 1) {
				printf("\tcmpq %%%s, %%%s\n", fst->reg, snd->reg);
				printf("\tpush %rax\n");
				printf("\tsetne %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %%%s, %%%s\n", fst->reg, snd->reg);
				printf("\tsetne %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		}
	}
}

void notequal_term(long fst, char *snd, char *dst){
	printf("\tcmpq $%li, %%%s\n", fst, snd);
	printf("\tsetne %%%s\n",get_byte_register(dst));
	printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst), get_32_register(dst));
	printf("\tnegq %%%s\n",dst);
}

void notequal_term_var(long fst, tree_t *snd, tree_t *dst){

	if(snd->is_var == 1) {
		if(dst->is_var == 1) {
			printf("\tcmpq $%d, %d(%%%s)\n", fst, snd->offset*8+8, snd->reg);
			printf("\tpush %%rax\n");
			printf("\tsetne %%%s\n",get_byte_register("rax"));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
			printf("\tnegq %%%s\n","rax");
			printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
			printf("\tpop %%rax\n");
		} else if (dst->is_var == 0) {
			printf("\tcmpq $%d, %d(%%%s)\n", fst, snd->offset*8+8, snd->reg);
			printf("\tsetne %%%s\n",get_byte_register(dst->reg));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
			printf("\tnegq %%%s\n",dst->reg);
		}
	} else if (snd->is_var == 0) {
		if(dst->is_var == 1) {
			printf("\tcmpq $%d, %%%s\n", fst, snd->reg);
			printf("\tpush %rax\n");
			printf("\tsetne %%%s\n",get_byte_register("rax"));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
			printf("\tnegq %%%s\n","rax");
			printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
			printf("\tpop %rax\n");
		} else if (dst->is_var == 0) {
			printf("\tcmpq $%d, %%%s\n", fst, snd->reg);
			printf("\tsetne %%%s\n",get_byte_register(dst->reg));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
			printf("\tnegq %%%s\n",dst->reg);
		}
	}
}

void sm_eq_expr(char *fst, char *snd, char *dst){
	printf("\tcmpq %%%s, %%%s\n", fst, snd);
	printf("\tsetle %%%s\n",get_byte_register(dst));
	printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst), get_32_register(dst));
	printf("\tnegq %%%s\n",dst);
}

void sm_eq_expr_var(tree_t *fst, tree_t *snd, tree_t *dst){
	/*printf("\tcmpq %%%s, %%%s\n", fst, snd);
	printf("\tsetle %%%s\n",get_byte_register(dst));
	printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst), get_32_register(dst));
	printf("\tnegq %%%s\n",dst);*/

	if(fst->is_var == 1) {
		if(snd->is_var == 1) {
			if(dst->is_var == 1) {
				printf("\tpush %%rax\n");
				printf("\tmovq %d(%%%s), %%rax\n", fst->offset*8+8, fst->reg);
				printf("\tcmpq %%rax, %d(%%%s)\n", snd->offset*8+8, snd->reg);
				printf("\tsetle %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%rax, %d(%%%s)\n", dst->offset*8+8, dst->reg);
				printf("\tpop %%rax\n");
			} else if (dst->is_var == 0) {
				printf("\tpush %%rax\n");
				printf("\tmovq %d(%%%s), %%rax\n", fst->offset*8+8, fst->reg);
				printf("\tcmpq %%rax, %d(%%%s)\n", snd->offset*8+8, snd->reg);
				printf("\tpop %%rax\n");
				printf("\tsetle %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		} else if (snd->is_var == 0) {
			if(dst->is_var == 1) {
				printf("\tcmpq %d(%%%s), %%%s\n", fst->offset*8+8, fst->reg, snd->reg);
				printf("\tpush %rax\n");
				printf("\tsetle %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %d(%%%s), %%%s\n", fst->offset*8+8, fst->reg, snd->reg);
				printf("\tsetle %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		}
	} else if (fst->is_var == 0) {
		if(snd->is_var == 1) {
			if(dst->is_var == 1) {
				printf("\tcmpq %%%s, %d(%%%s)\n", fst->reg, snd->offset*8+8, snd->reg);
				printf("\tpush %%rax\n");
				printf("\tsetle %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %%rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %%%s, %d(%%%s) \n", fst->reg, snd->offset*8+8, snd->reg);
				printf("\tsetle %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		} else if (snd->is_var == 0) {
			if(dst->is_var == 1) {
				printf("\tcmpq %%%s, %%%s\n",fst->reg, snd->reg);
				printf("\tpush %rax\n");
				printf("\tsetle %%%s\n",get_byte_register("rax"));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
				printf("\tnegq %%%s\n","rax");
				printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
				printf("\tpop %rax\n");
			} else if (dst->is_var == 0) {
				printf("\tcmpq %%%s, %%%s\n", fst->reg, snd->reg);
				printf("\tsetle %%%s\n",get_byte_register(dst->reg));
				printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
				printf("\tnegq %%%s\n",dst->reg);
			}
		}
	}
}

void sm_eq_term(long fst, char *snd, char *dst){
	printf("\tcmpq $%li, %%%s\n", fst, snd);
	printf("\tsetle %%%s\n",get_byte_register(dst));
	printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst), get_32_register(dst));
	printf("\tnegq %%%s\n",dst);
}

void sm_eq_term_var(long fst, tree_t *snd, tree_t *dst){
	fst = fst - 1;
	if(snd->is_var == 1) {
		if(dst->is_var == 1) {
			printf("\tcmpq $%d, %d(%%%s)\n",fst, snd->offset*8+8, snd->reg);
			printf("\tpush %%rax\n");
			printf("\tsetg %%%s\n",get_byte_register("rax"));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
			printf("\tnegq %%%s\n","rax");
			printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
			printf("\tpop %%rax\n");
		} else if (dst->is_var == 0) {
			printf("\tcmpq $%d, %d(%%%s)\n", fst, snd->offset*8+8, snd->reg);
			printf("\tsetg %%%s\n",get_byte_register(dst->reg));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
			printf("\tnegq %%%s\n",dst->reg);
		}
	} else if (snd->is_var == 0) {
		if(dst->is_var == 1) {
			printf("\tcmpq $%d, %%%s\n",fst,  snd->reg);
			printf("\tpush %rax\n");
			printf("\tsetg %%%s\n",get_byte_register("rax"));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register("rax"), get_32_register("rax"));
			printf("\tnegq %%%s\n","rax");
			printf("\tmovq %%%s, %d(%%%s)\n", "rax", dst->offset*8+8, dst->reg);
			printf("\tpop %rax\n");
		} else if (dst->is_var == 0) {
			printf("\tcmpq $%d, %%%s\n", fst, snd->reg);
			printf("\tsetg %%%s\n",get_byte_register(dst->reg));
			printf("\tmovzbl %%%s, %%%s\n",get_byte_register(dst->reg), get_32_register(dst->reg));
			printf("\tnegq %%%s\n",dst->reg);
		}
	}
}
