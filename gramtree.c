#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"gramtree.h"
#include"variabletable.h"
#define YYERROR_VERBOSE
struct gramtree *gramTree(char* name,int num,...)
{
	int flag=1;
	va_list valist;//定义变长参数列表
	struct gramtree *newfather=(struct gramtree *)malloc(sizeof(struct gramtree));//新生成的父节点
	struct gramtree *temp=(struct gramtree*)malloc(sizeof(struct gramtree));
	if(!newfather)
	{
		yyerror("out of space!");
		exit(0);
	}
	newfather->name=name;
	va_start(valist,num);//初始化变长参数为num后的参数
	if(num>0)//num>0为非终结符：变长参数均为语法树结点，孩子兄弟表示法
    	{
        	temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第一个结点设为左孩子
        	newfather->leftchild=temp;
        	newfather->lineno=temp->lineno;//父节点的行号等于左孩子的行号

        	if(num>=2) //可以规约到a的语法单元>=2
        	{
			int i;
            		for(i=0; i<num-1; ++i)//取变长参数列表中的剩余结点，依次设置成兄弟结点
            		{
                		temp->rightchild=va_arg(valist,struct gramtree*);
                		temp=temp->rightchild;
            		}
        	}
    	}
    	else //num==0为终结符或产生空的语法单元：第1个变长参数表示行号，产生空的语法单元行号为-1。
    	{
        	int t=va_arg(valist, int); //取第1个变长参数
        	newfather->lineno=t;
        	if((!strcmp(newfather->name,"ID"))||(!strcmp(newfather->name,"TYPE"))||(!strcmp(newfather->name,"RELOP")))//"ID,TYPE,INTEGER，借助union保存yytext的值
        	{
        		char*t;
        		t=(char*)malloc(sizeof(char* )*40);
        		strcpy(t,yytext);
        		newfather->content=t;
        	}
        	
        	else if(!strcmp(newfather->name,"INT")) {newfather->INT=atoi(yytext);}
        	else if(!strcmp(newfather->name,"INT8"))
		{
			newfather->INT=strtol(yytext,NULL,8);
		}
		else if(!strcmp(newfather->name,"INT16"))
		{
			newfather->INT=strtol(yytext,NULL,16);
		}
		else
		{
		    newfather->leftchild = newfather->rightchild = NULL;
		    //free(newfather);
		    //newfather = NULL;

		}
    	}
    	return newfather;
}
void circulate(struct gramtree* newfather,int level,int flag)
{
	if(flag==1){
		if(newfather!=NULL)
	    	{
			int i;
			for(i=0; i<level; ++i)//孩子结点相对父节点缩进2个空格
		    		printf("  ");
			if(newfather->lineno!=-1){ //产生空的语法单元不需要打印信息
		    		printf("%s ",newfather->name);//打印语法单元名字，ID/TYPE/INTEGER要打印yytext的值
		    		if((!strcmp(newfather->name,"ID"))||(!strcmp(newfather->name,"TYPE"))||(!strcmp(newfather->name,"RELOP")))printf(":%s ",newfather->content);
		    		else if(!strcmp(newfather->name,"INT"))printf(":%d",newfather->INT);
				else if(!strcmp(newfather->name,"INT8")) printf(":%d",newfather->INT);
				else if(!strcmp(newfather->name,"INT16")) printf(":%d",newfather->INT);
		    		else if(!strcmp(newfather->name,"STRUCT")) printf(":struct");
		    		else if(!strcmp(newfather->name,"RETURN")) printf(":return");
		    		else if(!strcmp(newfather->name,"WHILE")) printf(":while");
		    		else if(!strcmp(newfather->name,"IF")) printf(":if");
		    		else if(!strcmp(newfather->name,"ELSE")) printf(":else");
		    		else if(!strcmp(newfather->name,"SEMI")) printf(":;");
		    		else if(!strcmp(newfather->name,"COMMA")) printf(":,");
		    		else if(!strcmp(newfather->name,"PLUS")) printf(":+");
		    		else if(!strcmp(newfather->name,"MINUS")) printf(":-");
		    		else if(!strcmp(newfather->name,"ASSIGNOP")) printf(":=");
		    		else if(!strcmp(newfather->name,"STAR")) printf(":*");
		    		else if(!strcmp(newfather->name,"DIV")) printf(":/");
		    		else if(!strcmp(newfather->name,"AND")) printf(":&&");
		    		else if(!strcmp(newfather->name,"OR")) printf(":||");
		    		else if(!strcmp(newfather->name,"DOT")) printf(":.");
		    		else if(!strcmp(newfather->name,"NOT")) printf(":!");
		    		else if(!strcmp(newfather->name,"LP")) printf(":(");
		    		else if(!strcmp(newfather->name,"RP")) printf(":)");
		    		else if(!strcmp(newfather->name,"LB")) printf(":[");
		    		else if(!strcmp(newfather->name,"RB")) printf(":]");
		    		else if(!strcmp(newfather->name,"LC")) printf(":{");
		    		else if(!strcmp(newfather->name,"RC")) printf(":}");
		    		else
		        		printf("(%d)",newfather->lineno);
		}
		else
		{
			printf("Empty : Empty");
		}
		printf("\n");\
		circulate(newfather->leftchild,level+1,flag);//遍历左子树
		circulate(newfather->rightchild,level,flag);//遍历右子树
	    }
    }
    
}
void yyerror( char*s,...) //变长参数错误处理函数
{
    va_list ap;
    va_start(ap,s);
    fprintf(stderr,"Error type B at line %d:%s\n",yylineno,s);
    //fprintf(stderr,"%d:error:",yylineno);//错误行号
    //vfprintf(stderr,s,ap);
    //fprintf(stderr,"\n");
}
/*int main()
{
    printf(">");
    return yyparse(); //启动文法分析，调用词法分析
}*/
