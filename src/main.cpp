#include <iostream>
#include <sstream>
#include "Utils/Utils.h"
#include "./Interpreter/Interpreter.h"
#include <ctime>

using namespace std;

CatalogManager catalogManager;

int main(int argc, char *argv[]) {
    
    catalogManager.initial_catalog();

    clock_t start_t, end_t;

    
    switch (argc)
    {
    case 1: {
        Prompt("\n\n\t*** Welcome to MiniSQL System ***\n", green);

        string input_string;                    // get the input string by lines
        Interpreter interpreter;

        while (interpreter.GetState() != State::QUIT) {
            //* print line prompt
            if (interpreter.GetState() == State::IDLE)
                cout << " >> ";
            else
                cout << "    ";

            getline(cin, input_string);
            start_t = clock();

            bool rst = interpreter.ProcessInput(input_string);
            //cout << input_string << endl;

            if (rst) {
                istringstream ss(input_string);
                interpreter.ReadInput(ss);
            }
            else {
                interpreter.SetState(State::IDLE);
            }

            end_t=clock();
            cout<<endl<<"runtime = "<<(double)(end_t-start_t)<<endl;
        }

        Prompt("\n\t*** Bye ***\n\n", green);
    } break;
    case 2: {
		//Interpreter interpreter;
		//string file_path = argv[1];

		//if (interpreter.ParseFileInput(file_path)) {
		//	interpreter.ExecFile(file_path);
		//}
		//else {
		//	PromptErr("Execution failed due to some syntax error.");
		//}
    } break;
    default:
        PromptErr("[Syntax Error] too much arguments, expect 1 or 2");
        return 0;
    }
}
