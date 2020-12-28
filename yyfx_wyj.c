#include<stdio.h>
#include<stdlib.h>
#include"class.h"
#include"gramtree.h"
#include"variabletable.h"
void Program(struct gramtree* node);
void ExtDefList(struct gramtree* node);
void ExtDef(struct gramtree* node);
Type Specifier(struct gramtree* node);
Type StructSpecifier(struct gramtree* node);
void Tag(struct gramtree* node,char* name_);
void ExtDecList(struct gramtree* node,Type specifier_tp);
int VarDec(struct gramtree* node,char* name_,int dimension);
void FunDec(Type return_type,struct gramtree* tree,int is_defining);
Type* VarList(struct gramtree* tree, int *var_num);
Type ParamDec(struct gramtree* tree);
void CompSt(struct gramtree* node,int flag);
void DefList(struct gramtree* node,int flag);
void Def(struct gramtree* node,int flag);
void DecList(struct gramtree* node,Type type_,int flag);
void Dec(struct gramtree* node,Type type_,int flag);
void StmtList(struct gramtree* node);
void Stmt(struct gramtree* node);
void OptTag(struct gramtree* node,char* name_);
Type Exp(struct gramtree* node){};

void Program(struct gramtree* node)
{
    ExtDefList(node->leftchild);
    printf("");
}
void ExtDefList(struct gramtree* node)   //ExtDef 
{
    if(node==NULL){  // 空的产生式
        return;
    }
    if(node->leftchild!=NULL)  //ExtDef ExtDefList
    {
        struct gramtree* cur = node->leftchild;
        ExtDef(cur);
        ExtDefList(cur->rightchild);
    }
}

void ExtDef(struct gramtree* node)
{
    struct gramtree* cur = node->leftchild;
    Type specifier = Specifier(cur);  //name是Specifier叶节点的值
    if(specifier==NULL)  //Specifier出错
    {
        return;
    }
    //如果定义全局变量
    if(strcmp(cur->rightchild->name,"ExtDecList")==0)  //Specifier ExtDecList SEMI
    {
        ExtDecList(cur->rightchild,specifier);
    }
    //如果定义结构体
    else if(strcmp(cur->rightchild->name,"SEMI")==0)  //Specifier SEMI
    {
        //可能是int;float;struct a;struct a{...};没有要求识别这些错误
        return;
    }
    //如果定义函数
    else if(strcmp(cur->rightchild->name,"FunDec")==0)
    {
        if(strcmp(cur->rightchild->rightchild->name,"SEMI")==0){
            //函数的声明
            FunDec(specifier,cur->rightchild,0);
            return;
        }
        //函数的定义
        FunDec(specifier,cur->rightchild,1);
        CompSt(cur->rightchild->rightchild,1);//OK
    }
}
Type Specifier(struct gramtree* node)   //TYPE | STRUCTSPECIFIER
{
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"TYPE")==0)  //TYPE terminal
    {
        if(strcmp(cur->IDTYPE,"int")==0) //int
        {
            return newBasic(0);
        }
        else if(strcmp(cur->IDTYPE,"float")==0)  //float
        {
            return newBasic(1);  
        }
    }
    else if(strcmp(cur->name,"StructSpecifier")==0)
    {
        return StructSpecifier(cur);
    }
    else
    {
        printf("undeclared Specifier productor!\n");
        return NULL;
    }
    
}

Type StructSpecifier(struct gramtree* node)
{
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->rightchild->name,"OptTag")==0)  //STRUCT OptTag LC DefList RC
    {
        insert_space_unit(0);  //新建一个结构体域，贾馥榕的代码会改，不传参
        char* struct_name;
        OptTag(cur->rightchild,struct_name);  //得到结构体的名字
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur,1);
        Type struct_tp = delete_struct_space(struct_name);
        return struct_tp;  //return 新建的Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name;
        Tag(cur->rightchild,struct_name);
        Type struct_tp = ifExist(struct_name);
        if(struct_tp==NULL)   //该名字的结构体不存在
        {
            printf("Error type 17 at Line %d: Undefined structure \"%s\"\n",cur->rightchild->lineno,struct_name);
            return NULL;
        }
        return struct_tp;
    }
}

void OptTag(struct gramtree* node,char* name_)
{
    if(node==NULL)
    {
        //结构体名字是空的
        if(name_!=NULL) free(name_);
        return;
    }
    struct gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(name_!=NULL) free(name_);   //先释放再申请
        name_= (char*)malloc(sizeof(char)*strlen(cur->IDTYPE));
        strcpy(name_,cur->IDTYPE);
    }
}

