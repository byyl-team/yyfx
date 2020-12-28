//
//  variabletable.c
//  yyfx
//
//  Created by 贾馥榕 on 2020/12/17.
//  Copyright © 2020年 贾馥榕. All rights reserved.
//

#include "variabletable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "class.h"
int space_deep;//域的深度
struct hash_cao * hash_table;
struct space_unit * top;
struct node* func_top;
void init(){
    /*开始时调用*/
    initTypeList();
    space_deep=0;
    hash_table=(struct hash_cao*)malloc(16384*sizeof(struct hash_cao));
    func_top=NULL;
    top=NULL;
    insert_space_unit(0);
}

/*************插入一个域space **************/
void  insert_space_unit(int is_func,...){
    /*
     插入一个域
     输入：域名（如果不需要此域名，请告知我进行修改）
     */
    space_deep++;
    struct space_unit* new_space=(struct space_unit*)malloc(sizeof(struct space_unit));
    new_space->is_func=is_func;
    va_list arg_ptr;
    va_start(arg_ptr,is_func);//从memnum（的下一个）开始获取变量
    if(is_func){
        char *space_name;
        //printf("******* %s *********\n",space_name);
        space_name=va_arg(arg_ptr, char*);//结构体名字
        //new_space->space_name=space_name;
        //还是用strcpy
        //printf("******* %s *********\n",space_name);
        new_space->space_name=(char*)malloc(sizeof(48));
        strcpy(new_space->space_name, space_name);
    }
    if(top==NULL){
        top=new_space;
        new_space->forw_space=NULL;
    }
    else{
        new_space->forw_space=top;//top是原来的最内层，现在不是了
        top=new_space;//新的top是新的space
    }
    printf("成功插入一个域! 当前域的深度s=%d\n",space_deep);
}

void insert_variable_unit_bytype(char *vi_name,Type type){
    /*
     创建一个变量（非函数、非数组），插入变量符号表中，函数和数组的创建在接下来的函数中实现
     输入：vi_name 变量名
     type 变量类型
     */
    /********变量名查重*********/
    if(search_repeat(vi_name)){
        printf("找到重名变量/结构体类型,无法插入变量节点\n");
        return;
        //重名，返回！
    }
    struct node *new_node=(struct node *)malloc(sizeof(struct node));
    new_node->vi_name=(char*)malloc(sizeof(char)*40);
    strcpy(new_node->vi_name, vi_name);//yeah
    new_node->type=type;
    new_node->space_deep=space_deep;
    new_node->is_func=0;
    printf("构造了变量名为%s，所在域深度为%d的变量\n",new_node->vi_name,new_node->space_deep );
    //插入对应的hash cao
    int val=pjw_hash(vi_name);
    printf("插入哈希表的编号为%d的槽子\n",val);
    //hash_table[val]此时已经（在init函数中）分配了相应的空间，可以直接访问了
    printf("1 if\n");
    if(hash_table[val].forw==NULL){//. or ->
        //原来这个槽空
        /*hash_table[val]的类型是hash_cao*/
        hash_table[val].forw=new_node;
        new_node->hash_forw_node=NULL;
    }
    else{
        //原来槽中有几个node了
        new_node->hash_forw_node=hash_table[val].forw;
        hash_table[val].forw=new_node;
    }
    printf("2 if\n");
    //给栈顶的域top这个维度的链表中也插入变量节点
    if(top->forw==NULL){//top是栈顶的域，top->forw是栈顶域链表的头指针
        top->forw=new_node;
        new_node->space_forw_node=NULL;
    }
    else{
        new_node->space_forw_node=top->forw;
        top->forw=new_node;
    }
    printf("插入变量节点成功\n");
}
/*******插入一个新的变量（非函数、非数组），即结构体 *******/
void insert_variable_unit(char *vi_name,char * type){
    /*
     创建一个变量（非函数、非数组），插入变量符号表中，函数和数组的创建在接下来的函数中实现
     输入：vi_name 变量名
          type 变量类型
     */
    /********变量名查重*********/
    if(search_repeat(vi_name)){
        printf("找到重名变量/结构体类型,无法插入变量节点\n");
        return;
        //重名，返回！
    }
    struct node *new_node=(struct node *)malloc(sizeof(struct node));
    new_node->vi_name=(char*)malloc(sizeof(char)*40);
    strcpy(new_node->vi_name, vi_name);//yeah
    printf("变量类型:%s\n",type);
    Type new_variable_type=NULL;
    new_variable_type=ifExist(type);
    if(new_variable_type==NULL){
        printf("不存在此变量类型～\n");
        return;
    }
    new_node->type=new_variable_type;
    new_node->space_deep=space_deep;
    new_node->is_func=0;
    printf("构造了变量名为%s，所在域深度为%d的变量\n",new_node->vi_name,new_node->space_deep );
    //插入对应的hash cao
    int val=pjw_hash(vi_name);
    printf("插入哈希表的编号为%d的槽子\n",val);
    //hash_table[val]此时已经（在init函数中）分配了相应的空间，可以直接访问了
    if(hash_table[val].forw==NULL){//. or ->
        //原来这个槽空
        /*hash_table[val]的类型是hash_cao*/
        hash_table[val].forw=new_node;
        new_node->hash_forw_node=NULL;
    }
    else{
        //原来槽中有几个node了
        new_node->hash_forw_node=hash_table[val].forw;
        hash_table[val].forw=new_node;
    }
    //给栈顶的域top这个维度的链表中也插入变量节点
    if(top->forw==NULL){//top是栈顶的域，top->forw是栈顶域链表的头指针
        top->forw=new_node;
        new_node->space_forw_node=NULL;
    }
    else{
        new_node->space_forw_node=top->forw;
        top->forw=new_node;
    }
    printf("插入变量节点成功\n");
}

