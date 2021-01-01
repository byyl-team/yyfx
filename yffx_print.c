//
//  yffx_print.c
//  
//
//  Created by 贾馥榕 on 2020/12/28.
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"class.h"
#include"gramtree.h"
#include"variabletable.h"
#define NULL 0
void Program(struct gramtree* node);
void ExtDefList(struct gramtree* node);
void ExtDef(struct gramtree* node);
Type Specifier(struct gramtree* node);
Type StructSpecifier(struct gramtree* node);
void Tag(struct gramtree* node,char** name_);
void ExtDecList(struct gramtree* node,Type specifier_tp);
int VarDec(struct gramtree* node,char** name_,int dimension);
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
void OptTag(struct gramtree* node,char** name_);
Type Exp(struct gramtree* node,int* flag);
Type Args(struct gramtree* node,int* count,Type* type_list);

void Program(struct gramtree* node)
{
    init();
    ExtDefList(node->leftchild);
    printf("Program\n");
}
void ExtDefList(struct gramtree* node)   //ExtDef
{
    printf("ExtDefList\n");
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
    printf("ExtDef\n");
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
    printf("Specifier\n");
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"TYPE")==0)  //TYPE terminal
    {
        if(strcmp(cur->content,"int")==0) //int
        {
            return newBasic(0);
        }
        else if(strcmp(cur->content,"float")==0)  //float
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
    printf("StructSpecifier\n");
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->rightchild->name,"OptTag")==0)  //STRUCT OptTag LC DefList RC
    {
        insert_space_unit(0);  //新建一个结构体域，贾馥榕的代码会改，不传参
        char* struct_name=NULL;
        OptTag(cur->rightchild,&struct_name);  //得到结构体的名字
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur,1);
	//printf("return from DefList\n");
        Type struct_tp = delete_struct_space(struct_name);
	//printf("return from delete struct space\n");
        return struct_tp;  //return 新建的Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name=NULL;
        Tag(cur->rightchild,&struct_name);
        Type struct_tp = ifExist(struct_name);
        if(struct_tp==NULL)   //该名字的结构体不存在
        {
            printf("Error type 17 at Line %d: Undefined structure \"%s\"\n",cur->rightchild->lineno,struct_name);
            return NULL;
        }
        return struct_tp;
    }
}

void OptTag(struct gramtree* node,char** name_)
{
    printf("OptTag\n");
    if(node==NULL)
    {
        //结构体名字是空的
        if(*name_!=NULL) free(*name_);
        return;
    }
    struct gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(*name_!=NULL) free(*name_);   //先释放再申请
        *name_= (char*)malloc(sizeof(char)*strlen(cur->content));
        strcpy(*name_,cur->content);
    }
}

void Tag(struct gramtree* node,char** name_)
{
    printf("Tag\n");
    if(node==NULL)
    {
        return;
    }
    if(node->leftchild!=NULL)  //ID
    {
        if(*name_!=NULL) free(*name_);  //先释放再申请
        *name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->content));
        strcpy(*name_,node->leftchild->content);  //名字复制给name_
    }
}

