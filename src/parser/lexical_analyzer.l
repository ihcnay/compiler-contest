%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines=1;
int pos_start=1;
int pos_end=1;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}


%%
 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */
\&\&   { pass_node(yytext); return AND;}
\|\|   { pass_node(yytext); return OR;}
\%   { pass_node(yytext); return BFH;}
\!   { pass_node(yytext); return NOT;}
\+ 	{ pass_node(yytext); return ADD;}
\- 	{ pass_node(yytext);return SUB;}
\* 	{ pass_node(yytext);return MUL;}
\/ 	{ pass_node(yytext);return DIV;}
else { pass_node(yytext);return ELSE;}
if   { pass_node(yytext);return IF;}
int  { pass_node(yytext);return INT;}
return    { pass_node(yytext);return RETURN;}
void { pass_node(yytext);return VOID;}
while { pass_node(yytext);return WHILE;}
float { pass_node(yytext);return FLOAT;}
continue { pass_node(yytext);return CONTINUE;}
break { pass_node(yytext);return BREAK;}
const { pass_node(yytext);return CONST;}
\r\n|\n|\r   {}
\<    { pass_node(yytext); return LESS;}
\<\=   { pass_node(yytext); return LAE;}
\>    { pass_node(yytext); return GRE;}
\>\=   { pass_node(yytext); return GAE;}
\=\=   { pass_node(yytext); return EQ;}
\!\=   { pass_node(yytext); return UE;}
\=    { pass_node(yytext); return E_;}
\;    { pass_node(yytext); return FH;}
\,    { pass_node(yytext); return DH;}
\(   { pass_node(yytext); return ZK;}
\)   { pass_node(yytext); return YK;}
\[   { pass_node(yytext); return ZF;}
\]   { pass_node(yytext); return YF;}
\{   { pass_node(yytext); return ZH;}
\}   { pass_node(yytext); return YH;}
[ \t]     {}

[eE] { pass_node(yytext); return E;}
[pP] { pass_node(yytext); return P;}

[a-efA-EF] { pass_node(yytext); return A_FNE;}
[g-oq-wyzG-OQ-WYZ] { pass_node(yytext); return G_ZNPX;}
[xX] { pass_node(yytext); return X;}
0    { pass_node(yytext); return ZERO;}
[1-7]    { pass_node(yytext); return ONE_SEVEN;}
[8-9]    { pass_node(yytext); return EIGHT_NINE;}

\.   { pass_node(yytext); return DOT;}
\/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+\/ { pass_node(yytext);}
\/\/.+[\r\n]? { pass_node(yytext);}
. { return ERROR; }
 /****请在此补全所有flex的模式与动作  end******/
%%
