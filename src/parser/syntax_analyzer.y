%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "syntax_tree.h"

// external functions from lex
extern int yylex();
extern int yyparse();
extern int yyrestart();
extern FILE * yyin;

// external variables from lexical_analyzer module
extern int lines;
extern char * yytext;
extern int pos_end;
extern int pos_start;

// Global syntax tree
syntax_tree *gt;

// Error reporting
void yyerror(const char *s);

// Helper functions written for you with love
syntax_tree_node *node(const char *node_name, int children_num, ...);
%}

/* TODO: Complete this definition.
   Hint: See pass_node(), node(), and syntax_tree.h.
         Use forward declaring. */
%union {
    struct _syntax_tree_node* node;
}

/* TODO: Your tokens here. */
%token <node> AND
%token <node> OR
%token <node> BFH
%token <node> NOT
%token <node> ADD
%token <node> SUB
%token <node> MUL
%token <node> DIV
%token <node> ELSE
%token <node> IF
%token <node> INT
%token <node> RETURN
%token <node> VOID
%token <node> WHILE
%token <node> FLOAT

%token <node> CONTINUE
%token <node> BREAK
%token <node> CONST

%token <node> LESS
%token <node> LAE
%token <node> GRE
%token <node> GAE
%token <node> EQ
%token <node> UE
%token <node> E
%token <node> FH
%token <node> DH
%token <node> ZK
%token <node> YK
%token <node> ZF
%token <node> YF
%token <node> ZH
%token <node> YH


%token <node> E
%token <node> P
%token <node> X
%token <node> A_FNE
%token <node> G_ZNPX
%token <node> ZERO
%token <node> ONE_SEVEN
%token <node> EIGHT_NINE
%token <node> ERROR
%token <node> DOT


%type <node> CompUnit Decl ConstDecl BType ConstDef ConstInitVal VarDecl
%type <node> VarDef InitVal FuncDef FuncType FuncFParams
%type <node> FuncFParam Block BlockItem Stmt
%type <node> Exp Cond LVal PrimaryExp Number
%type <node> UnaryExp UnaryOp FuncRParams MulExp AddExp RelExp
%type <node> EqExp LAndExp LOrExp ConstExp
%type <node> Ident IntConst floatConst
%type <node> SCompUnit
%type <node> CDef CCD CCI CVD CIV CFFP CCFFP CB
%type <node> ident_nondigit ident_digit
%type <node> decimal_const octal_const hexadecimal_const
%type <node> nonzero_digit digit octal_digit
%type <node> hexadecimal_prefix hexadecimal_digit
%type <node> dec_flo_cons hex_flo_cons fra_cons exp_part
%type <node> dig_seq hex_fra_cons bin_exp_part hex_dig_seq

%start SCompUnit

%%
SCompUnit
:CompUnit{$$ = node( "SCompUnit", 1, $1); gt->root = $$;}


CompUnit
:Decl{$$ = node( "CompUnit", 1, $1);}
|FuncDef{$$ = node( "CompUnit", 1, $1);}
|CompUnit Decl{$$ = node( "CompUnit", 2, $1, $2);}
|CompUnit FuncDef{$$ = node( "CompUnit", 2, $1, $2);}

Decl
:ConstDecl{$$ = node( "Decl", 1, $1);}
|VarDecl{$$ = node( "Decl", 1, $1);}

ConstDecl
:CONST BType ConstDef FH{$$ = node( "ConstDecl", 4, $1,$2,$3,$4);}
|CONST BType ConstDef CDef FH{$$ = node( "ConstDecl", 5, $1,$2,$3,$4,$5);}

CDef
:DH ConstDef{$$ = node( "CDef", 2, $1,$2);}
|DH ConstDef CDef{$$ = node( "CDef", 3, $1,$2,$3);}

BType
:INT{$$ = node( "BType", 1, $1);}
|FLOAT{$$ = node( "BType", 1, $1);}

ConstDef
:Ident E ConstInitVal{$$ = node( "ConstDef", 3, $1,$2,$3);}
|Ident CCD E ConstInitVal{$$ = node( "ConstDef", 4, $1,$2,$3,$4);}

CCD
:ZF ConstExp YF{$$ = node( "CCD", 3, $1,$2,$3);}
|ZF ConstExp YF CCD{$$ = node( "CCD", 4, $1,$2,$3,$4);}

ConstInitVal
:ConstExp{$$ = node( "ConstInitVal", 1, $1);}
|ZH YH{$$ = node( "ConstInitVal", 2, $1, $2);} 
|ZH CCI YH{$$ = node( "ConstInitVal", 3, $1, $2, $3);}

