#ifndef _API1_H_
#define _API1_H_
#include<iostream>
#include<string>
#include<vector>
#include"catalog_manager/CatalogManager.h"
#include"catalog_manager/Address.h"
#include"catalog_manager/attribute.h"
#include"catalog_manager/Index.h"
#include"catalog_manager/Table.h"
#include"catalog_manager/FieldType.h"
#include"IndexManager/IndexManager.h"
#include"RecordManager/RecordManager.h"
#include"buffer/buffer.h"
using namespace std;
//全局变量，interpreter或者main用的时候extern一下
string dbname;
CatalogManager catalog_manager;
IndexManager index_manager;
RecordManager record_manager;
//备注：没跟interpreter模块耦合，record中两个函数createtable和droptable需要加一下。其它有问题可以随时找我
class API {
public:
	void initial() {
		catalog_manager.initial_catalog();
		//index_manager.initial_index();
		//BufferManager.initial_buffer();
	}
	void store() {
		catalog_manager.store_catalog();
		//record_manager.store_record();
	}
	bool create_table(string tabName, Table tab) {
		if (record_manager.createTable(tabName) && catalog_manager.create_table(tab)) {
			string indexName = tabName + "_index";  //refactor index name
			string attributeName = catalog_manager.get_primary_key(tabName);
			Index index = Index(indexName, tabName, attributeName);
			if (catalog_manager.create_index(index) == false) return false; //create index on Catalog Manager
			int type = catalog_manager.get_type_for_match_IndexManager(tabName, attributeName);
			if (index_manager.create_index(dbname, tabName, catalog_manager.get_primary_key(tabName), type) == false) return false; //create index on Index Manager
			return true;
		}
		else return false;
	}
	bool drop_table(string tabName) {
		for (int i = 0; i < catalog_manager.get_attribute_num(tabName); i++) {
			string attrName = catalog_manager.get_attribute_name(tabName, i);
			string indexName = catalog_manager.get_index_name(tabName, attrName);  //find index if exists
			if (!indexName.empty()) {
				int type = catalog_manager.get_type_for_match_IndexManager(tabName, attrName);
				if (index_manager.drop_index(dbname, tabName, attrName, type) == false) return false; //drop index at Index Manager
			}
		}
		if (catalog_manager.drop_table(tabName) && record_manager.dropTable(tabName)) return true;
		else return false;
	}
	bool create_index(Index index) {
		string tabName = index.tableName;
		string attrName = index.attributeName;
		int type = catalog_manager.get_type_for_match_IndexManager(tabName, attrName);
		if (index_manager.create_index(dbname, tabName, attrName, type) && catalog_manager.create_index(index))
			return true;
		else return false;
	}
	bool drop_index(string indexName) {
		Index index = catalog_manager.get_index(indexName);
		string tabName = index.tableName;
		string attrName = index.attributeName;
		int type = catalog_manager.get_type_for_match_IndexManager(tabName, attrName);
		if (index_manager.drop_index(dbname, tabName, attrName, type) && catalog_manager.drop_index(indexName))
			return true;
		else return false;
	}
	bool insert_row(string tabName, Tuple row) {
		return record_manager.insertValue(tabName, row);
	}
	/*
	SelectCondition find_index_condition(string tabName, vector<SelectCondition> conditions){
		SelectCondition onecondition;
		onecondition.attributeIndex = -1;//为空
		for (int i = 0; i < conditions.size(); i++) {
			int attrid = conditions[i].attributeIndex;
			string attrName=catalog_manager.get_attribute_name(tabName,attrid);
			if (!(catalog_manager.get_index_name(tabName, attrName)).empty()) {
				onecondition = conditions[i];
				conditions.erase(conditions.begin()+i);
				break;
			}
		}
		return onecondition;
	}
	*/
	int delete_row(string tabName, vector<SelectCondition> conditions) {
		int numberOfRecords = 0;
		numberOfRecords = record_manager.deleteTuple(tabName, conditions);
		catalog_manager.delete_row_num(tabName, numberOfRecords);
		return numberOfRecords;
	}
	vector<Tuple> select(string tabName, vector<SelectCondition> conditions) {
		vector<Tuple> resultSet;
		resultSet = record_manager.searchQuery(tabName, conditions);
		return resultSet;
	}
	vector<Tuple> select_all(string tabName) {
		vector<Tuple> resultSet;
		resultSet = record_manager.searchQuery(tabName);
		return resultSet;
	}

};

#endif // !_API1_H_



