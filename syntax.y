%{
#include <stdio.h>
#include<unistd.h>
#include<string.h>
#include "gramtree.h"
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
%right ASSIGNOP
%left AND OR
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%nonassoc LOWER_THAN_Exp_RP LOWER_THAN_Exp_RB
%left LB RB LP RP
%left DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList {$$=gramTree("Program",1,$1);printf("\n");circulate($$,0,flag);}
;
ExtDefList : ExtDef ExtDefList{$$=gramTree("ExtDefList",2,$1,$2);}
| {$$=gramTree("ExtDefList",0,-1);}
;
ExtDef : Specifier ExtDecList SEMI//变量定义:检查是否重定义Error type 3
	{
	$$=gramTree("ExtDef",3,$1,$2,$3);
	if(exitvar($2)) printf("Error type 3 at Line %d:Redefined Variable 		'%s'\n",yylineno,$2->content);
	else newvar(2,$1,$2);
	}
| Specifier SEMI{$$=gramTree("ExtDef",2,$1,$2);}
|Specifier FunDec CompSt//函数定义:检查实际返回类型与函数类型是否匹配Error type 8
        {
        $$=gramTree("ExtDef",3,$1,$2,$3);
        newfunc(4,$1);
        }
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
StructSpecifier : STRUCT OptTag LC DefList RC //结构体定义:检查是否重定义Error type 16
        {
        $$=gramTree("StructSpecifier",5,$1,$2,$3,$4,$5);
        if(exitstruc($2))	printf("Error type 16 at Line %d:Duplicated name '%s'\n",yylineno,$2->content);
        else newstruc(1,$2);
        }
| error RC {$$=gramTree("StructSpecifier",1,$2);flag=0;}
| STRUCT Tag //结构体引用:检查是否未定义就引用Error type 17
	{
        $$=gramTree("StructSpecifier",2,$1,$2);
        if(!exitstruc($2)) printf("Error type 17 at Line %d:undefined structure '%s'\n",yylineno,$2->content);
        }
| StructSpecifier_miss_RC {$$=gramTree("StructSpecifier",1,$1);flag=0;}
;
StructSpecifier_miss_RC : STRUCT OptTag LC DefList {$$=gramTree("StructSpecifier_miss_RC",4,$1,$2,$3,$4);printf("Error type B at line %d :Missing RC \n",$4->lineno);flag=0;}
;
OptTag : ID{$$=gramTree("OptTag",1,$1);}
| {$$=gramTree("OptTag",0,-1);}
;
Tag : ID{$$=gramTree("Tag",1,$1);}
;
VarDec : ID{$$=gramTree("VarDec",1,$1);$$->tag=1;}
| VarDec LB INT RB{$$=gramTree("VarDec",4,$1,$2,$3,$4);$$->content=$1->content;$$->tag=4;}
| error RB{$$=gramTree("VarDec",1,$2);flag=0;}
| VarDec_miss_RB {$$=gramTree("VarDec",1,$1);flag=0;}
;
VarDec_miss_RB : VarDec LB INT {$$=gramTree("VarDec_miss_RB",3,$1,$2,$3);printf("Error type B at line %d :Missing RB \n",$3->lineno);flag=0;}
;
FunDec : ID LP VarList RP//函数定义:检查是否重复定义Error type 4
        {
	$$=gramTree("FunDec",4,$1,$2,$3,$4);$$->content=$1->content;
        if(exitfunc($1)) printf("Error type 4 at Line %d:Redefined Function '%s'\n",yylineno,$1->content);
        else newfunc(2,$1);
	}
| ID LP RP //函数定义:检查是否重复定义Error type 4
        {
	$$=gramTree("FunDec",3,$1,$2,$3);$$->content=$1->content;
        if(exitfunc($1)) printf("Error type 4 at Line %d:Redefined Function '%s'\n",yylineno,$1->content);
        else newfunc(2,$1);
        }
| error RP{$$=gramTree("FunDec",1,$2);flag=0;}
| FunDec_miss_RP {$$=gramTree("FunDec",1,$1);flag=0;}
;
FunDec_miss_RP : ID LP VarList {$$=gramTree("FunDec_miss_RP",3,$1,$2,$3);printf("Error type B at line %d :Missing RP \n",$3->lineno);flag=0;}
| ID LP {$$=gramTree("FunDec_miss_RP",2,$1,$2);printf("Error type B at line %d :Missing RP \n",$2->lineno);flag=0;}
;
VarList : ParamDec COMMA VarList  {$$=gramTree("VarList",3,$1,$2,$3);}
| ParamDec {$$=gramTree("VarList",1,$1);}
;
ParamDec : Specifier VarDec{$$=gramTree("ParamDec",2,$1,$2);newvar(2,$1,$2);newfunc(1);}
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
Def : Specifier DecList SEMI//变量或数组定义:检查变量是否重定义 Error type 3
	{
	$$=gramTree("Def",3,$1,$2,$3);
        if(exitvar($2)||exitarray($2))  printf("Error type 3 at Line %d:Redefined Variable '%s'\n",yylineno,$2->content);
        else if($2->tag==4) newarray(2,$1,$2);
        else newvar(2,$1,$2);
	}
| error SEMI {$$=gramTree("Def",1,$2);flag=0;}
| Def_miss_SEMI {$$=gramTree("Def",1,$1);flag=0;}
;
Def_miss_SEMI : Specifier DecList {$$=gramTree("Def_miss_SEMI",2,$1,$2);printf("Error type B at line %d :Missing SEMI \n",$2->lineno);flag=0;}
;
DecList : Dec {$$=gramTree("DecList",1,$1);}
|Dec COMMA DecList {$$=gramTree("DecList",3,$1,$2,$3);$$->tag=$3->tag;}
;
Dec : VarDec {$$=gramTree("Dec",1,$1);}
|VarDec ASSIGNOP Exp {$$=gramTree("Dec",3,$1,$2,$3);$$->content=$1->content;}
;
Exp : Exp ASSIGNOP Exp//检查等号左右类型匹配判断Error type 5
	{
	$$=gramTree("Exp",3,$1,$2,$3);
        if($1->type==$3->type){printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",yylineno);}
        }
|Exp AND Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp OR Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp RELOP Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp PLUS Exp{$$=gramTree("Exp",3,$1,$2,$3);//检查操作符左右类型Error type 7
        if(strcmp($1->type,$3->type)){printf("Error type 7 at Line %d:Type mismatched for operand.\n ",yylineno);}}
|Exp MINUS Exp{$$=gramTree("Exp",3,$1,$2,$3);}
|Exp STAR Exp{$$=gramTree("Exp",3,$1,$2,$3);//检查操作符左右类型Error type 7
        if(strcmp($1->type,$3->type)){printf("Error type 7 at Line %d:Type mismatched for operand.\n ",yylineno);}}

|Exp DIV Exp{$$=gramTree("Exp",3,$1,$2,$3);//检查操作符左右类型Error type 7
        if(strcmp($1->type,$3->type)){printf("Error type 7 at Line %d:Type mismatched for operand.\n ",yylineno);}}

|LP Exp RP{$$=gramTree("Exp",3,$1,$2,$3);}
|MINUS Exp {$$=gramTree("Exp",2,$1,$2);}
|NOT Exp {$$=gramTree("Exp",2,$1,$2);}
|ID LP Args RP {$$=gramTree("Exp",4,$1,$2,$3,$4);//函数引用:检查是否未定义就调用Error type 2 
        if(!exitfunc($1)){printf("Error type 2 at Line %d:undefined Function %s\n ",yylineno,$1->content);}}

|ID LP RP {$$=gramTree("Exp",3,$1,$2,$3);}
|Exp LB Exp RB {$$=gramTree("Exp",4,$1,$2,$3,$4);//数组引用：是否定义&标识误用&下标 Error type 10，Error type 12
        if(strcmp($3->type,"int"))printf("Error type 12 at Line %d:%.1f is not a integer.\n",yylineno,$3->value);
        if((!exitarray($1))&&(exitvar($1)||exitfunc($1)))printf("Error type 10 at Line %d:'%s'is not an array.\n ",yylineno,$1->content);
        else if(!exitarray($1)){printf("Error type 2 at Line %d:undefined Array %s\n ",yylineno,$1->content);}}

|Exp DOT ID {$$=gramTree("Exp",3,$1,$2,$3);//结构体引用:检查点号引用Error type 13
        if(!exitstruc($1))printf("Error type 13 at Line %d:Illegal use of '.'.\n",yylineno);}

|ID //变量引用:检查是否定义Error type 1 
        {
        $$=gramTree("Exp",1,$1);
        if(!exitvar($1)&&!exitarray($1))
            printf("Error type 1 at Line %d:undefined variable %s\n ",yylineno,$1->content);
        else $$->type=typevar($1);
        }
|INT {$$=gramTree("Exp",1,$1);$$->tag=3;$$->type="int";}
|FLOAT{$$=gramTree("Exp",1,$1);$$->tag=3;$$->type="float";$$->value=$1->value;}
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
Args : Exp COMMA Args {$$=gramTree("Args",3,$1,$2,$3);rpnum+=1;}
|Exp {$$=gramTree("Args",1,$1);rpnum+=1;}
;
%%
/*
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
}*/
/*
 yyerror(char *msg)
 {
 fprintf(stderr,"error:%s\n",msg);
 }*/


