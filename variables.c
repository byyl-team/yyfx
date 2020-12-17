#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>
#define INT 0
#define FLOAT 1
#define MAXTYPENUM 10000  //最大的变量类型数量

typedef struct Type_* Type;   //注意这里是指针！！
typedef struct FieldList_* FieldList;


struct Type_
{
    enum{
        BASIC, ARRAY, STRUCTURE   //类型：基础类型(int,float);数组;结构体
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
    int pre;  //全局变量类型，前驱指针
    int nxt;  //全局变量类型，后继指针
};

struct FieldList_
{ 
    char* name;   //名字
    Type type;    //域类型
    FieldList tail;  //下一个域
    int memnum;    //成员变量的个数
    int hasArray;  //有利于在判断结构体结构等价时剪枝
};

static Type typelist;
static int typeNum;

Type initTypeList();    //初始化类型列表
int ifExist(char* name_);  //是否存在某个名字的类型，参数：名字  返回值：存在则返回位置，不存在返回-1
Type newBasic(int type_);  //新的固定类型的变量，参数：0 int   1 float
Type newArray(Type type_,int size_);    //新的数组变量，参数:基本类型type  数组大小size
Type newStructure(char* name_); //新的结构体变量，参数：名字
Type StructureAdd(Type struct_,int memnum,...);    //结构体内包含若干成员变量，参数：父亲结构体，要包含的成员变量数目，(循环)成员变量Type，成员变量名称char*,...
bool isEqual(Type a,Type b);   //判断两个类型是否等价
bool hideType(Type struct_);  //把这个结构体从链表中去掉
void printType(Type t);      //打印该类型信息
void printTypeList();        //打印全局类型链表




Type initTypeList()
{
    typelist = (Type)malloc(MAXTYPENUM*sizeof(struct Type_));
    typelist[0].kind = BASIC;   //0处是int型
    typelist[0].u.basic = INT;
    typelist[0].pre = -1;      //链表首
    typelist[0].nxt = 1; 
    typelist[1].kind = BASIC;   //1处是float型
    typelist[1].u.basic = FLOAT;
    typelist[1].pre = 0;     //链表第二个
    typelist[1].nxt = -1;
    typeNum = 2;
}

int ifExist(char* name_)
{
    int i;
    for(i=0;i!=-1;i = typelist[i].nxt)  //遍历全局变量类型链表
    {
        if(typelist[i].kind!=STRUCTURE) continue;
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
    //在1后面插入类型
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //防止1处是链表尾而越界
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    hideType(type_);  //基本类型从全局类型中删去
    return typelist+typeNum-1;
}

Type newStructure(char* name)
{
    if(typeNum>=MAXTYPENUM)
    {
        printf("发生错误：变量类型过多\n");
        exit(1);
    }
    typelist[typeNum].kind = STRUCTURE;
    typelist[typeNum].u.structure = (FieldList)malloc(sizeof(struct FieldList_));
    //结构体名字
    int namesize = strlen(name);
    typelist[typeNum].u.structure->name = (char*)malloc(sizeof(char)*namesize);
    strcpy(typelist[typeNum].u.structure->name,name);
    //结构体后继指针
    typelist[typeNum].u.structure->tail = NULL;  //父亲struct的FieldList_中的type是空的，tail指向了第一个成员变量
    //成员变量个数
    typelist[typeNum].u.structure->memnum = 0;
    //是否含有数组
    typelist[typeNum].u.structure->hasArray = 0;
    //在1后面插入类型
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //防止1处是链表尾而越界
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    return typelist+typeNum-1;
}

bool hideType(Type struct_)  //从全局变量类型链表里删去
{
    if(struct_->pre!=-1)
        typelist[struct_->pre].nxt = struct_->nxt;
    if(struct_->nxt!=-1)
        typelist[struct_->nxt].pre = struct_->pre;
}

Type StructureAdd(Type struct_,int memnum,...)  //参数：父亲结构体，要包含的成员变量数目，(循环)成员变量Type，成员变量名称char*,...
{
    va_list arg_ptr;
    va_start(arg_ptr,memnum);
    Type structi;    //循环中的成员变量type
    FieldList fieldcur;  //上一个成员变量的FieldList
    FieldList temp;      //当前成员变量的FieldList
    char* name_;      //循环中的成员变量name
    if(memnum<=0)
    {
        return struct_;
    }
    

    struct_->u.structure->memnum += memnum;  //成员变量的个数
    int i;
    for(i=0;i<memnum;i++)  //取memnum个成员变量
    {
        structi = va_arg(arg_ptr,Type);   //成员变量的Type
        name_ = va_arg(arg_ptr,char*);    //成员变量的名字
        //下面开始创建相应的FieldList
        temp = (FieldList)malloc(sizeof(struct FieldList_));
        temp->name = (char*)malloc(sizeof(char)*strlen(name_));
        strcpy(temp->name,name_);    //成员变量的名字复制过来
        temp->type = structi;         
        temp->tail = NULL;         //一开始没有后继
        if(structi->kind==STRUCTURE)  //如果是结构体，有一层FieldList的嵌套，把相应的值赋值
        {   
            temp->memnum = structi->u.structure->memnum;  
            temp->hasArray = structi->u.structure->hasArray; 
        }
        else if(structi->kind==ARRAY)
        {
            temp->memnum = 1;
            temp->hasArray = 1;  //有数组
        }
        else
        {
            temp->memnum = 1;
            temp->hasArray = 0; //没有数组
        }
        if(temp->hasArray==1)  //成员变量有数组
        {
            struct_->u.structure->hasArray = 1;  //推出该结构体有数组
        }
        if(i==0)  //当前是第一个成员变量
        {
            struct_->u.structure->tail = temp;  //链表头指向它
        }
        else    //不是第一个成员变量
        {
            fieldcur->tail = temp;   //上一个指向它
        }
        fieldcur = temp;
        if(structi->kind!=BASIC)   //如果不是基本变量类型
        {
            hideType(structi);   //不能出现在全局变量类型链表里了。
        }  
    }
    return struct_;

}


void printType(Type t)
{
    if(t->kind==BASIC)
    {
        printf("basic:%d",t->u.basic);
        return;
    }
    else if(t->kind==ARRAY)
    {
        printType(t->u.array.elem);
        printf("[%d]",t->u.array.size);
    }
    else if(t->kind==STRUCTURE)
    {
        printf("struct %s{ ",t->u.structure->name);
        FieldList cur = t->u.structure->tail;
        while(cur!=NULL)
        {
            printf("%s:",cur->name);
            printType(cur->type);
            printf(" ");
            cur = cur->tail;
        }
        printf("} ");
    }
}

void printTypeList()
{
    int i;
    for(i=0;i!=-1;i = typelist[i].nxt)
    {
        printType(&typelist[i]);
        printf("\n");
    }
}

bool isEqual(Type a,Type b)
{
    if(a->kind!=b->kind)
    {
        return false;
    }
    if(a->kind==BASIC)
    {
        return a->u.basic==b->u.basic?true:false;
    }
    else if(a->kind==ARRAY)
    {
        if(isEqual(a->u.array.elem,b->u.array.elem))
        {
            if(a->u.array.dimension==b->u.array.dimension)
            {
                return true;
            }
        }
    }
    else if(a->kind==STRUCTURE)
    {
        int anum = a->u.structure->memnum;
        int bnum = b->u.structure->memnum;
        if(anum!=bnum) return false;  //成员变量数目不等一定不等价
        if(a->u.structure->hasArray || b->u.structure->hasArray) //含有数组则一定不等价
        {
            return false;
        }
        int compare[anum+2];  //compare标记数组
        int i=0,j=0;
        for(i=1;i<=anum;i++) compare[i] = 0;  //初始化为0
        FieldList acur,bcur;
        acur = a->u.structure;
        for(i=1;i<=anum;i++)  //对a的每个成员变量
        {
            acur = acur->tail;
            bcur = b->u.structure;
            for(j=1;j<=bnum;j++) //从b的成员变量中找
            {
                bcur = bcur->tail;
                if(compare[j]==0 && isEqual(acur->type,bcur->type))  //没有标记过并且相等
                {
                    compare[j] = 1;
                    break;
                }
            }
            if(j>bnum) return false;  //没找到：不等价
        }
        return true;
    }
    return false;
}



int main()
{

 /*  一些测试代码
    initTypeList();
    printTypeList();
    Type temp,temp2,temp3,temp4,temp5;
    temp5 = newStructure("lalala");
    temp = newBasic(0);
    temp = newArray(temp,3); 
    temp5 = StructureAdd(temp5,1,temp,"naughty");

    temp2 = newBasic(1);
    temp3 = newStructure("apple");
    temp3 = StructureAdd(temp3,2,temp,"red",temp5,"white");
    temp4 = newStructure("banana");
    temp4 = StructureAdd(temp4,2,temp5,"white",temp,"yellow");

    temp = newArray(temp,3);
    temp = newArray(temp,5);
    temp = newArray(temp,9);
   
    temp = newStructure("happy");
    temp2 = newBasic(1);
    temp2 = StructureAdd(temp,1,temp2,"son");
    printf("hereee\n");
    temp3 = newStructure("father");
    printf("here\n");
    temp3 = StructureAdd(temp3,1,temp2,"mid");

    printf("%d\n",temp5->u.structure->hasArray);
    printTypeList();

    if(isEqual(temp3,temp4))
    {
        printf("temp3 and temp4 are equal\n");
    }
    else
    {
        printf("not equal\n");
    }
    */

    return 0;
}
