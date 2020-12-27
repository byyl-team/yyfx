#include<stdio.h>
#include<stdlib.h>


void Program(gramtree* node)
{
    ExtDefList(node->leftchild);
}
void ExtDefList(gramtree* node)   //ExtDef 
{
    if(node==NULL){  // �յĲ���ʽ
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
    Type specifier = Specifier(cur);  //name��SpecifierҶ�ڵ��ֵ
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
        if(strcmp(cur->rightchild->rightchild,"SEMI")==0){
            //����������
            FunDec(specifier,cur->rightchild,0);
            return;
        }
        //�����Ķ���
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
        insert_space_unit(0);  //�½�һ���ṹ���򣬼���ŵĴ����ģ�������
        char* struct_name;
        OptTag(cur->rightchild,struct_name);  //�õ��ṹ�������
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur);
        Type struct_cur = newStructure(struct_name);
        //���ü���ŵĺ�����ɾ���򣬱����ŵ��ṹ��struct_cur��
        //return �½���Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name;
        Tag(cur->rightchild,struct_name);
        Type struct_tp = ifExist(char* name_);
        if(struct_tp==NULL)   //�����ֵĽṹ�岻����
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
        //�ṹ�������ǿյ�
        if(name_!=NULL) free(name_);
        return;
    }
    gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(name_!=NULL) free(name_);   //���ͷ�������
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
        if(name_!=NULL) free(name_);  //���ͷ�������
        name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->IDTYPE));  
        strcpy(name_,node->leftchild->IDTYPE);  //���ָ��Ƹ�name_
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
    else  //δ����Ĳ���ʽ
    {
        printf("undeclared VarDec productor!\n");
        return 0;
    }
    
}
void FunDec(Type return_type,struct gramtree tree,int is_defining){
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
        struct gramtree varlist=tree->leftchild->rightchild->rightchild;
        int *var_num=(char*)malloc(sizeof(int));
        *var_num=0;
        Typelist=VarList(varlist,var_num);
        //�����Ӧ�ĺ���
        insert_func_unit_bytype(func_name,return_type,var_num,Typelist,1);
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
    struct gramtree varlist=tree->leftchild->rightchild->rightchild;
    int *var_num=(char*)malloc(sizeof(int));
    *var_num=0;
    Typelist=VarList(varlist,var_num);
    //�����Ӧ�ĺ���
    insert_func_unit_bytype(func_name,return_type,var_num,Typelist,1);
}
Type* VarList(struct gramtree tree, int *var_num){
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
        Type type=ParamDec(tree->leftchild);
        Type* typelist=(Type*)malloc(sizeof(Type));
        typelist[0]=type;
        //memcpy(typelist,&type,sizeof(Type));//ok
        return typelist;
    }
    //�������(ParamDec COMMA VarList)
    else{
        Type first_para_type=ParamDec(gramtree->leftchild);//ParamDec����
        Type *typelist=VarList(gramtree->leftchild->rightchild->rightchild,var_num);//VarList���ͱ�
        //��ʱ��var_num��VarList�еõ���
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
    return specifier;//���ر�������
}

void CompSt(struct gramtree* node,int flag) //LC DefList StmtList RC  flag:�Ƿ��Ǻ���
{
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = node->rightchild;  //cur:DefList
    if(flag==0){
        insert_space_unit(0);
        //��������ʱ��Ĳ�����FunDecphoning���
    }
    DefList(cur);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
    //void delete_space_unit(int is_struct,...)
    delete_space_unit(0);
}

void DefList(struct gramtree* node)
{
    if(node==NULL)  //�յĲ���ʽ
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


int main()
{

    return 0;
}
