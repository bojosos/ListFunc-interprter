#pragma once

#include "interpreter.h"

#include <vector>

class Library
{
public:
    static Node* Equals(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Less(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Nand(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Length(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Head(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Tail(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* List(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Concat(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* If(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Read(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Write(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* WriteLn(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Int(const std::vector<Node*>& args, Interpreter* interpreter);

    static Node* Add(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Sub(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Mul(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Div(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Mod(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Sqrt(const std::vector<Node*>& args, Interpreter* interpreter);
};