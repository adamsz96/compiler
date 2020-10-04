%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "codegen.h"
#include "symbol_manager.h"
#include <stdbool.h>

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
		exit(2);
}
 
int yywrap()
{
        return 0;
} 
  
main()
{
        yyparse();
	/*fprintf(stdout, "Successful\n");*/
	return 0;
} 

%}

%token t_end
%token t_return
%token t_if
%token t_then
%token t_else
%token t_loop
%token t_break
%token t_cont
%token t_var
%token t_not
%token t_and
%token t_assign
%token t_sm_eq

%token num
%token id

%start Program

@autoinh ids
@autosyn node
@autosyn var_count
@autoinh offset
@autoinh depth

@attributes { char *name; } id
@attributes { int value; } num
@attributes { int var_count; } Funcdef
@attributes { symbol_manager_t* var_used; } Pars ParsList Lexpr ExprList
@attributes { symbol_manager_t* var_used; tree_t* node; char* reg; int offset; int is_var;} Expr Term PlusList MultList AndList
@attributes { symbol_manager_t* var_used;  int var_count; int offset; int depth; int else_if; } Stats
@attributes { symbol_manager_t* var_used; symbol_manager_t* var_ret; tree_t* node;  int var_count; int offset; int depth;  int else_if; } Stat
@traversal @preorder LRpre
@traversal @postorder RLpost
@traversal @postorder codegen
@traversal @preorder reg

%%

Program:	
		| Program Funcdef ';'
		;

Funcdef:	id '(' Pars ')' Stats t_end
		@{ 
			@i @Stats.var_used@ = @Pars.var_used@;
			@i @Stats.offset@ = 0;
			@i @Stats.depth@ = 0;
			@i @Stats.else_if@ = -1;
			@codegen @revorder(1) function(@id.name@, @Funcdef.var_count@);
		@}
		;


Pars:		@{ 
			@i @Pars.var_used@ = new_symbol_manager(); 
		@}
		| id ParsList
		@{ 
			@i @Pars.var_used@ = join(add_param(new_symbol_manager(), @id.name@), @ParsList.var_used@);
		@}
		;

ParsList:	@{ 
			@i @ParsList.var_used@ = new_symbol_manager(); 
		@}
		| ',' id ParsList
		@{ 
			@i @ParsList.0.var_used@ = join(add_param(new_symbol_manager(), @id.name@),@ParsList.1.var_used@); 
		@}
		;

Stats:		@{
			@i @Stats.0.var_count@ = 0;
		@}
		| Stat ';' Stats
		@{

			@i @Stat.var_used@ = @Stats.0.var_used@;
			@i @Stats.1.var_used@ = @Stat.var_ret@; 
			@i @Stats.0.var_count@ = @Stat.var_count@ + @Stats.1.var_count@;

			@i @Stats.1.offset@ = @Stats.0.offset@ + @Stat.var_count@;
			@i @Stat.offset@ = @Stats.0.offset@;

			@i @Stat.depth@ = @Stats.0.depth@ + 10;
			@i @Stats.1.depth@ = @Stats.0.depth@ + 10;
			@i @Stats.1.else_if@ = @Stats.0.else_if@;
			@i @Stat.else_if@ = @Stats.0.else_if@;

			@codegen else_jump(@Stats.0.else_if@);
			@codegen @revorder(1) burm_label(@Stat.node@); burm_reduce(@Stat.node@, 1);
		@}
		;