void insert_array_unit(char *array_name, int dimension,Type array_type){
    /*
     创建数组变量，把数组变量插入到符号表中
     输入：
     array_name 数组名称
     dimension 数组（总）维度
     size 整型数组变量，dimension个元素，代表要定义的数组各个维度每个维度的size，比如定义 int a[2][4][6] size=[2,4,6]
     array_type 最底层数组元素类型 比如定义int a[2][4][6] array_type'int'
     */
    if(search_repeat(array_name)){
        printf("找到重名变量/结构体类型,无法插入变量节点\n");
        return;
        //重名，返回！
    }
    Type new_array_type=array_type;//最底层的是我们的array_type 什么类型的数组
    int i;
    for(i=dimension-1;i>=0;i--){
        //从低到高创建数组变量
        new_array_type=newArray(new_array_type);
    }
    struct node *new_node=(struct node *)malloc(sizeof(struct node));
    new_node->vi_name=(char*)malloc(sizeof(char)*40);
    strcpy(new_node->vi_name, array_name);//yeah
    new_node->type=new_array_type;//改成类型表中的Type
    new_node->space_deep=space_deep;
    new_node->is_func=0;
    printf("构造了变量名为%s，所在域深度为%d的变量\n",new_node->vi_name,new_node->space_deep );
    //插入对应的hash cao
    int val=pjw_hash(array_name);
    printf("插入哈希表的编号为%d的槽子\n",val);
    //hash_table[val]此时已经（在init函数中）分配了相应的空间，可以直接访问了
    if(hash_table[val].forw==NULL){//. or ->
        //原来这个槽空
        /*hash_table[val]的类型是hash_cao*/
        hash_table[val].forw=new_node;
        new_node->hash_forw_node=NULL;
    }
    else{
        //原来槽中有几个node了
        new_node->hash_forw_node=hash_table[val].forw;
        hash_table[val].forw=new_node;
    }
    //给栈顶的域top这个维度的链表中也插入变量节点
    if(top->forw==NULL){//top是栈顶的域，top->forw是栈顶域链表的头指针
        top->forw=new_node;
        new_node->space_forw_node=NULL;
    }
    else{
        new_node->space_forw_node=top->forw;
        top->forw=new_node;
    }
    printf("插入数组变量节点成功\n");
    //new_array_type就是我们node中的type！（因为boooth is pointer! ）
}
Type create_array(Type array_type, int dimension){
    Type new_array_type=array_type;//最底层的是我们的array_type 什么类型的数组
    int i;
    for(i=dimension-1;i>=0;i--){
        //从低到高创建数组变量
        new_array_type=newArray(new_array_type);
    }
    return new_array_type;
}

