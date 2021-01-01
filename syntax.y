%{
#include <stdio.h>
#include<unistd.h>
#include "gramtree.h"
#include"variabletable.h"
//#include"yyfx.h"
    int yylex();
    int yyrestart();
    //#include "lex.yy.c"
    int flag=1;
    %}
%union
{
    struct gramtree* newfather;
    double d;
}
%token <newfather> INT FLOAT
%token <newfather> ID STRUCT TYPE RETURN IF ELSE WHILE SPACE SPACEN COMMA SEMI ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR NOT LB RB LP RP LC RC ERROR DOT
%type <newfather> Program ExtDefList ExtDef ExtDecList ExpDef_miss_SEMI Specifier StructSpecifier StructSpecifier_miss_RC OptTag Tag VarDec VarDec_miss_RB FunDec FunDec_miss_RP VarList ParamDec CompSt StmtList Stmt Stmt_miss_SEMI DefList Def Def_miss_SEMI Exp_miss_RB Exp_miss_RP   DecList Dec Exp  Args
%nonassoc LOWER_THAN_Exp_RP LOWER_THAN_Exp_RB
%right ASSIGNOP
%left AND OR
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT

%left LB RB LP RP
%left DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList {$$=gramTree("Program",1,$1);printf("\n");circulate($$,0,flag);Program($$);}
;
ExtDefList : ExtDef ExtDefList{$$=gramTree("ExtDefList",2,$1,$2);}
| {$$=gramTree("ExtDefList",0,-1);}
;
ExtDef : Specifier ExtDecList SEMI{$$=gramTree("ExtDef",3,$1,$2,$3);}
| Specifier SEMI{$$=gramTree("ExtDef",2,$1,$2);}
|Specifier FunDec CompSt {$$=gramTree("ExtDef",3,$1,$2,$3);}
| Specifier FunDec SEMI{$$=gramTree("ExtDef",3,$1,$2,$3);}
|ExpDef_miss_SEMI{$$=gramTree("ExtDef",1,$1);flag=0;}
;

ExpDef_miss_SEMI : Specifier ExtDecList {$$=gramTree("ExpDef_miss_SEMI",2,$1,$2);printf("Error type B at line %d :Missing SEMI \n",$2->lineno);flag=0;}
|Specifier {$$=gramTree("ExpDef_miss_SEMI",1,$1);printf("Error type B at line %d :Missing SEMI \n",$1->lineno);flag=0;}
;
ExtDecList : VarDec{$$=gramTree("ExtDecList",1,$1);}
| VarDec COMMA ExtDecList{$$=gramTree("ExtDecList",3,$1,$2,$3);}
;
Specifier : TYPE {$$=gramTree("Specifier",1,$1);}
| StructSpecifier {$$=gramTree("Specifier",1,$1);}
;
StructSpecifier : STRUCT OptTag LC DefList RC{$$=gramTree("StructSpecifier",5,$1,$2,$3,$4,$5);}
| error RC {$$=gramTree("StructSpecifier",1,$2);flag=0;}
| STRUCT Tag{$$=gramTree("StructSpecifier",2,$1,$2);}
| StructSpecifier_miss_RC {$$=gramTree("StructSpecifier",1,$1);flag=0;}
;
StructSpecifier_miss_RC : STRUCT OptTag LC DefList {$$=gramTree("StructSpecifier_miss_RC",4,$1,$2,$3,$4);printf("Error type B at line %d :Missing RC \n",$4->lineno);flag=0;}
;
OptTag : ID{$$=gramTree("OptTag",1,$1);}
| {$$=gramTree("OptTag",0,-1);}
;
Tag : ID{$$=gramTree("Tag",1,$1);}
;
VarDec : ID{$$=gramTree("VarDec",1,$1);}
| VarDec LB INT RB{$$=gramTree("VarDec",4,$1,$2,$3,$4);}
| error RB{$$=gramTree("VarDec",1,$2);flag=0;}
| VarDec_miss_RB {$$=gramTree("VarDec",1,$1);flag=0;}
;
VarDec_miss_RB : VarDec LB INT {$$=gramTree("VarDec_miss_RB",3,$1,$2,$3);printf("Error type B at line %d :Missing RB \n",$3->lineno);flag=0;}
;
FunDec : ID LP VarList RP{$$=gramTree("FunDec",4,$1,$2,$3,$4);}
| ID LP RP{$$=gramTree("FunDec",3,$1,$2,$3);}
| error RP{$$=gramTree("FunDec",1,$2);flag=0;}
| FunDec_miss_RP {$$=gramTree("FunDec",1,$1);flag=0;}
;
FunDec_miss_RP : ID LP VarList {$$=gramTree("FunDec_miss_RP",3,$1,$2,$3);printf("Error type B at line %d :Missing RP \n",$3->lineno);flag=0;}
| ID LP {$$=gramTree("FunDec_miss_RP",2,$1,$2);printf("Error type B at line %d :Missing RP \n",$2->lineno);flag=0;}
;
VarList : ParamDec COMMA VarList  {$$=gramTree("VarList",3,$1,$2,$3);}
| ParamDec {$$=gramTree("VarList",1,$1);}
;
ParamDec : Specifier VarDec{$$=gramTree("ParamDec",2,$1,$2);}
;
CompSt : LC DefList StmtList RC{$$=gramTree("Compst",4,$1,$2,$3,$4);}
| error RC {$$=gramTree("Compst",1,$2);flag=0;}

