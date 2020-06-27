#include <vector>
#include <cctype>
#include <sstream>
#include <iostream>
#include <regex>
#include <string>
#include "../Utils/Utils.h"
#include "API.h"

using namespace std;

//* INSERT INTO ...
void InsertQuery::SetTableName(string tablename) {
	insert_table_name = tablename;
}

void InsertQuery::Clear() {
	insert_table_name = "";
	vector<InsertVal>().swap(insert_values);
}

void InsertQuery::Insert(string insert_val) {
	NumType this_type = DEFAULT;
	string val;
	if (insert_val[0] == '\\') {
		this_type = CHAR;
		insert_val.erase(0,1);
		vector<string> parse_vec = split(insert_val, '\\');
		for (int i = 0; i < parse_vec.size(); i++) {
			unsigned int each_char_val;
			stringstream ss;
			ss << hex << parse_vec[i];
			ss >> each_char_val;
			val.push_back(each_char_val);
		}
	}
	else if (is_integer(insert_val)) {
		this_type = INT;
		val = insert_val;
	}
	else if (is_float(insert_val)) {
		this_type = FLOAT;
		val = insert_val;
	}
	else
		val = insert_val;
		
	insert_values.push_back(InsertVal(val, this_type));
}

Result InsertQuery::Query(CatalogManager& catalog_manager, RecordManager& record_manager, IndexManager& index_manager) {	
	vector<pair<NumType, string>> insert_values_to_records;
	for (int i = 0; i < insert_values.size(); i++) {
		insert_values_to_records.push_back(pair<NumType, string>(NumType(insert_values[i].given_type), insert_values[i].val));
	}

	// 判断有没有index
	int hasIndex = 0;
	for(int i=0; i<catalog_manager.get_attribute_num(insert_table_name); i++){
		if(catalog_manager.is_index_exist(insert_table_name, catalog_manager.get_attribute_name(insert_table_name, i)))
			hasIndex++;
	}

	// 插入record数据
	Table table = catalog_manager.get_table(insert_table_name);
	int r = record_manager.insertValue(&table, insert_values_to_records, hasIndex, index_manager);
	if(r < 0){
		cerr << "API: 插入数据出错" << endl;
		return ERROR;
	}
	if(r==1){
		cout << "API: 插入 1 条数据" << endl;
		catalog_manager.add_row_num(insert_table_name);
	}

	// 插入index数据
	if(hasIndex > 0){ // 表示这个表有索引
		for(int i=0; i<catalog_manager.get_attribute_num(insert_table_name); i++){
			if(catalog_manager.is_index_exist(insert_table_name, catalog_manager.get_attribute_name(insert_table_name, i)))
				index_manager.insert_index(DB_NAME, insert_table_name, catalog_manager.get_attribute_name(insert_table_name, i),
				    catalog_manager.get_attribute_type(insert_table_name, i), insert_values_to_records[i].second,
						catalog_manager.get_table(insert_table_name).rowNum);
		}
	}
	return SUCCESS;
}




//------------------------------------------------------------------------------







//* DROP INDEX ...
void API::DropIndex(string drop_index) {
	Prompt("Drop Index");
}




//------------------------------------------------------------------------------------




//* DROP TABLE ...
void API::DropTable(string drop_table) {
	Prompt("Drop Table");
}





//------------------------------------------------------------------------------------------





//* CREATE INDEX ...
void API::CreateIndex(string index_name, string table_name, string attr_name) {
	Prompt("Create Index\t" + index_name + "\t" + table_name + "\t" + attr_name);
	catalog_manager.create_index(index_name, table_name, attr_name);
	index_manager.create_index(DB_NAME, table_name, attr_name, catalog_manager.get_attribute_type(table_name, attr_name));
}




//--------------------------------------------------------------------------------------------






//* CREATE TABLE ...

void CreateTable::SetTableName(string &given_table_name) {
	tmp_table_name = given_table_name;
}