/***********输入参数类型是Type类型变量************/
void insert_func_unit_bytype(char *func_name,Type return_type,int param_size,Type *param_types,int is_defining){
    /*
     创建函数（声明/定义时都要调用，因为可以判断重定义、定义与声明不符合的错误）
     输入：
     func_name 函数名
     return type 返回值类型
     param_size 输入参数总个数
     param_types 函数参数的类型数组
     is_defining 定义函数设置为1，声明设置为0（因为C语言没有bool类型，用整型代替）
     输出错误的格式如要调整，请告知我修改
     */
    int flag=able_define_func_bytype(func_name, return_type, param_size, param_types, is_defining);
    if(!flag){
        printf("函数定义/声明失败\n");
        return;
    }
    if(flag==2){
        printf("之前已经声明/定义过，不重复插入\n");
        return;
    }
    struct node *new_node=(struct node *)malloc(sizeof(struct node));//当场建立，分配内存的思路对！不变
    new_node->vi_name=(char*)malloc(sizeof(char)*40);
    strcpy(new_node->vi_name, func_name);//yeah
    //如果不存在这个类型返回啥 NULL
    new_node->type=return_type;
    new_node->space_deep=space_deep;
    new_node->is_func=1;
    //首次插入节点，is_defined未初始化，可能是任何值。防微杜渐，要初始化。逻辑：如果当前允许定义，那么就是第一个定义，否则就 不是定义是声明 ->0
    if(is_defining){
        new_node->is_defined=1;
    }
    else{
        new_node->is_defined=0;
    }
    new_node->param_size=param_size;
    //new_node->param=(struct unit**)malloc(param_size*sizeof(struct unit*));
    /*二维数组如何分配内存C语言 https://blog.csdn.net/wzy_1988/article/details/9136373*/
    new_node->param_type=(Type *)malloc(param_size*sizeof(Type));//Type已经是指针了
    int i;
    for (i=0;i<param_size;i++){
         new_node->param_type[i]=param_types[i];
    }
    printf("构造了函数名为%s，所在域深度为%d，有%d个参数的函数变量\n",new_node->vi_name,new_node->space_deep,new_node->param_size);
    //插入对应的hash cao
    int val=pjw_hash(func_name);
    printf("插入哈希表的编号为%d的槽子\n",val);
    if(hash_table[val].forw==NULL){//. or ->
        //原来这个槽空
        /*hash_table[val]的类型是hash_cao*/
        hash_table[val].forw=new_node;
        new_node->hash_forw_node=NULL;
    }
    else{
        //原来槽中有几个node了
        new_node->hash_forw_node=hash_table[val].forw;
        hash_table[val].forw=new_node;
    }
    //给栈顶的域top这个维度的链表中也插入变量节点
    if(top->forw==NULL){//top是栈顶的域，top->forw是栈顶域链表的头指针
        top->forw=new_node;
        new_node->space_forw_node=NULL;
    }
    else{
        new_node->space_forw_node=top->forw;
        top->forw=new_node;
    }
    //插入函数的自己的链表中
    if(func_top==NULL){
        func_top=new_node;
        new_node->nxt_func_node=NULL;
    }
    else{
        new_node->nxt_func_node=func_top;
        func_top=new_node;
    }
    printf("插入函数节点成功\n");
}

