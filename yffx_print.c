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
Type Exp(struct gramtree* node);
Type Args(struct gramtree* node,int count,Type* type_list);

void Program(struct gramtree* node)
{
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
        char* struct_name=NULL;
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
    printf("OptTag\n");
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
        name_= (char*)malloc(sizeof(char)*strlen(cur->content));
        strcpy(name_,cur->content);
    }
}

void Tag(struct gramtree* node,char* name_)
{
    printf("Tag\n");
    if(node==NULL)
    {
        return;
    }
    if(node->leftchild!=NULL)  //ID
    {
        if(name_!=NULL) free(name_);  //先释放再申请
        name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->content));
        strcpy(name_,node->leftchild->content);  //名字复制给name_
    }
}

void ExtDecList(struct gramtree* node,Type specifier_tp)
{
    printf("ExtDecList\n");
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
    printf("VarDec\n");
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"ID")==0)   //ID
    {
        if(name_!=NULL) free(name_);
        name_ = (char*)malloc(sizeof(char)*strlen(node->content));
        strcpy(name_,node->content);
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
    printf("ParamDec\n");
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
    printf("CompSt\n");
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
    printf("DefList\n");
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
    printf("Def\n");
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
    printf("StmtList\n");
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
    printf("Stmt\n");
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


Type Exp(struct gramtree* node)
{
    struct gramtree* cur=node->leftchild;
    if(cur->name=="INT"){
    	if(cur->rightchild==NULL) return newBasic(0);
    	else if(cur->rightchild->name=="ASSIGNOP"){//检查赋值号左边出现中只有右值的表达式Error type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}
    }
    else if(cur->name=="FLOAT"){
    	if(cur->rightchild==NULL) return newBasic(1);
    	else if(cur->rightchild->name=="ASSIGNOP"){//检查赋值号左边出现中只有右值的表达式Error type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}
    }

    else if(cur->name=="ID"){
    	if(cur->rightchild==NULL){
    		if(search_repeat(cur->content)) return search_variable_type(cur->content);//不确定
    		else printf("Error type 1 at Line %d:undefined variable %s\n ",cur->lineno,cur->content);
    		return NULL;
    	}
    	else if(cur->rightchild->name=="LP"){
    		if(cur->rightchild->rightchild->name=="Args")
    		{
    			if(cur->rightchild->rightchild->rightchild->name=="RP"){
                    int Count=0;
                    Type * Type_list;
                    Args(cur->rightchild->rightchild,Count,Type_list);
    				if(search_func(cur->content)!=NULL){
    				int common=0;
    				for(int i=0;i<Count;i++){
    					if(isEqual(Type_list[i],search_func(cur->content)->param_type[i])) common++;
				}
                            if(Count==search_func(cur->content)->param_size&&common==Count-1) return search_variable_type(cur->content);
                            else{
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
    				    Type t=Exp(cur);
						if(judge_type(t)==0||judge_type(t)==1||judge_type(t)==2||judge_type(t)==3)//error 11 对普通对量使用函数调用操作符（.....）
                        	{
                            printf("Error type 11 at Line %d:%s is not a function\n ",cur->lineno,cur->content);
                            return NULL;
                        }
                        printf("Error type 2 at Line %d:undefined Function %s\n ",cur->lineno,cur->content);
                        return NULL;

    				}
    			}
    		}
    		else if(cur->rightchild->rightchild->name=="RP"){
    			if(search_func(cur->content)!=NULL) return search_variable_type(cur->content);
    			else//函数引用:检查是否未定义就调用Error type 2
    			{
    			    Type t=Exp(cur);
					if(judge_type(t)==0||judge_type(t)==1||judge_type(t)==2||judge_type(t)==3)//error 11 对普通对量使用函数调用操作符（.....）
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

    else if(cur->name=="NOT"){
    	if(cur->rightchild->name=="Exp"){
    		
    		return Exp(cur->rightchild);
    	}
    }

    else if(cur->name=="MINUS"){
    	if(cur->rightchild->name=="Exp"){
    		
    		return Exp(cur->rightchild);
    	}
    }

    else if(cur->name=="Exp"){
    	if(cur->rightchild->name=="ASSIGNOP"){
    		if(cur->rightchild->rightchild->name=="Exp"){
    			if(Exp(cur)==Exp(cur->rightchild->rightchild)) return Exp(cur);
    			else//检查等号左右类型匹配判断Error type 5
    			{
    				printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",cur->lineno);
    				return NULL;
    			}
    		}
    	}
    	else if(cur->rightchild->name=="AND"||cur->rightchild->name=="OR"||cur->rightchild->name=="RELOP"){
    		if(cur->rightchild->rightchild->name=="Exp")return Exp(cur);
    	}
    	else if(cur->rightchild->name=="PLUS"||cur->rightchild->name=="MINUS"||cur->rightchild->name=="STAR"||cur->rightchild->name=="DIV"){
    		if(cur->rightchild->rightchild->name=="Exp"){
    			if(judge_type(Exp(cur))==0&&judge_type(Exp(cur->rightchild->rightchild))==0) return newBasic(0);
    			else if(judge_type(Exp(cur))==1&&judge_type(Exp(cur->rightchild->rightchild))==1) return newBasic(1);
    			else//检查操作符左右类型Error type 7
    			{
    				printf("Error type 7 at Line %d:Type mismatched for operand.\n ",cur->lineno);
    				return NULL;
    			}
    		}

    	}


    	else if(cur->rightchild->name=="LB"){
    		if(cur->rightchild->rightchild->name=="Exp"){
    			if(cur->rightchild->rightchild->rightchild->name=="RB"){
                    ///error 10   error 12
                    Type t=Exp(cur);
                    if(t!=NULL){
                        if(judge_type(t)==2){///judge_type:0int，1float,2数组，3结构体
                            if(judge_type(Exp(cur->rightchild->rightchild))!=0){
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

    			}
    		}
    	}

    	else if(cur->rightchild->name=="DOT"){
            if(cur->rightchild->rightchild->name=="ID"){
                Type t=Exp(cur);
                if(t!=NULL){
                    if(judge_type(t)==3){
                        if(structMem(t,1,cur->content)==NULL){///error 14访问结构体中未定义过的域
                            printf("Error type 14 at Line %d:Non-existent field \"%s\"\n",cur->lineno,cur->content);
                            return NULL;
                        }
                        else return structMem(t,1,cur->content);
                    }
                    else{///error 13 对非结构体型变量使用DOT
                        printf("Error type 13 at Line %d:Illegal use of\".\"\n ",cur->lineno);
                        return NULL;
                    }
                }
            }
    	}

    }
}
Type Args(struct gramtree* node,int count,Type* type_list){
    struct gramtree* cur=node->leftchild;
    if(cur->name="Exp"){
        type_list[count]=Exp(cur);
        count++;
        if(cur->rightchild==NULL){
            return NULL;
        }
        else if(cur->rightchild->name=="COMMA"){
            if(cur->rightchild->rightchild->name=="Args"){
                Args(cur->rightchild->rightchild,count,type_list);
            }
        }
    }
}


