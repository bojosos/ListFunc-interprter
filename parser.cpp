#include "parser.h"

#include <vector>
#include <iostream>
#include <fstream>

char Parser::Peek() const
{
    return m_Text[m_Pos];
}

void Parser::SkipLine()
{
    auto notNewLine = [](char c) { return c != '\n'; };
    SkipWhile(notNewLine); // skip all symbols
    Advance(); // skip \n
}

void Parser::Advance(uint32_t count)
{
    m_Pos += count;
}

ProgramNode* Parser::ParseProgram(const std::string& text)
{
    m_Text = text;
    m_Pos = 0;

    std::vector<FunctionDecl*> decls;
    std::vector<Node*> nodes;
    std::vector<ModuleNode*> modules;
    while (m_Pos < m_Text.size())
    {
        SkipWhile(isspace);
        if (isalpha(Peek())) // we have start of a string
        {
            uint64_t oldPos = m_Pos;
            SkipWhile(isalnum); // stupid lookahead, fine as it's only done for declarations once
            SkipWhile(isspace);
            if (Peek() == '-') // look for ->
            {
                m_Pos = oldPos;
                decls.push_back(ParseFunctionDecl());
                SkipWhile(isspace);
                continue;
            }
            m_Pos = oldPos;
        }
        Node* node = ParseExpression();
        if (node->nodeType == NodeType::Module)
        {
            ModuleNode* moduleNode = static_cast<ModuleNode*>(node);
            std::string moduleName = moduleNode->moduleName + ".lfn";
            std::ifstream input(moduleName, std::ios::ate);
            if (!input.is_open())
            {
                std::cout << "Module " << moduleNode->moduleName << " not found." << std::endl;
                continue;
            }
            size_t size = input.tellg();
            input.seekg(0);
            std::string data;
            data.resize(size);
            input.read(&data[0], size);

            Parser parser;
            ProgramNode* moduleProgram = parser.ParseProgram(data);
            if (moduleProgram != nullptr)
            {
                moduleNode->moduleNode = moduleProgram;
                modules.push_back(moduleNode);
            }
        }
        else
            nodes.push_back(node);
        SkipWhile(isspace);
    }

    ProgramNode* programNode = new ProgramNode();
    programNode->functionDeclarations = std::move(decls);
    programNode->nodes = std::move(nodes);
    programNode->modules = std::move(modules);
    return programNode;
}

Node* Parser::ParseExpression(const std::string& string)
{
    // m_Text = string;
    // return ParseExpression();
    return nullptr;
}

Node* Parser::ParseExpression()
{
    // std::cout << Peek() << std::endl;
    if (Peek() == '[') // list
    {
        SkipWhile(isspace);
        return ParseListLiteral();
    }
    else if (std::isdigit(Peek()) || Peek() == '-')
        return ParseRealLiteral();
    else if (std::isalpha(Peek()))
        return ParseFunctionCall();
    else if (Peek() == '#')
    {
        if (m_InsideFunctionBody) // arg
        {
            Advance();
            std::string val;
            while (isdigit(Peek()))
            {
                val += Peek();
                Advance();
            }
            ArgId* argid = new ArgId();
            argid->value = (uint8_t)std::stoul(val);
            return argid;
        }
        else // otherwise module
        {
            Advance();
            std::string moduleName;
            while (isalnum(Peek()))
            {
                moduleName += Peek();
                Advance();
            }
            ModuleNode* moduleNode = new ModuleNode();
            moduleNode->moduleName = moduleName;
            return moduleNode;
        }
    }
    // std::cout << m_InsideFunctionBody << " " << m_Pos << std::endl;
    // std::cout << Peek() << std::endl;
    // std::cout << "Error" << std::endl;
    return nullptr;
}

ListNode* Parser::ParseListLiteral()
{
    Advance(); // skip [
    std::list<Node*> elements;
    while (Peek() != ']')
    {
        elements.push_back(ParseExpression());
        SkipWhile(isspace);
    }
    if (m_ErrorStack.empty())
    {
        Advance(); // skip ]
        ListNode* list = new ListNode();
        ListLiteral* literal = new ListLiteral();
        literal->elements = std::move(elements);
        list->lists.push_back(literal);
        return list;
    }
    return nullptr;
}

