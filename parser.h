#pragma once

#include "interpreter.h"

#include <stack>
#include <string>

enum class ErrorType
{

};

struct Error
{
    uint32_t line;
    uint32_t column;
    ErrorType errorType;
};

class Parser
{
public:
    ProgramNode* ParseProgram(const std::string& text);
    Node* ParseExpression();
    Node* ParseExpression(const std::string& text);

private:
    FloatNode* ParseRealLiteral();
    IntNode* ParseIntLiteral();
    ListNode* ParseListLiteral();
    FunctionCall* ParseFunctionCall();
    FunctionDecl* ParseFunctionDecl();
    ModuleNode* ParseImport();

    char Peek() const;
    void SkipLine();
    void Advance(uint32_t count = 1);

    template <typename Func>
    void SkipWhile(Func func)
    {
        while (func(m_Text[m_Pos]))
            Advance();
    }

public:
    static void PrintAst(ProgramNode* node);
    static void PrintNode(Node* node, uint32_t tabs = 0);

private:
    bool m_InsideFunctionBody = false;
    std::stack<Error> m_ErrorStack;
    std::string m_Text;
    uint64_t m_Pos;
};