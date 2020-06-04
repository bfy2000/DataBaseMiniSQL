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

enum CMP {
	EQUAL,
	NOT_EQUAL,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL  
};

struct WhereExpr {
	string expr1;
	CMP cmp;
	string expr2;
	WhereExpr(string &in_expr1, CMP &in_cmp, string &in_expr2)
		:expr1{ in_expr1 }, cmp{ in_cmp }, expr2{ in_expr2 } {}
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
	void SetWhereExpr(string& expr2);
	void SetCmp(CMP in_cmp);
	void SetExpr1(string& in_expr1);

private:
	vector<string> select_attr_list;
	string select_table_name;
	bool select_all;
	vector<WhereExpr> where_expr_list;
	string tmp_expr1;
	CMP tmp_cmp;
};

class DeleteQuery
{
public:
	DeleteQuery();
	~DeleteQuery();

	void Query();
	void Clear();
	void SetDeleteTable(string& table);
	void SetExpr1(string& in_expr1);
	void SetCmp(CMP in_cmp);
	void SetWhereExpr(string& expr2);

private:
	string delete_table_name;
	vector<WhereExpr> where_expr_list;
	string tmp_expr1;
	CMP tmp_cmp;
};








