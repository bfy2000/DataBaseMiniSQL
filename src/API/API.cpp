#include <vector>
#include <cctype>
#include <sstream>
#include <iostream>
#include <regex>
#include <string>
#include "../Utils/Utils.h"
#include "API.h"

using namespace std;

vector<string> split(const std::string& s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

bool is_float(string myString) {
	std::istringstream iss(myString);
	float f;
	iss >> noskipws >> f; // noskipws considers leading whitespace invalid
	// Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

bool is_integer(std::string const& n) noexcept
{
	if (std::isdigit(n[0]) || (n.size() > 1 && (n[0] == '-' || n[0] == '+')))
	{
		for (std::string::size_type i{ 1 }; i < n.size(); ++i)
			if (!std::isdigit(n[i]))
				return false;
		return true;
	}
	return false;
}

string parse_string(string& s) {
	string rst = "";
	return rst;
}


InsertQuery::InsertQuery() {

}


InsertQuery::~InsertQuery() {

}

void InsertQuery::Clear() {
	vector<InsertVal>().swap(insert_values);
}

void InsertQuery::Insert(string insert_val) {
	Type this_type = UNDEFINED;
	string val;
	if (insert_val[0] == '\\') {
		this_type = STRING;
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

void InsertQuery::Query() {
	cout << "INSERT VALUES:" << endl;
	for (int i = 0; i < insert_values.size(); i++) {
		cout << insert_values[i].val << "\ttype: " << insert_values[i].given_type << endl;
	}
}

void ExecFile(string filename) {
	filename.erase(0, 1);
	string final_filename = "";
	vector<string> parse_vec = split(filename, '\\');
	for (int i = 0; i < parse_vec.size(); i++) {
		unsigned int each_char_val;
		stringstream ss;
		ss << hex << parse_vec[i];
		ss >> each_char_val;
		final_filename.push_back(each_char_val);
	}
	Prompt("Execfile: "+final_filename);
}

void DropIndex(string drop_index) {
	Prompt("Drop Index");
}

void DropTable(string drop_table) {
	Prompt("Drop Table");
}

void CreateIndex(string index_name, string table_name, string attr_name) {
	Prompt("Create Index\t" + index_name + "\t" + table_name + "\t" + attr_name);
}