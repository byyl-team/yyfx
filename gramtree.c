#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include"gramtree.h"
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
		if(num==1)//只有一个孩子
        	{
            		newfather->content=temp->content;//父节点的语义值等于左孩子的语义值
            		newfather->tag=temp->tag;
        	}
        	else //可以规约到a的语法单元>=2
        	{
            		for(int i=0; i<num-1; ++i)//取变长参数列表中的剩余结点，依次设置成兄弟结点
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
        	if(!strcmp(newfather->name,"INT"))
        	{
        		newfather->type="INT";
        	}
        	
        	else if(!strcmp(newfather->name,"FLOAT")){
        		newfather->type="FLOAT";
        		newfather->value=atof(yytext);
        	}
        	else {
        		char* s;
            		s=(char*)malloc(sizeof(char* )*40);
            		strcpy(s,yytext);//存储词法单元的语义值
            		newfather->content=s;
        	}
    	}
    	return newfather;
}
void circulate(struct gramtree* newfather,int level,int flag)
{
	if(flag==1){
		if(newfather!=NULL)
	    	{
			for(int i=0; i<level; ++i)//孩子结点相对父节点缩进2个空格
		    		printf("  ");
			if(newfather->lineno!=-1){ //产生空的语法单元不需要打印信息
		    		printf("%s ",newfather->name);//打印语法单元名字，ID/TYPE/INTEGER要打印yytext的值
		    		if((!strcmp(newfather->name,"ID"))||(!strcmp(newfather->name,"TYPE"))||(!strcmp(newfather->name,"RELOP")))printf(":%s ",newfather->content);
		    		else if(!strcmp(newfather->name,"INT"))printf(":%s",newfather->type);
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

/*====(1)变量符号表的建立和查询================*/
void newvar(int num,...)//1)创建变量符号表
{
    va_list valist; //定义变长参数列表
    struct var *a=(struct var*)malloc(sizeof(struct var));//新生成的父节点
    struct gramtree *temp=(struct gramtree*)malloc(sizeof(struct gramtree));
    va_start(valist,num);//初始化变长参数为num后的参数
    temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第一个结点
    a->type=temp->content;
    temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第二个结点
    a->name=temp->content;
    vartail->next=a;
    vartail=a;
}

int  exitvar(struct gramtree* tp)//2)查找变量是否已经定义,是返回1，否返回0
{
    struct var* p=(struct var*)malloc(sizeof(struct var*));
    p=varhead->next;
    int flag=0;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
        {
            flag=1;    //存在返回1
            return 1;
        }
        p=p->next;
    }
    if(!flag)
    {
        return 0;//不存在返回0
    }
}

char* typevar(struct gramtree*tp)//3)查找变量类型
{
    struct var* p=(struct var*)malloc(sizeof(struct var*));
    p=varhead->next;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
            return p->type;//返回变量类型
        p=p->next;
    }
}
/*====(2)函数符号表的建立和查询================*/
void newfunc(int num,...)//1)创建函数符号表
{
    va_list valist; //定义变长参数列表
    struct gramtree *temp=(struct gramtree*)malloc(sizeof(struct gramtree));
    va_start(valist,num);//初始化变长参数为num后的参数
    switch(num)
    {
    case 1:
        functail->pnum+=1;//参数个数加1
        break;
    case 2://记录函数名
        temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第1个结点
        functail->name=temp->content;
        break;
    case 3://记录实际返回值
        temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第1个结点
        functail->rtype=temp->type;
        break;
    default://记录函数类型,返回类型不匹配则报出错误
        rpnum=0;//将实参个数清0
        temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第1个结点
        if(functail->rtype!=NULL)//实际返回类型和函数定义的返回类型比较
        {
            if(strcmp(temp->content,functail->rtype))printf("Error type 8 at Line %d:Type mismatched for return.\n",yylineno);
        }
        functail->type=temp->type;
        functail->tag=1;//标志为已定义
        struct func *a=(struct func*)malloc(sizeof(struct func));
        functail->next=a;//尾指针指向下一个空结点
        functail=a;
        break;
    }
}

int  exitfunc(struct gramtree* tp)//2)查找函数是否已经定义,是返回1，否返回0
{
    int flag=0;
    struct func* p=(struct func*)malloc(sizeof(struct func*));
    p=funchead->next;
    while(p!=NULL&&p->name!=NULL&&p->tag==1)
    {
        if(!strcmp(p->name,tp->content))
        {
            flag=1;    //存在返回1
            return 1;
        }
        p=p->next;
    }
    if(!flag)
        return 0;//不存在返回0
}
char* typefunc(struct gramtree*tp)//3)查找函数类型
{
    struct func* p=(struct func*)malloc(sizeof(struct func*));
    p=funchead->next;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
            return p->type;//返回函数类型
        p=p->next;
    }
}

int pnumfunc(struct gramtree*tp)//4)查找函数的形参个数
{
    struct func* p=(struct func*)malloc(sizeof(struct func*));
    p=funchead->next;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
            return p->pnum;//返回形参个数
        p=p->next;
    }
}

