#include <iostream>
#include <fstream>

#include "parser.h"
#include "library.h"
#include "interpreter.h"

int main (int argc, char** argv)
{
    if (argc == 1)
    {
        Parser parser;
        Interpreter interpreter;
        std::string text;
        std::cout << ">>> ";
        while (std::getline(std::cin, text) && text != ":q")
        {
            Node* node = parser.ParseExpression(text);
            if (node == nullptr)
            {
                std::cout << ">>> ";
                continue;
            }
            Library::Write({ node }, &interpreter);
            if (node->nodeType == NodeType::FunctionCall && static_cast<FunctionCall*>(node)->functionName == "writeln")
            {
                std::cout << ">>> ";
                delete node;
                continue;
            }
            interpreter.FreeNodes();
            delete node;
            std::cout << std::endl;
            std::cout << ">>> ";
        }
    }
    else
    {
        std::ifstream in(argv[1], std::ios::ate);
        if (!in.is_open())
        {
            std::cout << "Couldn't find file " << argv[1] << std::endl;
            return 0;
        }
        uint64_t pos = in.tellg();
        in.seekg(0);
        std::string file;
        file.resize(pos);
        in.read(&file[0], pos);
        in.close();

        Parser parser;
        ProgramNode* program = parser.ParseProgram(file, argv[1]);
        if (program != nullptr)
        {
            // Parser::PrintAst(program);
            Interpreter interpreter;
            interpreter.Evaluate(program);
            interpreter.FreeNodes();
        }
        delete program;
    }

    return 0;
}