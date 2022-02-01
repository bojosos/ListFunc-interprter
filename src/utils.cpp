#include "utils.h"

std::string Utils::NodeTypeToString(NodeType nodeType)
{
    switch (nodeType)
    {
    case NodeType::FloatLiteral: return "FloatLiteral";
    case NodeType::IntLiteral: return "IntLiteral";
    case NodeType::StringLiteral: return "StringLiteral";
    case NodeType::ListNode: return "ListNode";
    case NodeType::FunctionCall: return "FunctionCall";
    case NodeType::FunctionDecl: return "FunctionDecl";
    case NodeType::ArgId: return "ArgId";
    case NodeType::Program: return "Program";
    case NodeType::Module: return "Module";
    default: return "";
    }   
}

FloatNode* NodeConverter::ToFloat(Node* node, Interpreter* interpreter)
{
    FloatNode* res = new FloatNode(interpreter);
    if (node->nodeType == NodeType::IntLiteral)
        res->value = static_cast<IntNode*>(node)->value;
    else if (node->nodeType == NodeType::FloatLiteral)
        res->value = static_cast<FloatNode*>(node)->value;
    return res;
}

IntNode* NodeConverter::ToInt(Node* node, Interpreter* interpreter)
{
    IntNode* res = new IntNode(interpreter);
    if (node->nodeType == NodeType::IntLiteral)
        res->value = static_cast<IntNode*>(node)->value;
    else if (node->nodeType == NodeType::FloatLiteral)
        res->value = static_cast<FloatNode*>(node)->value;
    return res;
}