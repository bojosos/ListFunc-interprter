#pragma once

#include "interpreter.h"

#include <vector>
#include <string>
#include <list>

enum class NodeType
{
    FloatLiteral,
    IntLiteral,
    ListNode,
    StringLiteral,

    FunctionCall,
    FunctionDecl,
    ArgId,

    Program,
    Module,
};

enum class ListType
{
    LiteralList,
    FunctionalList
};

struct FunctionDecl;
struct ModuleNode;
struct FunctionCall;
class Interpreter;

struct Node
{
    Node(NodeType type, Interpreter* interp = nullptr) : nodeType(type) { if (interp) interp->RegisterNode(this); }
    virtual ~Node() = default;

    virtual std::string Repr() const { return std::string(); }
    virtual Node* Clone(Interpreter* interp) const = 0;

    NodeType nodeType;
};

struct ProgramNode : public Node
{
    ProgramNode(Interpreter* interp = nullptr) : Node(NodeType::Program, interp) { }
    ~ProgramNode();

    virtual Node* Clone(Interpreter* interp) const override;

    std::vector<ModuleNode*> modules;
    std::vector<FunctionDecl*> functionDeclarations; // so that function decls can be used in the whole file
    std::vector<FunctionCall*> nodes; // other nodes (expressions)
};

struct List
{
    List(ListType listType) : listType(listType) { }
    virtual ~List() = default;

    virtual List* Clone(Interpreter* interp) const = 0;
    ListType listType;
};

struct ListLiteral : public List
{
    ListLiteral() : List(ListType::LiteralList) { }
    ~ListLiteral();

    virtual List* Clone(Interpreter* interp) const override;
    std::list<Node*> elements;
};

struct FunctionalList : public List
{
    FunctionalList() : List(ListType::FunctionalList) { }
    ~FunctionalList() = default;

    virtual List* Clone(Interpreter* interp) const override;
    double startValue;
    double step;
    uint64_t size;
};

struct ListNode : public Node
{
    ListNode(Interpreter* interp = nullptr) : Node(NodeType::ListNode, interp) { }
    ~ListNode();
    
    virtual Node* Clone(Interpreter* interp) const override;
    
    std::list<List*> lists;
};

struct IntNode : public Node
{
    IntNode(Interpreter* interp = nullptr) : Node(NodeType::IntLiteral, interp) { }
    ~IntNode() = default;

    virtual std::string Repr() const override { return std::to_string(value); }
    virtual Node* Clone(Interpreter* interp) const override;
    int64_t value;
};

struct FloatNode : public Node
{
    FloatNode(Interpreter* interp = nullptr) : Node(NodeType::FloatLiteral, interp) { }
    ~FloatNode() = default;

    virtual Node* Clone(Interpreter* interp) const override;
    double value;
};

struct StringNode : public Node
{
    StringNode(Interpreter* interp = nullptr) : Node(NodeType::StringLiteral, interp) { }
    ~StringNode() = default;

    virtual Node* Clone(Interpreter* interp) const override;

    std::string string;
};

struct ArgId : public Node
{
    ArgId(Interpreter* interp = nullptr) : Node(NodeType::ArgId, interp) { }
    ~ArgId() = default;

    virtual Node* Clone(Interpreter* interp) const override;
    
    uint32_t value;
};

struct FunctionCall : public Node
{
    FunctionCall(Interpreter* interp = nullptr) : Node(NodeType::FunctionCall, interp) { }
    ~FunctionCall();

    virtual Node* Clone(Interpreter* interp) const override;

    std::string functionName;
    std::vector<Node*> args;
};

struct FunctionDecl : public Node
{
    FunctionDecl(Interpreter* interp = nullptr) : Node(NodeType::FunctionDecl, interp) { }
    ~FunctionDecl();

    virtual Node* Clone(Interpreter* interp) const override;

    std::string functionName;
    Node* body;
};

struct ModuleNode : public Node
{
    ModuleNode(Interpreter* interp = nullptr) : Node(NodeType::Module, interp) { }
    ~ModuleNode();

    virtual Node* Clone(Interpreter* interp) const override;

    std::string moduleName;
    ProgramNode* moduleNode;
};