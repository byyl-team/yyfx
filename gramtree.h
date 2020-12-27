extern int yylineno;//行号
extern char* yytext;//语素
void yyerror(char *s,...);//错误处理函数
/*抽象语法树*/
struct gramtree
{
	int lineno;//行号
	char* name;//语法单元名称
	struct gramtree *leftchild;//左节点
	struct gramtree *rightchild;//右节点
	union//存放ID、TYPE、INT、FLOAT节点的值
	{
		char *IDTYPE;
		int INT;
		float FLOAT;
	};
};
/*构造抽象语法树，边长参数
name：语素名字
num：变长参数中语法节点个数*/
struct gramtree *gramTree(char* name,int num,...);
/*遍历抽象语法树，level为树的层次*/
void circulate(struct gramtree* newfather,int level,int flag);
