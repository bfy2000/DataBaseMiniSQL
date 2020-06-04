#include <string>
#include <sstream>
#include <iostream>
#include <regex>

#include "Utils.h"

using namespace std;

void Prompt(std::string prompt, mode color) {
	Color::Modifier prompt_red(Color::FG_RED);
	Color::Modifier prompt_green(Color::FG_GREEN);
	Color::Modifier prompt_blue(Color::FG_BLUE);
	Color::Modifier prompt_def(Color::FG_DEFAULT);
	switch (color)
	{
	case red: std::cout << prompt_red << prompt << prompt_def << std::endl; break;
	case green: std::cout << prompt_green << prompt << prompt_def << std::endl; break;
	case blue: std::cout << prompt_blue << prompt << prompt_def << std::endl; break;
	default: std::cout << prompt << std::endl; break;
	}
}

void Prompt(std::string prompt) {
	std::cout << prompt << std::endl;
}

void PromptErr(std::string prompt) {
	Prompt(prompt, red);
}


//* auxiliary functions 
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

bool is_expr(string& s) {
	if (regex_match(s, regex("^[a-zA-Z0-9_]*$")) || s[0] == '\\')
		return true;
	else
		return is_integer(s) || is_float(s);
}

