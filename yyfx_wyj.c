#include<stdio.h>
#include<stdlib.h>


void Program(gramtree* node)
{
    ExtDefList(node->leftchild);
}
void ExtDefList(gramtree* node)   //ExtDef 
{
    if(node==NULL){  // 空的产生式
        return;
    }
    if(node->leftchild!=NULL)  //ExtDef ExtDefList
    {
        gramtree* cur = node->leftchild;
        ExtDef(cur);
        ExtDefList(cur->rightchild);
    }
}

void ExtDef(gramtree* node)
{
    gramtree* cur = node->leftchild;
    Type specifier = Specifier(cur);  //name是Specifier叶节点的值
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
        if(strcmp(cur->rightchild->rightchild,"SEMI")==0){
            //函数的声明
            FunDec(specifier,cur->rightchild,0);
            return;
        }
        //函数的定义
        FunDec(specifier,cur->rightchild,1);
        Compst(cur->rightchild->rightchild,1);//OK
    }
}
Type Specifier(gramtree* node)   //TYPE | STRUCTSPECIFIER
{
    gramtree* cur = node->leftchild;
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

Type StructSpecifier(gramtree* node)
{
    gramtree* cur = node->leftchild;
    if(strcmp(cur->rightchild->name,"OptTag")==0)  //STRUCT OptTag LC DefList RC
    {
        insert_space_unit(0);  //新建一个结构体域，贾馥榕的代码会改，不传参
        char* struct_name;
        OptTag(cur->rightchild,struct_name);  //得到结构体的名字
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur);
        Type struct_cur = newStructure(struct_name);
        //调用贾馥榕的函数，删除域，变量放到结构体struct_cur里
        //return 新建的Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name;
        Tag(cur->rightchild,struct_name);
        Type struct_tp = ifExist(char* name_);
        if(struct_tp==NULL)   //该名字的结构体不存在
        {
            printf("Error type 17 at Line %d: Undefined structure \"%s\"\n",cur->rightchild->lineno,struct_name);
        }
        return struct_tp;
    }
}

void OptTag(gramtree* node,char* name_)
{
    if(node==NULL)
    {
        //结构体名字是空的
        if(name_!=NULL) free(name_);
        return;
    }
    gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(name_!=NULL) free(name_);   //先释放再申请
        name_= (char*)malloc(sizeof(char)*strlen(cur->IDTYPE));
        strcpy(name_,cur->IDTYPE);
    }
}

void Tag(gramtree* node,char* name_)
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

void ExtDecList(gramtree* node,Type specifier_tp)
{
    gramtree* cur = node->leftchild;
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

int VarDec(gramtree* node,char* name_,int dimension)
{
    gramtree* cur = node->leftchild;
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
void FunDec(Type return_type,struct gramtree tree,int is_defining){
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
        struct gramtree varlist=tree->leftchild->rightchild->rightchild;
        int *var_num=(char*)malloc(sizeof(int));
        *var_num=0;
        Typelist=VarList(varlist,var_num);
        //定义对应的函数
        insert_func_unit_bytype(func_name,return_type,var_num,Typelist,1);
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
    struct gramtree varlist=tree->leftchild->rightchild->rightchild;
    int *var_num=(char*)malloc(sizeof(int));
    *var_num=0;
    Typelist=VarList(varlist,var_num);
    //定义对应的函数
    insert_func_unit_bytype(func_name,return_type,var_num,Typelist,1);
}
Type* VarList(struct gramtree tree, int *var_num){
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
        Type type=ParamDec(tree->leftchild);
        Type* typelist=(Type*)malloc(sizeof(Type));
        typelist[0]=type;
        //memcpy(typelist,&type,sizeof(Type));//ok
        return typelist;
    }
    //多个儿子(ParamDec COMMA VarList)
    else{
        Type first_para_type=ParamDec(gramtree->leftchild);//ParamDec类型
        Type *typelist=VarList(gramtree->leftchild->rightchild->rightchild,var_num);//VarList类型表
        //此时的var_num是VarList中得到的
        Type *curlist=(Type*)malloc(sizeof(Type)*(*var_num+1));
        curlist[0]=first_para_type;
        memcpy(curlist+sizeof(Type),typelist,sizeof(Type)*(*var_num));
        *var_num=*var_num+1;
        return typelist;
    }
}

Type ParamDec(struct gramtree tree){
    Type specifier= Specifier(tree->leftchild);
    char *shadiao;
    specifier=VarDec(tree->leftchild->rightchild,shadiao,specifier);
    //int VarDec(gramtree* node,char* name_,int dimension);
    return specifier;//返回变量类型
}

void CompSt(struct gramtree* node,int flag) //LC DefList StmtList RC  flag:是否是函数
{
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = node->rightchild;  //cur:DefList
    if(flag==0){
        insert_space_unit(0);
        //函数定义时域的插入在FunDecphoning完成
    }
    DefList(cur);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
    //void delete_space_unit(int is_struct,...)
    delete_space_unit(0);
}

void DefList(struct gramtree* node)
{
    if(node==NULL)  //空的产生式
    {
        return;
    }
    struct gramtree* cur= node->leftchild; //Def DefList
    Def(cur);
    cur = cur->rightchild;
    DefList(cur);
}

void Def(struct gramtree* node)  //Specifier DecList SEMI
{
    struct gramtree* cur = node->leftchild;  //cur:Specifier
    Type specifier_tp = Specifier(cur);
    cur = cur->rightchild;
    DecList(cur,specifier_tp);
}


void DecList(struct gramtree* node,Type type_)
{
    struct gramtree* cur = node->leftchild;
    Dec(cur,type_);  
    if(cur->rightchild==NULL)  //Dec
    {
        return;
    }
    else  //Dec COMMA DecList
    {
        cur = cur->rightchild;  //cur:COMMA
        cur = cur->rightchild;  //cur:DecList
        DecList(cur,type_);
    }
    
}

void Dec(struct gramtree* node,Type type_)
{
    struct gramtree* cur = node->leftchild;
    int dimension = 0;
    char* name_;
    dimension = VarDec(cur,name_,dimension);
    if(dimension==0)
    {
        insert_variable_unit_bytype(name_,type_);
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


int main()
{

    return 0;
}
