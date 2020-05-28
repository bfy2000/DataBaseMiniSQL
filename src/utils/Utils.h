#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <ostream>

using namespace std;

namespace Color {
	enum Code {
		FG_RED = 31,
		FG_GREEN = 32,
		FG_BLUE = 34,
		FG_DEFAULT = 39,
		BG_RED = 41,
		BG_GREEN = 42,
		BG_BLUE = 44,
		BG_DEFAULT = 49
	};
	class Modifier {
		Code code;
	public:
		Modifier(Code pCode) : code(pCode) {}
		friend std::ostream&
			operator<<(std::ostream& os, const Modifier& mod) {
			return os << "\033[" << mod.code << "m";
		}
	};
}

//* overall configuration of MiniSQL
const int PAGES_SIZE = 4096;
const int MAX_FRAME_SIZE = 100;

// * colored prompt
enum mode {red, green, blue, def} ;

void Prompt(string prompt, mode color);
void Prompt(string prompt);
void PromptErr(string prompt);


vector<string> split(const string& s, char delim);

bool is_float(string myString);

bool is_integer(string const& n) noexcept;





#endif // UTILS_H