Stat:		t_return Expr
		@{
			@i @Expr.var_used@ = @Stat.var_used@;
			@i @Stat.var_ret@ = @Stat.var_used@;
			@i @Expr.reg@ = next_register(NULL);
			@i @Expr.is_var@ = 0;
			@i @Stat.node@ = new_node(OP_RET, @Expr.node@, NULL);
			@i @Stat.var_count@ = 0;
			
		@}
		| t_if Expr t_then Stats t_end
		@{
			@i @Expr.var_used@ = @Stat.var_used@;
 			
			@i @Stat.var_count@ = @Stats.var_count@;

			@i @Stats.var_used@ = copy_manager(@Stat.var_used@);
			@i @Expr.reg@ = next_register(NULL);
			@i @Expr.offset@ = 0;
			@i @Expr.is_var@ = 0;
			@i @Stat.var_ret@ = @Stat.var_used@;
			@i @Stat.node@ = new_control_node(OP_IF, @Expr.node@, NULL, @Stat.depth@);
			@i @Stats.depth@ = @Stat.depth@ + 1;
			@i @Stats.else_if@ = @Stat.else_if@;
			@codegen end_if(@Stat.depth@);
		@}
		| t_if Expr t_then Stats t_else Stats t_end
		@{
			@i @Expr.var_used@ = @Stat.var_used@;
			@i @Expr.reg@ = next_register(NULL);
			
			@i @Expr.offset@ = 0;
			@i @Expr.is_var@ = 0;
			
			@i @Stats.0.var_used@ = copy_manager(@Stat.var_used@);
			@i @Stats.1.var_used@ = copy_manager(@Stat.var_used@);
			
			@i @Stats.0.depth@ = @Stat.depth@ + 1;
			@i @Stats.1.depth@ = @Stat.depth@ + 1;

			@i @Stat.var_ret@ = @Stat.var_used@;
			@i @Stat.node@ = new_control_node(OP_ELSE_IF, @Expr.node@, NULL, @Stat.depth@);

			@i @Stat.var_count@ = @Stats.0.var_count@ + @Stats.1.var_count@;

			@i @Stats.0.else_if@ = @Stat.depth@;
			@i @Stats.1.else_if@ = @Stat.else_if@;

			@codegen end_if(@Stat.depth@);
		@}
		| id ':' t_loop Stats t_end
		@{
			@i @Stats.var_used@ = add_lab_with_copy(@Stat.var_used@, @id.name@, @Stat.depth@); 

			@i @Stats.depth@ = @Stat.depth@ + 1;

			@i @Stat.var_ret@ = @Stat.var_used@; 
			@i @Stat.node@ = new_control_node(OP_LOOP, NULL, NULL, @Stat.depth@);

			@i @Stat.var_count@ = @Stats.var_count@;

			@i @Stats.else_if@ = @Stat.else_if@;

			@codegen @revorder(1) start_loop(@Stat.depth@);
			@codegen end_loop(@Stat.depth@);
		@}
		| t_break id
		@{
			@LRpre check_usage_validity(@Stat.var_used@, @id.name@, true);
			@i @Stat.var_ret@ = @Stat.var_used@; 
			@i @Stat.node@ = new_control_node(OP_BREAK, NULL, NULL, @Stat.depth@);

			@i @Stat.var_count@ = 0;

			@codegen break_loop(get_depth(@Stat.var_used@, @id.name@));
		@}
		| t_cont id
		@{
			@LRpre check_usage_validity(@Stat.var_used@, @id.name@, true);
			@i @Stat.var_ret@ = @Stat.var_used@; 	
			@i @Stat.node@ = new_control_node(OP_CONT, NULL, NULL, @Stat.depth@);

			@i @Stat.var_count@ = 0;

			@codegen cont_loop(get_depth(@Stat.var_used@, @id.name@));
		@}
		| t_var id t_assign Expr
		@{
			@i @Expr.var_used@ = @Stat.var_used@;
			@i @Stat.var_ret@ = add_var(@Stat.var_used@, @id.name@, @Stat.offset@);
			@i @Expr.reg@ = "rsp";
			@i @Expr.is_var@ = 1;
			@i @Expr.offset@ = @Stat.offset@;
			@i @Stat.node@ = new_node(OP_ASS, new_var_leaf(@id.name@, @Stat.offset@),@Expr.node@);
			@i @Stat.var_count@ = 1;
		@}
		| id t_assign Expr
		@{
			@LRpre check_usage_validity(@Stat.var_used@, @id.name@, false);

			@i @Expr.var_used@ = @Stat.var_used@;
			@i @Stat.var_ret@ = @Stat.var_used@;
			@i @Expr.reg@ = next_register(NULL);
			@i @Expr.is_var@ = 0;
			@i @Expr.offset@ = 0;

			@i @Stat.node@ = new_node(OP_ASS, new_var_leaf_r(@id.name@,get_reg(@Stat.var_used@, @id.name@), get_offset(@Stat.var_used@, @id.name@), get_is_var(@Stat.var_used@, @id.name@)),@Expr.node@);

			@i @Stat.var_count@ = 0;
		@}
		/*| Expr
		@{
			@i @Expr.var_used@ = @Stat.var_used@;
			@i @Stat.var_ret@ = @Stat.var_used@; 
			@i @Stat.node@ = NULL;
		@}*/
		;

/*Lexpr:	id
		@{
			
		@}
		| '*' Term
		@{
			@i @Term.var_used@ = @Lexpr.var_used@;
		@}
		;*/