CCI
:ConstInitVal{$$ = node( "CCI", 1, $1);}
|ConstInitVal DH CCI{$$ = node( "CCI", 3, $1, $2, $3);}

VarDecl
:BType VarDef FH{$$ = node( "VarDecl", 3, $1,$2,$3);}
|BType VarDef CVD FH{$$ = node( "VarDecl", 4, $1,$2,$3,$4);}

CVD
:DH Vardef{$$ = node( "CVD", 2, $1,$2);}
|DH Vardef CVD{$$ = node( "CVD", 3, $1,$2,$3);}

VarDef
:Ident{$$ = node( "VarDef", 1, $1);}
|Ident CVDef{$$ = node( "VarDef", 2, $1,$2);}
|Ident E InitVal{$$ = node( "VarDef", 3, $1,$2,$3);}
|Ident CVDef E InitVal{$$ = node( "VarDef", 4, $1,$2,$3,$4);}

CVDef
:ZF ConstExp YF{$$ = node( "CVDef", 3, $1,$2,$3);}

InitVal
:Exp{$$ = node( "InitVal", 1, $1);}
|ZH YH{$$ = node( "InitVal", 2, $1,$2);}
|ZH CIV YH{$$ = node( "InitVal", 3, $1,$2,$3);}

CIV
:InitVal{$$ = node( "CIV", 1, $1);}
|InitVal DH CIV{$$ = node( "CIV", 3, $1,$2,$3);}

FuncDef
:FuncType Ident ZK YK Block{$$ = node( "FuncDef", 5, $1,$2,$3,$4,$5);}
|FuncType Ident ZK FuncFParams YK Block{$$ = node( "FuncDef", 6, $1,$2,$3,$4,$5,$6);}

FuncType
:VOID{$$ = node( "FuncType", 1, $1);}
|INT{$$ = node( "FuncType", 1, $1);}
|FLOAT{$$ = node( "FuncType", 1, $1);}

FuncFParams
:FuncFParam{$$ = node( "FuncFParams", 1, $1);}
|FuncFParam DH FuncFParams{$$ = node( "FuncFParams", 2, $1,$2);}

FuncFParam
:BType Ident{$$ = node( "FuncFParam", 2, $1,$2);}
|BType Ident CFFP{$$ = node( "FuncFParam", 3, $1,$2,$3);}

CFFP
:ZF YF{$$ = node( "CFFP", 2, $1,$2);}
|ZF YF CCFFP{$$ = node( "CFFP", 3, $1,$2,$3);}

CCFFP
:ZF Exp YF{$$ = node( "CCFFP", 3, $1,$2,$3);}
|ZF Exp YF CCFFP{$$ = node( "CCFFP", 4, $1,$2,$3,$4);}

Block
:ZH YH{$$ = node( "Block", 2, $1,$2);}
|ZH CB YH{$$ = node( "Block", 3, $1,$2,$3);}

CB
:BlockItem{$$ = node( "CB", 1, $1);}
|BlockItem CB{$$ = node( "CB", 2, $1,$2);}

BlockItem
:Decl{$$ = node( "BlockItem", 1, $1);}
|Stmt{$$ = node( "BlockItem", 1, $1);}

Stmt
:LVal E Exp FH{$$ = node( "Stmt", 4, $1,$2,$3,$4);}
|FH{$$ = node( "Stmt", 1, $1);}
|Exp FH{$$ = node( "Stmt", 2, $1,$2);}
|Block{$$ = node( "Stmt", 1, $1);}
|IF ZK Cond YK Stmt{$$ = node( "Stmt", 5, $1,$2,$3,$4,$5);}
|IF ZK Cond YK Stmt ELSE Stmt{$$ = node( "Stmt", 7, $1,$2,$3,$4,$5,$6,$7);}
|WHILE ZK Cond YK Stmt{$$ = node( "Stmt", 5, $1,$2,$3,$4,$5);}
|BREAK FH{$$ = node( "Stmt", 2, $1,$2);}
|CONTINUE FH{$$ = node( "Stmt", 2, $1,$2);}
|RETURN FH{$$ = node( "Stmt", 2, $1,$2);}
|RETURN Exp FH{$$ = node( "Stmt", 3, $1,$2,$3);}

Exp
:AddExp{$$ = node( "Exp", 1, $1);}

Cond
:LOrExp{$$ = node( "Cond", 1, $1);}

