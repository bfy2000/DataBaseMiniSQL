//#ifndef _TABLE_H_
//#define _TABLE_H_
//#include<iostream>
//#include<string>
//#include<vector>
//#include"attribute.h"
//#include"Index.h"
//using namespace std;
//class Table {
//public:
//	string tableName;
//	string primaryKey;
//	vector<Attribute> attributeVector;
//	vector<Index>	indexVector;
//	int indexNum;
//	int attributeNum;
//	int rowNum;
//	int rowLength;
//	Table() { ; }
//	Table(string tableName, string primaryKey, vector<Attribute> attributeVector) {
//		this->tableName = tableName;
//		this->primaryKey = primaryKey;
//		this->indexVector = vector<Index>();
//		this->indexNum = 0;
//		this->attributeVector = attributeVector;
//		this->attributeNum = attributeVector.size();
//		this->rowNum = 0;
//		for (int i = 0; i < attributeVector.size(); i++) {
//			if (attributeVector[i].attributeName == primaryKey)
//				attributeVector[i].isUnique = true;
//			this->rowLength += attributeVector[i].type.get_length();
//		}
//	}
//	Table(string tableName, string primaryKey, vector<Attribute> attributeVector, vector<Index> indexVector, int rowNum) {
//		this->tableName = tableName;
//		this->primaryKey = primaryKey;
//		this->attributeVector = attributeVector;
//		this->indexVector = indexVector;
//		this->indexNum = indexVector.size();
//		this->attributeVector = attributeVector;
//		this->attributeNum = attributeVector.size();
//		this->rowNum = rowNum;
//		for (int i = 0; i < attributeVector.size(); i++) {
//			this->rowLength += attributeVector[i].type.get_length();
//		}
//	}
//};
//#endif // !_TABLE_H_
//
