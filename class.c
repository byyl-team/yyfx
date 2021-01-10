#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>
#include"class.h"

void initTypeList()
{
    typelist = (Type)malloc(MAXTYPENUM*sizeof(struct Type_));
    typelist[0].kind = BASIC;   //0处是int型
    typelist[0].u.basic = INT_;
    typelist[0].pre = -1;      //链表首
    typelist[0].nxt = 1; 
    typelist[1].kind = BASIC;   //1处是float型
    typelist[1].u.basic = FLOAT_;
    typelist[1].pre = 0;     //链表第二个
    typelist[1].nxt = -1;
    typeNum = 2;
}
Type ifExistStruct(char* name_)
{
    int i;
    for(i=0;i!=-1;i = typelist[i].nxt)  //遍历全局变量类型链表
    {
        if(typelist[i].kind!=STRUCTURE) continue;
        if(strcmp(name_,typelist[i].u.structure->name)==0)  //字符串相等
        {
            return &typelist[i];
        }
    }
    return NULL;
}
int judge_type(Type type_)  //0:int  1：float  2：array  3:struct
{
    if(type_->kind==BASIC)
    {
        if(type_->u.basic==0)  //0:int
        {
            return 0;
        }
        else return 1;  //1:float
    }
    else if(type_->kind==ARRAY)  //2:array
    {
        return 2;
    }
    else
    {
        return 3;  //struct
    }
    
}
Type ifExist(char* name_)
{
    int i;
    if(!strcmp("int", name_)){
        return newBasic(0);
    }
    else if(!strcmp("float", name_)){
        return newBasic(1);
    }
    for(i=0;i!=-1;i = typelist[i].nxt)  //遍历全局变量类型链表
    {
        if(typelist[i].kind!=STRUCTURE) continue;
        if(strcmp(name_,typelist[i].u.structure->name)==0)  //字符串相等
        {
            return &typelist[i];
        }
    }
    return NULL;
}

Type newBasic(int type_)  //0:整数  1：浮点数
{
    if(type_==INT_)
    {
        return typelist;
    }
    else
    {
        return typelist+1;
    } 
}

Type newArray(Type type_)
{
    if(typeNum>=MAXTYPENUM)
    {
        printf("发生错误：变量类型过多\n");
        exit(1);
    }
    typelist[typeNum].kind = ARRAY;
    typelist[typeNum].u.array.elem = type_;
    typelist[typeNum].u.array.dimension = type_->u.array.dimension+1;
    //在1后面插入类型
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //防止1处是链表尾而越界
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    //hideType(type_);  //基本类型从全局类型中删去  基类不能删！！
    return typelist+typeNum-1;
}

Type newStructure(char* name)
{
    if(typeNum>=MAXTYPENUM)
    {
//        printf("发生错误：变量类型过多\n");
        exit(1);
    }
    typelist[typeNum].kind = STRUCTURE;
    typelist[typeNum].u.structure = (FieldList)malloc(sizeof(struct FieldList_));
    //结构体名字
    int namesize = strlen(name);
    typelist[typeNum].u.structure->name = (char*)malloc(sizeof(char)*namesize);
    strcpy(typelist[typeNum].u.structure->name,name);
    //结构体后继指针
    typelist[typeNum].u.structure->tail = NULL;  //父亲struct的FieldList_中的type是空的，tail指向了第一个成员变量
    //成员变量个数
    typelist[typeNum].u.structure->memnum = 0;
    //是否含有数组
    typelist[typeNum].u.structure->hasArray = 0;
    //在1后面插入类型
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //防止1处是链表尾而越界
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    return typelist+typeNum-1;
}

bool hideType(Type struct_)  //从全局变量类型链表里删去
{
    if(struct_->pre!=-1)
        typelist[struct_->pre].nxt = struct_->nxt;
    if(struct_->nxt!=-1)
        typelist[struct_->nxt].pre = struct_->pre;
    return true;
}

