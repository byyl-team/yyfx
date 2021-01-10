#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>
#include"class.h"

void initTypeList()
{
    typelist = (Type)malloc(MAXTYPENUM*sizeof(struct Type_));
    typelist[0].kind = BASIC;   //0����int��
    typelist[0].u.basic = INT_;
    typelist[0].pre = -1;      //������
    typelist[0].nxt = 1; 
    typelist[1].kind = BASIC;   //1����float��
    typelist[1].u.basic = FLOAT_;
    typelist[1].pre = 0;     //����ڶ���
    typelist[1].nxt = -1;
    typeNum = 2;
}
Type ifExistStruct(char* name_)
{
    int i;
    for(i=0;i!=-1;i = typelist[i].nxt)  //����ȫ�ֱ�����������
    {
        if(typelist[i].kind!=STRUCTURE) continue;
        if(strcmp(name_,typelist[i].u.structure->name)==0)  //�ַ������
        {
            return &typelist[i];
        }
    }
    return NULL;
}
int judge_type(Type type_)  //0:int  1��float  2��array  3:struct
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
    for(i=0;i!=-1;i = typelist[i].nxt)  //����ȫ�ֱ�����������
    {
        if(typelist[i].kind!=STRUCTURE) continue;
        if(strcmp(name_,typelist[i].u.structure->name)==0)  //�ַ������
        {
            return &typelist[i];
        }
    }
    return NULL;
}

Type newBasic(int type_)  //0:����  1��������
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
        printf("�������󣺱������͹���\n");
        exit(1);
    }
    typelist[typeNum].kind = ARRAY;
    typelist[typeNum].u.array.elem = type_;
    typelist[typeNum].u.array.dimension = type_->u.array.dimension+1;
    //��1�����������
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //��ֹ1��������β��Խ��
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    //hideType(type_);  //�������ʹ�ȫ��������ɾȥ  ���಻��ɾ����
    return typelist+typeNum-1;
}

Type newStructure(char* name)
{
    if(typeNum>=MAXTYPENUM)
    {
//        printf("�������󣺱������͹���\n");
        exit(1);
    }
    typelist[typeNum].kind = STRUCTURE;
    typelist[typeNum].u.structure = (FieldList)malloc(sizeof(struct FieldList_));
    //�ṹ������
    int namesize = strlen(name);
    typelist[typeNum].u.structure->name = (char*)malloc(sizeof(char)*namesize);
    strcpy(typelist[typeNum].u.structure->name,name);
    //�ṹ����ָ��
    typelist[typeNum].u.structure->tail = NULL;  //����struct��FieldList_�е�type�ǿյģ�tailָ���˵�һ����Ա����
    //��Ա��������
    typelist[typeNum].u.structure->memnum = 0;
    //�Ƿ�������
    typelist[typeNum].u.structure->hasArray = 0;
    //��1�����������
    typelist[typeNum].nxt = typelist[1].nxt;
    if(typelist[1].nxt!=-1)  //��ֹ1��������β��Խ��
        typelist[typelist[1].nxt].pre = typeNum;
    typelist[1].nxt = typeNum;
    typelist[typeNum].pre = 1;
    typeNum++;
    return typelist+typeNum-1;
}

bool hideType(Type struct_)  //��ȫ�ֱ�������������ɾȥ
{
    if(struct_->pre!=-1)
        typelist[struct_->pre].nxt = struct_->nxt;
    if(struct_->nxt!=-1)
        typelist[struct_->nxt].pre = struct_->pre;
    return true;
}

