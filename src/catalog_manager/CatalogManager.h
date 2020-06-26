#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include<iostream>
#include<fstream>
#include <sstream>
#include<algorithm>
#include<map>
#include<string>
#include<exception>
#include"Index.h"
#include"Attribute.h"
#include"../RecordManager/Table.h"
#include"Address.h"
#include"FieldType.h"
class CatalogManager {
private:
	map<string, Table> tables;
	map<string, Index> indexes;
	string tableFilename = "table_catalog";
	string indexFilename = "index_catalog";


	//从 table_catalog文件中读取所有模式信息并插入到map tables中
	void initial_table() {
		ifstream file;
		file.open(tableFilename, ios::in);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return;
		}
		else
		{
			string tmpTableName, tmpPrimaryKey;
			int tmpIndexNum, tmpAttributeNum, tmpRowNum;
			while (!file.eof())
			{
				vector<Attribute> tmpAttributeVector = vector<Attribute>();
				vector<Index> tmpIndexVector = vector<Index>();
				getline(file, tmpTableName);
				getline(file, tmpPrimaryKey);
				file >> tmpRowNum;
				file >> tmpIndexNum;
				for (int i = 0; i < tmpIndexNum; i++) {
					string tmpIndexName, tmpAttributeName;
					getline(file, tmpIndexName);
					getline(file, tmpAttributeName);
					Index temp(tmpIndexName, tmpTableName, tmpAttributeName);
					tmpIndexVector.push_back(temp);
				}
				file >> tmpAttributeNum;
				for (int i = 0; i < tmpAttributeNum; i++) {
					string tmpAttributeName, tmpType;
					NumType tmpNumType;
					int tmpLength;
					bool tmpIsUnique;
					getline(file,tmpAttributeName);
					getline(file, tmpType);
					file >> tmpLength;
					file >> tmpIsUnique;
					enum NumType ex;
					if (tmpType == "CHAR")	 ex = CHAR;
					else if (tmpType == "FLOAT")  ex = FLOAT;
					else  ex = INT;
					tmpNumType = ex;
					Attribute temp_attribute(tmpAttributeName, tmpNumType, tmpLength, tmpIsUnique);
					tmpAttributeVector.push_back(temp_attribute);
				}
				Table temp_table(tmpTableName, tmpPrimaryKey, tmpAttributeVector, tmpIndexVector, tmpRowNum);
				tables.insert(make_pair(tmpTableName, temp_table));
			}
			file.close();
		}

	}
	void initial_index() {
		ifstream file;
		file.open(indexFilename, ios::in);
		if (!file)
		{
			cout << "file " << indexFilename << " fail to open!" << endl;
			file.close();
			return;
		}
		else
		{
			string tmpIndexName, tmpTableName, tmpAttributeName;
			int tmpBlockNum, tmpRootNum;
			while (!file.eof()) {
				getline(file, tmpIndexName);
				getline(file, tmpTableName);
				getline(file, tmpAttributeName);
				file >> tmpBlockNum;
				file >> tmpRootNum;
				Index temp_index(tmpIndexName, tmpTableName, tmpAttributeName, tmpBlockNum, tmpRootNum);
				indexes.insert(make_pair(tmpIndexName, temp_index));
			}
			file.close();
		}
	}
	void store_table() {
		ofstream file;
		file.open(tableFilename, ios::out | ios::trunc);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return;
		}
		else
		{
			Table tmpTable;
			map<string, Table>::iterator iter = tables.begin();
			while (iter != tables.end())
			{
				tmpTable = iter->second;
				file << tmpTable.tableName << endl;
				file << tmpTable.primaryKey << endl;
				file << tmpTable.rowNum;
				file << tmpTable.indexNum;
				for (int i = 0; i < tmpTable.indexNum; i++) {
					Index tmpIndex = tmpTable.indexVector.at(i);
					file << tmpIndex.indexName << endl;
					file << tmpIndex.attributeName << endl;
				}
				file<<tmpTable.attributeNum;
				for (int i = 0; i < tmpTable.attributeNum; i++) {
					Attribute tmpAttribute = tmpTable.attributeVector.at(i);
					file << tmpAttribute.attributeName << endl;
					NumType temp_type = tmpAttribute.type.get_type();
					string s;
					switch (temp_type) {
					case CHAR:
						s= "CHAR";
					case INT:
						s= "INT";
					case FLOAT:
						s= "FLOAT";
					}
					file << s << endl;
					file << tmpAttribute.type.get_length();
					file << tmpAttribute.isUnique;
				}
				iter++;
			}
			file.close();
		}
	}
	void store_index() {
		ofstream file;
		file.open(tableFilename, ios::out | ios::trunc);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return;
		}
		else
		{
			Index tmpIndex;
			map<string, Index>::iterator iter = indexes.begin();
			while (iter!=indexes.end()) {
				tmpIndex = iter->second;				
				file << tmpIndex.indexName << endl;
				file << tmpIndex.tableName << endl;
				file << tmpIndex.attributeName << endl;
				file << tmpIndex.blockNum;
				file << tmpIndex.rootNum;
				iter++;
			}
			file.close();
		}
	}
	bool is_index_exist(string indexName) {
		if(indexes.find(indexName)==indexes.end()) return false;
		else return true;
	}
	bool is_attribute_exist(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			if (tmpTable.attributeVector[i].attributeName==attributeName)
				return true;
		}
		return false;
	}