/*******插入一个新的函数*******/
void insert_func_unit(char *func_name,char * return_type,int param_size,char** param_types,int is_defining){
    /*
     创建函数（声明/定义时都要调用，因为可以判断重定义、定义与声明不符合的错误）
     输入：
     func_name 函数名
     return type 返回值类型
     param_size 输入参数总个数
     param_types 字符串数组，函数输入参数的类型组成的字符串数组 比如 ['int','int','float']
     is_defining 定义函数设置为1，声明设置为0（因为C语言没有bool类型，用整型代替）
     输出错误的格式如要调整，请告知我修改
     */
    int flag=able_define_func(func_name,param_size,param_types, is_defining);
    /*修改able_define_func函数-->able_define_func_bytype ok
     */
    if(!flag){
        printf("函数定义/声明失败\n");
        return;
    }
    if(flag==2){
        printf("之前已经声明/定义过，不重复插入\n");
        return;
    }
    struct node *new_node=(struct node *)malloc(sizeof(struct node));//当场建立，分配内存的思路对！不变
    new_node->vi_name=(char*)malloc(sizeof(char)*40);
    strcpy(new_node->vi_name, func_name);//yeah
    Type new_func_return_type=ifExist(return_type);
    //printf("after ifExist in insert_func_unit\n");
    //如果不存在这个类型返回啥 NULL
    if(new_func_return_type==NULL){
        printf("不存在此返回值变量类型\n");
        return;
    }
    new_node->type=new_func_return_type;
    new_node->space_deep=space_deep;
    new_node->is_func=1;
    //首次插入节点，is_defined未初始化，可能是任何值。防微杜渐，要初始化。逻辑：如果当前允许定义，那么就是第一个定义，否则就 不是定义是声明 ->0
    if(is_defining){
        new_node->is_defined=1;
    }
    else{
        new_node->is_defined=0;
    }
    new_node->param_size=param_size;
    //new_node->param=(struct unit**)malloc(param_size*sizeof(struct unit*));
    /*二维数组如何分配内存C语言 https://blog.csdn.net/wzy_1988/article/details/9136373*/
    new_node->param_type=(Type *)malloc(param_size*sizeof(Type));//Type已经是指针了
    int i;
    for (i=0;i<param_size;i++){
        /*
         new_node->param_type[i]=param_types[i];
         */
        Type tmp_type=ifExist(param_types[i]);
        if(tmp_type==NULL){
            printf("不存在第%d个参数的类型\n",i+1);
            return;
        }
        new_node->param_type[i]=tmp_type;
    }
    printf("构造了函数名为%s，所在域深度为%d，有%d个参数的函数变量\n",new_node->vi_name,new_node->space_deep,new_node->param_size);
    //插入对应的hash cao
    int val=pjw_hash(func_name);
    printf("插入哈希表的编号为%d的槽子\n",val);
    if(hash_table[val].forw==NULL){//. or ->
        //原来这个槽空
        /*hash_table[val]的类型是hash_cao*/
        hash_table[val].forw=new_node;
        new_node->hash_forw_node=NULL;
    }
    else{
        //原来槽中有几个node了
        new_node->hash_forw_node=hash_table[val].forw;
        hash_table[val].forw=new_node;
    }
    //给栈顶的域top这个维度的链表中也插入变量节点
    if(top->forw==NULL){//top是栈顶的域，top->forw是栈顶域链表的头指针
        top->forw=new_node;
        new_node->space_forw_node=NULL;
    }
    else{
        new_node->space_forw_node=top->forw;
        top->forw=new_node;
    }
    //插入函数的自己的链表中
    if(func_top==NULL){
        func_top=new_node;
        new_node->nxt_func_node=NULL;
    }
    else{
        new_node->nxt_func_node=func_top;
        func_top=new_node;
    }
    printf("插入函数节点成功\n");
}


/******按名查找变量，返回最内的域的同名变量的node的指针******/
struct node * search_variable(char* vi_name){
    /*
     按名查找变量
     输入：vi_name 变量名
     返回值：node结点指针
     */
    int val=pjw_hash(vi_name);
    printf("查找变量名位于哈希表的编号为%d的槽子\n",val);
    struct node * p=hash_table[val].forw;//head
    while(p!=NULL){
        if(!strcmp(p->vi_name, vi_name)){//找到
            printf("在全局范围内查到了给定名字的变量\n");
            return p;//返回指针，最内层的同名变量/函数
        }
        p=p->hash_forw_node;
    }
    return NULL;
}

/******输入变量名，返回变量类型（变量返回值/函数）*******/
Type search_variable_type(char* vi_name){
    /*
     按名查找变量类型
     输入：vi_name 变量名
     返回值：对应类型的Type（定义见class.h）
    注意，如果判断结构体中某个域的类型，或者数组某个维度的类型，不能用这个函数实现查找类型，使用class中的接口实现
     */
    struct node * now_node=search_variable(vi_name);
    if(now_node==NULL){
        printf("未找到此变量\n");
        return NULL;
    }
    return now_node->type;
}