Type StructureAdd(Type struct_,int memnum,...)  //���������׽ṹ�壬Ҫ�����ĳ�Ա������Ŀ��(ѭ��)��Ա����Type����Ա��������char*,...
{
//	printf("in StructureAdd\n");printf("in StructureAdd\n");    
va_list arg_ptr;
    va_start(arg_ptr,memnum);
    Type structi;    //ѭ���еĳ�Ա����type
    FieldList fieldcur;  //��һ����Ա������FieldList
    FieldList temp;      //��ǰ��Ա������FieldList
    char* name_;      //ѭ���еĳ�Ա����name
    if(memnum<=0)
    {
        return struct_;
    }
    

    struct_->u.structure->memnum += memnum;  //��Ա�����ĸ���
    int i;
//printf("%d member\n",memnum);
    for(i=0;i<memnum;i++)  //ȡmemnum����Ա����
    {
        structi = va_arg(arg_ptr,Type);   //��Ա������Type
        name_ = va_arg(arg_ptr,char*);    //��Ա����������
        //���濪ʼ������Ӧ��FieldList
        temp = (FieldList)malloc(sizeof(struct FieldList_));
        temp->name = (char*)malloc(sizeof(char)*strlen(name_));
        strcpy(temp->name,name_);    //��Ա���������ָ��ƹ���
        temp->type = structi;         
        temp->tail = NULL;         //һ��ʼû�к��
        if(structi->kind==STRUCTURE)  //����ǽṹ�壬��һ��FieldList��Ƕ�ף�����Ӧ��ֵ��ֵ
        {   
            temp->memnum = structi->u.structure->memnum;  
            temp->hasArray = structi->u.structure->hasArray; 
        }
        else if(structi->kind==ARRAY)
        {
            temp->memnum = 1;
            temp->hasArray = 1;  //������
        }
        else
        {
            temp->memnum = 1;
            temp->hasArray = 0; //û������
        }
        if(temp->hasArray==1)  //��Ա����������
        {
            struct_->u.structure->hasArray = 1;  //�Ƴ��ýṹ��������
        }
	temp->tail = struct_->u.structure->tail;            
struct_->u.structure->tail = temp;  //����ͷָ����
        fieldcur = temp;
        if(structi->kind!=BASIC)   //������ǻ�����������
        {
            hideType(structi);   //���ܳ�����ȫ�ֱ��������������ˡ�
        }  
    }
    return struct_;

}

Type arrayMem(Type type_,int dms)
{
    if(type_->kind==BASIC || type_->kind==STRUCTURE)
    {
  //      printf("");  //�����Ͳ������飡����ô֪ͨ��ȥ�أ�
        return NULL;
    }
    if(type_->u.array.dimension<dms)
    {
    //    printf(""); //Ҳ������Ϊ��ͬ���Ĵ�������
        return NULL;
    }
    Type cur = type_;
    int i;
    for(i=1;i<=dms;i++)
    {
        cur = cur->u.array.elem;  //ָ��ǰ����Ļ�����
    }
    return cur;
}


Type structMem(Type type_,int num,...)  //�ṹ��ĳ�Ա��������
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
            printf("");  //������û�г�Ա��������ô֪ͨ��ȥ��
            return NULL;
        }
        name_= va_arg(arg_ptr,char*);  //��һ����Ա����������
        //�ҵ�ǰ�ṹ��ĳ�Ա����
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
            printf("");  //û�������Ա����
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
if(anum!=bnum) return false;  //��Ա������Ŀ����һ�����ȼ�
        if(a->u.structure->hasArray || b->u.structure->hasArray) //����������һ�����ȼ�
        {
            return false;
        }
        int compare[anum+2];  //compare�������
        int i=0,j=0;
        for(i=1;i<=anum;i++) compare[i] = 0;  //��ʼ��Ϊ0
        FieldList acur,bcur;
        acur = a->u.structure;
        for(i=1;i<=anum;i++)  //��a��ÿ����Ա����
        {
            acur = acur->tail;
            bcur = b->u.structure;
            for(j=1;j<=bnum;j++) //��b�ĳ�Ա��������
            {
                bcur = bcur->tail;
                if(compare[j]==0 && isEqual(acur->type,bcur->type))  //û�б�ǹ��������
                {
                    compare[j] = 1;
                    break;
                }
            }
            if(j>bnum) return false;  //û�ҵ������ȼ�
        }
        return true;
    }
    return false;
}



//int main()
//{
    /*  ���Դ���
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