Expr:		Term
		@{
			@i @Term.var_used@ = @Expr.var_used@;
			@i @Term.reg@ = @Expr.reg@;
			@i @Term.is_var@ = @Expr.is_var@;
			@i @Term.offset@ = @Expr.offset@;
			@i @Expr.node@ = @Term.node@;
			
		@}
		| t_not Term
		@{
			@i @Term.var_used@ = @Expr.var_used@;
			@i @Term.reg@ = @Expr.reg@;
			@i @Term.is_var@ = @Expr.is_var@;
			@i @Term.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node_r(OP_NOT, @Term.node@, NULL,@Expr.reg@, @Expr.offset@, @Expr.is_var@);
		@}
		| '-' Term
		@{
			@i @Term.var_used@ = @Expr.var_used@;
			@i @Term.reg@ = @Expr.reg@;
			@i @Term.is_var@ = @Expr.is_var@;
			@i @Term.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node_r(OP_SUB, @Term.node@, NULL,@Expr.reg@, @Expr.offset@, @Expr.is_var@);
		@}
		| '*' Term
		@{
			@i @Term.var_used@ = @Expr.var_used@;
			@i @Term.reg@ = @Expr.reg@;
			@i @Term.is_var@ = @Expr.is_var@;
			@i @Term.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node(OP_POINTER, @Term.node@, NULL);
		@}
		| Term '+' Term PlusList
		@{
			@i @Term.0.var_used@ = @Expr.var_used@;
			@i @Term.1.var_used@ = @Expr.var_used@;
			@i @PlusList.var_used@ = @Expr.var_used@;
			@i @Term.0.reg@ = @Expr.reg@;
			@i @Term.1.reg@ = @Expr.reg@;
			@i @PlusList.reg@ = @Expr.reg@;
			@i @Term.0.is_var@ = @Expr.is_var@;
			@i @Term.1.is_var@ = @Expr.is_var@;
			@i @PlusList.is_var@ = @Expr.is_var@;
			@i @Term.0.offset@ = @Expr.offset@;
			@i @Term.1.offset@ = @Expr.offset@;
			@i @PlusList.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node(OP_ADD, new_node(OP_ADD, @PlusList.node@,@Term.1.node@),@Term.0.node@);

		@}
		| Term '*' Term MultList
		@{
			@i @Term.0.var_used@ = @Expr.var_used@;
			@i @Term.1.var_used@ = @Expr.var_used@;
			@i @MultList.var_used@ = @Expr.var_used@;
			@i @Term.0.reg@ = @Expr.reg@;
			@i @Term.1.reg@ = @Expr.reg@;
			@i @MultList.reg@ = @Expr.reg@;
			@i @Term.0.is_var@ = @Expr.is_var@;
			@i @Term.1.is_var@ = @Expr.is_var@;
			@i @MultList.is_var@ = @Expr.is_var@;
			@i @Term.0.offset@ = @Expr.offset@;
			@i @Term.1.offset@ = @Expr.offset@;
			@i @MultList.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node(OP_MULT,new_node(OP_MULT,@MultList.node@,@Term.1.node@),@Term.0.node@);
		@}
		| Term t_and Term AndList
		@{
			@i @Term.0.var_used@ = @Expr.var_used@;
			@i @Term.1.var_used@ = @Expr.var_used@;
			@i @AndList.var_used@ = @Expr.var_used@;
			@i @Term.0.reg@ = @Expr.reg@;
			@i @Term.1.reg@ = @Expr.reg@;
			@i @AndList.reg@ = @Expr.reg@;
			@i @Term.0.is_var@ = @Expr.is_var@;
			@i @Term.1.is_var@ = @Expr.is_var@;
			@i @AndList.is_var@ = @Expr.is_var@;
			@i @Term.0.offset@ = @Expr.offset@;
			@i @Term.1.offset@ = @Expr.offset@;
			@i @AndList.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node(OP_AND,new_node(OP_AND,@AndList.node@,@Term.1.node@),@Term.0.node@);
		@}
		| Term t_sm_eq Term
		@{
			@i @Term.0.var_used@ = @Expr.var_used@;
			@i @Term.1.var_used@ = @Expr.var_used@;
			@i @Term.0.reg@ = @Expr.reg@;
			@i @Term.1.reg@ = @Expr.reg@;
			@i @Term.0.is_var@ = @Expr.is_var@;
			@i @Term.1.is_var@ = @Expr.is_var@;
			@i @Term.0.offset@ = @Expr.offset@;
			@i @Term.1.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node_r(OP_SM_EQ, new_node_r(OP_EMPTY,@Term.1.node@,NULL,@Expr.reg@, @Expr.offset@, @Expr.is_var@), @Term.0.node@, @Expr.reg@, @Expr.offset@, @Expr.is_var@);
		@}
		| Term '#' Term
		@{
			@i @Term.0.var_used@ = @Expr.var_used@;
			@i @Term.1.var_used@ = @Expr.var_used@;
			@i @Term.0.reg@ = @Expr.reg@;
			@i @Term.1.reg@ = @Expr.reg@;
			@i @Term.0.is_var@ = @Expr.is_var@;
			@i @Term.1.is_var@ = @Expr.is_var@;
			@i @Term.0.offset@ = @Expr.offset@;
			@i @Term.1.offset@ = @Expr.offset@;
			@i @Expr.node@ = new_node_r(OP_NEQ, new_node_r(OP_EMPTY,@Term.1.node@,NULL,@Expr.reg@, @Expr.offset@, @Expr.is_var@), @Term.0.node@, @Expr.reg@, @Expr.offset@, @Expr.is_var@);
		@}
		;

