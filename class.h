#ifndef CLASS_H
#define CLASS_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>
#define INT 0
#define FLOAT 1
#define MAXTYPENUM 10000  //最大的变量类型数量
#define MAXMEMNUM 10000

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

void initTypeList();    //初始化类型列表
Type ifExist(char* name_);  //是否存在某个名字的类型，参数：名字  返回值：存在则返回该Type，不存在返回NULL
Type ifExistStruct(char* name_);//是否存在某个名字的结构体类型
Type newBasic(int type_);  //基础类型，参数：0 int   1 float
Type newArray(Type type_);    //数组类型，参数:基本类型type  数组大小size
Type newStructure(char* name_); //结构体类型，参数：名字
Type StructureAdd(Type struct_,int memnum,...);    //结构体内包含若干成员变量，参数：父亲结构体，要包含的成员变量数目，(循环)成员变量Type，成员变量名称char*,...
Type arrayMem(Type type_,int dms);     //返回数组第几维度的类型，参数：（数组）类型，维度
Type structMem(Type type_,int num,...);  //结构体的成员变量类型，参数：（结构体）类型，后面成员变量个数num，num个成员变量名。比如类型node.id,调用函数structMem(node,1,"id");
bool isEqual(Type a,Type b);   //判断两个类型是否等价
bool hideType(Type struct_);  //把这个结构体从链表中去掉
void printType(Type t);      //打印该类型信息
void printTypeList();        //打印全局类型链表

#endif
