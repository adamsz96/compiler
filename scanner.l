%{
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symbol_manager.h"
#include "tree.h"
%}

%option noyywrap
DIGIT    [0-9]
ID       [a-zA-Z_][a-zA-Z0-9_]*
HEX	 [0-9a-fA-F]
KEYWORD  (end|return|if|then|else|loop|break|cont|var|not|and)
LEX	 ("("|")"|","|";"|":"|":="|"*"|"-"|"+"|"<="|"#")
COMMENT  "//"[^"\n"]*

%%
end		return t_end;
return		return t_return;
if		return t_if;
then		return t_then;
else		return t_else;
loop		return t_loop;
break		return t_break;
cont		return t_cont;
var		return t_var;
not		return t_not;
and		return t_and;

"("		return '(';
")"		return ')';
","		return ',';
";"		return ';';
":"		return ':';
"*"		return '*';
"-"		return '-';
"+"		return '+';
"#"		return '#';

"<="		return t_sm_eq;
":="		return t_assign;

{DIGIT}+    	{
            		return num;@{ @num.value@=atoi( yytext ); @}
            	}

"0x"{HEX}+	{
			return num;@{ @num.value@=strtol(yytext + 2,NULL,16); @}
		}

{ID}        	{
			return id;@{ @id.name@=strdup(yytext); @}
	    	}

{COMMENT}  	

[ \t\n]+        

.           	{ 
			printf (" Scanner error: unexpected character\n");
			exit(1);
		}
%%