PlusList:	@{ @i @PlusList.0.node@ = new_num_leaf(0, @PlusList.reg@, @PlusList.offset@, @PlusList.is_var@); @}
		| '+' Term PlusList
		@{
			@i @Term.var_used@ = @PlusList.0.var_used@;
			@i @PlusList.1.var_used@ = @PlusList.0.var_used@;
			@i @Term.reg@ = @PlusList.0.reg@;
			@i @PlusList.1.reg@ = @PlusList.0.reg@;
			@i @Term.is_var@ = @PlusList.0.is_var@;
			@i @PlusList.1.is_var@ = @PlusList.0.is_var@;
			@i @Term.offset@ = @PlusList.0.offset@;
			@i @PlusList.1.offset@ = @PlusList.0.offset@;
			@i @PlusList.0.node@ = new_node(OP_ADD, @PlusList.1.node@,@Term.0.node@);
		@}
		;

MultList:	@{ @i @MultList.0.node@ = new_num_leaf(1, @MultList.reg@, @MultList.offset@, @MultList.is_var@); @}
		| '*' Term MultList
		@{
			@i @Term.var_used@ = @MultList.0.var_used@;
			@i @MultList.1.var_used@ = @MultList.0.var_used@;
			@i @Term.reg@ = @MultList.0.reg@;
			@i @MultList.1.reg@ = @MultList.0.reg@;
			@i @Term.is_var@ = @MultList.0.is_var@;
			@i @MultList.1.is_var@ = @MultList.0.is_var@;
			@i @Term.offset@ = @MultList.0.offset@;
			@i @MultList.1.offset@ = @MultList.0.offset@;
			@i @MultList.0.node@ = new_node(OP_MULT, @MultList.1.node@, @Term.0.node@);
		@}
		;

AndList:	@{ @i @AndList.0.node@ = new_num_leaf(9223372036854775807, @AndList.reg@, @AndList.offset@, @AndList.is_var@); @}
		| AndList t_and Term
		@{
			@i @Term.var_used@ = @AndList.0.var_used@;
			@i @AndList.1.var_used@ = @AndList.0.var_used@;
			@i @Term.reg@ = @AndList.0.reg@;
			@i @AndList.1.reg@ = @AndList.0.reg@;
			@i @Term.is_var@ = @AndList.0.is_var@;
			@i @AndList.1.is_var@ = @AndList.0.is_var@;
			@i @Term.offset@ = @AndList.0.offset@;
			@i @AndList.1.offset@ = @AndList.0.offset@;
			@i @AndList.0.node@ = new_node(OP_AND, @AndList.1.node@, @Term.0.node@);
		@}
		;

Term:		'(' Expr ')'
		@{
			@i @Expr.var_used@ = @Term.var_used@;
			@i @Expr.reg@ = next_register(@Term.reg@);
			@i @Expr.offset@ = 0;
			@i @Expr.is_var@ = 0;
		@}
		| num
		@{
			@i @Term.node@ = new_num_leaf(@num.value@, @Term.reg@, @Term.offset@, @Term.is_var@);
		@}
		| id
		@{
			@i @Term.node@ = new_id_leaf(@id.name@, get_reg(@Term.var_used@, @id.name@), get_offset(@Term.var_used@, @id.name@), get_is_var(@Term.var_used@, @id.name@));

		@}
		/*| id '(' ExprList ')'
		@{
			@i @ExprList.var_used@ = @Term.var_used@;
			@i @Term.node@ = NULL;
		@}*/
		;

/*ExprList:	
		| Expr
		@{
			@i @Expr.var_used@ = @ExprList.var_used@;
		@}
		| Expr ',' ExprList
		@{
			@i @Expr.var_used@ = @ExprList.0.var_used@;
			@i @ExprList.1.var_used@ = @ExprList.0.var_used@;
		@}*/
