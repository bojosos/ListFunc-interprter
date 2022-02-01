#include "nodes.h"
#include "interpreter.h"

#include <cassert>

ProgramNode::~ProgramNode()
{
    for (ModuleNode* module : modules)
        delete module;
    for (FunctionDecl* decl : functionDeclarations)
        delete decl;
    for (FunctionCall* call : nodes)
        delete call;
}

Node* ProgramNode::Clone(Interpreter* interpreter) const
{
    assert(false);
    ProgramNode* newNode = new ProgramNode(interpreter);
    return newNode;
}

ListLiteral::~ListLiteral()
{
    for (Node* node : elements)
        delete node;
}

List* ListLiteral::Clone(Interpreter* interpreter) const
{
    ListLiteral* newList = new ListLiteral();
    for (Node* node : elements)
    {
        Node* clone = node->Clone(interpreter);
        newList->elements.push_back(clone);
    }
    return newList;
}

List* FunctionalList::Clone(Interpreter* interpreter) const
{
    FunctionalList* newList = new FunctionalList();
    newList->startValue = startValue;
    newList->step = step;
    newList->size = size;
    return newList;
}

ListNode::~ListNode()
{
    for (List* list : lists)
        delete list;
}

Node* ListNode::Clone(Interpreter* interpreter) const
{
    ListNode* node = new ListNode(interpreter);
    for (List* list : lists)
        node->lists.push_back(list->Clone(interpreter));
    return node;
}

Node* IntNode::Clone(Interpreter* interpreter) const
{
    IntNode* newNode = new IntNode(interpreter);
    newNode->value = value;
    return newNode;
}

Node* FloatNode::Clone(Interpreter* interpreter) const
{
    FloatNode* newNode = new FloatNode(interpreter);
    newNode->value = value;
    return newNode;
}

Node* StringNode::Clone(Interpreter* interpreter) const
{
    StringNode* newNode = new StringNode(interpreter);
    newNode->string = string;
    return newNode;
}

Node* ArgId::Clone(Interpreter* interpreter) const
{
    ArgId* newNode = new ArgId(interpreter);
    newNode->value = value;
    return newNode;
}

FunctionCall::~FunctionCall()
{
    for (Node* arg : args)
        delete arg;
}

Node* FunctionCall::Clone(Interpreter* interpreter) const
{
    FunctionCall* newNode = new FunctionCall(nullptr);
    newNode->functionName = functionName;
    for (Node* arg : args)
        newNode->args.push_back(arg->Clone(nullptr));
    return newNode;
}

FunctionDecl::~FunctionDecl()
{
    delete body;
}

Node* FunctionDecl::Clone(Interpreter* interpreter) const
{
    FunctionDecl* newNode = new FunctionDecl(nullptr);
    newNode->functionName = functionName;
    newNode->body = body->Clone(interpreter);
    return newNode;
}

ModuleNode::~ModuleNode()
{
    delete moduleNode;
}

Node* ModuleNode::Clone(Interpreter* interpreter) const
{
    return nullptr;
}