#ifndef __RECORDMANAGER_H__
#define __RECORDMANAGER_H__
#include "base.h"
#include "Table.h"
#include "attribute.h"
#include "FieldType.h"
#include "CatalogManager.h"

class RecordManager {
public:
	//Table* isExistTable(string tableName);
	//bool createTable(string tableName, string primaryKey, vector<Attribute> attributeVector);
	//bool dropTable(string tableName);
	bool insertValue(string tableName, Tuple tuple);
	int deleteTuple(string tableName, vector<SelectCondition> selectConditions);
	int deleteTuple(string tableName);
	vector<Tuple> searchQuery(string tableName, vector<SelectCondition> selectConditions);
	vector<Tuple> searchQuery(string tableName);
	void PrintTable(Table *table) {
		for (int i = 0; i < table->attributeVector.size(); i++) {
			cout << table->attributeVector[i].attributeName << "\t";
		}
		cout << endl;

		char* tmpChar = (char*)malloc(table->rowLength * sizeof(char));
		if (tmpChar == NULL) {
			cerr << "memory error" << endl;
			return;
		}
		for (int i = 0; i < table->rowNum; i++) {
			readData(table->tableName, "db_name?", i, tmpChar, table->rowLength, 0);
			Tuple tuple = getTupleByIndex(table, i);
			for (int j = 0; j < tuple.getData().size(); i++) {
				tuple.getData()[j].printElement();
				cout << "\t";
			}
			cout << endl;
		}
		free(tmpChar);
	}

private:
	bool judgeCondition(Tuple tuple, SelectCondition condition);
	bool isMatchTheAttribute(Table* table, Tuple* tuple);
	bool isConflictTheUnique(Table* table, Tuple* tuple);
	bool isConflict(Tuple* tuple1, Tuple* tuple2, int index);
	Tuple charToTuple(Table *table, char* c) {
		Tuple tuple;
		//Tuple的数据存储格式
		//Tuple中第一位0表示已经删除,1表示没有删除
		//Tuple中第2-5位（4个整数）表示index
		//Tuple中剩余位数表示数据
		if (*c == '0') {
			tuple.setIsDeleted(true);
		}
		else {
			tuple.setIsDeleted(false);
		}
		c = c + sizeof(char);

		int* tmp = (int*)malloc(sizeof(int));
		memcpy(tmp, c, sizeof(int));
		tuple.setIndex(*tmp);
		free(tmp);
		c = c + sizeof(int);

		int n = table->attributeNum;
		for (int i = 0; i < n; i++) {
			Element tmpElement;
			tmpElement.setType(table->attributeVector[i].type.get_type());
			tmpElement.setLength(table->attributeVector[i].type.get_length());
			tmpElement.charToElement(c);
			tuple.push_back_Data(tmpElement);
			c = c + table->attributeVector[i].type.get_length();
		}
		return tuple;
	}
	void tupleToChar(Tuple tuple, char* c) {
		//功能：将tuple拷贝进c地址
		//使用此函数需要保证tuple是合理的，关键是string的长度合理
		//使用此函数需要保证c的合理性
		if (tuple.getIsDeleted() == true) {
			*c = '0';
		}
		else {
			*c = '1';
		}
		c = c + 1;

		int t = tuple.getIndex();
		memcpy(c, &(t), sizeof(int));
		c = c + 4;

		int n = tuple.getData().size();
		for (int i = 0; i < n; i++) {
			tuple.getData()[i].elementToChar(c);
			c = c + tuple.getData()[i].length;
		}
	}
	Tuple getTupleByIndex(Table* table, int index);

};

#endif