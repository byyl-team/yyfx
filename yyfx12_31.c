//
//  yffx_print.c
//  
//
//  Created by ����� on 2020/12/28.
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
Type Exp(struct gramtree* node);
Type Args(struct gramtree* node,int count,Type* type_list);

void Program(struct gramtree* node)
{
    init();
    ExtDefList(node->leftchild);
    printf("Program\n");
}
void ExtDefList(struct gramtree* node)   //ExtDef
{
    printf("ExtDefList\n");
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
    printf("ExtDef\n");
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
        insert_space_unit(0);  //�½�һ���ṹ���򣬼���ŵĴ����ģ�������
        char* struct_name=NULL;
        OptTag(cur->rightchild,&struct_name);  //�õ��ṹ�������
        cur = cur->rightchild;  //cur:OptTag
        cur = cur->rightchild;  //cur:LC
        cur = cur->rightchild;  //cur:DefList
        DefList(cur,1);
	//printf("return from DefList\n");
        Type struct_tp = delete_struct_space(struct_name);
	//printf("return from delete struct space\n");
        return struct_tp;  //return �½���Type
    }
    else if(strcmp(cur->rightchild->name,"Tag")==0)   //STRUCT Tag
    {
        char* struct_name=NULL;
        Tag(cur->rightchild,&struct_name);
        Type struct_tp = ifExist(struct_name);
        if(struct_tp==NULL)   //�����ֵĽṹ�岻����
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
        //�ṹ�������ǿյ�
        if(*name_!=NULL) free(*name_);
        return;
    }
    struct gramtree* cur = node->leftchild;
    if(cur!=NULL)   //ID
    {
        if(*name_!=NULL) free(*name_);   //���ͷ�������
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
        if(*name_!=NULL) free(*name_);  //���ͷ�������
        *name_ = (char*)malloc(sizeof(char)*strlen(node->leftchild->content));
        strcpy(*name_,node->leftchild->content);  //���ָ��Ƹ�name_
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
        insert_variable_unit_bytype(name_,specifier_tp);  //�������������
    }
    else
    {
	printf("going to insert an array.name:%s,dimension:%d",name_,dimension);
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
    else  //δ����Ĳ���ʽ
    {
        printf("undeclared VarDec productor!\n");
        return 0;
    }
    
}