void CreateTable::InsertAttr(string &attr_name) {
	int n = attr_list.size();
	attr_list.push_back(Attribute());
	attr_list[n].attributeName = attr_name;
}

void CreateTable::InsertType(NumType given_type) {
	int n = attr_list.size() - 1;
	attr_list[n].type.set_type(given_type);
}

void CreateTable::InsertSize(int given_size) {
	int n = attr_list.size() - 1;
	attr_list[n].type.set_length(given_size);
}

void CreateTable::InsertUnique() {
	int n = attr_list.size() - 1;
	attr_list[n].isUnique = true;
}

void CreateTable::InsertPrimary(string& attr_name) {
	primary_list.push_back(attr_name);
}

void CreateTable::Clear() {
	tmp_table_name = "";
	vector<Attribute>().swap(attr_list);
	vector<string>().swap(primary_list);
}

int CreateTable::Query(CatalogManager& catalog_manager, IndexManager& index_manager) {
	Prompt("Create Table");
	if(catalog_manager.create_table(tmp_table_name, attr_list) < 0){
		cerr << "API: 建立数据表失败 0" << endl;
		return ERROR;
	}
	for(int i=0; i<primary_list.size(); i++){//循环创建索引
		if(catalog_manager.create_index(tmp_table_name+"_primary", tmp_table_name, primary_list[i]) < 0){
			cerr << "API: 建立索引失败 1" << endl;
			return ERROR;
		}

		FieldType f;
		for(int j=0; j<attr_list.size(); j++){
			if(attr_list[j].attributeName == primary_list[i]){
				f = attr_list[j].type;
				break;
			}
		}
		if(index_manager.create_index(DB_NAME, tmp_table_name, primary_list[i], f) < 0){
			cerr << "API: 建立索引失败 2" << endl;
			return ERROR;
		}
	}
	return SUCCESS;
}




//--------------------------------------------------------------------------



//* SELECT * FROM ...
SelectQuery::SelectQuery() :select_all(false) {}
SelectQuery::~SelectQuery() {}

Result WhereExpr_To_SelectCondition(CatalogManager& catalog_manager, 
                                    string& tableName, vector<WhereExpr>& wheres, 
																		vector<SelectCondition>& selectConditions){
	selectConditions.clear();
	for(int i=0; i<wheres.size(); i++){
		SelectCondition s;
		s.attributeIndex = catalog_manager.get_attribute_index(tableName, wheres[i].expr1);
		s.opt = wheres[i].cmp;

		NumType t = catalog_manager.get_attribute_type(tableName, wheres[i].expr1).get_type();
		if(INT == t){
			s.value = Element(stoi(wheres[i].expr2));
		} else if(FLOAT == t){
			s.value = Element(stof(wheres[i].expr2));
		} else if(CHAR == t){
			s.value = Element(wheres[i].expr2);
		} else {
			return ATTRIBUTE_TYPE_ERROR;
		}
		
		selectConditions.push_back(s);
	}
	return SUCCESS;
}

