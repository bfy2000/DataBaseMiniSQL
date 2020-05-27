#pragma once

#include <sstream>
#include <string>
#include "../API/API.h"
using namespace std;

enum class State {
	// default state	
	IDLE,

	// create index & table
	CREATE,
	CREATE_INDEX,
	CREATE_TABLE,
	CREATE_INDEX_PARSED,
	ON,
	CREATE_INDEX_TABLE_PARSED,
	CREATE_INDEX_LEFT_BRACKET,
	CREATE_INDEX_ATTR_PARSED,
	CREATE_INDEX_RIGHT_BRACKET,
	CREATE_TABLE_PARSED,
	CREATE_TABLE_LEFT_BRACKET,
	CREATE_TABLE_RIGHT_BRACKET,
	CREATE_TABLE_PRIMARY,
	CREATE_ATTR_PARSED,
	CREATE_PRIMARY_KEY,
	CREATE_PRIMARY_LEFT_BRACKET,
	CREATE_PRIMARY_RIGHT_BRACKET,
	PRIMARY_ATTR_PARSED,
	CREATE_COMMA,
	CREATE_INT_PARSED,
	CREATE_FLOAT_PARSED,
	CREATE_CHAR_PARSED,
	CHAR_LEFT_BRACKET,
	CHAR_RIGHT_BRACKET,
	CHAR_BIT_PARSED,
	UNIQUE_PARSED,


	// select
	SELECT,
	SELECT_ALL,
	SELECT_ATTR,
	SELECT_ATTR_COMMA,
	SELECT_FROM,
	SELECT_TABLE_PARSED,

	// insert
	INSERT,
	INTO,
	INSERT_PARSED,
	VALUES,
	INSERT_COMMA,
	INSERT_LEFT_BRACKET,
	INSERT_RIGHT_BRACKET,
	INSERT_VALUE_RECEIVED,

	// drop table & index
	DROP,
	DROP_INDEX,
	DROP_TABLE,
	DROP_INDEX_PARSED,
	DROP_TABLE_PARSED,
	
	// execute file
	EXECFILE,
	
	// quit
	QUIT
};




class Interpreter
{
public:
	Interpreter();
	~Interpreter();

	
	State GetState();
	void SetState(State set_state_code);
	bool ProcessInput(string& input_string);
	void ReadInput(istringstream& input);
	
private:
	State state_code;
	string tmp_drop_obj;
	string tmp_insert_table;
	string tmp_index_name;
	string tmp_table_name;
	string tmp_attr_name;

	InsertQuery insert_values_query;
	CreateTable create_table;
	SelectQuery select_query;
};

























