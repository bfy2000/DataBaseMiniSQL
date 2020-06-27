#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include<iostream>
#include<fstream>
#include <sstream>
#include<algorithm>
#include<map>
#include<string>
#include<exception>
#include"../IndexManager/IndexManager.h"
#include"Index.h"
#include"../Public/Attribute.h"
#include"../Public/Table.h"
#include"Address.h"
#include"../Public/FieldType.h"
#include"../Public/Base.h"
/*Catalog Manager模块介绍：
	整个CatalogManager类就是Catalog manager的核心
	主要功能就是维护了两个map，存储table和index的一些模式信息
	在数据库启动的时候把相关的模式信息读入内存，关闭时再将更新的信息存入硬盘
	因为两个文件比较小，Catalog模块采用直接访问磁盘文件的形式，不通过Buffer Manager，Catalog中的数据也不要求分块存储。
	存储的模式信息如下：
		- 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。
		- 表中每个字段的定义信息，包括字段类型、是否唯一等。
		- 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。
*/

struct tableAttrName{
	string tableName;
	string attributeName;
};

class CatalogManager {
private:
	map<string, Table> tables;
	map<string, Index> indexes;
	map<string, string> table_attr_To_index;
	string tableFilename = "table_catalog";
	string indexFilename = "index_catalog";