void ExtDecList(struct gramtree* node,Type specifier_tp)
{
    printf("ExtDecList\n");
    struct gramtree* cur = node->leftchild;
    char* name_ = NULL;
    int dimension = 0;
    dimension = VarDec(cur,&name_,dimension);
    printf("ExtDecList name:%s\n",name_);
    if(dimension==0)
    {
        insert_variable_unit_bytype(name_,specifier_tp);  //变量插入变量表
    }
    else
    {
	printf("going to insert an array.name:%s,dimension:%d",name_,dimension);
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

int VarDec(struct gramtree* node,char** name_,int dimension)
{
    printf("%s \n",node->name);
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"ID")==0)   //ID
    {
	//printf("entered if ID\n");
        if(*name_!=NULL) free(*name_);
	//printf("feeed\n");
        *name_ = (char*)malloc(sizeof(char)*strlen(cur->content));
	//printf("malloced\n");
        strcpy(*name_,cur->content);
//printf("strcpyedd name:%s\n",*name_);
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
    printf("FunDec\n");
    //函数加入符号表
    if(is_defining==0){
        //函数声明
        if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
            //无参数
            char *func_name=tree->leftchild->content;
	    //printf("lalala %s\n",func_name);
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
            return;
        }
        //有参数
        char *func_name=tree->leftchild->content;
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
        char *func_name=tree->leftchild->content;
printf("fun_name: %s\n",func_name);        

        insert_func_unit_bytype(func_name,return_type,0,NULL,1);
extern struct node* func_top;
        printf("top fun name=%s\n",func_top->vi_name);    
        insert_space_unit(1,func_name);//加入一个域    
return;
    }
    //有参数
    char *func_name=tree->leftchild->content;
    
    Type* Typelist;//问题：是否需要提前分配内存？？？或者在函数中分配内存也行，目前是在函数中分配的
    struct gramtree* varlist=tree->leftchild->rightchild->rightchild;
    int *var_num=(int*)malloc(sizeof(int));
    *var_num=0;
    Typelist=VarList(varlist,var_num);
    printf("varList var_num:%d\n",*var_num);
    //定义对应的函数
    int pi=0;
    for(pi=0;pi<*var_num;pi++){
	if(Typelist[pi]==NULL){printf("Typelist[%d] is NULL\n",pi);}
}
    insert_func_unit_bytype(func_name,return_type,*var_num,Typelist,1);
    extern struct node* func_top;
    printf("func_top name:%s\n",func_top->vi_name);
    insert_space_unit(1,func_name);//加入一个域 ok
}
Type* VarList(struct gramtree* tree, int *var_num){
    printf("VarList\n");
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
	if(typelist[0]==NULL){
            printf("VarList->ParamDec: typelist=NULL\n");
        }
        //memcpy(typelist,&type,sizeof(Type));//ok
        return typelist;
    }
    //多个儿子(ParamDec COMMA VarList)
else{
        Type first_para_type=ParamDec(tree->leftchild);//ParamDec类型 P
        if(first_para_type==NULL){
            printf("first para_type is NULL\n");
            return VarList(tree->leftchild->rightchild->rightchild,var_num);
        }
        Type *typelist=VarList(tree->leftchild->rightchild->rightchild,var_num);//VarList类型表
        //此时的var_num是VarList中得到的
        if(typelist==NULL){
            printf("typelist  is NULL\n");
        }
	printf("in varList: var_num=%d\n",*var_num);
        Type *curlist=(Type*)malloc(sizeof(Type)*((*var_num)+1));
	printf("size of Type =%d\n",sizeof(Type));
	printf("size of curlist=%d\n",sizeof(curlist));
	//memcpy(curlist,&first_para_type,sizeof(Type));
        curlist[0]=first_para_type;
	if(first_para_type==NULL){
            printf("first para_type is NULL\n");
        }
	printf("first_para_type=%d\n",first_para_type->kind);
        if(curlist[0]==NULL){
            printf("curlist[0] is NULL\n");
        }
	int pi=0;
	printf("curlist[0] type=%d\n",curlist[0]->kind);
	for (pi=0; pi<*var_num;pi++) {
            curlist[pi+1]=typelist[pi];
        }       
// memcpy(curlist+sizeof(Type),typelist,sizeof(Type)*(*var_num));
	//int pi;
	for(pi=0;pi<*var_num;pi++){
            if(typelist[pi]==NULL){
                printf("typelist[%d] is NULL\n",pi);
            }
        }
        for (pi=0; pi<*var_num; pi++) {
            if(curlist[pi+1]==NULL){
                printf("curlist[%d] is NULL\n",pi+1);
            }
		printf("curlist[%d] type=%d\n",pi+1,curlist[pi+1]->kind);
        }        
*var_num=*var_num+1;
        if (curlist==NULL) {
            printf("curlist is NULL\n");
        }
        return curlist;
    }
}
Type ParamDec(struct gramtree* tree){
    printf("ParamDec\n");
    Type specifier= Specifier(tree->leftchild);
    
    if(specifier==NULL){
        
        return NULL;
        
    }    
    char *shadiao = NULL;
    
    int dem=VarDec(tree->leftchild->rightchild,&shadiao,0);
    
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
    printf("in CompSt name is %s\n",node->name);
    //printf("CompSt\n");
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = cur->rightchild;  //cur:DefList
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
    printf("DefList name:%s\n",node->name);
    if(node==NULL)  //空的产生式
    {
	//printf("node==NULL\n");
        return;
    }
    struct gramtree* cur= node->leftchild; //Def DefList
    if(cur==NULL) return;
    Def(cur,flag);
    cur = cur->rightchild;
    DefList(cur,flag);
}

