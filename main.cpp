#include <iostream>
#include <fstream>

#include "parser.h"
#include "interpreter.h"

int main ()
{
    std::ifstream in("test.lfn", std::ios::ate);
    uint64_t pos = in.tellg();
    in.seekg(0);
    std::string file;
    file.resize(pos);
    in.read(&file[0], pos);
    in.close();

    Parser parser;
    ProgramNode* program = parser.ParseProgram(file);
    // std::cout << "Parsed" << std::endl;
    // Parser::PrintAst(program);
    Interpreter interpreter;
    interpreter.Evaluate(program);
    // std::cout << "Done" << std::endl;

    return 0;
}