public:
	//initialize Catalog，read all the information about schemes from files table_catalog and index_catalog 
	void initial_catalog(){
		initial_table();
		initial_index();
	}

	void store_catalog() {
		store_table();
		store_index();
	}

	void show_catalog() {
		show_table();
		cout << endl;
		show_index();
	}

	void show_index() {
		Index tmpIndex;
		map<string, Index>::iterator iter=indexes.begin();
		int idx = 5, tab = 5, attr = 9;
		//cout<<"There are " + indexes.size() + " indexes in the database: "<<endl;
		while (iter!=indexes.end()) {
			tmpIndex = iter->second;
			idx = tmpIndex.indexName.length() > idx ? tmpIndex.indexName.length() : idx;
			tab = tmpIndex.tableName.length() > tab ? tmpIndex.tableName.length() : tab;
			attr = tmpIndex.attributeName.length() > attr ? tmpIndex.attributeName.length() : attr;
			iter++;
		}
		string idxs, tabs, attrs;
		stringstream ss;
		ss << idx;
		idxs = ss.str();
		ss << tab;
		tabs = ss.str();
		ss << attr;
		attrs = ss.str();
		string format = "|%-" + idxs + "s|%-" + tabs + "s|%-" + attrs + "s|\n";
		iter = indexes.begin();
		cout<<format<<"INDEX"<<"TABLE"<<"ATTRIBUTE"<<endl;
		while (iter != indexes.end()) {
			tmpIndex = iter->second;
			cout<<format<< tmpIndex.indexName<< tmpIndex.tableName<< tmpIndex.attributeName<<endl;
			iter++;
		}
	}

	int get_max_attr_length(Table tab) {
		int len = 9;//the size of "ATTRIBUTE"
		for (int i = 0; i < tab.attributeVector.size(); i++) {
			int v = tab.attributeVector[i].attributeName.length();
			len = v > len ? v : len;
		}
		return len;
	}

	void show_table() {
		Table tmpTable;
		Attribute tmpAttribute;
		map<string, Table>::iterator iter = tables.begin();
		while (iter!=tables.end()) {
			tmpTable = iter->second;
			cout<<"[TABLE] " << tmpTable.tableName<<endl;
			stringstream ss;
			ss << get_max_attr_length(tmpTable);
			string s = ss.str();
			string format = "|%-" + s + "s";
			format += "|%-5s|%-6s|%-6s|\n";
			cout<<format<< "ATTRIBUTE"<< "TYPE"<< "LENGTH"<< "UNIQUE"<<endl;
			for (int i = 0; i < tmpTable.attributeNum; i++) {
				tmpAttribute = tmpTable.attributeVector[i];
				cout<<format<< tmpAttribute.attributeName<< tmpAttribute.type.get_type()<< tmpAttribute.type.get_length()<< tmpAttribute.isUnique<<endl;
			}
			if (iter != tables.end()) cout<<"--------------------------------"<<endl;
			iter++;
		}
	}

	Table* get_table(string tableName) {
		return &(tables[tableName]);
	}

	Index get_index(string indexName) {
		return indexes[indexName];
	}

	string get_primary_key(string tableName) {
		return tables[tableName].primaryKey;
	}

	int get_row_length(string tableName) {
		return tables[tableName].rowLength;
	}

	int get_attribute_num(string tableName) {
		return tables[tableName].attributeNum;
	}

	int get_row_num(string tableName) {
		return tables[tableName].rowNum;
	}

	//check
	bool is_primary_key(string tableName, string attributeName) {
		if (tables.find(tableName)!=tables.end()) {
			Table tmpTable = tables[tableName];
			return tmpTable.primaryKey==attributeName;
		}
		else {
			cout<<"The table " << tableName << " doesn't exist"<<endl;
			return false;
		}
	}

	bool is_unique(string tableName, string attributeName) {
		if (tables.find(tableName)!=tables.end()) {
			Table tmpTable = tables[tableName];
			for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
				Attribute tmpAttribute = tmpTable.attributeVector[i];
				if (tmpAttribute.attributeName==attributeName) {
					return tmpAttribute.isUnique;
				}
			}
			//if (i >= tmpTable.attributeVector.size()) {
			cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
			return false;
			//}
		}
		cout<<"The table " << tableName << " doesn't exist"<<endl;
		return false;
	}

	bool is_unique(string tableName, int i){
		return is_unique(tableName, get_attribute_name(tableName, i));
	}

	bool drop_table(string tableName)  {
		Table tmpTable = tables[tableName];
		for (int i = 0; i < tmpTable.indexVector.size(); i++) {
			indexes.erase(tmpTable.indexVector[i].indexName);
		}
		tables.erase(tableName);
		return true;
	}

	bool is_index_key(string tableName, string attributeName) {
		if (tables.find(tableName)!=tables.end()) {
			Table tmpTable = tables[tableName];
			if (is_attribute_exist(tableName, attributeName)) {
				for (int i = 0; i < tmpTable.indexVector.size(); i++) {
					if (tmpTable.indexVector[i].attributeName==attributeName)
						return true;
				}
			}
			else {
				cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
			}
		}
		else
			cout<<"The table " << tableName << " doesn't exist"<<endl;
		return false;
	}

	string get_index_name(string tableName, string attributeName) {
		if (tables.find(tableName)!=tables.end()) {
			Table tmpTable = tables[tableName];
			if (is_attribute_exist(tableName, attributeName)) {
				for (int i = 0; i < tmpTable.indexVector.size(); i++) {
					if (tmpTable.indexVector[i].attributeName==attributeName)
						return tmpTable.indexVector[i].indexName;
				}
			}
			else {
				cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
			}
		}
		else
			cout<<"The table " << tableName << " doesn't exist"<<endl;
		return 0;
	}

	string get_attribute_name(string tableName, int i) {
		return tables[tableName].attributeVector.at(i).attributeName;
	}

	int get_attribute_index(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName==attributeName)
				return i;
		}
		cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
		return -1;
	}

	FieldType get_attribute_type(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName==attributeName)
				return tmpAttribute.type;
		}
		cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
		return FieldType();
	}

	FieldType get_attribute_type(string tableName, int i){
		return get_attribute_type(tableName, get_attribute_name(tableName, i));
	}

	int get_length(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName==attributeName)
				return tmpAttribute.type.get_length();
		}
		cout<<"The attribute " << attributeName << " doesn't exist"<<endl;
		return -1;
	}

	string get_type(string tableName, int i) {
		NumType type=tables[tableName].attributeVector.at(i).type.get_type();
		switch (type) {
		case CHAR:
			return "CHAR";
		case INT:
			return "INT";
		case FLOAT:
			return "FLOAT";
		}
	}

	int get_length(string tableName, int i) {
		return tables[tableName].attributeVector.at(i).type.get_length();
	}

	void add_row_num(string tableName) {
		tables[tableName].rowNum++;
	}

	void delete_row_num(string tableName, int num) {
		tables[tableName].rowNum -= num;
	}

	bool update_index_table(string indexName, Index tmpIndex) {
		map<string, Index>::iterator iter = indexes.find(indexName);
		indexes.erase(iter);
		indexes[indexName] = tmpIndex;
		return true;
	}

	bool is_attribute_exist(vector<Attribute> attributeVector, string attributeName) {
		for (int i = 0; i < attributeVector.size(); i++) {
			if (attributeVector[i].attributeName==attributeName)
				return true;
		}
		return false;
	}

	bool create_table(Table newTable) {
		tables.insert(make_pair(newTable.tableName, newTable));
		//indexes.put(newTable.indexes.firstElement().indexName, newTable.indexes.firstElement());
		return true;
	}

	bool create_index(Index newIndex) {
		Table tmpTable = tables[newIndex.tableName];
		tmpTable.indexVector.push_back(newIndex);
		tmpTable.indexNum = tmpTable.indexVector.size();
		indexes.insert(make_pair(newIndex.indexName, newIndex));
		return true;
	}

	bool drop_index(string indexName) {
		Index tmpIndex = get_index(indexName);
		Table tmpTable = tables[tmpIndex.tableName];
		remove(tmpTable.indexVector.begin(), tmpTable.indexVector.end(), tmpIndex);
		//tmpTable.indexVector.remove(tmpIndex);
		tmpTable.indexNum = tmpTable.indexVector.size();
		indexes.erase(indexName);
		return true;
	}

	int get_type_for_match_IndexManager(string &tableName, int i){
		FieldType ft = get_attribute_type(tableName, i);
		if(ft.get_type() == INT){
			return 0;//参考IndexManager.find_element(), 0 表示 int
		} else if(ft.get_type() == FLOAT){
			return -1;//参考IndexManager.find_element(), -1 表示 float
		} else if(ft.get_type() == CHAR){
			return ft.get_length();//参考IndexManager.find_element(), 大于0 表示 char串 且代表长度
		}
		return -2;
	}
};

#endif // !_CATALOGMANAGER_H_