void Def(struct gramtree* node,int flag)  //Specifier DecList SEMI
{
    printf("Def name:%s\n",node->name);
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
    printf("DecList\n");
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
    printf("Dec\n");
    struct gramtree* cur = node->leftchild;
    int dimension = 0;
    char* name_ = NULL;
    dimension = VarDec(cur,&name_,dimension);
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
	int flag = 1;
        Type right_tp = Exp(cur,&flag);    //这里用到思宇和译元的函数
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
    printf("StmtList\n");
    if(node==NULL)  //空产生式
    {
        return;
    }
    struct gramtree* cur = node->leftchild;  //Stmt StmtList
    if(cur==NULL) return;
    Stmt(cur);
    cur = cur->rightchild;  //cur:StmtList
    StmtList(cur);
}


void Stmt(struct gramtree* node)
{
    int flag=1;
    printf("Stmt\n");
    struct gramtree* cur = node->leftchild;
    printf("in Stmt: Stmt->%s\n",cur->name);
    if(strcmp(cur->name,"Exp")==0)  //Exp SEMI
    {
	flag = 1;
        Exp(cur,&flag);//1
    }
    else if(strcmp(cur->name,"Compst")==0)  //CompSt
    {
printf("Stmt->Compst\n");        
CompSt(cur,0);  //不是函数！ok
    }
//RETURN Exp SEMI
    if(strcmp(node->leftchild->name,"RETURN")==0){
        //查找当前函数
        struct space_unit* space=find_nearest_func_space();
        //根据func_name查找函数
        struct node* func_node=search_func(space->space_name);
	flag = 1;
        if(!isEqual(Exp(node->leftchild->rightchild,&flag),func_node->type)){
            printf("error8\n");
        }
        return;
    }
    //IF LP Exp RP Stmt
    if(strcmp(node->leftchild->name,"IF")==0 && strcmp(node->leftchild->rightchild->rightchild->rightchild->rightchild->name,"Stmt")==0){
        //因为【假设】中说明了Exp一定是int类型的，所以其实不需要判断此处Exp的类型，但是可能要对Exp做一些其他的分析，比如说Exp可能是一个表达式，那么这个表达式的操作数是否符合要求，因此有必要调用Exp函数进行分析
printf("IF LP Exp RP Stmt\n");       
printf("LP\n");
 flag = 1;
 Type exp_type=Exp(node->leftchild->rightchild->rightchild,&flag);
printf("RP\n");        
Stmt(node->leftchild->rightchild->rightchild->rightchild->rightchild);
return;
    }
    //IF LP EXP RP STMT ELSE STMT
    if(strcmp(node->leftchild->name,"IF")==0){
	flag = 1;
        Type exp_type=Exp(node->leftchild->rightchild->rightchild,&flag);
        Stmt(node->leftchild->rightchild->rightchild->rightchild->rightchild);
        Stmt(node->leftchild->rightchild->rightchild->rightchild->rightchild->rightchild->rightchild);
        return;
    }
    //WHILE LP EXP RP STMT
    if(strcmp(node->leftchild->name,"WHILE")==0){
	flag = 1;
        Type exp_type=Exp(node->leftchild->rightchild->rightchild,&flag);
        Stmt(node->leftchild->rightchild->rightchild->rightchild->rightchild);
        return;
    }
}