/********按名搜索函数在变量符号表中的指针*******/
struct node* search_func(char *func_name){
    /*
     按名搜索函数在变量符号表中的指针
     输入：func_name 函数名
     */
    struct node* tmp_func=func_top;
    while(tmp_func!=NULL){
        if(!strcmp(tmp_func->vi_name, func_name)){
            printf("查找到了给定名字的函数\n");
            return tmp_func;
        }
        tmp_func=tmp_func->nxt_func_node;
    }
    return NULL;
}
struct node* last_func(){
    return NULL;
}

void check_all_func_defined(){
    /*检查所有声明的函数是否定义了*/
    struct node* tmp_func=func_top;
    int count_no_defined=0;
    while(tmp_func!=NULL){
        if(!tmp_func->is_defined){
            printf("存在未定义函数%s\n",tmp_func->vi_name);
            count_no_defined++;
        }
        tmp_func=tmp_func->nxt_func_node;
    }
    if(count_no_defined==0){
        printf("所有函数均已定义\n");
    }
}
int able_define_func_bytype(char *func_name,Type return_type ,int param_size,Type* param_types,int is_defining){
    /*
     输入：
     func_name 函数名
     param_size 函数形参综述
     param_types 函数参数的类型数组
     is_defining 定义函数设置为1，声明设置为0（因为C语言没有bool类型，用整型代替）
     返回值：
     0. 重复定义，不可以定义
     1. 没声明过，可以定义
     2. 声明过，和之前声明的符合，可以定义
     */
    struct node* rep_func=search_func(func_name);
    if(rep_func==NULL){
        return 1;//没声明过，可以定义
    }
    else{
        //查找到同名的函数声明/定义
        printf("查找到同名的函数声明/定义\n");
        if(rep_func->is_defined){//已经定义过了
            if(is_defining){
                printf("重定义函数\n");
                return 0;
            }
            //是否可以在定义之后声明，应该也不可以
            else{
                printf("在定义之后声明\n");
                return 0;
            }
        }
        //没定义过、只声明过，检查现在的声明/定义和之前的声明(返回值类型，形参表中变量个数和类型)是否冲突
        if(!isEqual(return_type, rep_func->type)){
            printf("与之前的同名函数声明冲突：函数返回值类型不同\n");
            return 0;
        }
        //just be declared and watch for colision
        if(param_size!=rep_func->param_size){
            //之前声明函数和现在要定义/声明的函数的参数个数不同
            printf("原有：%d,当前：%d\n",rep_func->param_size,param_size);
            printf("与之前的同名函数声明冲突：函数参数表参数个数不同\n");
            return 0;
        }
        else{
            //之前声明函数和现在要定义/声明的函数的参数个数相同
	    int i;
            for(i=0;i<param_size;i++){
                if(!isEqual(rep_func->param_type[i], param_types[i])){
                    printf("当前函数第%d个参数与已声明函数的参数类型不一致\n",i+1);
                    return 0;
                }
            }
            printf("与之前的同名函数声明一致\n");
            if(is_defining){
                rep_func->is_defined=1;
            }
            return 2;
        }
    }
}

