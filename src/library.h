#pragma once

#include "interpreter.h"

#include <vector>

class Library
{
public:
    // Special function used for calling more than one function
    static Node* Call(const std::vector<Node*>& args, Interpreter* interpreter);

    // Special function used for testing
    static Node* Require(const std::vector<Node*>& args, Interpreter* interpreter);

    // Logic ops
    static Node* Equals(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Less(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Nand(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* If(const std::vector<Node*>& args, Interpreter* interpreter);
    
    // List ops
    static Node* Length(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Head(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Tail(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* List(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Cons(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Concat(const std::vector<Node*>& args, Interpreter* interpreter);
    
    // IO ops
    static Node* Read(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Write(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* WriteLn(const std::vector<Node*>& args, Interpreter* interpreter);

    // Math ops
    static Node* Int(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Add(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Sub(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Mul(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Div(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Mod(const std::vector<Node*>& args, Interpreter* interpreter);
    static Node* Sqrt(const std::vector<Node*>& args, Interpreter* interpreter);

private:
    static void WriteUtil(Node* node, Interpreter* interpreter);
};