Type Exp(struct gramtree* node,int* flag)  //是否可以作为左值
{
    printf("Exp namee:%s\n",node->name);
    struct gramtree* cur=node->leftchild;
    //printf("Exp->leftchild:  %s\n",cur->name);
    if(strcmp(cur->name,"INT")==0||strcmp(cur->name,"INT16")==0||strcmp(cur->name,"INT8")==0){
    	if(cur->rightchild==NULL)  //INT
        {
            *flag = (*flag)&0;  //不能作为左值
            return newBasic(0);
        }
         /*
    	else if(cur->rightchild->name=="ASSIGNOP"){//检查赋值号左边出现中只有右值的表达式Error type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}*/
    }
    else if(strcmp(cur->name,"FLOAT")==0){  //FLOAT
    	if(cur->rightchild==NULL) 
        {
            *flag = (*flag)&0;
            return newBasic(1);
        }
        /*
    	else if(cur->rightchild->name=="ASSIGNOP"){//检查赋值号左边出现中只有右值的表达式Error type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}*/
    }

    else if(strcmp(cur->name,"ID")==0){
	//printf("cur->name=ID\n");
    	if(cur->rightchild==NULL){  //ID
		//printf("Exp->ID\n");
		*flag = (*flag)&1;
    		if(search_variable(cur->content)!=NULL)
		{
		    return search_variable_type(cur->content);//不确定
		}
    		else printf("Error type 1 at Line %d:undefined variable %s\n ",cur->lineno,cur->content);
    		
return NULL;
    	}
    	else if(strcmp(cur->rightchild->name,"LP")==0){
		*flag = (*flag)&0;
    		if(strcmp(cur->rightchild->rightchild->name,"Args")==0)  //ID LP Args RP
    		{
    			if(strcmp(cur->rightchild->rightchild->rightchild->name,"RP")==0){
                    int Count=0;
                    Type * Type_list;   //didn't malloc!!!
                    Type_list = (Type*)malloc(20*sizeof(Type));
                    Args(cur->rightchild->rightchild,&Count,Type_list);
			        printf("going to search_func in ID LP Args RP\n");
    				if(search_func(cur->content)!=NULL){
                        int common=0,i;
			printf("Count:%d. origin params:%d\n",Count,search_func(cur->content)->param_size);
                        for(i=0;i<Count;i++){
			    if(search_func(cur->content)->param_type[i]==NULL) printf("func param[%d] is NULL\n",i);
			    printf("i:%d\n",i);
				
                            if(isEqual(Type_list[i],search_func(cur->content)->param_type[i])) common++;printf("i:%d\n",i);
                        }
			printf("xunhuan ended.common:%d origin params:%d\n",common,search_func(cur->content)->param_size);
                        if(Count==(search_func(cur->content)->param_size)&&common==Count)
			{
			    return search_variable_type(cur->content);
			}
                        else{
				printf("Arg unfit. common num:%d func given %d params. origin func has %d params.\n",common,Count,search_func(cur->content)->param_size);
                                printf("Error type 9 at Line %d:Function %s is not applicable for arguments ",cur->lineno,cur->content);
                                //printf("\"(");
                                //for(int i=0;i<Count-1;i++){
                                    //printf("%s,",Type_list[i]);
                                //}
                                //printf("%s)\"",Type_list[Count-1]);
                                return NULL;
                        }

    				}
    				else//函数引用:检查是否未定义就调用Error type 2
    				{
    				    //Type t=Exp(cur);
						if(search_variable(cur->content)!=NULL)//error 11 对普通对量使用函数调用操作符（.....）
                        	{
                            printf("Error type 11 at Line %d:%s is not a function\n ",cur->lineno,cur->content);
                            return NULL;
                        }
                        printf("Error type 2 at Line %d:undefined Function %s\n ",cur->lineno,cur->content);
                        return NULL;

    				}
    			}
    		}
    		else if(strcmp(cur->rightchild->rightchild->name,"RP")==0){  //ID LP RP
			printf("going to search_func:%s\n",cur->content);
    			if(search_func(cur->content)!=NULL) 
                {
		    printf("searched func:%s\n",cur->content);
                    if(search_func(cur->content)->param_size==0)  //应该判断函数参数是否为空
                    {
                        return search_variable_type(cur->content);
                    }
                    else
                    {
                        printf("Error type 9 at Line %d:Function %s is not applicable for arguments ",cur->lineno,cur->content);
                    }
                    
                }        
    			else//函数引用:检查是否未定义就调用Error type 2
    			{
					//printf("did not find func:%s\n",cur->content);
					if(search_variable(cur->content)!=NULL)//error 11 对普通对量使用函数调用操作符（.....）
                    {
                            printf("Error type 11 at Line %d:%s is not a function\n ",cur->lineno,cur->content);
                            return NULL;
                    }
    				printf("Error type 2 at Line %d:undefined Function %s\n ",cur->lineno,cur->content);
    				return NULL;
    			}
    		}
    	}
    }

    else if(strcmp(cur->name,"NOT")==0){
    	if(strcmp(cur->rightchild->name,"Exp")==0){
		int tflag = 1;
		Type cur_tp = Exp(cur->rightchild,&tflag);
    		*flag = (*flag)&0;
    		return cur_tp;
    	}
    }

    else if(strcmp(cur->name,"MINUS")==0){
    	if(strcmp(cur->rightchild->name,"Exp")==0){
		int tflag = 1;
		Type cur_tp = Exp(cur->rightchild,&tflag);
    		*flag = (*flag)&0;
    		return cur_tp;
    	}
    }

    else if(strcmp(cur->name,"Exp")==0){
    	if(strcmp(cur->rightchild->name,"ASSIGNOP")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
		int tflag = 1;
                Type l_tp = Exp(cur,&tflag);
		if(tflag!=1)
		{
		    printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
		    *flag = (*flag)&0;
		    return l_tp;
		}
		tflag = 1;
		*flag = (*flag)&0;
                Type r_tp = Exp(cur->rightchild->rightchild,&tflag);
		if(l_tp==NULL || r_tp==NULL) return NULL; 
    			if(isEqual(l_tp,r_tp))
			{
			    return l_tp;
			}
    			else//检查等号左右类型匹配判断Error type 5
    			{
    				printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",cur->lineno);
    				return l_tp;  //是不是改为l_tp更合理一点？
    			}
    		}
    	}
    	else if(strcmp(cur->rightchild->name,"AND")==0||strcmp(cur->rightchild->name,"OR")==0){
	    int tflag = 1;
            Type l_tp = Exp(cur,&tflag);
	    tflag = 1;
            Type r_tp = Exp(cur->rightchild->rightchild,&tflag); 
	    *flag = (*flag)&0;
	    if(l_tp==NULL || r_tp==NULL) return NULL; 
            if(judge_type(l_tp)!=0 || judge_type(r_tp)!=0)
            {
                printf("Error type 7 at Line %d:Type mismatched for operands.\n",cur->lineno);
            } 
	    
            return newBasic(0);
    		//if(strcmp(cur->rightchild->rightchild->name,"Exp")==0)return Exp(cur);
    	}
        else if(strcmp(cur->rightchild->name,"RELOP")==0)  //两边操作数要求怎么样？？？
        {
	    int tflag = 1;
            Type l_tp = Exp(cur,&tflag);
	    tflag = 1;
            Type r_tp = Exp(cur->rightchild->rightchild,&tflag); 
	    *flag = (*flag)&0;
	    if(l_tp==NULL || r_tp==NULL) return NULL;
            if(isEqual(l_tp,r_tp)) return l_tp;  
            else//检查等号左右类型匹配判断Error type 5
            {
                printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",cur->lineno);
                return l_tp;  //是不是改为l_tp更合理一点？
            }
        }
    	else if(strcmp(cur->rightchild->name,"PLUS")==0||strcmp(cur->rightchild->name,"MINUS")==0||strcmp(cur->rightchild->name,"STAR")==0||strcmp(cur->rightchild->name,"DIV")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
		int tflag = 1;
                Type l_tp = Exp(cur,&tflag);
		tflag = 1;
                Type r_tp = Exp(cur->rightchild->rightchild,&tflag);
		*flag = (*flag)&0; 
		if(l_tp==NULL || r_tp==NULL) return NULL;
                if(isEqual(l_tp,r_tp))
                {
                    return l_tp; 
                } 
    			else//检查操作符左右类型Error type 7
    			{
    				printf("Error type 7 at Line %d:Type mismatched for operand.\n ",cur->lineno);
    				return l_tp;  //返回左值？
    			}
    		}

    	}


    	else if(strcmp(cur->rightchild->name,"LB")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
    			if(strcmp(cur->rightchild->rightchild->rightchild->name,"RB")==0){
                    ///error 10   error 12
		    int tflag = 1;
                    Type t=Exp(cur,&tflag);
		    *flag = (*flag)&1;
                    if(t!=NULL){
                        if(judge_type(t)==2){///judge_type:0int，1float,2数组，3结构体
			    tflag = 1;
                            if(judge_type(Exp(cur->rightchild->rightchild,&tflag))!=0){
                                printf("Error type 12 at Line %d:%s is not an integer.\n ",cur->lineno,cur->content);
                                return NULL;
                            }
                            else return arrayMem(t,1);
                        }
                        else{
                            printf("Error type 10 at Line %d:%s is not an array.\n ",cur->lineno,cur->content);
                            return NULL;
                        }
                    }
		    else return NULL;

    			}
    		}
    	}

    	else if(strcmp(cur->rightchild->name,"DOT")==0){
            if(strcmp(cur->rightchild->rightchild->name,"ID")==0){
		printf("start Exp DOT ID\n"); 
		int tflag = 1;
                Type t=Exp(cur,&tflag);
		printf("Exp DOT ID got Exp\n");
		*flag = (*flag)&1;
                if(t!=NULL){
		    printf("t!=NULL\n");
                    if(judge_type(t)==3){
			printf("Exp DOT ID Exp type is 3\n");
                        if(structMem(t,1,cur->rightchild->rightchild->content)==NULL){///error 14访问结构体中未定义过的域
                            printf("Error type 14 at Line %d:Non-existent field \"%s\"\n",cur->lineno,cur->content);
                            return NULL;
                        }
                        else
			{
			    printf("found Exp DOT ID\n");
			    return structMem(t,1,cur->rightchild->rightchild->content);
			}
 
                    }
                    else{///error 13 对非结构体型变量使用DOT
                        printf("Error type 13 at Line %d:Illegal use of\".\"\n ",cur->lineno);
                        return NULL;
                    }
                }
		else return NULL;
            }
    	}

    }
    else if(strcmp(cur->name,"LP")==0)
    {
	int tflag = 1;
        return Exp(cur->rightchild,&tflag);
    }
}

Type Args(struct gramtree* node,int* count,Type* type_list){
    printf("here Args\n");
    struct gramtree* cur=node->leftchild;
    if(strcmp(cur->name,"Exp")==0){
	int tflag = 1;
        type_list[*count]=Exp(cur,&tflag);
        printf("Type_list[%d] fuzhile\n",*count);
        *count = (*count)+1;
        if(cur->rightchild==NULL){
	    printf("Args return NULL\n");
            return NULL;
        }
        else if(strcmp(cur->rightchild->name,"COMMA")==0){
            if(strcmp(cur->rightchild->rightchild->name,"Args")==0){
		printf("Args Exp COMMA Args\n");
                Args(cur->rightchild->rightchild,count,type_list);
            }
        }
    }
}


