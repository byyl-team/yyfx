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
    if(node==NULL){  // �յĲ���ʽ
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
    Type specifier = Specifier(cur);  //name��SpecifierҶ�ڵ��ֵ
    if(specifier==NULL)  //Specifier����
    {
        return;
    }
    //�������ȫ�ֱ���
    if(strcmp(cur->rightchild->name,"ExtDecList")==0)  //Specifier ExtDecList SEMI
    {
        ExtDecList(cur->rightchild,specifier);
    }
    //�������ṹ��
    else if(strcmp(cur->rightchild->name,"SEMI")==0)  //Specifier SEMI
    {
        //������int;float;struct a;struct a{...};û��Ҫ��ʶ����Щ����
        return;
    }
    //������庯��
    else if(strcmp(cur->rightchild->name,"FunDec")==0)
    {
        if(strcmp(cur->rightchild->rightchild->name,"SEMI")==0){
            //����������
            FunDec(specifier,cur->rightchild,0);
            return;
        }
        //�����Ķ���
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
        insert_space_unit(0);  //�½�һ���ṹ���򣬼���ŵĴ����ģ�������
        char* struct_name;
        OptTag(cur->rightchild,struct_name);  //�õ��ṹ�������
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur,1);
        Type struct_tp = delete_struct_space(struct_name);
        return struct_tp;  //return �½���Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name;
        Tag(cur->rightchild,struct_name);
        Type struct_tp = ifExist(struct_name);
        if(struct_tp==NULL)   //�����ֵĽṹ�岻����
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
        //�ṹ�������ǿյ�
        if(name_!=NULL) free(name_);
        return;
    }
    struct gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(name_!=NULL) free(name_);   //���ͷ�������
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
        if(name_!=NULL) free(name_);  //���ͷ�������
        name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->IDTYPE));  
        strcpy(name_,node->leftchild->IDTYPE);  //���ָ��Ƹ�name_
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
        insert_variable_unit_bytype(name_,specifier_tp);  //�������������
    }
    else
    {
        insert_array_unit(name_, dimension, specifier_tp); //������������
    }
    //VarDec
    if(cur->rightchild==NULL)
    {
        return;
    }
    //VarDec COMMA ExtDecList   
    cur = cur->rightchild;  //cur:COMMA
    cur = cur->rightchild;  //cur:ExtDecList
    ExtDecList(cur,specifier_tp);  //һֱ��ExtDecList����ʽ��VarDecΪֹ
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
    else  //δ����Ĳ���ʽ
    {
        printf("undeclared VarDec productor!\n");
        return 0;
    }
    
}

void FunDec(Type return_type,struct gramtree* tree,int is_defining){
    //����������ű�
    if(is_defining==0){
        //��������
        if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
            //�޲���
            char *func_name=tree->leftchild->name;
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
            return;
        }
        //�в���
        char *func_name=tree->leftchild->name;
        Type* Typelist;//���⣺�Ƿ���Ҫ��ǰ�����ڴ棿���������ں����з����ڴ�Ҳ�У�Ŀǰ���ں����з����
        struct gramtree* varlist=tree->leftchild->rightchild->rightchild;
        int *var_num=(int*)malloc(sizeof(int));
        *var_num=0;
        Typelist=VarList(varlist,var_num);
        if(Typelist==NULL){
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
        }
        //�����Ӧ�ĺ���
        insert_func_unit_bytype(func_name,return_type,*var_num,Typelist,0);
        return;
    }
    if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
        //�޲���
        char *func_name=tree->leftchild->name;
        insert_space_unit(1,func_name);//����һ����
        insert_func_unit_bytype(func_name,return_type,0,NULL,1);
        return;
    }
    //�в���
    char *func_name=tree->leftchild->name;
    insert_space_unit(1,func_name);//����һ���� ok
    Type* Typelist;//���⣺�Ƿ���Ҫ��ǰ�����ڴ棿���������ں����з����ڴ�Ҳ�У�Ŀǰ���ں����з����
    struct gramtree* varlist=tree->leftchild->rightchild->rightchild;
    int *var_num=(int*)malloc(sizeof(int));
    *var_num=0;
    Typelist=VarList(varlist,var_num);
    //�����Ӧ�ĺ���
    insert_func_unit_bytype(func_name,return_type,*var_num,Typelist,1);
}
Type* VarList(struct gramtree* tree, int *var_num){
    //�ʼ������0
    //һ�����ӣ�ParamDec��
    //*var_num=*var_num+1;
    /*
     �������������ʱ��һ��Ҫע��Ҫ��var_numָ����䶯̬�ڴ�ռ�
     ��һ�ε������������ʱ�򴫽����Ĳ�����0
     ���ý���֮��ͱ�ɷ����õ�����ֵ��
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
    //�������(ParamDec COMMA VarList)
    else{
        Type first_para_type=ParamDec(tree->leftchild);//ParamDec���� P
        if(first_para_type=NULL){
            return VarList(tree->leftchild->rightchild->rightchild,var_num);
        }
        Type *typelist=VarList(tree->leftchild->rightchild->rightchild,var_num);//VarList���ͱ�
        //��ʱ��var_num��VarList�еõ���
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


void CompSt(struct gramtree* node,int flag) //LC DefList StmtList RC  flag:�Ƿ��Ǻ���
{
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = node->rightchild;  //cur:DefList
    if(flag==0){
        insert_space_unit(0);
        //��������ʱ��Ĳ�����FunDecphoning���
    }
    DefList(cur,0);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
    //void delete_space_unit(int is_struct,...)
    delete_space_unit(0);
}

void DefList(struct gramtree* node,int flag)
{
    if(node==NULL)  //�յĲ���ʽ
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
    if(specifier_tp==NULL)   //˵��specifier������
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


void Dec(struct gramtree* node,Type type_,int flag)  //flag 1���ṹ��  0������
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
            printf("Error type 15 at Line %d: Illegal initialization in a structure.\n");  //��������15���ṹ�嶨��ʱ������г�ʼ��
            return;
        }
        cur = cur->rightchild;  //cur:ASSIGNOP
        cur = cur->rightchild;  //cur:Exp
        Type right_tp = Exp(cur);    //�����õ�˼�����Ԫ�ĺ���
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
    if(node==NULL)  //�ղ���ʽ
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
        CompSt(cur,0);  //���Ǻ�����ok
    }
    else if(strcmp(cur->name,"RETURN")==0)  //RETURN Exp SEMI
    {
        cur = cur->rightchild;  //Exp
        Type return_tp = Exp(cur);

        
    }
}