int able_define_func(char *func_name,int param_size,char** param_types,int is_defining){
    /*
     输入：
     func_name 函数名
     param_size 函数形参综述
     param_types 函数形参变量类型
     is_defining 定义函数设置为1，声明设置为0（因为C语言没有bool类型，用整型代替）
     返回值：
     0. 重复定义，不可以定义
     1. 没声明过，可以定义
     2. 声明过，和之前声明的符合，可以定义
     */
    struct node* rep_func=search_func(func_name);
    if(rep_func==NULL){
        return 1;//没声明过，可以定义
    }
    else{
        //查找到同名的函数声明/定义
        printf("查找到同名的函数声明/定义\n");
        if(rep_func->is_defined){//已经定义过了
            if(is_defining){
                printf("重定义函数\n");
                return 0;
            }
            //是否可以在定义之后声明，应该也不可以
            else{
                printf("在定义之后声明\n");
                return 0;
            }
        }
        //没定义过、只声明过，检查现在的声明/定义和之前的声明是否冲突
            //just be declared and watch for colision
        if(param_size!=rep_func->param_size){
            //之前声明函数和现在要定义/声明的函数的参数个数不同
            printf("原有：%d,当前：%d\n",rep_func->param_size,param_size);
            printf("与之前的同名函数声明冲突：函数参数表参数个数不同\n");
            return 0;
        }
        else{
            //之前声明函数和现在要定义/声明的函数的参数个数相同
	    int i;
            for(i=0;i<param_size;i++){
                //rep_func一个Type，当前一个Type(需要ifExist得到)，通过isEqual判断即可
                Type new_type=ifExist(param_types[i]);
                if(new_type==NULL){
                    printf("不存在第%d个参数类型\n",i+1);
                    return 0;
                }
                else{
                    if(!isEqual(new_type, rep_func->param_type[i])){
                        //存在不等的类型
                        printf("当前函数第%d个参数与已声明函数的参数类型不一致\n",i+1);
                        return 0;
                    }
                }
            }
            printf("与之前的同名函数声明一致\n");
            if(is_defining){
                rep_func->is_defined=1;
            }
            return 2;
        }
    }
}



/*
int cmp_consistence(struct node* n1,struct node* n2){
    if(n1->param_size!=n2->param_size){
        printf("两函数参数表要求的参数个数不同\n");
        return 0;
    }
    printf("两函数参数表要求的参数个数相同\n");
    //for every param
    for(int i=0;i<n1->param_size;i++){
        //比较两者的类型是否相同
        //不同：立即return 0;
        //相同，什么都不做，继续比较
    }
    return 1;
}
*/
/************变量查重，如果有重复就返回true***************/
int search_repeat(char* vi_name){
    /*
     变量查重
     输入：变量名
     输出：存在同名变量则返回1，其他0
     */
    struct node *pet=search_variable(vi_name);
    Type pet_struct_type=NULL;
    pet_struct_type=ifExistStruct(vi_name);
    if (pet==NULL){
        printf("全局范围内无同名变量\n");
        if(space_deep==1){//struct is global variable 全局变量的变量名需要和结构体类型比较
            printf("目前变量deep=1，是全局变量\n");
            if(pet_struct_type==NULL){//无结构体类型同名
                printf("全局范围内无同名结构体\n");
                return 0;
            }
            else{
                //有结构体类型重名
                printf("error3 有同名结构体类型，不能重复定义\n");
                return 1;
            }
        }
        return 0;
    }
    else{
        //有重名变量
        printf("全局范围内有同名变量,需要进一步确认深度信息\n");
        /********还要先确认深度是否一致，因为找到的最内域的不一定是当前的域*******/
        if(pet->space_deep==space_deep){
            //深度一致，同域同名！
            printf("error3 有（深度相同）同名变量，不能重复定义\n");
            return 1;
        }
        //不同域的同名，可以忽略
        printf("相同作用域内无同名变量\n");
        if(space_deep==1){//struct is global variable
            printf("目前变量deep=1，是全局变量\n");
            if(pet_struct_type==NULL){//无结构体类型同名
                printf("全局范围内无同名结构体\n");
                return 0;
            }
            else{
                //有结构体类型重名
                printf("error3 有同名结构体类型，不能重复定义\n");
                return 1;
            }
        }
        return 0;
    }
}

Type delete_struct_space(char *struct_name){
    if(search_repeat(struct_name)){
        printf("有重名结构体或变量\n");
        return NULL;
    }
    Type struct_type=newStructure(struct_name);
    space_deep--;
    struct node *p=top->forw;
    while (p!=NULL) {
        top->forw=p->space_forw_node;//更换表头为下一个node
        //再在hash中删除p
        int val=pjw_hash(p->vi_name);
        hash_table[val].forw=p->hash_forw_node;
        /*
         Type StructureAdd(Type struct_,int memnum,...);  (循环)成员变量Type，成员变量名称char*,...
         如何从p中得到其类型和名称
         */
        struct_type=StructureAdd(struct_type, 1,p->param_type,p->vi_name);
        struct node *tmpp=p;
        p=p->space_forw_node;
        free(tmpp);//释放内存
    }
    struct space_unit *tmp_space=top;
    top=top->forw_space;//删除这个域
    free(tmp_space);
    printf("删除域，删除之后当前域的深度为%d\n",space_deep);
    return struct_type;
}

