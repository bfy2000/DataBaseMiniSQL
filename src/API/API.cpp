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
InsertQuery::InsertQuery() {

}

InsertQuery::~InsertQuery() {

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

void InsertQuery::Query(RecordManager& record_manager) {	
	vector<pair<FieldType, string>> insert_values_to_records;
	for (int i = 0; i < insert_values.size(); i++) {
		insert_values_to_records.push_back(pair<FieldType, string>(FieldType(insert_values[i].given_type), insert_values[i].val));
	}
	record_manager.insertValue(insert_table_name, insert_values_to_records);
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
}




//--------------------------------------------------------------------------------------------






//* CREATE TABLE ...
CreateTable::CreateTable() { }

CreateTable::~CreateTable() { }

void CreateTable::SetTableName(string &given_table_name) {
	tmp_table_name = given_table_name;
}

void CreateTable::InsertAttr(string &attr_name) {
	int n = attr_list.size();
	attr_list.push_back(CreateAttr());
	attr_list[n].name = attr_name;
}

void CreateTable::InsertType(NumType given_type) {
	int n = attr_list.size() - 1;
	attr_list[n].type = given_type;
}

void CreateTable::InsertSize(int given_size) {
	int n = attr_list.size() - 1;
	attr_list[n].size = given_size;
}

void CreateTable::InsertUnique() {
	int n = attr_list.size() - 1;
	attr_list[n].unique = true;
}

void CreateTable::InsertPrimary(string& attr_name) {
	primary_list.push_back(attr_name);
}

void CreateTable::Clear() {
	tmp_table_name = "";
	vector<CreateAttr>().swap(attr_list);
	vector<string>().swap(primary_list);
}

void CreateTable::Query() {
	Prompt("Create Table");
}




//--------------------------------------------------------------------------



//* SELECT * FROM ...
SelectQuery::SelectQuery() :select_all(false) {}
SelectQuery::~SelectQuery() {}

void SelectQuery::Query(RecordManager& record_manager) {
	select_all = false;
	Prompt("SELECT ...");
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

void DeleteQuery::Query() {
	Prompt("DELETE ...");
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












