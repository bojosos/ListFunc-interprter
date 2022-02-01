#pragma once

#include "nodes.h"

#include <iostream>
#include <stack>
#include <string>

class Parser
{
public:
    ProgramNode* ParseProgram(const std::string& text, const std::string& filename = "");
    Node* ParseExpression();
    Node* ParseExpression(const std::string& text);

private:
    Node* ParseNumberLiteral();
    ListNode* ParseListLiteral();
    FunctionCall* ParseFunctionCall(const std::string& name);
    FunctionDecl* ParseFunctionDecl(const std::string& name);
    StringNode* ParseString();
    ModuleNode* ParseImport();
    std::string ParseFunctionName();

    /**
     * @brief Get current char.
     */
    char Peek();

    /**
     * @brief Skip chars until a new line symbol. Used for comments.
     * 
     */
    void SkipLine();

    /**
     * @brief Move cursor.
     */
    void Advance(uint32_t count = 1);

    /**
     * @brief Skip chars while a condition is met.
     */
    template <typename Func>
    void SkipWhile(Func func)
    {
        while (func(m_Text[m_Pos]))
        {
            if (m_Pos >= m_Text.size())
                return;
            Advance();
        }
    }

    /**
     * @brief Called whenever the parser encouters an error.
     * 
     * @param message 
     */
    void OnError(const std::string& message);

    /**
     * @brief Called whenever a module import fails.
     */
    void OnModuleError(const std::string& message);

public:
    /**
     * @brief Helper function that prints the program node in a tree-line structure
     * 
     * @param node 
     */
    static void PrintAst(ProgramNode* node);

    /**
     * @brief Function that prints a node and its children in a tree-line structure.
     */
    static void PrintNode(Node* node, uint32_t tabs = 0);

private:
    bool m_InsideFunctionDecl = false;
    std::string m_Text;
    uint64_t m_Pos;

    std::vector<Node*> m_CreatedNodes;


    // error information
    std::string m_Expectation;
    std::string m_Filename;
    uint32_t m_LinePos = 1;
    uint32_t m_ColumnPos = 1;
};