void FunDec(Type return_type,struct gramtree* tree,int is_defining){
    printf("FunDec\n");
    //����������ű�
    if(is_defining==0){
        //��������
        if(tree->leftchild->rightchild->rightchild->rightchild==NULL){
            //�޲���
            char *func_name=tree->leftchild->content;
	    //printf("lalala %s\n",func_name);
            insert_func_unit_bytype(func_name,return_type,0,NULL,0);
            return;
        }
        //�в���
        char *func_name=tree->leftchild->content;
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
        char *func_name=tree->leftchild->content;
        insert_space_unit(1,func_name);//����һ����
        insert_func_unit_bytype(func_name,return_type,0,NULL,1);
        return;
    }
    //�в���
    char *func_name=tree->leftchild->content;
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
    printf("VarList\n");
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


void CompSt(struct gramtree* node,int flag) //LC DefList StmtList RC  flag:�Ƿ��Ǻ���
{
    printf("in CompSt name is %s\n",node->name);
    //printf("CompSt\n");
    struct gramtree* cur = node->leftchild;  //cur:LC
    cur = cur->rightchild;  //cur:DefList
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
    printf("DefList name:%s\n",node->name);
    if(node==NULL)  //�յĲ���ʽ
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
    if(specifier_tp==NULL)   //˵��specifier������
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


void Dec(struct gramtree* node,Type type_,int flag)  //flag 1���ṹ��  0������
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
    printf("StmtList\n");
    if(node==NULL)  //�ղ���ʽ
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
    printf("Stmt\n");
    struct gramtree* cur = node->leftchild;
    if(strcmp(cur->name,"Exp")==0)  //Exp SEMI
    {
        Exp(cur);//1
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

Type Exp(struct gramtree* node,int* flag)  //�Ƿ������Ϊ��ֵ
{
    printf("Exp name:%s\n",node->name);
    struct gramtree* cur=node->leftchild;
    //printf("Exp->leftchild:  %s\n",cur->name);
    if(strcmp(cur->name,"INT")==0||strcmp(cur->name,"INT16")==0||strcmp(cur->name,"INT8")==0){
    	if(cur->rightchild==NULL)  //INT
        {
            *flag = 0;  //������Ϊ��ֵ
            return newBasic(0);
        }
         /*
    	else if(cur->rightchild->name=="ASSIGNOP"){//��鸳ֵ����߳�����ֻ����ֵ�ı��ʽError type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}*/
    }
    else if(strcmp(cur->name,"FLOAT")==0){  //FLOAT
    	if(cur->rightchild==NULL) 
        {
            *flag = 0;
            return newBasic(1);
        }
        /*
    	else if(cur->rightchild->name=="ASSIGNOP"){//��鸳ֵ����߳�����ֻ����ֵ�ı��ʽError type 6
            printf("Error type 6 at Line %d:the left-hand side of an assignment must be a variable.\n ",cur->lineno);
            return NULL;
    	}*/
    }

    else if(strcmp(cur->name,"ID")==0){
	//printf("cur->name=ID\n");
    	if(cur->rightchild==NULL){  //ID
		//printf("Exp->ID\n");
    		if(search_variable(cur->content)!=NULL) return search_variable_type(cur->content);//��ȷ��
    		else printf("Error type 1 at Line %d:undefined variable %s\n ",cur->lineno,cur->content);
    		
return NULL;
    	}
    	else if(strcmp(cur->rightchild->name,"LP")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Args")==0)  //ID LP Args RP
    		{
    			if(strcmp(cur->rightchild->rightchild->rightchild->name,"RP")==0){
                    int Count=0;
                    Type * Type_list;   //didn't malloc!!!
                    Type_list = (*Type_list)malloc(20*sizeof(Type));
                    Args(cur->rightchild->rightchild,Count,Type_list);
    				if(search_func(cur->content)!=NULL){
                        int common=0,i;
                        for(i=0;i<Count;i++){
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
    				else//��������:����Ƿ�δ����͵���Error type 2
    				{
    				    //Type t=Exp(cur);
						if(search_variable(cur->content)!=NULL)//error 11 ����ͨ����ʹ�ú������ò�������.....��
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
    			if(search_func(cur->content)!=NULL) 
                {
                    if(search_func(cur->content)->param_size==0)  //Ӧ���жϺ��������Ƿ�Ϊ��
                    {
                        return search_variable_type(cur->content);
                    }
                    else
                    {
                        printf("Error type 9 at Line %d:Function %s is not applicable for arguments ",cur->lineno,cur->content);
                    }
                    
                }        
    			else//��������:����Ƿ�δ����͵���Error type 2
    			{
					if(search_variable(cur->content)!=NULL)//error 11 ����ͨ����ʹ�ú������ò�������.....��
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
    		
    		return Exp(cur->rightchild);
    	}
    }

    else if(strcmp(cur->name,"MINUS")==0){
    	if(strcmp(cur->rightchild->name,"Exp")==0){
    		
    		return Exp(cur->rightchild);
    	}
    }

    else if(strcmp(cur->name,"Exp")==0){
    	if(strcmp(cur->rightchild->name,"ASSIGNOP")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
                Type l_tp = Exp(cur);
                Type r_tp = Exp(cur->rightchild->rightchild);
    			if(isEqual(l_tp,r_tp)) return l_tp;  //Ӧ�������е�
    			else//���Ⱥ���������ƥ���ж�Error type 5
    			{
    				printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",cur->lineno);
    				return l_tp;  //�ǲ��Ǹ�Ϊl_tp������һ�㣿
    			}
    		}
    	}
    	else if(strcmp(cur->rightchild->name,"AND")==0||strcmp(cur->rightchild->name,"OR")==0){
            Type l_tp = Exp(cur);
            Type r_tp = Exp(cur->rightchild->rightchild);  
            if(judge_type(l_tp)!=0 || judge_type(r_tp)!=0)
            {
                printf("Error type 7 at Line %d:Type mismatched for operands.\n",cur->lineno);
            } 
            return newBasic(0);
    		//if(strcmp(cur->rightchild->rightchild->name,"Exp")==0)return Exp(cur);
    	}
        else if(strcmp(cur->rightchild->name,"RELOP")==0)  //���߲�����Ҫ����ô��������
        {
            Type l_tp = Exp(cur);
            Type r_tp = Exp(cur->rightchild->rightchild); 
            if(isEqual(l_tp,r_tp)) return l_tp;  
            else//���Ⱥ���������ƥ���ж�Error type 5
            {
                printf("Error type 5 at Line %d:Type mismatched for assignment.\n ",cur->lineno);
                return l_tp;  //�ǲ��Ǹ�Ϊl_tp������һ�㣿
            }
        }
    	else if(strcmp(cur->rightchild->name,"PLUS")==0||strcmp(cur->rightchild->name,"MINUS")==0||strcmp(cur->rightchild->name,"STAR")==0||strcmp(cur->rightchild->name,"DIV")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
                Type l_tp = Exp(cur);
                Type r_tp = Exp(cur->rightchild->rightchild); 
                if(isEqual(l_tp,r_tp))
                {
                    return l_tp; 
                } 
    			else//����������������Error type 7
    			{
    				printf("Error type 7 at Line %d:Type mismatched for operand.\n ",cur->lineno);
    				return l_tp;  //������ֵ��
    			}
    		}

    	}


    	else if(strcmp(cur->rightchild->name,"LB")==0){
    		if(strcmp(cur->rightchild->rightchild->name,"Exp")==0){
    			if(strcmp(cur->rightchild->rightchild->rightchild->name,"RB")==0){
                    ///error 10   error 12
                    Type t=Exp(cur);
                    if(t!=NULL){
                        if(judge_type(t)==2){///judge_type:0int��1float,2���飬3�ṹ��
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

    	else if(strcmp(cur->rightchild->name,"DOT")==0){
            if(strcmp(cur->rightchild->rightchild->name,"ID")==0){
                Type t=Exp(cur);
                if(t!=NULL){
                    if(judge_type(t)==3){
                        if(structMem(t,1,cur->content)==NULL){///error 14���ʽṹ����δ���������
                            printf("Error type 14 at Line %d:Non-existent field \"%s\"\n",cur->lineno,cur->content);
                            return NULL;
                        }
                        else return structMem(t,1,cur->content);
                    }
                    else{///error 13 �Էǽṹ���ͱ���ʹ��DOT
                        printf("Error type 13 at Line %d:Illegal use of\".\"\n ",cur->lineno);
                        return NULL;
                    }
                }
            }
    	}

    }
    else(strcmp(cur->name,"LP")==0)
    {
        return Exp(cur->right);
    }
}
Type Args(struct gramtree* node,int count,Type* type_list){
    struct gramtree* cur=node->leftchild;
    if(strcmp(cur->name,"Exp")==0){
        type_list[count]=Exp(cur);
        count++;
        if(cur->rightchild==NULL){
            return NULL;
        }
        else if(strcmp(cur->rightchild->name,"COMMA")==0){
            if(strcmp(cur->rightchild->rightchild->name,"Args")==0){
                Args(cur->rightchild->rightchild,count,type_list);
            }
        }
    }
}


