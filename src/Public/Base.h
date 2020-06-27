#ifndef __BASE_H__
#define __BASE_H__
/* 
目的：枚举，用于返回类型为 int 的函数，给出函数执行结果的标记。
说明：如果函数执行没有错误，则方便 调用者 输出 “执行成功” 的信息；
      如果函数执行出错，方便出错处理；
      请在下面定义出错类型，比如没有这个表出错，则如下面的方式定义为-100
      1、负数：函数执行出错
      2、零：表示执行成功
      3、正数，使用场景如下：
           int selectQuery(vector<Tuple>& tuples);
           查询得到的结果tuples用 引用 的方式返回到调用者；返回值TABLE_NAME_ERROR表示查询到了几条数据
      4、出错数值不打紧，为避免重复，大家自己在下面申请，但是意义不要重复了。

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
      ERROR=-1, 
      SUCCESS=0,
      
      //indexmanager info
      WRONG_TYPE=-99,
      BPT_ALREADY_EXSIST=-98,
      NO_INDEX=-97,
      NO_SUCH_ELEMENT=-96,
      INSERT_FAIL=-95,
      DELETE_FAIL=-94,
      
      //API
      ATTRIBUTE_TYPE_ERROR=-60,
      NO_SUCH_TABLE=-59,
      NO_SUCH_ATTR=-58,
      UNIQUE_CONFLICT=-57,
      
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

#endif