	//从 table_catalog文件中读取所有模式信息并插入到map tables中
	Result initial_table() {
		ifstream file;
		file.open(tableFilename, ios::in);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return TABLE_CATALOG_FILE_READ_ERROR;
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
					else if(tmpType == "INT")  ex = INT;
					tmpNumType = ex;
					Attribute temp_attribute(tmpAttributeName, tmpNumType, tmpLength, tmpIsUnique);
					tmpAttributeVector.push_back(temp_attribute);
				}
				Table temp_table(tmpTableName, tmpPrimaryKey, tmpAttributeVector, tmpIndexVector, tmpRowNum);
				tables.insert(make_pair(tmpTableName, temp_table));
			}
			file.close();
		}
		return SUCCESS;
	}
	//从 index_catalog文件中读取所有模式信息并插入到map indexes中
	Result initial_index() {
		ifstream file;
		file.open(indexFilename, ios::in);
		if (!file)
		{
			cout << "file " << indexFilename << " fail to open!" << endl;
			file.close();
			return INDEX_CATALOG_FILE_READ_ERROR;
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
		return SUCCESS;
	}
	//将map tables中的内容写入table_catalog文件
	Result store_table() {
		ofstream file;
		file.open(tableFilename, ios::out, ios::trunc);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return TABLE_CATALOG_FILE_WRITE_ERROR;
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
		return SUCCESS;
	}
	//将map indexes中的内容写入index_catalog文件
	Result store_index() {
		ofstream file;
		file.open(tableFilename, ios::out, ios::trunc);
		if (!file)
		{
			cout << "file " << tableFilename << " fail to open!" << endl;
			file.close();
			return INDEX_CATALOG_FILE_WRITE_ERROR;
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
		return SUCCESS;
	}
	

public:
	//——————————————————————————————————————————
	//一些比较常用的函数：

	//初始化catalog，在程序开始之前需要调用，就是把硬盘中关于table和index的相关信息读入内存 
	//成功：SUCCESS=0；
	//失败：TABLE_CATALOG_FILE_READ_ERROR=-105 INDEX_CATALOG_FILE_READ_ERROR=-104
	Result initial_catalog(){
		Result res;
		res=initial_table();
		if (res != SUCCESS) return res;
		res=initial_index(); 
		return res;
	}

	//操作结束后，把在内存中修改的关于table和index的相关信息写入磁盘中
	//成功：SUCCESS=0；
	//失败：TABLE_CATALOG_FILE_WRITE_ERROR=-103 INDEX_CATALOG_FILE_WRITE_ERROR=-102
	Result store_catalog() {
		Result res;
		res=store_table();
		if (res != SUCCESS) return res;
		res=store_index();
		return res;
	}

	//新建一个表，将这个表push进map tables里
	//成功：SUCCESS=0；
	//失败：TABLE_NAME_EXSITED=-110
	Result create_table(string tableName, vector<Attribute> attrList){
		return create_table(Table(tableName, attrList));
	}
	Result create_table(Table newTable) {
		Result res= SUCCESS;

		if (is_table_exist(newTable.tableName) == true) return TABLE_NAME_EXSITED;
		tables.insert(make_pair(newTable.tableName, newTable));
		return res;
	}

	//删除一个表（删除它的所有模式信息记录）
	//成功：SUCCESS=0;
	//失败：TABLE_NAME_NOEXSIT=-109
	Result drop_table(string tableName) {
		Result res = SUCCESS;
		if (is_table_exist(tableName) == false) return TABLE_NAME_NOEXSIT;
		Table tmpTable = tables[tableName];
		for (int i = 0; i < tmpTable.indexVector.size(); i++) {
			indexes.erase(tmpTable.indexVector[i].indexName);
		}
		tables.erase(tableName);
		return res;
	}

	//新建一个index，将这个索引push进map indexes里，并且改变相应table的信息
	//成功：SUCCESS
	//失败：INDEX_NAME_EXSITED=-108 TABLE_NAME_NOEXSIT=-109
	Result create_index(string indexName, string tableName, string attrName){
		return create_index(Index(indexName, tableName, attrName));
	}
	Result create_index(Index newIndex) {
		Result res = SUCCESS;
		if (is_index_exist(newIndex.indexName) == true) return INDEX_NAME_EXSITED;
		if (is_table_exist(newIndex.tableName) == false) return TABLE_NAME_NOEXSIT;
		Table tmpTable = tables[newIndex.tableName];
		tmpTable.indexVector.push_back(newIndex);
		tmpTable.indexNum = tmpTable.indexVector.size();
		indexes.insert(make_pair(newIndex.indexName, newIndex));

		//为了方便查找
		table_attr_To_index.insert(make_pair(newIndex.tableName+newIndex.attributeName, newIndex.indexName));

		return res;
	}

	//删除一个index（删除它的所有模式信息记录，并改变相应table信息）
	//成功：SUCCESS
	//失败：INDEX_NAME_NOEXSIT=-107 TABLE_NAME_NOEXSIT=-109
	Result drop_index(string indexName) {
		Result res=SUCCESS;
		if (is_index_exist(indexName) == false) return INDEX_NAME_NOEXIST;
		Index tmpIndex = get_index(indexName);
		if(is_table_exist(tmpIndex.tableName) == false) return TABLE_NAME_NOEXSIT;
		Table tmpTable = tables[tmpIndex.tableName];
		remove(tmpTable.indexVector.begin(), tmpTable.indexVector.end(), tmpIndex);
		//tmpTable.indexVector.remove(tmpIndex);
		tmpTable.indexNum = tmpTable.indexVector.size();
		indexes.erase(indexName);
		
		table_attr_To_index.erase(tmpIndex.tableName+tmpIndex.attributeName);

		return res;
	}

	//更新一个index，把名字时indexName的索引更新为tmpIndex
	//成功：SUCCESS
	//失败：INDEX_NAME_NOEXSIT=-107 
	Result update_index_table(string indexName, Index tmpIndex) {
		Result res = SUCCESS;
		map<string, Index>::iterator iter = indexes.find(indexName);
		if (iter == indexes.end()) return INDEX_NAME_NOEXIST;
		indexes.erase(iter);
		indexes[indexName] = tmpIndex;
		return res;
	}

	//给某个table的行数+1
	//成功：SUCCESS=0;
	//失败：TABLE_NAME_NOEXSIT=-109
	Result add_row_num(string tableName) {
		Result res = SUCCESS;
		if (is_table_exist(tableName) == false) return TABLE_NAME_NOEXSIT;
		tables[tableName].rowNum++;
		return res;
	}
	//给某个table的行数-num
	//成功：SUCCESS=0;
	//失败：TABLE_NAME_NOEXSIT=-109
	Result delete_row_num(string tableName, int num) {
		Result res = SUCCESS;
		if (is_table_exist(tableName) == false) return TABLE_NAME_NOEXSIT;
		tables[tableName].rowNum -= num;
		return res;
	}

	//——————————————————————————————————————————
	//一些可能用到的check函数：

	//判断某个字段是不是主键
	bool is_primary_key(string tableName, string attributeName) {
		if (tables.find(tableName) != tables.end()) {
			Table tmpTable = tables[tableName];
			return tmpTable.primaryKey == attributeName;
		}
		else {
			cout << "The table " << tableName << " doesn't exist" << endl;
			return false;
		}
	}
	//判断某个字段是否唯一
	bool is_unique(string tableName, string attributeName) {
		if (tables.find(tableName) != tables.end()) {
			Table tmpTable = tables[tableName];
			for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
				Attribute tmpAttribute = tmpTable.attributeVector[i];
				if (tmpAttribute.attributeName == attributeName) {
					return tmpAttribute.isUnique;
				}
			}
			//if (i >= tmpTable.attributeVector.size()) {
			cout << "The attribute " << attributeName << " doesn't exist" << endl;
			return false;
			//}
		}
		cout << "The table " << tableName << " doesn't exist" << endl;
		return false;
	}
	//判断第i个字段是否唯一
	bool is_unique(string tableName, int i) {
		return is_unique(tableName, get_attribute_name(tableName, i));
	}
	//判断某个字段有没有索引
	bool is_index_key(string tableName, string attributeName) {
		if (tables.find(tableName) != tables.end()) {
			Table tmpTable = tables[tableName];
			if (is_attribute_exist(tableName, attributeName)) {
				for (int i = 0; i < tmpTable.indexVector.size(); i++) {
					if (tmpTable.indexVector[i].attributeName == attributeName)
						return true;
				}
			}
			else {
				cout << "The attribute " << attributeName << " doesn't exist" << endl;
			}
		}
		else
			cout << "The table " << tableName << " doesn't exist" << endl;
		return false;
	}
	//判断某一个表是否存在
	bool is_table_exist(string tableName) {
		if (tables.find(tableName) == tables.end())
		{
			cout << "The table " << tableName << " doesn't exist" << endl;
			return false;
		}
		else return true;
	}
	//判断某一个index是否存在
	bool is_index_exist(string tableName, string attrName){
		return is_index_exist(table_attr_To_index[tableName+attrName]);
	}
	bool is_index_exist(string indexName) {
		if (indexes.find(indexName) == indexes.end()) return false;
		else return true;
	}
	//判断某一个字段名是否存在
	bool is_attribute_exist(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			if (tmpTable.attributeVector[i].attributeName == attributeName)
				return true;
		}
		return false;
	}
	bool is_attribute_exist(vector<Attribute> attributeVector, string attributeName) {
		for (int i = 0; i < attributeVector.size(); i++) {
			if (attributeVector[i].attributeName == attributeName)
				return true;
		}
		return false;
	}
	
	//——————————————————————————————————————————
	//一些只读函数，可以得到相应的信息(按函数名可知功能）
	int get_type_for_match_IndexManager(string &tableName, int i) {
		FieldType ft = get_attribute_type(tableName, i);
		if (ft.get_type() == INT) {
			return 0;//参考IndexManager.find_element(), 0 表示 int
		}
		else if (ft.get_type() == FLOAT) {
			return -1;//参考IndexManager.find_element(), -1 表示 float
		}
		else if (ft.get_type() == CHAR) {
			return ft.get_length();//参考IndexManager.find_element(), 大于0 表示 char串 且代表长度
		}
		return -2;
	}
	int get_type_for_match_IndexManager(string &tableName, string &attributeName) {
		FieldType ft = get_attribute_type(tableName, attributeName);
		if (ft.get_type() == INT) {
			return 0;//参考IndexManager.find_element(), 0 表示 int
		}
		else if (ft.get_type() == FLOAT) {
			return -1;//参考IndexManager.find_element(), -1 表示 float
		}
		else if (ft.get_type() == CHAR) {
			return ft.get_length();//参考IndexManager.find_element(), 大于0 表示 char串 且代表长度
		}
		return -2;
	}
	int get_max_attr_length(Table tab) {
		int len = 9;//the size of "ATTRIBUTE"
		for (int i = 0; i < tab.attributeVector.size(); i++) {
			int v = tab.attributeVector[i].attributeName.length();
			len = v > len ? v : len;
		}
		return len;
	}
	Table get_table(string tableName) {
		return tables[tableName];
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
	string get_index_name(string tableName, string attributeName) {
		if (tables.find(tableName) != tables.end()) {
			Table tmpTable = tables[tableName];
			if (is_attribute_exist(tableName, attributeName)) {
				for (int i = 0; i < tmpTable.indexVector.size(); i++) {
					if (tmpTable.indexVector[i].attributeName == attributeName)
						return tmpTable.indexVector[i].indexName;
				}
			}
			else {
				cout << "The attribute " << attributeName << " doesn't exist" << endl;
			}
		}
		else
			cout << "The table " << tableName << " doesn't exist" << endl;
		return "";
	}
	string get_attribute_name(string tableName, int i) {
		return tables[tableName].attributeVector.at(i).attributeName;
	}
	int get_attribute_index(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName == attributeName)
				return i;
		}
		cout << "The attribute " << attributeName << " doesn't exist" << endl;
		return -1;
	}
	FieldType get_attribute_type(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName == attributeName)
				return tmpAttribute.type;
		}
		cout << "The attribute " << attributeName << " doesn't exist" << endl;
		return FieldType();
	}
	FieldType get_attribute_type(string tableName, int i) {
		return get_attribute_type(tableName, get_attribute_name(tableName, i));
	}
	int get_length(string tableName, string attributeName) {
		Table tmpTable = tables[tableName];
		Attribute tmpAttribute;
		for (int i = 0; i < tmpTable.attributeVector.size(); i++) {
			tmpAttribute = tmpTable.attributeVector.at(i);
			if (tmpAttribute.attributeName == attributeName)
				return tmpAttribute.type.get_length();
		}
		cout << "The attribute " << attributeName << " doesn't exist" << endl;
		return -1;
	}
	string get_type(string tableName, int i) {
		NumType type = tables[tableName].attributeVector.at(i).type.get_type();
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


	//——————————————————————————————————————————
	//一些Debug时可能需要用到的函数：

	//一些show函数（按函数名可知功能）
	void show_catalog() {
		show_table();
		cout << endl;
		show_index();
	}
	void show_table() {
		Table tmpTable;
		Attribute tmpAttribute;
		map<string, Table>::iterator iter = tables.begin();
		while (iter != tables.end()) {
			tmpTable = iter->second;
			cout << "[TABLE] " << tmpTable.tableName << endl;
			stringstream ss;
			ss << get_max_attr_length(tmpTable);
			string s = ss.str();
			string format = "|%-" + s + "s";
			format += "|%-5s|%-6s|%-6s|\n";
			cout << format << "ATTRIBUTE" << "TYPE" << "LENGTH" << "UNIQUE" << endl;
			for (int i = 0; i < tmpTable.attributeNum; i++) {
				tmpAttribute = tmpTable.attributeVector[i];
				cout << format << tmpAttribute.attributeName << tmpAttribute.type.get_type() << tmpAttribute.type.get_length() << tmpAttribute.isUnique << endl;
			}
			if (iter != tables.end()) cout << "--------------------------------" << endl;
			iter++;
		}
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

};

#endif // !_CATALOGMANAGER_H_


