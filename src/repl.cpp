#include <iostream>

#include "parser.h"
#include "interpreter.h"
#include "library.h"

int main ()
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
    
    return 0;
}