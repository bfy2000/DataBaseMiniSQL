#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>

//* overall configuration of MiniSQL
const int PAGES_SIZE = 4096;
const int MAX_FRAME_SIZE = 100;

// * colored prompt
enum mode {red, green, blue, def} ;

void Prompt(std::string prompt, mode color);
void Prompt(std::string prompt);

//* types for flex & bison

// enum class CompareType { // for comparison
//     GreatThan,
//     GreatEqual,
//     LessThan,
//     LessEqual,
//     Equal,
//     NotEqual
// };






#endif // UTILS_H