LVal
:Ident{$$ = node( "LVal", 1, $1);}
|Ident CLV{$$ = node( "LVal", 2, $1,$2);}

CLV
:ZK Exp YK{$$ = node( "CLV", 3, $1,$2,$3);}
|ZK Exp YK CLV{$$ = node( "CLV", 4, $1,$2,$3,$4);}

PrimaryExp
:ZK Exp YK{$$ = node( "PrimaryExp", 3, $1,$2,$3);}
|LVAl{$$ = node( "PrimaryExp", 1, $1);}
|Number{$$ = node( "PrimaryExp", 1, $1);}

Number
:IntConst{$$ = node( "Number", 1, $1);}
|floatConst{$$ = node( "Number", 1, $1);}

UnaryExp
:PrimaryExp{$$ = node( "UnaryExp", 1, $1);}
|Ident ZK YK{$$ = node( "UnaryExp", 3, $1,$2,$3);}
|Ident ZK FuncRParams YK{$$ = node( "UnaryExp", 4, $1,$2,$3,$4);}
|UnaryOp UnaryExp{$$ = node( "UnaryExp", 2, $1,$2);}

UnaryOp
:ADD{$$ = node( "UnaryOp", 1, $1);}
|SUB{$$ = node( "UnaryOp", 1, $1);}
|NOT{$$ = node( "UnaryOp", 1, $1);}

FuncRParams
:Exp{$$ = node( "FuncRParams", 1, $1);}
|Exp DH FuncRParams{$$ = node( "FuncRParams", 3, $1,$2,$3);}

MulExp
:UnaryExp{$$ = node( "MulExp", 1, $1);}
|MulExp MUL UnaryExp{$$ = node( "MulExp", 3, $1,$2,$3);}
|MulExp DIV UnaryExp{$$ = node( "MulExp", 3, $1,$2,$3);}
|MulExp BFH UnaryExp{$$ = node( "MulExp", 3, $1,$2,$3);}

AddExp
:MulExp{$$ = node( "AddExp", 1, $1);}
|AddExp ADD MulExp{$$ = node( "AddExp", 3, $1,$2,$3);}
|AddExp SUB MulExp{$$ = node( "AddExp", 3, $1,$2,$3);}

RelExp
:AddExp{$$ = node( "RelExp", 1, $1);}
|RelExp LESS AddExp{$$ = node( "RelExp", 3, $1,$2,$3);}
|RelExp GRE AddExp{$$ = node( "RelExp", 3, $1,$2,$3);}
|RelExp LAE AddExp{$$ = node( "RelExp", 3, $1,$2,$3);}
|RelExp GAE AddExp{$$ = node( "RelExp", 3, $1,$2,$3);}

EqExp
:RelExp{$$ = node( "EqExp", 1, $1);}
|EqExp EQ RelExp{$$ = node( "EqExp", 3, $1,$2,$3);}
|EqExp UE RelExp{$$ = node( "EqExp", 3, $1,$2,$3);}

LAndExp
:EqExp{$$ = node( "LAndExp", 1, $1);}
|LAndExp AND EqExp{$$ = node( "LAndExp", 3, $1,$2,$3);}

LOrExp
:LAndExp{$$ = node( "LOrExp", 1, $1);}
|LOrExp OR LAndExp{$$ = node( "LOrExp", 3, $1,$2,$3);}

ConstExp
:AddExp{$$ = node( "ConstExp", 1, $1);}

Ident
:ident_nondigit{$$ = node( "Ident", 1, $1);}
|Ident ident_nondigit{$$ = node( "Ident", 2, $1,$2);}
|Ident ident_digit{$$ = node( "Ident", 2, $1,$2);}

ident_nondigit
:A_FNE{$$ = node( "ident_nondigit", 1, $1);}
|E{$$ = node( "ident_nondigit", 1, $1);}
|G_ZNPX{$$ = node( "ident_nondigit", 1, $1);}
|P{$$ = node( "ident_nondigit", 1, $1);}
|X{$$ = node( "ident_nondigit", 1, $1);}

ident_digit
:ZERO{$$ = node( "ident_digit", 1, $1);}
|ONE_SEVEN{$$ = node( "ident_digit", 1, $1);}
|EIGHT_NINE{$$ = node( "ident_digit", 1, $1);}

IntConst
:decimal_const{$$ = node( "IntConst", 1, $1);}
|octal_const{$$ = node( "IntConst", 1, $1);}
|hexadecimal_const{$$ = node( "IntConst", 1, $1);}

