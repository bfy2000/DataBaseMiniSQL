#ifndef _INDEX_H_
#define _INDEX_H_
#include<iostream>
#include<string>
using namespace std;
class Index {
public:
	string indexName;
	string tableName;
	string attributeName;
	int rootNum;
	int blockNum = 0;
	Index() { ; }
	Index(string indexName, string tableName, string attributeName, int blockNum, int rootNum) {
		this->indexName = indexName;
		this->tableName = tableName;
		this->attributeName = attributeName;
		this->blockNum = blockNum;
		this->rootNum = rootNum;
	}

	Index(string indexName, string tableName, string attributeName) {
		this->indexName = indexName;
		this->tableName = tableName;
		this->attributeName = attributeName;
	}
};
#endif // !_INDEX_H_

