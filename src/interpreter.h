#pragma once

// #include "nodes.h"

#include <cassert>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <functional>

class Node;
class FunctionDecl;

class Interpreter
{
public:
    Interpreter();
    ~Interpreter();
    /**
     * @brief Evaluate a node.
     * For module it means importing all modules
     * 
     * @param node 
     * @return Node* 
     */
    Node* Evaluate(Node* node);
    void FreeNodes();

private:
    void RegisterNode(Node* node) { m_AllocatedNodes.push_back(node); }
    void OnError(const std::string& message);
    bool IsErrorFlagSet() const;

    friend class Library;
    friend struct Node;

    bool m_ErrorFlag = false;
    std::stack<std::vector<Node*>> m_Arguments;
    std::stack<std::string> m_CallStack;
    std::unordered_map<std::string, FunctionDecl*> m_SymbolTable;
    std::unordered_map<std::string, std::function<Node*(const std::vector<Node*>&, Interpreter*)>> m_LibraryFunctions;

    std::vector<Node*> m_AllocatedNodes;
};