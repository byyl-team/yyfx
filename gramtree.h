extern int yylineno;//行号
extern char* yytext;//语素
void yyerror(char *s,...);//错误处理函数
/*抽象语法树*/
struct gramtree
{
	int lineno;//行号
	char* name;//语法单元名称
	int tag;//1为变量，2为函数，3为常数，4为数组，5为结构体
	struct gramtree *leftchild;//左节点
	struct gramtree *rightchild;//右节点
	char* content;//语法单元语义值
	char* type;//语法单元数据类型，主要用于等号和操作符左右类型匹配判断
	float value;//场数值（记录int和float的数据值）
	union//存放ID、TYPE、INT、FLOAT节点的值
	{
		char *IDTYPE;
		int INT;
		float FLOAT;
	};
};
/*变量符号表的结点*/
struct var
{
    char* name;//变量名
    char* type;//变量类型
    struct var *next;//指针
}*varhead,*vartail;

/*函数符号表的结点*/
struct func
{
    int tag;//0表示未定义，1表示定义
    char* name;//函数名
    char* type;//函数类型
    char* rtype;//实际返回值类型
    int pnum;//形参数个数
    struct func *next;
}*funchead,*functail;
int rpnum;//记录函数实参个数

/*数组符号表的结点*/
struct array
{
    char* name;//数组名
    char* type;//数组类型
    struct array *next;
}*arrayhead,*arraytail;

/*结构体符号表的结点*/
struct struc
{
    char* name;//结构体名
    char* type;//数组类型
    struct struc *next;
}*struchead,*structail;


/*构造抽象语法树，变长参数
name：语素名字
num：变长参数中语法节点个数*/
struct gramtree *gramTree(char* name,int num,...);
/*遍历抽象语法树，level为树的层次*/
void circulate(struct gramtree* newfather,int level,int flag);
/*=====变量符号表========================*/
/*建立变量符号表*/
void newvar(int num,...);

/*查找变量是否已经定义,是返回1，否返回0*/
int  exitvar(struct gramtree*tp);

/*查找变量类型*/
char* typevar(struct gramtree*tp);

/*=================函数符号表==============*/
/*建立函数符号表,flag：1表示变量符号表，2表示函数符号表,num是参数个数*/
void newfunc(int num,...);

/*查找函数是否已经定义,是返回1，否返回0*/
int exitfunc(struct gramtree*tp);

/*查找函数类型*/
char* typefunc(struct gramtree*tp);

/*查找函数的形参个数*/
int pnumfunc(struct gramtree*tp);

/*=================数组符号表==============*/
/*建立数组符号表*/
void newarray(int num,...);

/*查找数组是否已经定义,是返回1，否返回0*/
int exitarray(struct gramtree*tp);

/*查找数组类型*/
char* typearray(struct gramtree*tp);

/*=================结构体符号表==============*/
/*建立结构体符号表*/
void newstruc(int num,...);

/*查找结构体是否已经定义,是返回1，否返回0*/
int exitstruc(struct gramtree*tp);



