#pragma once
#include <vector>
#include <string>

using namespace std;

enum Type {
	STRING,
	INT,
	FLOAT,
	UNDEFINED
};

struct InsertVal
{
	string val;
	Type given_type;
	InsertVal() : val(""), given_type(UNDEFINED) {}
	InsertVal(string given_val, Type given_t) 
		: val(given_val), given_type(given_t) {}
};


class InsertQuery
{
public:
	InsertQuery();
	~InsertQuery();

	void Clear();
	void Insert(string insert_val);
	void Query();

private:
	string insert_table_name;
	vector<InsertVal> insert_values;
};


void ExecFile(string filename);
void DropIndex(string drop_index);
void DropTable(string drop_table);
void CreateIndex(string index_name, string table_name, string attr_name);