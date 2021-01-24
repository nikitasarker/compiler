#include "ast.hpp"
#include <fstream>
#include <ostream>

extern FILE* yyin;

int main(int argc, char *argv[])
{
	std::string flag = argv[1];
	std::string input = argv[2];
	std::string output = argv[4];

	yyin = fopen(input.c_str(), "r");
	std::ofstream outfile (output);

    const Program *ast=parseAST();

	if (flag == "--translate") {
		//translator selection from cmd line
		ast->print(outfile);


	}
	else if (flag == "-S") {
		Context context = Context();
		//compiler selection from cmd line
		ast->printMIPS(outfile,context);
	}


	outfile.close();
    return 0;
}
int Program::indent = 0;
bool Program::mainPresent = false;
