#include<stdio.h>
#include<stdlib.h>
#include<string.h>
extern int yylineno;//行号
extern char* yytext;//语素

Type Exp(gramtree* node)
{
    gramtree* cur=node->leftchild;
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
    			cur->rightchild->rightchild->type=Args(cur->rightchild->rightchild);
    			if(cur->rightchild->rightchild->rightchild->name=="RP"){
                    int Count=0;
                    char Type_list[1000][20];
                    Args(cur->rightchild->rightchild,Count,Type_list);
    				if(search_func(cur->content)!=NULL){
                            if(Count==search_func(cur->content)->param_size&&Type_list==search_func(cur->content)->param_types) return search_variable_type(cur->content);
                            else{
                                    printf("Error type 9 at Line %d:Function %s is not applicable for arguments ",cur->lineno,cur->content);

                                    printf("\"(");
                                    for(int i=1;i<Count;i++){
                                        printf("%s,",Type_list[i]);
                                    }
                                    printf("%s)\"",Type_list[Count]);
                                    return NULL;
                            }

    				}
    				else//函数引用:检查是否未定义就调用Error type 2
    				{
    				    if(cur->is_func==0)//error 11 对普通对量使用函数调用操作符（.....）
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
    			    if(cur->is_func==0)//error 11 对普通对量使用函数调用操作符（.....）
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
    		cur->rightchild->type=Exp(cur->rightchild);
    		return Exp(cur->rightchild);
    	}
    }

    else if(cur->name=="MINUS"){
    	if(cur->rightchild->name=="Exp"){
    		cur->rightchild->type=Exp(cur->rightchild);
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
    			if(Exp(cur)=="INT"&&Exp(cur->rightchild->rightchild)=="INT") return newBasic(0);
    			else if(Exp(cur)=="FLOAT"||Exp(cur->rightchild->rightchild)=="FLOAT") return newBasic(1);
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
                                printf("Error type 12 at Line %d:%s is not an integer.\n ",cur->content,cur->lineno);
                                return NULL;
                            }
                            else return arrayMem(t,1);
                        }
                        else{
                            printf("Error type 10 at Line %d:%s is not an array.\n ",cur->content,cur->lineno);
                            RETURN NULL;
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
void Args(gramtree* node,int count,char** type_list){
    gramtree* cur=node->leftchild;
    if(cur->name="Exp"){
        count++;
        type_list[count]=Exp(cur);
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

int main()
{

    return 0;
}