Type StructureAdd(Type struct_,int memnum,...)  //参数：父亲结构体，要包含的成员变量数目，(循环)成员变量Type，成员变量名称char*,...
{
//	printf("in StructureAdd\n");printf("in StructureAdd\n");    
va_list arg_ptr;
    va_start(arg_ptr,memnum);
    Type structi;    //循环中的成员变量type
    FieldList fieldcur;  //上一个成员变量的FieldList
    FieldList temp;      //当前成员变量的FieldList
    char* name_;      //循环中的成员变量name
    if(memnum<=0)
    {
        return struct_;
    }
    

    struct_->u.structure->memnum += memnum;  //成员变量的个数
    int i;
//printf("%d member\n",memnum);
    for(i=0;i<memnum;i++)  //取memnum个成员变量
    {
        structi = va_arg(arg_ptr,Type);   //成员变量的Type
        name_ = va_arg(arg_ptr,char*);    //成员变量的名字
        //下面开始创建相应的FieldList
        temp = (FieldList)malloc(sizeof(struct FieldList_));
        temp->name = (char*)malloc(sizeof(char)*strlen(name_));
        strcpy(temp->name,name_);    //成员变量的名字复制过来
        temp->type = structi;         
        temp->tail = NULL;         //一开始没有后继
        if(structi->kind==STRUCTURE)  //如果是结构体，有一层FieldList的嵌套，把相应的值赋值
        {   
            temp->memnum = structi->u.structure->memnum;  
            temp->hasArray = structi->u.structure->hasArray; 
        }
        else if(structi->kind==ARRAY)
        {
            temp->memnum = 1;
            temp->hasArray = 1;  //有数组
        }
        else
        {
            temp->memnum = 1;
            temp->hasArray = 0; //没有数组
        }
        if(temp->hasArray==1)  //成员变量有数组
        {
            struct_->u.structure->hasArray = 1;  //推出该结构体有数组
        }
	temp->tail = struct_->u.structure->tail;            
struct_->u.structure->tail = temp;  //链表头指向它
        fieldcur = temp;
        if(structi->kind!=BASIC)   //如果不是基本变量类型
        {
            hideType(structi);   //不能出现在全局变量类型链表里了。
        }  
    }
    return struct_;

}

Type arrayMem(Type type_,int dms)
{
    if(type_->kind==BASIC || type_->kind==STRUCTURE)
    {
  //      printf("");  //该类型不是数组！该怎么通知上去呢？
        return NULL;
    }
    if(type_->u.array.dimension<dms)
    {
    //    printf(""); //也可以认为是同样的错误类型
        return NULL;
    }
    Type cur = type_;
    int i;
    for(i=1;i<=dms;i++)
    {
        cur = cur->u.array.elem;  //指向当前数组的基类型
    }
    return cur;
}


Type structMem(Type type_,int num,...)  //结构体的成员变量类型
{
    if(!num) return type_;
    va_list arg_ptr;
    va_start(arg_ptr,num);
    Type cur = type_;
    char* name_;
    FieldList list_;
    int i;
    for(i = 1;i<=num;i++)
    {
        if(cur->kind==BASIC || cur->kind==ARRAY)
        {
            printf("");  //该类型没有成员变量！怎么通知上去？
            return NULL;
        }
        name_= va_arg(arg_ptr,char*);  //下一个成员变量的名字
        //找当前结构体的成员变量
        list_ = cur->u.structure->tail;
        while(list_!=NULL)
        {
            if(strcmp(name_,list_->name)==0)
            {
                cur = list_->type;
                break;
            }
            list_ = list_->tail;
        }
        if(list_==NULL)
        {
            printf("");  //没有这个成员变量
            return NULL;
        }
    }
    return cur;
}


void printType(Type t)
{
    if(t->kind==BASIC)
    {
      //  printf("basic:%d",t->u.basic);
        return;
    }
    else if(t->kind==ARRAY)
    {
        printType(t->u.array.elem);
        //printf("[ ]");
    }
    else if(t->kind==STRUCTURE)
    {
        //printf("struct %s{ ",t->u.structure->name);
        FieldList cur = t->u.structure->tail;
        while(cur!=NULL)
        {
          //  printf("%s:",cur->name);
            printType(cur->type);
            printf(" ");
            cur = cur->tail;
        }
        //printf("} ");
    }
}

