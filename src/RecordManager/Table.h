#ifndef _TABLE_H_
#define _TABLE_H_
#include<iostream>
#include<string>
#include<vector>
#include"../catalog_manager/attribute.h"
#include"../catalog_manager/Index.h"
#include"../buffer/buffer.h"

using namespace std;
class Table {
public:
	string tableName;
	string primaryKey;
	int primaryKeyIndex;
	vector<Attribute> attributeVector;
	vector<Index>	indexVector;
	//vector<Tuple>   tuples;
	//char* data; // 用来保存所有的数据
	//FileInfo file;
	int indexNum;
	int attributeNum;
	int rowNum; // 表示总共有多少条数据有效
	int rowLength; // 表示每条数据的长度是多少字节
	Table() { ; }
	Table(string tableName, string primaryKey, vector<Attribute> attributeVector) {
		this->tableName = tableName;
		this->primaryKey = primaryKey;
		//this->primaryKeyIndex = primaryKeyIndex;
		this->indexVector = vector<Index>();
		this->indexNum = 0;

		this->attributeVector = attributeVector;
		this->attributeNum = attributeVector.size();

		this->rowNum = 0;

		this->rowLength = sizeof(char);//第0个字节用来记录数据是否删除
		this->rowLength += sizeof(int);//第1~4个字节用来记录数据是否删除
		for (int i = 0; i < attributeVector.size(); i++) {
			if (attributeVector[i].attributeName == primaryKey) {
				attributeVector[i].isUnique = true;
				primaryKeyIndex = i;
			}
			this->rowLength += attributeVector[i].type.get_length();
		}
	}
	Table(string tableName, string primaryKey, vector<Attribute> attributeVector, vector<Index> indexVector, int rowNum) {
		this->tableName = tableName;
		this->primaryKey = primaryKey;
		this->attributeVector = attributeVector;
		this->indexVector = indexVector;
		this->indexNum = indexVector.size();
		this->attributeVector = attributeVector;
		this->attributeNum = attributeVector.size();
		this->rowNum = rowNum;
		for (int i = 0; i < attributeVector.size(); i++) {
			this->rowLength += attributeVector[i].type.get_length();
		}
	}

private:

};
#endif // !_TABLE_H_

