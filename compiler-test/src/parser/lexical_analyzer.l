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
\+ 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD;}


\- 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return SUB;}
\* 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return MUL;}
\/ 	{pos_start = pos_end; pos_end += 1; pass_node(yytext); return DIV;}
else {pos_start = pos_end; pos_end += 4; pass_node(yytext);return ELSE;}
if   {pos_start = pos_end; pos_end += 2; pass_node(yytext);return IF;}
int  {pos_start = pos_end; pos_end += 3; pass_node(yytext);return INT;}
return    {pos_start = pos_end; pos_end += 6; pass_node(yytext);return RETURN;}
void {pos_start = pos_end; pos_end += 4; pass_node(yytext);return VOID;}
while {pos_start = pos_end; pos_end += 5; pass_node(yytext);return WHILE;}
float {pos_start = pos_end; pos_end += 5; pass_node(yytext);return FLOAT;}
\r\n|\n|\r   {lines++;pos_start = pos_end;pos_end = 1;}
\<    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return LESS;}
\<\=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return LAE;}
\>    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return GRE;}
\>\=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return GAE;}
\=\=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return EQ;}
\!\=   {pos_start = pos_end; pos_end += 2; pass_node(yytext); return UE;}
\=    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return E;}
\;    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return FH;}
\,    {pos_start = pos_end; pos_end += 1; pass_node(yytext); return DH;}
\(   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ZK;}
\)   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return YK;}
\[   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ZF;}
\]   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return YF;}
\{   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return ZH;}
\}   {pos_start = pos_end; pos_end += 1; pass_node(yytext); return YH;}
[ \t]     {pos_start = pos_end;pos_end +=1;}
[a-zA-Z]+ {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return ID;}
[0-9]+    {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return INTEGER;}
([0-9]+\.)|([0-9]+\.[0-9]+)|(\.[0-9]+)  {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext); return FLOATPOINT;}
\/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+\/ {pos_start = pos_end; pos_end += strlen(yytext); pass_node(yytext);}
. { pos_start = pos_end; pos_end++; return ERROR; }
 /****请在此补全所有flex的模式与动作  end******/
%%
