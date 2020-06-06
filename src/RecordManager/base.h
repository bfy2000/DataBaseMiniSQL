#ifndef __BASE_H__
#define __BASE_H__

#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>
#include "Element.h"
#include "buffer.h"
#include "Table.h"

//enum Type { INT, CHAR, FLOAT };/*数据类型*/

//用于where的判断
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
	Operator opt;
	Element value;//对于><=操作，只需要用到value
};

//元组
class Tuple {
private:
	std::vector<Element> data;
	bool isDeleted_;
	int index;//当前这个元组，在表中的第几行，从0开始

public:
	Tuple() : isDeleted_(false) {};
	Tuple(int elementNum) {
		Tuple();
		data.resize(elementNum);
	}

	//拷贝函数
	Tuple(const Tuple& copytuple) {
		data = copytuple.data; 
	//	attr = copytuple.attr; 
	}

	Tuple(const std::vector<Element>& tuples) {
		for (auto it : tuples) {
			push_back_Data(it);
		}
	}
	bool operator==(const Tuple& t) {
		if (t.data.size() != data.size()) {
			return false;
		}
		for (auto i = 0; i < data.size(); i++) {
			if (data[i] != t.data[i]) return false;
		}
		return true;
	}

	//输入元组
	//初步设想是在insert操作时按顺序一个一个把data导进来
	void push_back_Data(const Element& d) { this->data.push_back(d); }

	//打印一个元祖的数据，按顺序输出，中间的间隔还需调整
	void Printdata(const std::vector<int>& width) {
		int i = 0;
		for (auto it = data.begin(); it != data.end(); it++, i++) {
			std::cout << "|";
			if ((*it).type == INT)
				std::cout << std::left << std::setw(width[i]) << (*it).m_int;
			else if ((*it).type == FLOAT)
				std::cout << std::left << std::setw(width[i]) << (*it).m_float;
			else
				std::cout << std::left << std::setw(width[i]) << (*it).m_char;
		}
		std::cout << "|\n";
	}
	int getWidth(int i) {
		assert(i >= 0 && i < data.size());
		if (data[i].type == INT) {
			auto item = std::to_string(data[i].m_int);
			return item.size();
		}
		else if (data[i].type == FLOAT) {
			auto item = std::to_string(data[i].m_float);
			return item.size();
		}
		else {
			assert(false);
		}
	}

	//返回数据
	std::vector<Element> getData() const { return this->data; }
	bool getIsDeleted() { return isDeleted_; }
	void setIsDeleted(bool s) { isDeleted_ = s; }
	int getIndex() { return index; }
	void setIndex(int i) { index = i; }
};

/*定义数据库*/
class DataBase {
public:
	std::string dataBaseName;
	std::vector<Table> tables;//存储当前数据库中所有的数据表
	DataBase(std::string name) :dataBaseName(name) {}
};

/*定义数据表---采用catalog中的Table定义*/
/*class Table {
public:
	std::string tableName;				//表名
	std::vector<TableAttr> tableAttrs;	//一个表有多个属性
	//int attrNum;						//属性数
	int primaryKeyIndex;				//数据表中的主键
	std::string primaryKey;				//主键名字
	//vector<BlockInfo> data;			//数据表中的数据
	int blockNum;						//占用的block数
	int recordNum;						//记录条数
	int recordLength;					//每条记录的长度

	Table() : blockNum(0), recordNum(0), recordLength(0) {}
	Table(std::string name, std::string primaryKey) : tableName(name), primaryKey(primaryKey) {}
	~Table() {}
};*/

/*定义表中某一个属性---采用catalog中的Table定义*/
/*class TableAttr {
public:
	std::string attrName;//属性名
	Type type;//属性类型
};*/

/*BlockInfo与FileInfo为来自buffer的类定义*/

/*class FileInfo {
public:
	int type;
	std::string fileName;
	int recordAmout;
	int freeNum;
	int recordLength;
	FileInfo* next;
	BlockInfo* first;
};

class BlockInfo {
public:
	int blockNum;
	bool dirtyBit;
	BlockInfo* next;
	FileInfo file;
	int charNum;
	char* cBlock;
	int call_times;
	int lock;
	bool isfree;
};*/

#endif // BASE