#include "base.h"
#include "FieldType.h"


/* 
目的：枚举，用于返回类型为 int 的函数，给出函数执行结果的标记。
说明：如果函数执行没有错误，则方便 调用者 输出 “执行成功” 的信息；
      如果函数执行出错，请在下面定义出错类型，比如表名出错，则如下面的方式定义为-100
      1、负数：函数执行出错
      2、零：表示执行成功
      3、正数，使用场景如下：
           int selectQuery(vector<Tuple>& tuples);
           查询得到的结果tuples用 引用 的方式返回到调用者；返回值TABLE_NAME_ERROR表示查询到了几条数据
      4、出错数值不打紧，为避免重复，大家自己在下面申请，但是不要重复了。

系统相关: [-20,0]
Interpreter:    [-40,-20)
API:            [-60,-40)
Record:         [-80,-60)
Index:          [-100,-80)
Catalog:        [-120,-100)
Buffer:         [-140,-120)

 */
enum Result {
      TABLE_NAME_ERROR=-100, 
      TABLE_NAME_EXSITED=-110,//创建一个新表的时候，该表名已经存在
      TABLE_NAME_NOEXSIT=-109,//删除一个新表的时候，该表名不存在 或创建/删除一个新索引时，该索引对应的表名不存在
      INDEX_NAME_EXSITED=-108,//创建一个新索引的时候，该索引名已经存在
      INDEX_NAME_NOEXIST=-107,//删除或更新一个索引的时候，该索引名不存在
      TABLE_CATALOG_FILE_READ_ERROR=-105,//table_catalog打开错误
      INDEX_CATALOG_FILE_READ_ERROR=-104,//index_catalog打开错误
      TABLE_CATALOG_FILE_WRITE_ERROR=-103,//table_catalog写入错误
      INDEX_CATALOG_FILE_WRITE_ERROR=-102,//index_catalog写入错误
      ERROR=-1, 
      SUCCESS=0
};

/*
表示元素的类型
*/
enum NumType { CHAR, INT, FLOAT, DEFAULT };

/*
表示where条件的运算符号
*/
enum CMP {
	EQUAL,
	NOT_EQUAL,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL  
};