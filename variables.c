#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#define INT 0
#define FLOAT 1
#define MAXTYPENUM 500

typedef struct Type_* Type;   //注意这里是指针！！
typedef struct FieldList_* FieldList;


struct Type_
{
    enum{
        BASIC, ARRAY, STRUCTURE
    }kind;
    union{
        //基本类型 int,float
        int basic;
        //数组类型 
        struct {
            Type elem;  //元素类型
            int size;   //数组大小
            int dimension;  //数组维度
        }array;
        //结构体类型
        FieldList structure;
    } u;
};

struct FieldList_
{
    char* name;   //名字
    Type type;    //域类型
    FieldList tail;  //下一个域
    int memnum;    //成员变量的个数
};

static Type typelist;
static int typeNum;

Type initTypeList();    //初始化类型列表
int ifExist(char* name_);  //是否存在某个名字的类型
Type newBasic(int type_);  //新的固定类型的变量
Type newArray(Type type_,int size_);    //新的数组变量
Type newStructure(char* name_,Type* type_); //新的结构体变量
Type StructureAdd(Type struct_,...);    //结构体内包含若干成员变量
//还缺一个判等价的函数



Type initTypeList()
{
    typelist = (Type)malloc(MAXTYPENUM*sizeof(struct Type_));
    typelist[0].kind = BASIC;   //0处是int型
    typelist[0].u.basic = INT;
    typelist[1].kind = BASIC;   //1处是float型
    typelist[1].u.basic = FLOAT;
    typeNum = 2;
}

int ifExist(char* name_)
{
    int i;
    for(i=0;i<typeNum;i++)
    {
        if(strcmp(name_,typelist[i].u.structure->name)==0)  //字符串相等
        {
            return i;
        }
    }
    return -1;
}

Type newBasic(int type_)  //0:整数  1：浮点数
{
    if(type_==INT)
    {
        return typelist;
    }
    else
    {
        return typelist+1;
    } 
}

Type newArray(Type type_,int size_)
{
    if(typeNum>=MAXTYPENUM)
    {
        printf("发生错误：变量类型过多\n");
        exit(1);
    }
    typelist[typeNum].kind = ARRAY;
    typelist[typeNum].u.array.elem = type_;
    typelist[typeNum].u.array.size = size_;
    typelist[typeNum].u.array.dimension = type_->u.array.dimension+1;
    typeNum++;
    return typelist+typeNum-1;
}

Type newStructure(char* name,Type* type_)
{
    if(typeNum>=MAXTYPENUM)
    {
        printf("发生错误：变量类型过多\n");
        exit(1);
    }
    typelist[typeNum].kind = BASIC;
    int namesize = strlen(name);
    typelist[typeNum].u.structure->name = (char*)malloc(sizeof(char)*namesize);
    strcpy(typelist[typeNum].u.structure->name)   
}



int main()
{

    char a[] = "abc";
    printf("%c\n",*a);
    return 0;
}
