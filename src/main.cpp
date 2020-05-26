#include <iostream>
#include <sstream>
#include "Utils/Utils.h"
#include "./Interpreter/Interpreter.h"

using namespace std;

int main(int argc, char **argv) {
    Prompt("\n\n\t*** Welcome to MiniSQL System ***\n\n", green);

    string input_string;                    // get the input string by lines
    Interpreter interpreter;

    while (interpreter.GetState() != State::QUIT) {
        
        //* print line prompt
        if (interpreter.GetState() == State::IDLE)
            cout << " >> ";
        else
            cout << "    ";
        
        getline(cin, input_string);

		bool rst = interpreter.ProcessInput(input_string);
        //cout << input_string << endl;

        if (rst) {
			istringstream ss(input_string);
			interpreter.ReadInput(ss);
        }
        else {
            interpreter.SetState(State::IDLE);
        }
    }

    Prompt("\n\n\t*** Bye ***\n\n", green);
}
