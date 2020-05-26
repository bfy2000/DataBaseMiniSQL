#include "Utils.h"
#include <string>
#include <sstream>
#include <iostream>

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