void Tag(struct gramtree* node,char* name_)
{
    if(node==NULL)
    {
        return;
    }
    if(node->leftchild!=NULL)  //ID
    {
        if(name_!=NULL) free(name_);  //先释放再申请
        name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->IDTYPE));  
        strcpy(name_,node->leftchild->IDTYPE);  //名字复制给name_
    }
}

void ExtDecList(struct gramtree* node,Type specifier_tp)
{
    struct gramtree* cur = node->leftchild;
    char* name_;
    int dimension = 0;
    dimension = VarDec(cur,name_,dimension);   
    if(dimension==0)
    {
        insert_variable_unit_bytype(name_,specifier_tp);  //变量插入变量表
    }
    else
    {
        insert_array_unit(name_, dimension, specifier_tp); //数组插入变量表
    }
    //VarDec
    if(cur->rightchild==NULL)
    {
        return;
    }
    //VarDec COMMA ExtDecList   
    cur = cur->rightchild;  //cur:COMMA
    cur = cur->rightchild;  //cur:ExtDecList
    ExtDecList(cur,specifier_tp);  //一直到ExtDecList产生式是VarDec为止
}

int VarDec(struct gramtree* node,char* name_,int dimension)
{
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"ID")==0)   //ID
    {
        if(name_!=NULL) free(name_);
        name_ = (char*)malloc(sizeof(char)*strlen(node->IDTYPE));
        strcpy(name_,node->IDTYPE);
        return dimension;
    }
    else if(strcmp(cur->name,"VarDec")==0)  //VarDec LB INT RB
    {
        int dims = VarDec(cur,name_,dimension);
        return dims+1;
    }
    else  //未定义的产生式
    {
        printf("undeclared VarDec productor!\n");
        return 0;
    }
    
}

void FunDec(Type return_type,struct gramtree* tree,int is_defining){
    //函数加入符号表
    if(is_defining==0){
        //函数声明
        if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
            //无参数
            char *func_name=tree->leftchild->name;
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
            return;
        }
        //有参数
        char *func_name=tree->leftchild->name;
        Type* Typelist;//问题：是否需要提前分配内存？？？或者在函数中分配内存也行，目前是在函数中分配的
        struct gramtree* varlist=tree->leftchild->rightchild->rightchild;
        int *var_num=(int*)malloc(sizeof(int));
        *var_num=0;
        Typelist=VarList(varlist,var_num);
        if(Typelist==NULL){
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
        }
        //定义对应的函数
        insert_func_unit_bytype(func_name,return_type,*var_num,Typelist,0);
        return;
    }
    if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
        //无参数
        char *func_name=tree->leftchild->name;
        insert_space_unit(1,func_name);//加入一个域
        insert_func_unit_bytype(func_name,return_type,0,NULL,1);
        return;
    }
    //有参数
    char *func_name=tree->leftchild->name;
    insert_space_unit(1,func_name);//加入一个域 ok
    Type* Typelist;//问题：是否需要提前分配内存？？？或者在函数中分配内存也行，目前是在函数中分配的
    struct gramtree* varlist=tree->leftchild->rightchild->rightchild;
    int *var_num=(int*)malloc(sizeof(int));
    *var_num=0;
    Typelist=VarList(varlist,var_num);
    //定义对应的函数
    insert_func_unit_bytype(func_name,return_type,*var_num,Typelist,1);
}
Type* VarList(struct gramtree* tree, int *var_num){
    //最开始传的是0
    //一个儿子（ParamDec）
    //*var_num=*var_num+1;
    /*
     调用这个函数的时候一定要注意要给var_num指针分配动态内存空间
     第一次调用这个函数的时候传进来的参数是0
     调用结束之后就变成分析得到的数值了
     */
    if(tree->leftchild->rightchild==NULL){
        *var_num=1;
        Type type=ParamDec(tree->leftchild);//P
        if(type==NULL){
            return NULL;
        }
        Type* typelist=(Type*)malloc(sizeof(Type));
        typelist[0]=type;
        //memcpy(typelist,&type,sizeof(Type));//ok
        return typelist;
    }
    //多个儿子(ParamDec COMMA VarList)
    else{
        Type first_para_type=ParamDec(tree->leftchild);//ParamDec类型 P
        if(first_para_type=NULL){
            return VarList(tree->leftchild->rightchild->rightchild,var_num);
        }
        Type *typelist=VarList(tree->leftchild->rightchild->rightchild,var_num);//VarList类型表
        //此时的var_num是VarList中得到的
        Type *curlist=(Type*)malloc(sizeof(Type)*(*var_num+1));
        curlist[0]=first_para_type;
        memcpy(curlist+sizeof(Type),typelist,sizeof(Type)*(*var_num));
        *var_num=*var_num+1;
        return typelist;
    }
}
Type ParamDec(struct gramtree* tree){

    Type specifier= Specifier(tree->leftchild);

    if(specifier==NULL){

        return NULL;

    }

    char *shadiao;

    int dem=VarDec(tree->leftchild->rightchild,shadiao,0);

    if(dem==0){

        return specifier;

    }

    else{

        //Type create_array(Type array_type, int dimension)

        Type new_array_type=create_array(specifier, dem);

        return new_array_type;

    }

    //int VarDec(gramtree* node,char* name_,int dimension);

}


