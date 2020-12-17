#ifndef CLASS_H
#define CLASS_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<stdarg.h>
#define INT 0
#define FLOAT 1
#define MAXTYPENUM 10000  //���ı�����������

typedef struct Type_* Type;   //ע��������ָ�룡��
typedef struct FieldList_* FieldList;

struct Type_
{
    enum{
        BASIC, ARRAY, STRUCTURE   //���ͣ���������(int,float);����;�ṹ��
    }kind;
    union{
        //�������� int,float
        int basic;
        //�������� 
        struct {
            Type elem;  //Ԫ������
            int size;   //�����С
            int dimension;  //����ά��
        }array;
        //�ṹ������
        FieldList structure;
    } u;
    int pre;  //ȫ�ֱ������ͣ�ǰ��ָ��
    int nxt;  //ȫ�ֱ������ͣ����ָ��
};

struct FieldList_
{ 
    char* name;   //����
    Type type;    //������
    FieldList tail;  //��һ����
    int memnum;    //��Ա�����ĸ���
    int hasArray;  //���������жϽṹ��ṹ�ȼ�ʱ��֦
};

static Type typelist;
static int typeNum;

Type initTypeList();    //��ʼ�������б�
Type ifExist(char* name_);  //�Ƿ����ĳ�����ֵ����ͣ�����������  ����ֵ�������򷵻ظ�Type�������ڷ���NULL
Type newBasic(int type_);  //�������ͣ�������0 int   1 float
Type newArray(Type type_,int size_);    //�������ͣ�����:��������type  �����Сsize
Type newStructure(char* name_); //�ṹ�����ͣ�����������
Type StructureAdd(Type struct_,int memnum,...);    //�ṹ���ڰ������ɳ�Ա���������������׽ṹ�壬Ҫ�����ĳ�Ա������Ŀ��(ѭ��)��Ա����Type����Ա��������char*,...
bool isEqual(Type a,Type b);   //�ж����������Ƿ�ȼ�
bool hideType(Type struct_);  //������ṹ���������ȥ��
void printType(Type t);      //��ӡ��������Ϣ
void printTypeList();        //��ӡȫ����������

#endif