FloatNode* Parser::ParseRealLiteral()
{
    bool negative = false;
    if (Peek() == '-')
        negative = true;
    std::string num;
    while (isdigit(Peek()) || Peek() == '.')
    {
        num += Peek();
        Advance();
    }

    FloatNode* node = new FloatNode();
    node->value = std::stod(num) * (negative ? -1 : 1);
    return node;
}

FunctionCall* Parser::ParseFunctionCall()
{
    std::string funcName;
    while (isalnum(Peek()))
    {
        funcName += Peek();
        Advance();
    }
    SkipWhile(isspace);
    if (Peek() != '(') // Error
        void();
    Advance();
    std::vector<Node*> args;
    while (Peek() != ')')
    {
        SkipWhile(isspace);
        args.push_back(ParseExpression());
        if (!m_ErrorStack.empty())
            return nullptr;
        SkipWhile(isspace);
        if (Peek() == ',') // in not comma and not ) error here
            Advance();
        SkipWhile(isspace);
    }
    Advance();
    FunctionCall* funcCall = new FunctionCall();
    funcCall->functionName = funcName;
    funcCall->args = args;
    return funcCall;
}

FunctionDecl* Parser::ParseFunctionDecl()
{
    std::string funcName;
    while (isalnum(Peek()))
    {
        funcName += Peek();
        Advance();
    }
    
    SkipWhile(isspace);
    if (Peek() != '-') // error
        void();
    Advance();
    if (Peek() != '>') // error
        void();
    Advance();
    SkipWhile(isspace);
    FunctionDecl* funcDecl = new FunctionDecl();
    funcDecl->functionName = funcName;
    m_InsideFunctionBody = true;
    funcDecl->body = ParseExpression();
    m_InsideFunctionBody = false;
    return funcDecl;
}

void Parser::PrintNode(Node* node, uint32_t tabs)
{
    std::string t;
    for (uint32_t i = 0; i < tabs; i++)
        t += '\t';
    std::cout << t;
    if (node->nodeType == NodeType::ArgId)
        std::cout << "ArgId: #" << (int)static_cast<ArgId*>(node)->value << std::endl;
    else if (node->nodeType == NodeType::FloatLiteral)
        std::cout << "Float: " << static_cast<FloatNode*>(node)->value << std::endl;
    else if (node->nodeType == NodeType::IntLiteral)
        std::cout << "Int: " << static_cast<IntNode*>(node)->value << std::endl;
    else if (node->nodeType == NodeType::FunctionCall)
    {
        std::cout << "Function Call: " << static_cast<FunctionCall*>(node)->functionName << std::endl;
        for (Node* node : static_cast<FunctionCall*>(node)->args)
            PrintNode(node, tabs+1);
    }
    else if (node->nodeType == NodeType::ListNode)
    {
        ListNode* list = static_cast<ListNode*>(node);
        std::cout << "[";
        for (List* l : list->lists)
        {
            if (l->listType == ListType::FunctionalList)
            {
                FunctionalList* fnlst = static_cast<FunctionalList*>(l);
                 std::cout << fnlst->startValue << ", " << fnlst->step << ", " << fnlst->size;
            }
            else
            {
                ListLiteral* litlst = static_cast<ListLiteral*>(l);
                for (Node* node : litlst->elements)
                    if (node->nodeType == NodeType::IntLiteral || node->nodeType == NodeType::FloatLiteral)
                        PrintNode(node, tabs+1);
            }
            std::cout << " ";
        }
        std::cout << "]";
    }
}

void Parser::PrintAst(ProgramNode* node)
{
    std::cout << node->functionDeclarations.size() << std::endl;
    std::cout << node->nodes.size() << std::endl;
    if (node->functionDeclarations.size() == 0)
        std::cout << "Here" << std::endl;
    for (FunctionDecl* decl : node->functionDeclarations)
    {
        std::cout << "Function decl: " << decl->functionName << " -> ";
        PrintNode(decl->body);
    }
    for (Node* node : node->nodes)
        PrintNode(node);
}