void CompSt(struct gramtree* node,int flag) //LC DefList StmtList RC  flag:是否是函数
{
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = node->rightchild;  //cur:DefList
    if(flag==0){
        insert_space_unit(0);
        //函数定义时域的插入在FunDecphoning完成
    }
    DefList(cur,0);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
    //void delete_space_unit(int is_struct,...)
    delete_space_unit(0);
}

void DefList(struct gramtree* node,int flag)
{
    if(node==NULL)  //空的产生式
    {
        return;
    }
    struct gramtree* cur= node->leftchild; //Def DefList
    Def(cur,flag);
    cur = cur->rightchild;
    DefList(cur,flag);
}

void Def(struct gramtree* node,int flag)  //Specifier DecList SEMI
{
    struct gramtree* cur = node->leftchild;  //cur:Specifier
    Type specifier_tp = Specifier(cur);
    if(specifier_tp==NULL)   //说明specifier出错了
    {
        return;
    }
    cur = cur->rightchild;
    DecList(cur,specifier_tp,flag);
}


void DecList(struct gramtree* node,Type type_,int flag)
{
    struct gramtree* cur = node->leftchild;
    Dec(cur,type_,flag);
    if(cur->rightchild==NULL)  //Dec
    {
        return;
    }
    else  //Dec COMMA DecList
    {
        cur = cur->rightchild;  //cur:COMMA
        cur = cur->rightchild;  //cur:DecList
        DecList(cur,type_,flag);
    }
    
}


void Dec(struct gramtree* node,Type type_,int flag)  //flag 1：结构体  0：函数
{
    struct gramtree* cur = node->leftchild;
    int dimension = 0;
    char* name_;
    dimension = VarDec(cur,name_,dimension);
    if(dimension==0)
    {
        insert_variable_unit_bytype(name_,type_);
        //insert_variable_unit(name_,type_);
    }
    else
    {
        insert_array_unit(name_, dimension, type_);
    }
    if(cur->rightchild==NULL)  //VarDec
    {
        return;
    }
    else   //VarDec ASSIGNOP Exp
    {
        if(flag)
        {
            printf("Error type 15 at Line %d: Illegal initialization in a structure.\n");  //错误类型15：结构体定义时对域进行初始化
            return;
        }
        cur = cur->rightchild;  //cur:ASSIGNOP
        cur = cur->rightchild;  //cur:Exp
        Type right_tp = Exp(cur);    //这里用到思宇和译元的函数
        Type left_tp = search_variable_type(name_);
        if(isEqual(left_tp,right_tp))
        {
            return;
        }
        else
        {
            printf("Error type 5 at Line %d: Type mismatched for assignment.\n",cur->lineno);
            return;
        }
        
    }
    
}

void StmtList(struct gramtree* node)
{
    if(node==NULL)  //空产生式
    {
        return;
    }
    struct gramtree* cur = node->leftchild;  //Stmt StmtList
    Stmt(cur);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
}


void Stmt(struct gramtree* node)
{
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"Exp")==0)  //Exp SEMI
    {
        Exp(cur);
    }
    else if(strcmp(cur->name,"CompSt")==0)  //CompSt
    {
        CompSt(cur,0);  //不是函数！ok
    }
    else if(strcmp(cur->name,"RETURN")==0)  //RETURN Exp SEMI
    {
        cur = cur->rightchild;  //Exp
        Type return_tp = Exp(cur);

        
    }
}



