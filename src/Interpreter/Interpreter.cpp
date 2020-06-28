#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <regex>
#include <fstream>

#include "../API/API.h"
#include "Interpreter.h"
#include "../Utils/Utils.h"

using namespace std;

Interpreter::Interpreter()
	:state_code{State::IDLE} {

}

Interpreter::~Interpreter() {

}

State Interpreter::GetState() {
	return state_code;
}

void Interpreter::SetState(State set_state_code) {
	state_code = set_state_code;
}

bool Interpreter::ProcessInput(string& input_string) {
	// make a string copy
	string rst = "";
	bool quoted = false;

	for (size_t i = 0; i < input_string.length(); i++) {
		if (input_string[i] == '\'' && !quoted) {
			quoted = true;
		}
		else if (input_string[i] == '\'' && quoted) {
			quoted = false;
		}
		else if (input_string[i] == '\\' && quoted) {
			switch (input_string[i + 1])
			{
			case '\\':rst += "\\5c"; break;
			case 'n':rst += "\\a"; break;
			case 'r':rst += "\\d"; break;
			case 't':rst += "\\9"; break;
			case '\'': rst += "\\5c"; break;
			default: {
				string err_msg = "[Invalid Argument] invalid escape character: \\";
				err_msg.push_back(input_string[i + 1]);
				PromptErr(err_msg);
				return false;
			} break;
			}
			i++;
		}
		else if (input_string[i] == '\\' && !quoted) {
			PromptErr("[Invalid Argument] invalid character: \\");
			return false;
		}
		else if (quoted) {
			stringstream stream;
			stream << hex << static_cast<int>(input_string[i]);
			rst += '\\' + stream.str();
		}
		else {
			rst += input_string[i];
		}
	}
	if (quoted) {
		PromptErr("[Invalid Argument] unquoted string!");
		return false;
	}
	
	for (size_t i = 0; i < rst.length(); i++) {
		if (rst[i] == '(' || rst[i] == ')' || rst[i] == ';' || rst[i] == ',') {
			rst.insert(i, " ");
			i += 2;
			rst.insert(i, " ");
		}
		else if (rst[i] == '=') {
			rst.insert(i, " ");
			i += 2;
			rst.insert(i, " ");
		} 
		else if (rst[i] == '!' && rst[i + 1] == '=') {
			rst.insert(i, " ");
			i += 3;
			rst.insert(i, " ");
		}
		else if ((rst[i] == '>' || rst[i] == '<')) {
			if (rst[i + 1] == '=') {
				rst.insert(i, " ");
				i += 3;
				rst.insert(i, " ");
			}
			else {
				rst.insert(i, " ");
				i += 2;
				rst.insert(i, " ");
			}
		}
	}
	input_string = rst;
	return true;
}



