#pragma once
#include <vector>
#include <string>

using namespace std;

enum Type {		// attribute types
	STRING,
	INT,
	FLOAT,
	UNDEFINED
};

struct InsertVal		// insert values
{
	string val;			// value
	Type given_type;	// type
	InsertVal() : val(""), given_type(UNDEFINED) {}
	InsertVal(string given_val, Type given_t) 
		: val(given_val), given_type(given_t) {}
};

class InsertQuery
{
public:
	InsertQuery();
	~InsertQuery();

	void Clear();		// clear all saved data
	void Insert(string insert_val);	// insert value
	void Query();		// main process function
private:
	string insert_table_name;
	vector<InsertVal> insert_values;
};


void DropIndex(string drop_index);
void DropTable(string drop_table);
void CreateIndex(string index_name, string table_name, string attr_name);


struct CreateAttr {
	string name;
	Type type;
	bool unique;
	bool primary;
	int size;
	CreateAttr() : name{ "" }, type{ UNDEFINED }, size{1}, unique{ false }, primary{ false }{}
};

class CreateTable
{
public:
	CreateTable();
	~CreateTable();
	
	void SetTableName(string &given_table_name);
	void InsertAttr(string &attr_name);
	void InsertType(Type given_type);
	void InsertSize(int given_size);
	void InsertUnique();
	void InsertPrimary(string& attr_name);
	void Clear();
	void Query();

private:
	string tmp_table_name;
	vector<CreateAttr> attr_list;
	vector<string> primary_list;
};


class SelectQuery
{
public:
	SelectQuery();
	~SelectQuery();

	void Query();
	void Insert(string &attr);
	void Clear();
	void SetSelectAll();
	void SetSelectTable(string &table);

private:
	vector<string> select_attr_list;
	string select_table_name;
	bool select_all;
};










