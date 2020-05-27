#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <regex>

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
			else if (next_word != "" && next_word != ";") {
				PromptErr("[Syntax Error] undefined operation: " + next_word);
				return;
			}
		} break;
		case State::EXECFILE: {
			if (next_word[0] == '\\') {
				ExecFile(next_word);
				state_code = State::IDLE;
			}
			else {
				PromptErr("[Syntax Error] illegal file path, expect a string");
				state_code = State::IDLE;
				return;
			}
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
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*;$"))) {
				DropIndex(next_word.substr(0, next_word.size() - 1));
				state_code = State::IDLE;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				state_code = State::DROP_INDEX_PARSED;
				tmp_drop_obj = next_word;
			}
			else {
				PromptErr("[Syntax Error] unreferenced index: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_INDEX_PARSED: {
			if (next_word == ";") {
				DropIndex(tmp_drop_obj);
				state_code = State::IDLE;
			}
			else if (next_word == "") {}
			else {
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
			else {
				PromptErr("[Syntax Error] unreferenced index: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::DROP_TABLE_PARSED: {
			if (next_word == ";") {
				DropTable(tmp_drop_obj);
				state_code = State::IDLE;
			}
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] unreferenced statement: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT: {
			if (next_word == "INTO" || next_word == "into")
				state_code = State::INTO;
			else if (next_word == "") {}
			else {
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
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] invalid table name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT_PARSED: {
			if (next_word == "VALUES" || next_word == "values") {
				state_code = State::VALUES;
			}
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] unreferenced keyword, expect VALUES");
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::VALUES: {
			if (next_word == "(") {
				state_code = State::INSERT_LEFT_BRACKET;
			}
			else if (next_word == "") {}
			else {
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
			else if (next_word == "") {}
			else {
				insert_values_query.Insert(next_word);
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
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] invalid symbol: " + next_word);
				insert_values_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::INSERT_COMMA: {
			if (next_word == ")" || next_word == ";") {
				PromptErr("[Invalid Argument] expect another insert value");
				insert_values_query.Clear();
				state_code = State::IDLE;
				return;
			}
			else if (next_word == "") {}
			else {
				insert_values_query.Insert(next_word);
				state_code = State::INSERT_VALUE_RECEIVED;
			}
		} break;
		case State::INSERT_RIGHT_BRACKET: {
			if (next_word == ";") {
				insert_values_query.Query();
				insert_values_query.Clear();
				state_code = State::IDLE;
			}
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] invalid symbol, expect ;");
				insert_values_query.Clear();
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
			else if (next_word == "") {}
			else {
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
			else if (next_word == "") {}
			else {
				PromptErr("[Syntax Error] invalid index name: " + next_word);
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INDEX_PARSED: {
			if (next_word == "ON" || next_word == "on")
				state_code = State::ON;
			else if (next_word == "") {}
			else {
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
			else if (next_word == "") {}
			else {
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
				CreateIndex(tmp_index_name, tmp_table_name, tmp_attr_name);
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
				create_table.SetTableName(next_word);
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
				create_table.InsertAttr(next_word);
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
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_PRIMARY_KEY: {
			if (next_word == "(")
				state_code = State::CREATE_PRIMARY_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect (");
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;

		case State::CREATE_PRIMARY_LEFT_BRACKET: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				create_table.InsertPrimary(next_word);
				state_code = State::PRIMARY_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::PRIMARY_ATTR_PARSED: {
			if (next_word == ")")
				state_code = State::CREATE_PRIMARY_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect )");
				create_table.Clear();
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
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_COMMA: {
			if (next_word == "PRIMARY" || next_word == "primary") {
				state_code = State::CREATE_TABLE_PRIMARY;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				create_table.InsertAttr(next_word);
				state_code = State::CREATE_ATTR_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_ATTR_PARSED: {
			if (next_word == "INT" || next_word == "int") {
				state_code = State::CREATE_INT_PARSED;
				create_table.InsertType(INT);
			}
			else if (next_word == "FLOAT" || next_word == "float") {
				state_code = State::CREATE_FLOAT_PARSED;
				create_table.InsertType(FLOAT);
			}
			else if (next_word == "CHAR" || next_word == "char") {
				state_code = State::CREATE_CHAR_PARSED;
				create_table.InsertType(STRING);
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid attribute type: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_INT_PARSED: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_CHAR_PARSED: {
			if (next_word == "(")
				state_code = State::CHAR_LEFT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_FLOAT_PARSED: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;

		case State::CHAR_LEFT_BRACKET: {
			if (regex_match(next_word, regex("^[0-9]*$"))) {
				create_table.InsertSize(stoi(next_word));
				state_code = State::CHAR_BIT_PARSED;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid char size: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CHAR_BIT_PARSED: {
			if (next_word == ")")
				state_code = State::CHAR_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CHAR_RIGHT_BRACKET: {
			if (next_word == "UNIQUE" || next_word == "unique") {
				create_table.InsertUnique();
				state_code = State::UNIQUE_PARSED;
			}
			else if (next_word == ",")
				state_code = State::CREATE_COMMA;
			else if (next_word == ")")
				state_code = State::CREATE_TABLE_RIGHT_BRACKET;
			else if (next_word != "") {
				PromptErr("[Syntax Error] invalid key word: " + next_word);
				create_table.Clear();
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
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::CREATE_TABLE_RIGHT_BRACKET: {
			if (next_word == ";") {
				create_table.Query();
				state_code = State::IDLE;
			}
			else if (next_word != "") {
				PromptErr("[Syntax Error] expect ;");
				create_table.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT: {
			if (next_word == "*") {
				select_query.SetSelectAll();
				state_code = State::SELECT_ALL;
			}
			else if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				select_query.Insert(next_word);
				state_code = State::SELECT_ATTR;
			}
			else if (next_word == "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ALL: {
			if (next_word == "FROM" || next_word == "from")
				state_code = State::SELECT_FROM;
			else if (next_word == "") {
				PromptErr("[Syntax Error] expect FROM");
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ATTR: {
			if (next_word == ",")
				state_code = State::SELECT_ATTR_COMMA;
			else if(next_word == "FROM" || next_word == "from")
				state_code = State::SELECT_FROM;
			else if (next_word == "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_FROM: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$")))
				state_code = State::SELECT_TABLE_PARSED;
			else if (next_word == "") {
				PromptErr("[Syntax Error] invalid table name: " + next_word);
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_ATTR_COMMA: {
			if (regex_match(next_word, regex("^[a-zA-Z0-9_]*$"))) {
				select_query.Insert(next_word);
				state_code = State::SELECT_ATTR;
			}
			else if (next_word == "") {
				PromptErr("[Syntax Error] invalid attribute name: " + next_word);
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		case State::SELECT_TABLE_PARSED: {
			if (next_word == ";") {
				select_query.Query();
				state_code = State::IDLE;
			}
			else if (next_word == "") {
				PromptErr("[Syntax Error] invalid keyword: " + next_word);
				select_query.Clear();
				state_code = State::IDLE;
				return;
			}
		} break;
		default:
			return;
			break;
		}
		if (!(input >> next_word))
			return;
	}
}







