/***********删除目前栈顶的域**************/
void delete_space_unit(int is_struct,...){
    /*
     二更：
     如果是结构体的域，那么在这个消失的时候，要把其中的所有变量，其实也就是结构体的成员变量
     调用王亚菁的接口加入结构体类型当中，完成结构体这个类型的定义
     增加一个int 1->是结构体
     ...是char*类型，输入当前这个域对应的结构体名字
     根据名字查找结构体
     Type ifExist(char* name_);  //是否存在某个名字的类型，参数：名字  返回值：存在则返回该Type，不存在返回NULL
     Type StructureAdd(Type struct_,int memnum,...);
     */
    va_list arg_ptr;
    char* struct_name;
    Type struct_type;
    va_start(arg_ptr,is_struct);//从memnum（的下一个）开始获取变量
    printf("删除域，删除之前当前域的深度为%d\n",space_deep);
    if(is_struct){//如果这个域对应一个结构体
        struct_name=va_arg(arg_ptr, char*);//结构体名字
        struct_type=ifExistStruct(struct_name);
        if(struct_type!=NULL||search_repeat(struct_name)){
            printf("有重名结构体或变量\n");
            return;
        }
        struct_type=newStructure(struct_name);
        space_deep--;
        struct node *p=top->forw;
        while (p!=NULL) {
            top->forw=p->space_forw_node;//更换表头为下一个node
            //再在hash中删除p
            int val=pjw_hash(p->vi_name);
            hash_table[val].forw=p->hash_forw_node;
            /*
             Type StructureAdd(Type struct_,int memnum,...);  (循环)成员变量Type，成员变量名称char*,...
             如何从p中得到其类型和名称
             */
            struct_type=StructureAdd(struct_type, 1,p->param_type,p->vi_name);
            struct node *tmpp=p;
            p=p->space_forw_node;
            free(tmpp);//释放内存
        }
        struct space_unit *tmp_space=top;
        top=top->forw_space;//删除这个域
        free(tmp_space);
        printf("删除域，删除之后当前域的深度为%d\n",space_deep);
        return;
    }
    /*
     删除栈顶space中的所有变量
     栈顶space中的所有变量一定都在hash表的表头，因此容易删除
     通过pjw我们可以快速知道其所在的槽，进而进行删除操作cao->forw=node->forw
     可以先在space中删除，再在hash中删除
     */
    space_deep--;
    struct node *p=top->forw;
    //top是space_unit【域】, top->forw是这个【域】链表的头：第一个node
    while(p!=NULL){
        if(p->is_func==true){
            //是函数，要在函数表中删除
            //但是因为函数本身只在最外层，所以我认为没有必要
        }
        //在space_unit中删除p
        top->forw=p->space_forw_node;//更换表头为下一个node
        //再在hash中删除p
        int val=pjw_hash(p->vi_name);
        hash_table[val].forw=p->hash_forw_node;
        struct node *tmpp=p;
        p=p->space_forw_node;
        free(tmpp);//释放内存
    }
    struct space_unit *tmp_space=top;
    top=top->forw_space;//删除这个域
    free(tmp_space);
    printf("删除域，删除之后当前域的深度为%d\n",space_deep);
}
struct space_unit* find_nearest_func_space(){
    //is_func=1
    struct space_unit *p=top;
    while(p!=NULL)
    {
        if(p->is_func){
            return p;
        }
    }
    return NULL;
}
/***************开个玩笑，潘校的hash函数******************/
int pjw_hash(char *name){
    int val=0,i;
    for(;*name;++name){
        val=(val<<2)+*name;
        if(i=val & ~0x3fff){
            val=(val^(i>>12))&0x3fff;
        }
    }
    return val;
}