;

StmtList : {$$=gramTree("StmtList",0,-1);}
| Stmt StmtList{$$=gramTree("StmtList",2,$1,$2);}
;
Stmt : Exp SEMI {$$=gramTree("Stmt",2,$1,$2);}
| CompSt {$$=gramTree("Stmt",1,$1);}
|RETURN Exp SEMI {$$=gramTree("Stmt",3,$1,$2,$3);}
| error SEMI {$$=gramTree("Stmt",1,$2);flag=0;}
|IF LP Exp RP Stmt {$$=gramTree("Stmt",5,$1,$2,$3,$4,$5);}
|IF LP Exp RP Stmt ELSE Stmt {$$=gramTree("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
|WHILE LP Exp RP Stmt {$$=gramTree("Stmt",5,$1,$2,$3,$4,$5);}
| Stmt_miss_SEMI {$$=gramTree("Stmt",1,$1);flag=0;}
;
Stmt_miss_SEMI : Exp {$$=gramTree("Stmt_miss_SEMI",1,$1);printf("Error type B at line %d :Missing SEMI \n",$1->lineno);flag=0;}//优先级！！！！
|RETURN Exp {$$=gramTree("Stmt_miss_SEMI",2,$1,$2);printf("Error type B at line %d :Missing SEMI \n",$2->lineno);flag=0;}
;
DefList : Def DefList{$$=gramTree("DefList",2,$1,$2);}
| {$$=gramTree("DefList",0,-1);}
;
Def : Specifier DecList SEMI {$$=gramTree("Def",3,$1,$2,$3);}
| error SEMI {$$=gramTree("Def",1,$2);flag=0;}
| Def_miss_SEMI {$$=gramTree("Def",1,$1);flag=0;}
;
Def_miss_SEMI : Specifier DecList {$$=gramTree("Def_miss_SEMI",2,$1,$2);printf("Error type B at line %d :Missing SEMI \n",$2->lineno);flag=0;}
;
DecList : Dec {$$=gramTree("DecList",1,$1);}
|Dec COMMA DecList {$$=gramTree("DecList",3,$1,$2,$3);}
;
Dec : VarDec {$$=gramTree("Dec",1,$1);}
|VarDec ASSIGNOP Exp {$$=gramTree("Dec",3,$1,$2,$3);}
;
Exp : Exp ASSIGNOP Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp AND Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp OR Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp RELOP Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp PLUS Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp MINUS Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp STAR Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp DIV Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|LP Exp RP{$$=gramTree("Exp",3,$1,$2,$3);}
|MINUS Exp {$$=gramTree("Exp",2,$1,$2);}
|NOT Exp {$$=gramTree("Exp",2,$1,$2);}
|ID LP Args RP {$$=gramTree("Exp",4,$1,$2,$3,$4);}
|ID LP RP {$$=gramTree("Exp",3,$1,$2,$3);}
|Exp LB Exp RB {$$=gramTree("Exp",4,$1,$2,$3,$4);}
|Exp DOT ID {$$=gramTree("Exp",3,$1,$2,$3);}
|ID {$$=gramTree("Exp",1,$1);}
|INT {$$=gramTree("Exp",1,$1);}
|FLOAT{$$=gramTree("Exp",1,$1);}
|error RP {$$=gramTree("Exp",1,$2);}
| error RB {$$=gramTree("Exp",1,$2);}
|Exp_miss_RP {$$=gramTree("Exp",1,$1);}
|Exp_miss_RB {$$=gramTree("Exp",1,$1);}
;
Exp_miss_RP : ID LP Args %prec LOWER_THAN_Exp_RP{$$=gramTree("Exp_miss_RP",3,$1,$2,$3);printf("Error type B at line %d :Missing RP \n",$3->lineno);flag=0;}
|LP Exp %prec LOWER_THAN_Exp_RP{$$=gramTree("Exp_miss_RP",2,$1,$2);printf("Error type B at line %d :Missing RP \n",$2->lineno);flag=0;}
|ID LP %prec LOWER_THAN_Exp_RP{$$=gramTree("Exp_miss_RP",2,$1,$2);printf("Error type B at line %d :Missing RP \n",$2->lineno);flag=0;}
;
Exp_miss_RB : Exp LB Exp %prec LOWER_THAN_Exp_RB{$$=gramTree("Exp_miss_RB",3,$1,$2,$3);printf("Error type B at line %d :Missing RB \n",$3->lineno);flag=0;}
;
Args : Exp COMMA Args {$$=gramTree("Args",3,$1,$2,$3);}
|Exp {$$=gramTree("Args",1,$1);}
;
%%

int main(int argc,char** argv){
    if(argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
 
   yyrestart(f);
   //yydebug=1;
yyparse();
    return 0;
}
/*
 yyerror(char *msg)
 {
 fprintf(stderr,"error:%s\n",msg);
 }*/

