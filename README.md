# yyfx
玩儿转语义分析


### class.c, class.h文件接口
  **功能：** 
    变量类型的定义，比较类型是否等价，支持输出某一类型、输出全局所有类型。
  **数据结构：**
  ```c
  typedef struct Type_* Type;   //注意是Type_类型的指针！！
  struct Type_
{
    enum{
        BASIC, ARRAY, STRUCTURE   //类型：基础类型(int,float);数组;结构体
    }kind;
    union{
        //基本类型 int,float
        int basic;
        //数组类型 
        struct {
            Type elem;  //元素类型
            int size;   //数组大小
            int dimension;  //数组维度
        }array;
        //结构体类型
        FieldList structure;
    } u;
    int pre;  //全局变量类型，前驱指针
    int nxt;  //全局变量类型，后继指针
};
  ```
  **重要函数：**
```c
    //初始化相关
    void initTypeList();    //初始化类型列表,程序开始的时候需要调用！
    
    //查询，查重相关
    Type ifExist(char* name_);  //是否存在某个名字的类型，参数：名字  返回值：存在则返回该Type，不存在返回NULL
    
    //定义类型相关（返回值都是类型Type）
    Type newBasic(int type_);  //基础类型，参数：0 int   1 float
    Type newArray(Type type_,int size_);    //数组类型，参数:基本类型type  数组大小size
    Type newStructure(char* name_); //结构体类型（空的），参数：名字  
    Type StructureAdd(Type struct_,int memnum,...);    //使结构体包含若干成员变量，参数：父亲结构体struct_，要包含的成员变量数目memnum，接下来是memnum个：成员变量Type，成员变量名称char*,...
    
    //类型等价相关
    Type arrayMem(Type type_,int dms);     //返回数组第几维度的类型，参数：（数组）类型，维度
    Type structMem(Type type_,int num,...);  //结构体的成员变量类型，参数：（结构体）类型，后面成员变量个数num，num个成员变量名。比如类型node.id,调用函数structMem(node,1,"id");
    bool isEqual(Type a,Type b);   //判断两个类型是否等价

    //debug相关
    void printType(Type t);      //打印该类型信息
    void printTypeList();        //打印全局所有类型
```

  **测试样例：**
  ```c
    initTypeList();
    printTypeList();
    Type temp,temp2,temp3,temp4,temp5;
    temp5 = newStructure("lalala"); //新的结构体类型
    temp = newBasic(0);   //int类型
    temp = newArray(temp,3); //int类型的数组
    temp5 = StructureAdd(temp5,1,temp,"naughty");  //包含数组类型的结构体
    printf("***\n");
    printTypeList();