decimal_const
:nonzero_digit{$$ = node( "decimal_const", 1, $1);}
|decimal_const digit{$$ = node( "decimal_const", 2, $1,$2);}

octal_const
:ZERO{$$ = node( "octal_const", 1, $1);}
|octal_const octal_digit{$$ = node( "octal_const", 2, $1,$2);}

hexadecimal_const
:hexadecimal_prefix hexadecimal_digit{$$ = node( "hexadecimal_const", 2, $1,$2);}
|hexadecimal_const hexadecimal_digit{$$ = node( "hexadecimal_const", 2, $1,$2);}

hexadecimal_prefix
:ZERO X{$$ = node( "hexadecimal_prefix", 2, $1,$2);}

digit
:ZERO{$$ = node( "digit", 1, $1);}
|ONE_SEVEN{$$ = node( "digit", 1, $1);}
|EIGHT_NINE{$$ = node( "digit", 1, $1);}

nonzero_digit
:ONE_SEVEN{$$ = node( "nonzero_digit", 1, $1);}
|EIGHT_NINE{$$ = node( "nonzero_digit", 1, $1);}

octal_digit
:ZERO{$$ = node( "octal_digit", 1, $1);}
|ONE_SEVEN{$$ = node( "octal_digit", 1, $1);}

hexadecimal_digit
:ZERO{$$ = node( "hexadecimal_digit", 1, $1);}
|ONE_SEVEN{$$ = node( "hexadecimal_digit", 1, $1);}
|EIGHT_NINE{$$ = node( "hexadecimal_digit", 1, $1);}
|A_FNE{$$ = node( "hexadecimal_digit", 1, $1);}
|E{$$ = node( "hexadecimal_digit", 1, $1);}

floatConst
:dec_flo_cons{$$ = node( "floatConst", 1, $1);}
|hex_flo_cons{$$ = node( "floatConst", 1, $1);}

dec_flo_cons
:fra_cons exp_part{}
|fra_cons{}
|dig_seq exp_part{}

hex_flo_cons
:hexadecimal_prefix hex_fra_cons bin_exp_part{}
|hexadecimal_prefix hex_dig_seq bin_exp_part{}

fra_cons
:dig_seq DOT dig_seq{}
|DOT dig_seq{}
|dig_seq DOT{}

exp_part
:E sign dig_seq{}
|E dig_seq{}

sign
:ADD{}
|SUB{}

dig_seq
:digit{}
|dig_seq digit{}

hex_fra_cons
:hex_dig_seq DOT hex_dig_seq{}
|DOT hex_dig_seq{}
|hex_dig_seq DOT{}

bin_exp_part
:P sign dig_seq{}
|P dig_seq{}

hex_dig_seq
:hexadecimal_digit{}
|hex_dig_seq hexadecimal_digit{}

%%


///args:arg-list{$$ = node("args",1,$1);}|{syntax_tree_node *emp = new_syntax_tree_node("epsilon"); $$ = node( "args", 1, emp);};



/// The error reporting function.
void yyerror(const char * s)
{
    // TO STUDENTS: This is just an example.
    // You can customize it as you like.
    fprintf(stderr, "error at line %d column %d: %s\n", lines, pos_start, s);
}

/// Parse input from file `input_path`, and prints the parsing results
/// to stdout.  If input_path is NULL, read from stdin.
///
/// This function initializes essential states before running yyparse().
syntax_tree *parse(const char *input_path)
{
    if (input_path != NULL) {
        if (!(yyin = fopen(input_path, "r"))) {
            fprintf(stderr, "[ERR] Open input file %s failed.\n", input_path);
            exit(1);
        }
    } else {
        yyin = stdin;
    }

    lines = pos_start = pos_end = 1;
    gt = new_syntax_tree();
    yyrestart(yyin);
    yyparse();
    return gt;
}

/// A helper function to quickly construct a tree node.
///
/// e.g. $$ = node("program", 1, $1);
syntax_tree_node *node(const char *name, int children_num, ...)
{
    syntax_tree_node *p = new_syntax_tree_node(name);
    syntax_tree_node *child;
    if (children_num == 0) {
        child = new_syntax_tree_node("epsilon");
        syntax_tree_add_child(p, child);
    } else {
        va_list ap;
        va_start(ap, children_num);
        for (int i = 0; i < children_num; ++i) {
            child = va_arg(ap, syntax_tree_node *);
            syntax_tree_add_child(p, child);
        }
        va_end(ap);
    }
    return p;
}
