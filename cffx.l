%{
	#include <stdlib.h>
	#include <stdio.h>
	#include<string.h>
	#include<regex.h>
	#include"syntax.tab.h"
	#include"gramtree.h"
	FILE* f;
	int column = 1;  //记录该行第几个字符
	char n = '\n';   
	char* nad=NULL;  //注释中换行符的位置

%}
%option yylineno
COMMENTM \/\*([^\*]*(\*)+[^\*\/])*[^\*]*\*+\/
COMMENTS \/\/.*
SPACE [ \t\f\r]+
SPACEN \n+
INT 0|[1-9][0-9]*
INT16 0[xX][a-fA-F0-9]+
INT8 0[1-7][0-7]*
FLOAT (([0-9]+\.[0-9]+)([e|E][\+-]?[0-9]+)?)|(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[e|E][\+-]?[0-9]+
FLOAT_ERROR ([0-9]+\.[0-9]*|[0-9]*\.[0-9]+|[0-9]*)((e|E)[\+-]?([0-9]+\.[0-9]*|[0-9]*\.[0-9]+|[0-9]*))?
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
TYPE int|float
SEMI ;
COMMA ,
PLUS \+
ASSIGNOP =
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
RELOP >|<|>=|<=|==|\!=
ID [a-z_A-Z][a-z_A-Z0-9]*
INT16_ERROR 0[xX][a-fA-F0-9]*[g-zG-Z]+[a-fA-F0-9]*
INT8_ERROR 0[0-7]*[89]+[0-7]*
IDERROR [a-zA-Z_][a-zA-Z0-9_]*[^ \n\t\f\r\p{Z}]*
%%
{COMMENTM} {
    //printf("NOTE at line %d char %d:%s \n",yylineno,column,yytext);  //多行注释
    column = (nad = strrchr(yytext,n))==NULL?(column+yyleng):(yyleng-(nad-yytext)); //根据多行注释最后换行符的位置，更新当前的列指针
}
{COMMENTS} {
    //printf("NOTE at line %d char %d:%s \n",yylineno,column,yytext);  //单行注释
    column+=yyleng;
}
{SPACE} {
    // printf("space at line %d char %d:%s!\n",yylineno,column,yytext);
    column+=yyleng;
}
{SPACEN} {
    column = 1;  //出现换行符，列指针回到开头
}
{INT} {
	//printf("INT data at line %d char %d: %s\n",yylineno,column,yytext);
	column+=yyleng;
	//yylval = atoi(yytext);
	yylval.newfather=gramTree("INT",0,yylineno);
	return INT;
	
}
	
{INT16} {
	//printf("INT16 at line %d char %d: charachters \"%s\"\n",yylineno,column,yytext);
	column+=yyleng;
	yylval.newfather=gramTree("INT16",0,yylineno);
	//yylval = strtol(yytext,NULL,16);
	return INT;
}
{INT8} {
    //printf("INT8 at line %d char %d: charachters \"%s\"\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("INT8",0,yylineno);
    //yylval = strtol(yytext,NULL,8);
    return INT;
}
{INT16_ERROR} {
    printf("ERROR Type A at line %d char %d:Illgal INT16 number \"%s\"\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("ERROR",0,yylineno);
    //yylval = strtol(yytext,NULL,16);
    return ERROR;
}
{INT8_ERROR} {
    printf("ERROR Type A at line %d char %d: Illgal INT8 number \"%s\"\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("ERROR",0,yylineno);
    //yylval = strtol(yytext,NULL,8);
    return ERROR;
}
{STRUCT} {
    column+=yyleng;
    yylval.newfather=gramTree("STRUCT",0,yylineno);
    return STRUCT;
}
{RETURN} {
    column+=yyleng;
    yylval.newfather=gramTree("RETURN",0,yylineno);
    return RETURN;
}
{IF} {
    column+=yyleng;
    yylval.newfather=gramTree("IF",0,yylineno);
    return IF;
}
{ELSE} {
    column+=yyleng;
    yylval.newfather=gramTree("ELSE",0,yylineno);
    return ELSE;
}
{WHILE} { 
    //printf("KEYWORD at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("WHILE",0,yylineno);
    return WHILE;
}
{TYPE} { 
    //printf("TYPE at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("TYPE",0,yylineno);
    return TYPE;
}
{ID} {
    //printf("ID at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    //yylval = strdup(yytext);
    yylval.newfather=gramTree("ID",0,yylineno);
    return ID;
}
{PLUS} { 
    //printf("PLUS at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("PLUS",0,yylineno);
    return PLUS;
}
{SEMI} { 
    //printf("SEMI at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("SEMI",0,yylineno);
    return SEMI;
}
{COMMA} { 
    //printf("COMMA at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("COMMA",0,yylineno);
    return COMMA;
}
{ASSIGNOP} { 
    column+=yyleng;
    yylval.newfather=gramTree("ASSIGNOP",0,yylineno);
    return ASSIGNOP;
}
{MINUS} { 
    column+=yyleng;
    yylval.newfather=gramTree("MINUS",0,yylineno);
    return MINUS;
}
{STAR} {
    //printf("STAR at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("STAR",0,yylineno);
    return STAR;
}
{DIV} { 
    //printf("DIV at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("DIV",0,yylineno);
    return DIV;
}
{AND} { 
    //printf("AND at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("AND",0,yylineno);
    return AND;
}
{OR} { 
    //printf("OR at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("OR",0,yylineno);
    return OR;
}
{DOT} { 
    //printf("DOT at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("DOT",0,yylineno);
    return DOT;
}
{NOT} { 
    //printf("NOT at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("NOT",0,yylineno);
    return NOT;
}
{LP} { 
    //printf("LP at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("LP",0,yylineno);
    return LP;
}
{RP} { 
    //printf("RP at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("RP",0,yylineno);
    return RP;
}
{LB} { 
    //printf("LB at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("LB",0,yylineno);
    return LB;
}
{RB} { 
    //printf("RB at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("RB",0,yylineno);
    return RB;
}
{LC} { 
    //printf("LC at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("LC",0,yylineno);
    return LC;
}
{RC} { 
    //printf("RC at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("RC",0,yylineno);
    return RC;
}
{RELOP} { 
    //printf("RELOP at line %d char %d: %s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("RELOP",0,yylineno);
    return RELOP;
}
{FLOAT} {
    //printf("FLOAT at line %d char %d:%s\n",yylineno,column,yytext);
    column+=yyleng;
    yylval.newfather=gramTree("FLOAT",0,yylineno);
    //yylval = atof(yytext);
    return FLOAT;
}
{FLOAT_ERROR} {
    printf("Error Type A at line %d,char %d:  Illgal float number: \"%s\"\n",yylineno,column,yytext);
    column+=yyleng;
    return ERROR;
}
. { 
    printf("Error type A at line %d:Mysterious character '%s'\n",yylineno,yytext);
    column+=yyleng;
}

%% 
/*
int main(int argc, char* argv[])
{
    if (argc > 1){
        if(!(yyin = fopen(argv[1], "r"))){
            perror(argv[1]);
            return 1;
        }
        while (yylex() != 0);
    }
    return 0;
}
*/
int yywrap()
{
        return 1;
}