void Interpreter::ReadInput(istringstream &input) {
	string next_word;
	input >> next_word;

	while (next_word != "") {
		switch (state_code)
		{
		case State::IDLE: {
			if (next_word == "SELECT" || next_word == "select")
				state_code = State::SELECT;
			else if (next_word == "QUIT" || next_word == "quit") {
				state_code = State::QUIT;
				return;
			}
			else if (next_word == "CREATE" || next_word == "create") {
				state_code = State::CREATE;
			}
			else if (next_word == "INSERT" || next_word == "insert") {
				state_code = State::INSERT;
			}
			else if (next_word == "EXECFILE" || next_word == "execfile") {
				state_code = State::EXECFILE;
			}
			else if (next_word == "DROP" || next_word == "drop") {
				state_code = State::DROP;
			}
			else if (next_word =="DELETE" || next_word =="delete") {
				state_code = State::DELETE;
			}
			else if (next_word != "" && next_word != ";") {
				PromptErr("[Syntax Error] undefined operation: " + next_word);
				return;
			}
		} break;
		case State::DELETE: {
			if (next_word == "FROM" || next_word == "from") {
				state_code = State::DELETE_FROM;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect FROM");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_FROM: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				state_code = State::DELETE_TABLE_PARSED;
				api.delete_query.SetDeleteTable(next_word);
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid delete table: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_TABLE_PARSED: {
			if (next_word == ";") {
				api.delete_query.Query(api.catalog_manager, api.record_manager, api.index_manager);
				state_code = State::IDLE;
			}
			else if (next_word == "WHERE" || next_word == "where") {
				state_code = State::DELETE_WHERE_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_WHERE_PARSED: {
			if (is_expr(next_word)) {
				api.delete_query.SetExpr1(next_word);
				state_code = State::DELETE_EXPR1_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_EXPR1_PARSED: {
			if (next_word == "=") {
				api.delete_query.SetCmp(EQUAL);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word == "!=") {
				api.delete_query.SetCmp(NOT_EQUAL);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word == ">") {
				api.delete_query.SetCmp(GREATER);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word == "<") {
				api.delete_query.SetCmp(LESS);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word == ">=") {
				api.delete_query.SetCmp(GREATER_EQUAL);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word == "<=") {
				api.delete_query.SetCmp(LESS_EQUAL);
				state_code = State::DELETE_CMP_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid comparison expression: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_CMP_PARSED: {
			if (is_expr(next_word)) {
				api.delete_query.SetWhereExpr(next_word);
				state_code = State::DELETE_EXPR2_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_EXPR2_PARSED: {
			if (next_word == "AND" || next_word == "and") {
				state_code = State::DELETE_AND_PARSED;
			}
			else if (next_word == ";") {
				api.delete_query.Query(api.catalog_manager, api.record_manager, api.index_manager);
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid argument: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DELETE_AND_PARSED: {
			if (is_expr(next_word)) {
				api.delete_query.SetExpr1(next_word);
				state_code = State::SELECT_EXPR1_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.delete_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::EXECFILE: {
			//if (next_word[0] == '\\') {
			//	next_word.erase(0, 1);
			//	string final_filename = "";
			//	vector<string> parse_vec = split(next_word, '\\');
			//	for (int i = 0; i < parse_vec.size(); i++) {
			//		unsigned int each_char_val;
			//		stringstream ss;
			//		ss << hex << parse_vec[i];
			//		ss >> each_char_val;
			//		final_filename.push_back(each_char_val);
			//	}
			//	if (ParseFileInput(final_filename)) {
			//		ExecFile(final_filename);
			//	}
			//	state_code = State::IDLE;
			//}
			//else {
			//	PromptErr("[Syntax Error] illegal file path, expect a string");
			//	state_code = State::IDLE;
			//	return;
			//}
			state_code = State::IDLE;
		} break;
		case State::QUIT:
			return;
		case State::DROP: {
			if (next_word == "INDEX" || next_word == "index")
				state_code = State::DROP_INDEX;
			else if (next_word == "TABLE" || next_word == "table")
				state_code = State::DROP_TABLE;
			else {
				PromptErr("[Syntax Error] unreferenced DROP object, expect INDEX or TABLE");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_INDEX: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				state_code = State::DROP_INDEX_PARSED;
				tmp_drop_obj = next_word;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced index: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_INDEX_PARSED: {
			if (next_word == ";") {
				api.DropIndex(tmp_drop_obj);
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced statement: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_TABLE: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				state_code = State::DROP_TABLE_PARSED;
				tmp_drop_obj = next_word;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced index: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_TABLE_PARSED: {
			if (next_word == ";") {
				api.DropTable(tmp_drop_obj);
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced statement: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT: {
			if (next_word == "INTO" || next_word == "into")
				state_code = State::INTO;
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced keyword: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INTO: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				tmp_insert_table = next_word;
				state_code = State::INSERT_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid table name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT_PARSED: {
			if (next_word == "VALUES" || next_word == "values") {
				state_code = State::VALUES;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced keyword, expect VALUES");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::VALUES: {
			if (next_word == "(") {
				state_code = State::INSERT_LEFT_BRACKET;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] unreferenced keyword, expect (");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT_LEFT_BRACKET: {
			if (next_word == ")") {
				state_code = State::INSERT_RIGHT_BRACKET;
				return;
			}
			else if (next_word != "") {
				api.insert_values_query.Insert(next_word);
				state_code = State::INSERT_VALUE_RECEIVED;
			}
		} break;
		case State::INSERT_VALUE_RECEIVED: {
			if (next_word == ",") {
				state_code = State::INSERT_COMMA;
			}
			else if (next_word == ")") {
				state_code = State::INSERT_RIGHT_BRACKET;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid symbol: " + next_word);
				api.insert_values_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT_COMMA: {
			if (next_word == ")" || next_word == ";") {
				PromptErr("[Invalid Argument] expect another insert value");
				api.insert_values_query.Clear();
				state_code = State::IDLE;
				return;
			}
			else if (next_word != "") {
				api.insert_values_query.Insert(next_word);
				state_code = State::INSERT_VALUE_RECEIVED;
			}
		} break;
		case State::INSERT_RIGHT_BRACKET: {
			if (next_word == ";") {
				api.insert_values_query.SetTableName(tmp_insert_table);
				api.insert_values_query.Query(api.catalog_manager, api.record_manager, api.index_manager);
				api.insert_values_query.Clear();
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid symbol, expect ;");
				api.insert_values_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE: {
			if (next_word == "INDEX" || next_word == "index") {
				state_code = State::CREATE_INDEX;
			}
			else if (next_word == "TABLE" || next_word == "table") {
				state_code = State::CREATE_TABLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect TABLE or INDEX");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				tmp_index_name = next_word;
				state_code = State::CREATE_INDEX_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid index name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_PARSED: {
			if (next_word == "ON" || next_word == "on")
				state_code = State::ON;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect ON");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::ON: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				tmp_table_name = next_word;
				state_code = State::CREATE_INDEX_TABLE_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid index name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_TABLE_PARSED: {
			if (next_word == "(")
				state_code = State::CREATE_INDEX_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect (");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_LEFT_BRACKET: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				tmp_attr_name = next_word;
				state_code = State::CREATE_INDEX_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_ATTR_PARSED: {
			if (next_word == ")")
				state_code = State::CREATE_INDEX_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect )");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_RIGHT_BRACKET: {
			if (next_word == ";") {
				api.CreateIndex(tmp_index_name, tmp_table_name, tmp_attr_name);
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid symbol, expect ;");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.create_table.SetTableName(next_word);
				state_code = State::CREATE_TABLE_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid table name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE_PARSED: {
			if (next_word == "(")
				state_code = State::CREATE_TABLE_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect (");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE_LEFT_BRACKET: {
			if (next_word == "PRIMARY" || next_word == "primary") {
				state_code = State::CREATE_TABLE_PRIMARY;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.create_table.InsertAttr(next_word);
				state_code = State::CREATE_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE_PRIMARY: {
			if (next_word == "KEY" || next_word == "key")
				state_code = State::CREATE_PRIMARY_KEY;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect KEY");
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_PRIMARY_KEY: {
			if (next_word == "(")
				state_code = State::CREATE_PRIMARY_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect (");
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;

		case State::CREATE_PRIMARY_LEFT_BRACKET: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.create_table.InsertPrimary(next_word);
				state_code = State::PRIMARY_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::PRIMARY_ATTR_PARSED: {
			if (next_word == ")")
				state_code = State::CREATE_PRIMARY_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect )");
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_PRIMARY_RIGHT_BRACKET: {
			if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect ) or ,");
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_COMMA: {
			if (next_word == "PRIMARY" || next_word == "primary") {
				state_code = State::CREATE_TABLE_PRIMARY;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.create_table.InsertAttr(next_word);
				state_code = State::CREATE_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_ATTR_PARSED: {
			if (next_word == "INT" || next_word == "int") {
				state_code = State::CREATE_INT_PARSED;
				api.create_table.InsertType(INT);
			}
			else if (next_word == "FLOAT" || next_word == "float") {
				state_code = State::CREATE_FLOAT_PARSED;
				api.create_table.InsertType(FLOAT);
			}
			else if (next_word == "CHAR" || next_word == "char") {
				state_code = State::CREATE_CHAR_PARSED;
				api.create_table.InsertType(CHAR);
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute type: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INT_PARSED: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				api.create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_CHAR_PARSED: {
			if (next_word == "(")
				state_code = State::CHAR_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_FLOAT_PARSED: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				api.create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;

		case State::CHAR_LEFT_BRACKET: {
			if (regex_match(next_word, regex("^[0-9]*$"))) {
				api.create_table.InsertSize(stoi(next_word));
				state_code = State::CHAR_BIT_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid char size: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CHAR_BIT_PARSED: {
			if (next_word == ")")
				state_code = State::CHAR_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CHAR_RIGHT_BRACKET: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				api.create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::UNIQUE_PARSED: {
			if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE_RIGHT_BRACKET: {
			if (next_word == ";") {
				api.create_table.Query(api.catalog_manager, api.index_manager);
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect ;");
				api.create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT: {
			if (next_word == "*") {
				api.select_query.SetSelectAll();
				state_code = State::SELECT_ALL;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.select_query.Insert(next_word);
				state_code = State::SELECT_ATTR;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ALL: {
			if (next_word == "FROM" || next_word == "from")
				state_code = State::SELECT_FROM;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect FROM");
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ATTR: {
			if (next_word == ",")
				state_code = State::SELECT_ATTR_COMMA;
			else if(next_word == "FROM" || next_word == "from")
				state_code = State::SELECT_FROM;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_FROM: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))){
				api.select_query.SetSelectTable(next_word);
				state_code = State::SELECT_TABLE_PARSED;
			}
				
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid table name: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ATTR_COMMA: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				api.select_query.Insert(next_word);
				state_code = State::SELECT_ATTR;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_TABLE_PARSED: {
			if (next_word == ";") {
				api.select_query.Query(api.catalog_manager, api.record_manager, api.index_manager);
				api.select_query.Clear();
				state_code = State::IDLE;
			}
			else if (next_word == "WHERE" || next_word == "where") {
				state_code = State::SELECT_WHERE_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_WHERE_PARSED: {
			if (is_expr(next_word)) {
				api.select_query.SetExpr1(next_word);
				state_code = State::SELECT_EXPR1_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_EXPR1_PARSED: {
			if (next_word == "=") {
				api.select_query.SetCmp(EQUAL);
				state_code = State::SELECT_CMP_PARSED;
			} 
			else if (next_word == "!=") {
				api.select_query.SetCmp(NOT_EQUAL);
				state_code = State::SELECT_CMP_PARSED;
			}
			else if(next_word == ">"){
				api.select_query.SetCmp(GREATER);
				state_code = State::SELECT_CMP_PARSED;
			}
			else if (next_word == "<") {
				api.select_query.SetCmp(LESS);
				state_code = State::SELECT_CMP_PARSED;
			}
			else if (next_word == ">=") {
				api.select_query.SetCmp(GREATER_EQUAL);
				state_code = State::SELECT_CMP_PARSED;
			}
			else if (next_word == "<=") {
				api.select_query.SetCmp(LESS_EQUAL);
				state_code = State::SELECT_CMP_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid comparison expression: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_CMP_PARSED: {
			if (is_expr(next_word)) {
				api.select_query.SetWhereExpr(next_word);
				state_code = State::SELECT_EXPR2_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_EXPR2_PARSED: {
			if (next_word == "AND" || next_word == "and") {
				state_code = State::SELECT_AND_PARSED;
			}
			else if (next_word == ";") {
				api.select_query.Query(api.catalog_manager, api.record_manager, api.index_manager);
				api.select_query.Clear();
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid argument: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_AND_PARSED: {
			if (is_expr(next_word)) {
				api.select_query.SetExpr1(next_word);
				state_code = State::SELECT_EXPR1_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid expression: " + next_word);
				api.select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		default:
			break;
		}
		if (!(input >> next_word))
			return;
	}
}

//
//bool Interpreter::ParseFileInput(string& file_path) {
//	ifstream infile(file_path);
//	if (infile.fail()) {
//		PromptErr("[Runtime Error] cannot open file: " + file_path);
//		return false;
//	}
//
//	string input_line;
//	while (getline(infile, input_line)) {
//		if (ProcessInput(input_line)) {
//			istringstream input(input_line);
//
//			string next_word;
//			input >> next_word;
//
//			while (next_word != "") {
//				switch (state_code)
//				{
//				case State::IDLE: {
//					if (next_word == "SELECT" || next_word == "select")
//						state_code = State::SELECT;
//					else if (next_word == "QUIT" || next_word == "quit") {
//						return false;
//					}
//					else if (next_word == "CREATE" || next_word == "create") {
//						state_code = State::CREATE;
//					}
//					else if (next_word == "INSERT" || next_word == "insert") {
//						state_code = State::INSERT;
//					}
//					else if (next_word == "EXECFILE" || next_word == "execfile") {
//						return false;
//					}
//					else if (next_word == "DROP" || next_word == "drop") {
//						state_code = State::DROP;
//					}
//					else if (next_word == "DELETE" || next_word == "delete") {
//						state_code = State::DELETE;
//					}
//					else if (next_word != "" && next_word != ";") {
//						//PromptErr("[Syntax Error] undefined operation: " + next_word);
//						return false;
//					}
//				} break;
//				case State::DROP: {
//					if (next_word == "INDEX" || next_word == "index")
//						state_code = State::DROP_INDEX;
//					else if (next_word == "TABLE" || next_word == "table")
//						state_code = State::DROP_TABLE;
//					else {
//						//PromptErr("[Syntax Error] unreferenced DROP object, expect INDEX or TABLE");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::DROP_INDEX: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::DROP_INDEX_PARSED;
//					}
//					else if (next_word != "") {
//						//PromptErr("[Syntax Error] unreferenced index: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::DROP_INDEX_PARSED: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] unreferenced statement: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::DROP_TABLE: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::DROP_TABLE_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] unreferenced index: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::DROP_TABLE_PARSED: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] unreferenced statement: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT: {
//					if (next_word == "INTO" || next_word == "into")
//						state_code = State::INTO;
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] unreferenced keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INTO: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::INSERT_PARSED;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] invalid table name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT_PARSED: {
//					if (next_word == "VALUES" || next_word == "values") {
//						state_code = State::VALUES;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] unreferenced keyword, expect VALUES");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::VALUES: {
//					if (next_word == "(") {
//						state_code = State::INSERT_LEFT_BRACKET;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] unreferenced keyword, expect (");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT_LEFT_BRACKET: {
//					if (next_word == ")") {
//						state_code = State::INSERT_RIGHT_BRACKET;
//					}
//					else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::INSERT_VALUE_RECEIVED;
//					}
//					else if (next_word != "") {
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT_VALUE_RECEIVED: {
//					if (next_word == ",") {
//						state_code = State::INSERT_COMMA;
//					}
//					else if (next_word == ")") {
//						state_code = State::INSERT_RIGHT_BRACKET;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] invalid symbol: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT_COMMA: {
//					if (next_word == ")" || next_word == ";") {
//						// PromptErr("[Invalid Argument] expect another insert value");
//						state_code = State::IDLE;
//						return false;
//					}
//					else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						insert_values_query.Insert(next_word);
//						state_code = State::INSERT_VALUE_RECEIVED;
//					}
//					else if (next_word != "") {
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::INSERT_RIGHT_BRACKET: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] invalid symbol, expect ;");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE: {
//					if (next_word == "INDEX" || next_word == "index") {
//						state_code = State::CREATE_INDEX;
//					}
//					else if (next_word == "TABLE" || next_word == "table") {
//						state_code = State::CREATE_TABLE;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] expect TABLE or INDEX");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_INDEX_PARSED;
//					}
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] invalid index name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX_PARSED: {
//					if (next_word == "ON" || next_word == "on")
//						state_code = State::ON;
//					else if (next_word == "") {}
//					else {
//						// PromptErr("[Syntax Error] expect ON");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::ON: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_INDEX_TABLE_PARSED;
//					}
//					else if (next_word == "") { 
//						// PromptErr("[Syntax Error] invalid index name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX_TABLE_PARSED: {
//					if (next_word == "(")
//						state_code = State::CREATE_INDEX_LEFT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect (");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX_LEFT_BRACKET: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_INDEX_ATTR_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid attribute name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX_ATTR_PARSED: {
//					if (next_word == ")")
//						state_code = State::CREATE_INDEX_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect )");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INDEX_RIGHT_BRACKET: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid symbol, expect ;");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_TABLE: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_TABLE_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid table name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_TABLE_PARSED: {
//					if (next_word == "(")
//						state_code = State::CREATE_TABLE_LEFT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect (");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_TABLE_LEFT_BRACKET: {
//					if (next_word == "PRIMARY" || next_word == "primary") {
//						state_code = State::CREATE_TABLE_PRIMARY;
//					}
//					else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_ATTR_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_TABLE_PRIMARY: {
//					if (next_word == "KEY" || next_word == "key")
//						state_code = State::CREATE_PRIMARY_KEY;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect KEY");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_PRIMARY_KEY: {
//					if (next_word == "(")
//						state_code = State::CREATE_PRIMARY_LEFT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect (");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_PRIMARY_LEFT_BRACKET: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::PRIMARY_ATTR_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid attribute name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::PRIMARY_ATTR_PARSED: {
//					if (next_word == ")")
//						state_code = State::CREATE_PRIMARY_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect )");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_PRIMARY_RIGHT_BRACKET: {
//					if (next_word == ",")
//						state_code = State::CREATE_COMMA;
//					else if (next_word == ")")
//						state_code = State::CREATE_TABLE_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect ) or ,");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_COMMA: {
//					if (next_word == "PRIMARY" || next_word == "primary") {
//						state_code = State::CREATE_TABLE_PRIMARY;
//					}
//					else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::CREATE_ATTR_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_ATTR_PARSED: {
//					if (next_word == "INT" || next_word == "int") {
//						state_code = State::CREATE_INT_PARSED;
//					}
//					else if (next_word == "FLOAT" || next_word == "float") {
//						state_code = State::CREATE_FLOAT_PARSED;
//					}
//					else if (next_word == "CHAR" || next_word == "char") {
//						state_code = State::CREATE_CHAR_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid attribute type: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_INT_PARSED: {
//					if (next_word == "UNIQUE" || next_word == "unique") {
//						state_code = State::UNIQUE_PARSED;
//					}
//					else if (next_word == ",")
//						state_code = State::CREATE_COMMA;
//					else if (next_word == ")")
//						state_code = State::CREATE_TABLE_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid key word: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_CHAR_PARSED: {
//					if (next_word == "(")
//						state_code = State::CHAR_LEFT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid key word: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_FLOAT_PARSED: {
//					if (next_word == "UNIQUE" || next_word == "unique") {
//						state_code = State::UNIQUE_PARSED;
//					}
//					else if (next_word == ",")
//						state_code = State::CREATE_COMMA;
//					else if (next_word == ")")
//						state_code = State::CREATE_TABLE_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid key word: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//
//				case State::CHAR_LEFT_BRACKET: {
//					if (regex_match(next_word, regex("^[0-9]*$"))) {
//						state_code = State::CHAR_BIT_PARSED;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid char size: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CHAR_BIT_PARSED: {
//					if (next_word == ")")
//						state_code = State::CHAR_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CHAR_RIGHT_BRACKET: {
//					if (next_word == "UNIQUE" || next_word == "unique") {
//						state_code = State::UNIQUE_PARSED;
//					}
//					else if (next_word == ",")
//						state_code = State::CREATE_COMMA;
//					else if (next_word == ")")
//						state_code = State::CREATE_TABLE_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid key word: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::UNIQUE_PARSED: {
//					if (next_word == ",")
//						state_code = State::CREATE_COMMA;
//					else if (next_word == ")")
//						state_code = State::CREATE_TABLE_RIGHT_BRACKET;
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] invalid key word: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::CREATE_TABLE_RIGHT_BRACKET: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word != "") {
//						// PromptErr("[Syntax Error] expect ;");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT: {
//					if (next_word == "*") {
//						select_query.SetSelectAll();
//						state_code = State::SELECT_ALL;
//					}
//					else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::SELECT_ATTR;
//					}
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] invalid attribute name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT_ALL: {
//					if (next_word == "FROM" || next_word == "from")
//						state_code = State::SELECT_FROM;
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] expect FROM");
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT_ATTR: {
//					if (next_word == ",")
//						state_code = State::SELECT_ATTR_COMMA;
//					else if (next_word == "FROM" || next_word == "from")
//						state_code = State::SELECT_FROM;
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] invalid keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT_FROM: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$")))
//						state_code = State::SELECT_TABLE_PARSED;
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] invalid table name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT_ATTR_COMMA: {
//					if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
//						state_code = State::SELECT_ATTR;
//					}
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] invalid attribute name: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				case State::SELECT_TABLE_PARSED: {
//					if (next_word == ";") {
//						state_code = State::IDLE;
//					}
//					else if (next_word == "") {
//						// PromptErr("[Syntax Error] invalid keyword: " + next_word);
//						state_code = State::IDLE;
//						return false;
//					}
//				} break;
//				default:
//					break;
//				}
//				if (!(input >> next_word))
//					break;
//			}
//		}
//		else
//			return false;
//	}
//	return true;
//}


void Interpreter::ExecFile(string& file_path) {
	Prompt("EXECFILE" + file_path);
}
































