#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <ostream>
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

void Prompt(std::string prompt, mode color);
void Prompt(std::string prompt);
void PromptErr(std::string prompt);






#endif // UTILS_H
