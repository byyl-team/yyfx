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
        insert_space_unit();  //新建一个结构体域，贾馥榕的代码会改，不传参
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
        insert_variable_unit(name_,specifier_tp);  //变量插入变量表
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

int main()
{

    return 0;
}