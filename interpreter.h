#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <list>
#include <functional>

enum class NodeType
{
    FloatLiteral,
    IntLiteral,
    ListNode,

    FunctionCall,
    FunctionDecl,
    ArgId,

    Program,
    Module,
};

template <typename Os>
Os& operator<<(Os& out, NodeType type)
{
    switch (type)
    {
    case NodeType::FloatLiteral: out << "FloatLiteral"; break;
    case NodeType::IntLiteral: out << "IntLiteral"; break;
    case NodeType::ListNode: out << "ListNode"; break;
    case NodeType::FunctionCall: out << "FunctionCall"; break;
    case NodeType::FunctionDecl: out << "FunctionDecl"; break;
    case NodeType::ArgId: out << "ArgId"; break;
    case NodeType::Program: out << "Program"; break;
    case NodeType::Module: out << "Module"; break;
    }
    return out;
}

enum class ListType
{
    LiteralList,
    FunctionalList
};

class FunctionDecl;
class ModuleNode;

struct Node
{
    Node(NodeType type) : nodeType(type) { }

    virtual std::string Repr() const { return std::string(); }
    virtual Node* Clone() const = 0;
    NodeType nodeType;
};

struct ProgramNode : public Node
{
    ProgramNode() : Node(NodeType::Program) { }

    virtual Node* Clone() const override
    {
        assert(false);
        ProgramNode* newNode = new ProgramNode();
        return newNode;
    }

    std::vector<ModuleNode*> modules;
    std::vector<FunctionDecl*> functionDeclarations; // so that function decls can be used in the whole file
    std::vector<Node*> nodes; // other nodes (expressions)
};

struct List
{
    List(ListType listType) : listType(listType) { }

    virtual List* Clone() const = 0;
    ListType listType;
};

struct ListLiteral : public List
{
    ListLiteral() : List(ListType::LiteralList) { }

    virtual List* Clone() const override
    {
        ListLiteral* newList = new ListLiteral();
        for (Node* node : elements)
            newList->elements.push_back(node->Clone());
        return newList;
    }
    std::list<Node*> elements;
};

struct FunctionalList : public List
{
    FunctionalList() : List(ListType::FunctionalList) { }

    virtual List* Clone() const override
    {
        FunctionalList* newList = new FunctionalList();
        newList->startValue = startValue;
        newList->step = step;
        newList->size = size;
        return newList;
    }
    double startValue;
    double step;
    uint64_t size;
};

struct ListNode : public Node
{
    ListNode() : Node(NodeType::ListNode) { }
    
    virtual Node* Clone() const
    {
        ListNode* node = new ListNode();
        for (List* list : lists)
            node->lists.push_back(list->Clone());
        return node;
    }
    
    std::list<List*> lists; // because of concatenation of infinite lists
};

struct IntNode : public Node
{
    IntNode() : Node(NodeType::IntLiteral) { }

    virtual std::string Repr() const override { return std::to_string(value); }
    virtual Node* Clone() const override
    {
        IntNode* newNode = new IntNode();
        newNode->value = value;
        return newNode;
    }
    int64_t value;
};

struct FloatNode : public Node
{
    FloatNode() : Node(NodeType::FloatLiteral) { }

    virtual std::string Repr() const override { return std::to_string(value); }
    virtual Node* Clone() const override
    {
        FloatNode* newNode = new FloatNode();
        newNode->value = value;
        return newNode;
    }
    double value;
};

struct ArgId : public Node
{
    ArgId() : Node(NodeType::ArgId) { }

    virtual Node* Clone() const override
    {
        ArgId* newNode = new ArgId();
        newNode->value = value;
        return newNode;
    }
    uint8_t value;
};

struct FunctionCall : public Node
{
    FunctionCall() : Node(NodeType::FunctionCall) { }

    virtual Node* Clone() const override
    {
        FunctionCall* newNode = new FunctionCall();
        newNode->functionName = functionName;
        for (Node* arg : args)
            newNode->args.push_back(arg->Clone());
        return newNode;
    }

    std::string functionName;
    std::vector<Node*> args;
};

struct FunctionDecl : public Node
{
    FunctionDecl() : Node(NodeType::FunctionDecl) { }

    virtual Node* Clone() const override
    {
        FunctionDecl* newNode = new FunctionDecl();
        newNode->functionName = functionName;
        newNode->body = body->Clone();
        return newNode;
    }

    std::string functionName;
    Node* body;
};

struct ModuleNode : public Node
{
    ModuleNode() : Node(NodeType::Module) { }

    std::string moduleName;
    ProgramNode* moduleNode;

    virtual Node* Clone() const override
    {
        assert(false);
    }

};

class Interpreter
{
public:
    Interpreter();
    Node* Evaluate(Node* node);

    ListNode* EvaluateList(Node* node, uint32_t size, uint32_t startIdx = 0);

private:
    std::unordered_map<std::string, FunctionDecl*> m_SymbolTable;
    std::unordered_map<std::string, std::function<Node*(const std::vector<Node*>&, Interpreter*)>> m_LibraryFunctions;
    std::stack<std::unordered_map<uint8_t, Node*>> m_Arguments;
    std::stack<std::string> m_ArgumentNames;
    int m_Indent = 0;
};