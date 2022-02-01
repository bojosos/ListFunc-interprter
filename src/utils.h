#pragma once

#include "nodes.h"

#include <string>

class Utils
{
public:
    static std::string NodeTypeToString(NodeType nodeType);
    
};

template <typename Os>
Os& operator<<(Os& out, NodeType type)
{
    out << Utils::NodeTypeToString(type);
    return out;
}

class NodeConverter
{
public:
    /**
     * @brief Helper for converting Int/Float node to FloatNode.
     */
    static FloatNode* ToFloat(Node* node, Interpreter* interpreter);
    /**
     * @brief Helper for converting Int/Float node to IntNode.
     */
    static IntNode* ToInt(Node* node, Interpreter* interpreter);
};