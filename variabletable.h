//
//  variabletable.h
//  yyfx
//
//  Created by 贾馥榕 on 2020/12/17.
//  Copyright © 2020年 贾馥榕. All rights reserved.
//

#ifndef variabletable_h
#define variabletable_h

#include <stdio.h>
#include "class.h"
//int space_deep=0;//域的深度
struct unit{//符号表中定义的，这里简要用来测试，老王里面的Type，直接include
    char* name;//假设是变量类型的名字
};
struct node{
    //内容：
    char * vi_name;//变量名或返回值类型
    struct unit * ptr;//变量类型信息
    int space_deep;//deep
    Type type;
    //for func
    int is_func;
    int is_defined;
    int param_size;//func
    struct unit ** param;//param of func
    //to type
    Type * param_type;
    //in link table
    struct node * hash_forw_node;//在hash槽链表中的下一个node节点
    struct node * space_forw_node;
    struct node * nxt_func_node;
};
/*
 node中包含两个指针，指向的类型都是node，分别是node在两个线性维度上的组织
 第一个：struct node * hase_forw 是在hash表槽链表中的下一个node
 第二个：struct node * space_forw_node 是在自己所在的space链表中的下一个node
 要注意区分
 */
struct space_unit{//代表一个域，其实是一个链表
    char * space_name;
    struct node * forw;//这个域链表的表头node指针(是最新插入的)
    struct space_unit * forw_space;//在space栈中的下一个space
    int is_func;
};
/*
 space_unit中包含两个指针
 第一个：struct node * forw 是指向本域【链表】的表头node（在space内部延伸）mama's daughter
 第二个：struct space_unit * forw_spac 是指向【域栈】中的下一个space_unit （在域的栈内延伸）
 mama and aunt3 4
 要注意区分，两者指向的数据类型不同
 */
struct hash_cao{//hash table中的一个槽，一个链表
    struct node * forw;//链表头指针
};
//struct hash_cao * hash_table;
//struct space_unit * top;
/************开始的时候执行，为哈希表分配空间**************/
void init();
void insert_space_unit(int is_func,...);//插入一个域space
void insert_variable_unit(char *vi_name,char * type);//创建一个变量（非函数、非数组），插入变量符号表中
void insert_variable_unit_bytype(char *vi_name,Type type);
void insert_array_unit(char *array_name, int dimension, Type array_type);//创建数组变量，把数组变量插入到符号表中
//*********

void insert_func_unit(char *func_name,char * return_type,int param_size,char** param_types,int is_defining);//创建函数
void insert_func_unit_bytype(char *func_name,Type return_type,int param_size,Type *param_types,int is_defining);


struct node * search_variable(char* vi_name);//按名查找变量，返回最内的域的同名变量的node的指针
Type search_variable_type(char* vi_name);//输入变量名，返回变量类型（变量/函数）
struct node* search_func(char *func_name);//按名搜索函数在变量符号表中的指针
int search_repeat(char* vi_name);//变量查重，如果有重复就返回true
void delete_space_unit(int is_struct,...);//删除目前栈顶的域
Type delete_struct_space(char *struct_name);
int pjw_hash(char *name);//哈希函数
struct node* search_func(char *func_name);//按名搜索函数在变量符号表中的指针
void check_all_func_defined();//检查所有声明的函数是否定义了
int able_define_func(char *func_name,int param_size,char** param_types,int is_defining);//是否可以定义/声明当前的函数
int able_define_func_bytype(char *func_name,Type return_type,int param_size,Type* param_types,int is_defining);
struct space_unit* find_nearest_func_space();
Type create_array(Type array_type, int dimension);
#endif /* variabletable_h */