/*====(3)数组符号表的建立和查询================*/
void newarray(int num,...)//1)创建数组符号表
{
    va_list valist; //定义变长参数列表
    struct array *a=(struct array*)malloc(sizeof(struct array));//新生成的父节点
    struct gramtree *temp=(struct gramtree*)malloc(sizeof(struct gramtree));
    va_start(valist,num);//初始化变长参数为num后的参数
    temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第一个结点
    a->type=temp->content;
    temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第二个结点
    a->name=temp->content;
    arraytail->next=a;
    arraytail=a;
}

int  exitarray(struct gramtree* tp)//2)查找数组是否已经定义,是返回1，否返回0
{
    struct array* p=(struct array*)malloc(sizeof(struct array*));
    p=arrayhead->next;
    int flag=0;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
        {
            flag=1;    //存在返回1
            return 1;
        }
        p=p->next;
    }
    if(!flag)
    {
        return 0;//不存在返回0
    }
}

char* typearray(struct gramtree* tp)//3)查找数组类型
{
    struct array* p=(struct array*)malloc(sizeof(struct array*));
    p=arrayhead->next;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
            return p->type;//返回数组类型
        p=p->next;
    }
}
/*====(4)结构体符号表的建立和查询================*/
void newstruc(int num,...)//1)创建结构体符号表
{
    va_list valist; //定义变长参数列表
    struct struc *a=(struct struc*)malloc(sizeof(struct struc));//新生成的父节点
    struct gramtree *temp=(struct gramtree*)malloc(sizeof(struct gramtree));
    va_start(valist,num);//初始化变长参数为num后的参数
    temp=va_arg(valist, struct gramtree*);//取变长参数列表中的第二个结点
    a->name=temp->content;
    structail->next=a;
    structail=a;
}

int  exitstruc(struct gramtree* tp)//2)查找结构体是否已经定义,是返回1，否返回0
{
    struct struc* p=(struct struc*)malloc(sizeof(struct struc*));
    p=struchead->next;
    int flag=0;
    while(p!=NULL)
    {
        if(!strcmp(p->name,tp->content))
        {
            flag=1;    //存在返回1
            return 1;
        }
        p=p->next;
    }
    if(!flag)
    {
        return 0;//不存在返回0
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
int main(int argc,char** argv)
{
    if(argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
 
   yyrestart(f);
    varhead=(struct var*)malloc(sizeof(struct var));//变量符号表头指针
    vartail=varhead;//变量符号表尾指针

    funchead=(struct func*)malloc(sizeof(struct func));//函数符号表头指针
    functail=(struct func*)malloc(sizeof(struct func));//函数符号表头指针
    funchead->next=functail;//函数符号表尾指针
    functail->pnum=0;
    arrayhead=(struct array*)malloc(sizeof(struct array));//数组符号表头指针
    arraytail=arrayhead;

    struchead=(struct struc*)malloc(sizeof(struct struc));//结构体符号表头指针
    structail=struchead;//结构体符号表尾指针

    return yyparse(); //启动文法分析，调用词法分析
}
/*int main()
{
    printf(">");
    return yyparse(); //启动文法分析，调用词法分析
}*/