void printTypeList()
{
    int i;
    for(i=0;i!=-1;i = typelist[i].nxt)
    {
        printType(&typelist[i]);
        //printf("\n");
    }
}

bool isEqual(Type a,Type b)
{
    if(a->kind!=b->kind)
    {
        return false;
    }
    if(a->kind==BASIC)
    {
        return a->u.basic==b->u.basic?true:false;
    }
    else if(a->kind==ARRAY)
    {
        if(isEqual(a->u.array.elem,b->u.array.elem))
        {
            if(a->u.array.dimension==b->u.array.dimension)
            {
                return true;
            }
        }
    }
    else if(a->kind==STRUCTURE)
    {
        int anum = a->u.structure->memnum;
        int bnum = b->u.structure->memnum;
	        if(strcmp(a->u.structure->name,b->u.structure->name)==0)
        {
            return true;
        }        
if(anum!=bnum) return false;  //成员变量数目不等一定不等价
        if(a->u.structure->hasArray || b->u.structure->hasArray) //含有数组则一定不等价
        {
            return false;
        }
        int compare[anum+2];  //compare标记数组
        int i=0,j=0;
        for(i=1;i<=anum;i++) compare[i] = 0;  //初始化为0
        FieldList acur,bcur;
        acur = a->u.structure;
        for(i=1;i<=anum;i++)  //对a的每个成员变量
        {
            acur = acur->tail;
            bcur = b->u.structure;
            for(j=1;j<=bnum;j++) //从b的成员变量中找
            {
                bcur = bcur->tail;
                if(compare[j]==0 && isEqual(acur->type,bcur->type))  //没有标记过并且相等
                {
                    compare[j] = 1;
                    break;
                }
            }
            if(j>bnum) return false;  //没找到：不等价
        }
        return true;
    }
    return false;
}



//int main()
//{
    /*  测试代码
    printf("hello\n");
    initTypeList();
    printTypeList();
    Type temp,temp2,temp3,temp4,temp5;
    temp5 = newStructure("lalala");
    temp = newBasic(0);
    temp = newArray(temp);
    temp= newArray(temp);
    temp2 = newBasic(1);
    temp5 = StructureAdd(temp5,2,temp,"naughty",temp2,"boy");
    printf("***\n");
    printTypeList();
    
    temp3 = structMem(temp5,1,"boy");
    if(isEqual(temp2,temp3))
    {
        printf("equal!\n");
    }
    else
    {
        printf("not equal!\n");
    }

    printType(ifExist("lalala"));
    initTypeList();
    printTypeList();
    Type temp,temp2,temp3,temp4,temp5;
    temp5 = newStructure("lalala");
    temp = newBasic(0);
    temp = newArray(temp,3); 
    temp5 = StructureAdd(temp5,1,temp,"naughty");
    printf("***\n");
    printTypeList();


    temp2 = newBasic(1);
    temp3 = newStructure("apple");
    temp3 = StructureAdd(temp3,2,temp,"red",temp5,"white");
    temp4 = newStructure("banana");
    temp4 = StructureAdd(temp4,2,temp5,"white",temp,"yellow");

    temp = newArray(temp,3);
    temp = newArray(temp,5);
    temp = newArray(temp,9);
   
    temp = newStructure("happy");
    temp2 = newBasic(1);
    temp2 = StructureAdd(temp,1,temp2,"son");
    printf("hereee\n");
    temp3 = newStructure("father");
    printf("here\n");
    temp3 = StructureAdd(temp3,1,temp2,"mid");

    printf("%d\n",temp5->u.structure->hasArray);
    printTypeList();

    if(isEqual(temp3,temp4))
    {
        printf("temp3 and temp4 are equal\n");
    }
    else
    {
        printf("not equal\n");
    }
    */

    //return 0;
//}