Result SelectQuery::Query(CatalogManager& catalog_manager, RecordManager& record_manager, IndexManager& index_manager) {
	//select_all = false;
	Prompt("SELECT ...");

	vector<SelectCondition> selectConditions;
	Result r = WhereExpr_To_SelectCondition(catalog_manager, select_table_name, where_expr_list, selectConditions);
	if(r < 0){
		cerr << "Error code: " << r << endl;
		return ERROR;
	}
	
	int hasIndex = 0;
	int hasCondition = selectConditions.size();
	for(int i=0; i<hasCondition; i++){
		if(index_manager.is_index_exist(DB_NAME, select_table_name, where_expr_list[i].expr1, catalog_manager.get_attribute_type(select_table_name, selectConditions[i].attributeIndex))){
			hasIndex = 1;
			break;
		}
	}

	vector<Tuple> tuples;
	Table table = catalog_manager.get_table(select_table_name);
	if(hasCondition == 0){//无条件查询
		r = record_manager.searchQuery(&table, tuples);
	} else if(hasIndex == 0){//无索引查询
		r = record_manager.searchQueryWithoutIndex(&table, selectConditions, tuples);
	} else {//有条件有索引查询
		r = record_manager.searchQuery(&table, index_manager, selectConditions, tuples);
	}
	if(r < 0){
		cerr << "Error code: " << r << endl;
		return ERROR;
	}

	// 用来显示查询结果
	if(select_all == false){
		for(int i=0; i<select_attr_list.size(); i++){
			cout << select_attr_list[i] << "\t";
		}
		cout << endl;
		for(int i=0; i<tuples.size(); i++){
			for(int j=0; j<select_attr_list.size(); j++){
				cout << tuples[i].getData()[catalog_manager.get_attribute_index(select_table_name, select_attr_list[j])].toString() << "\t";
			}
			cout << endl;
		}
	} else {
		for(int i=0; i<catalog_manager.get_table(select_table_name).attributeNum; i++){
			cout << catalog_manager.get_table(select_table_name).attributeVector[i].attributeName << "\t";
		}
		cout << endl;
		for(int i=0; i<tuples.size(); i++){
			for(int j=0; j<catalog_manager.get_table(select_table_name).attributeNum; j++){
				cout << tuples[i].getData()[j].toString() << "\t";
			}
			cout << endl;
		}
	}

	return SUCCESS;
}

void SelectQuery::Insert(string &attr) {
	select_attr_list.push_back(attr);
}

void SelectQuery::Clear() {
	select_table_name = "";
	select_all = false;
	tmp_expr1 = "";
	vector<string>().swap(select_attr_list);
	vector<WhereExpr>().swap(where_expr_list);
}

void SelectQuery::SetSelectAll() {
	select_all = true;
}

void SelectQuery::SetSelectTable(string& table) {
	select_table_name = table;
}

void SelectQuery::SetWhereExpr(string& expr2) {
	where_expr_list.push_back(WhereExpr(tmp_expr1, tmp_cmp, expr2));
}

void SelectQuery::SetCmp(CMP in_cmp) {
	tmp_cmp = in_cmp;
}


void SelectQuery::SetExpr1(string& in_expr1) {
	tmp_expr1 = in_expr1;
}

//--------------------------------------------------------------------------


DeleteQuery::DeleteQuery() {}

DeleteQuery::~DeleteQuery() {}

Result DeleteQuery::Query(CatalogManager& catalog_manager, RecordManager& record_manager, IndexManager& index_manager) {
	Prompt("DELETE ...");
	if(!catalog_manager.is_table_exist(delete_table_name)){
		cerr << "API: 没有这个表" << endl;
	}
	vector<SelectCondition> selectConditions;
	Result r = WhereExpr_To_SelectCondition(catalog_manager, delete_table_name, where_expr_list, selectConditions);
	if(r < 0){
		cerr << "API: delete Error" << endl;
		return ERROR;
	}
	Table table = catalog_manager.get_table(delete_table_name);
	int res = record_manager.deleteTuple(&table, index_manager, selectConditions);
	cout << "API: 成功删除 " << res << " 条数据" << endl;
	return SUCCESS;
}

void DeleteQuery::Clear() {
	delete_table_name = "";
	vector<WhereExpr>().swap(where_expr_list);
}

void DeleteQuery::SetExpr1(string& in_expr1) {
	tmp_expr1 = in_expr1;
}

void DeleteQuery::SetCmp(CMP in_cmp) {
	tmp_cmp = in_cmp;
}

void DeleteQuery::SetWhereExpr(string& expr2) {
	where_expr_list.push_back(WhereExpr(tmp_expr1, tmp_cmp, expr2));
}

void DeleteQuery::SetDeleteTable(string& table) {
	delete_table_name = table;
}












