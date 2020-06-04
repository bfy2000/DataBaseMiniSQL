模块调用时出现的一些些问题：

* `CatalogManager.h`中的`map<string, Table> tables;`应当为`map<string, Table*> tables;`
  * 理由如下：`RecordManager`中`insertRecord`等操作需从`CatalogManager`中`get_table`，若不为指针，则该操作针对临时变量完成，实际操作不会对Table产生影响。
  * 同时，改为指针也便于判断表是否存在。（若表的名字不存在，则返回nullptr的指针）
* `dropTable`以及`CreateTable`在`CatalogManager`中已有实现，不知
* `buffer`中的`bool readData(string fileName, string db_name, int index, char to_read[], int& length, int filetype);`不太清楚db_name需要传入什么参数
  * 因此，我在调用这个函数时db_name传入“db_name?”，协调时麻烦加以注意。

### RecordManager

`Record Manager`负责管理记录表中数据的数据文件。主要功能为实现记录的插入、删除与查找操作，并对外提供相应的接口。其中记录的查找操作要求能够支持不带条件的查找和带条件的查找（包括等值查找、不等值查找和区间查找）。

数据文件由一个或多个数据块组成，块大小应与缓冲区块大小相同。一个块中包含一条至多条记录，为简单起见，只要求支持定长记录的存储，且不要求支持记录的跨块存储。

#### 功能说明

![image-20200604234317583](C:\Users\SHR00\AppData\Roaming\Typora\typora-user-images\image-20200604234317583.png)

#### 接口说明

##### base.h

定义了一些通用的数据类型与枚举类型，例如Tuple, SelectCondition。其中，Table(数据表)的定义采用了`CatalogManager`模块，并对其做了相应的修改。

* SelectCondition

```c++
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} Operator;

class SelectCondition {
public:
	int attributeIndex;//当前条件针对第几个字段，约定从0开始
	Operator opt;//定义操作类型
	Element value;//对于><=操作，只需要用到value
};
```

* Tuple

```c++
class Tuple {
private:
	std::vector<Element> data;
	bool isDeleted_;//当前元祖是否删除
	int index;//当前这个元组，在表中的第几行，从0开始

public:
	Tuple() : isDeleted_(false) {};
	Tuple(int elementNum) {	}
	Tuple(const Tuple& copytuple) {	}//拷贝函数
	Tuple(const std::vector<Element>& tuples) {	}
	bool operator==(const Tuple& t) { }
	void push_back_Data(const Element& d) { }//输入元组
    	//初步设想是在insert操作时按顺序一个一个把data导进来
	std::vector<Element> getData() const { return this->data; } //返回数据
	bool getIsDeleted() { return isDeleted_; }
	void setIsDeleted(bool s) { isDeleted_ = s; }
	int getIndex() { return index; }
	void setIndex(int i) { index = i; }
};
```

##### Element

定义了元素，包含其类型以及数据存放，重载了相应比较的运算符

```c++
class Element {
public:
	NumType type;
	int m_int;
	float m_float;
	std::string m_char;
	int length; 
    // 注意 type=CHAR 时，这里的 length = 最大能保存字串转长度+1，因为最后一个是'\0'
	// 否则 length = 4（针对 INT 和 FLOAT）

	bool operator<(Element &e);
	bool operator==(const Element &e);
	bool operator>(Element &e);
	bool operator>=(Element &e);
	bool operator<=(Element &e);
	bool operator!=(const Element &e);
	
	friend std::ostream& operator<<(std::ostream& out, Element &e);

	//Element(std::string s, int char_num);
	Element(std::string s, int l = 1) {}
	Element(float f, int l = 4) {}
	Element(int i, int l = 4) {}
	Element();
	~Element();

	void setType(NumType t) { type = t; }
	void setLength(int l) { length = l; }

	void elementToChar(char* c) {}//转为char型
	void charToElement(char* c) {}
	void printElement() {}//打印元素信息，便于输出
};
```

##### RecordManager

```c++
class RecordManager {
public:
	//Table* isExistTable(string tableName);
	//bool createTable(string tableName, string primaryKey, vector<Attribute> attributeVector);
	//bool dropTable(string tableName);
	bool insertValue(string tableName, Tuple tuple);
    //insertValue返回值为false表示出错，会根据不同要求输出不同的错误信息，否则为正确执行
    
	int deleteTuple(string tableName, vector<SelectCondition> selectConditions);
	int deleteTuple(string tableName);
    //deleteTuple操作返回-1表示删除数据失败，否则返回删除的记录的条数
    
	vector<Tuple> searchQuery(string tableName, vector<SelectCondition> selectConditions);
	vector<Tuple> searchQuery(string tableName);
    //目前searchQuery仅支持and连接的数据或单个条件的查询
    //区间比较，需要其他模块，把类似于区间的操作解析为两个比较操作，
	//比如and连接的话，需要变成两个符号操作。
    
	void PrintTable(Table *table) {}//能够输出表中信息，但是格式上可能需要再加调整

private:
	bool judgeCondition(Tuple tuple, SelectCondition condition);
	bool isMatchTheAttribute(Table* table, Tuple* tuple);
	bool isConflictTheUnique(Table* table, Tuple* tuple);
	bool isConflict(Tuple* tuple1, Tuple* tuple2, int index);
    //用来判断插入数据时的错误
    Tuple getTupleByIndex(Table* table, int index);
    
	Tuple charToTuple(Table *table, char* c) {}
    //Tuple的数据存储格式
    //Tuple中第一位0表示已经删除,1表示没有删除
    //Tuple中第2-5位（4个整数）表示index
    //Tuple中剩余位数表示数据
    
	void tupleToChar(Tuple tuple, char* c) {}//功能：将tuple拷贝进c地址
	//使用此函数需要保证tuple是合理的，关键是string的长度合理
	//使用此函数需要保证c的合理性	
};
```

