#include "parser.h"

#include <vector>
#include <iostream>
#include <fstream>

char Parser::Peek()
{
    if (m_Text[m_Pos] == '/')
        SkipLine();
    return m_Text[m_Pos];
}

void Parser::SkipLine()
{
    auto notNewLine = [](char c) { return c != '\n' && c != '\r'; };
    SkipWhile(notNewLine); // skip all symbols
    SkipWhile(isspace);
}

void Parser::Advance(uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        m_ColumnPos++;
        if (m_Text[m_Pos] == '\n')
        {
            m_LinePos += 1;
            m_ColumnPos = 1;
        }
        if (m_Text[++m_Pos] == '/')
        {
            SkipLine();
            SkipWhile(isspace);
        }
    }
}

#define TERM true

void Parser::OnError(const std::string& message)
{
    if (!m_Filename.empty())
        std::cout << "\e[1m" << m_Filename << ":" << m_LinePos << ":" << m_ColumnPos << "\e[0m" << ": \e[1;31msyntax error: \e[0m" << "\e[1m" << message << "\e[0m" << std::endl;
    else
        std::cout << "\e[1m" << m_ColumnPos << "\e[0m" << ": \e[1;31msyntax error: \e[0m" << "\e[1m" << message << "\e[0m" << std::endl;
}

void Parser::OnModuleError(const std::string& message)
{
    if (!m_Filename.empty())
        std::cout << "\e[1m" << m_Filename << ":" << m_LinePos << ":" << m_ColumnPos << "\e[0m" << ": \e[1;31mmodule error: \e[0m" << "\e[1m" << message << "\e[0m" << std::endl;
    else
        std::cout << "\e[1m" << m_ColumnPos << "\e[0m" << ": \e[1;31mmodule error: \e[0m" << "\e[1m" << message << "\e[0m" << std::endl;
}

ProgramNode* Parser::ParseProgram(const std::string& text, const std::string& filename)
{
    m_Text = text;
    m_Pos = 0;
    m_Filename = filename;
    m_CreatedNodes.clear();

    std::vector<FunctionDecl*> decls;
    std::vector<FunctionCall*> nodes;
    std::vector<ModuleNode*> modules;

    auto cleanup = [&]() {
        for (FunctionDecl* decl : decls)
            delete decl;
        for (FunctionCall* call : nodes)
            delete call;
        for (ModuleNode* module : modules)
            delete module->moduleNode;
    };

    while (m_Pos < m_Text.size())
    {
        SkipWhile(isspace);
        if (isalpha(Peek()) || Peek() == '_') // we have start of a string
        {
            std::string name = ParseFunctionName();
            SkipWhile(isspace);
            if (Peek() == '-') // look for ->
            {
                FunctionDecl* decl = ParseFunctionDecl(name);
                if (decl == nullptr)
                {
                    cleanup();
                    return nullptr;
                }
                decls.push_back(decl);
                SkipWhile(isspace);
                continue;
            }
            else
            {
                FunctionCall* call = ParseFunctionCall(name);
                if (call == nullptr)
                {
                    cleanup();
                    return nullptr;
                }
                nodes.push_back(call);
                SkipWhile(isspace);
            }
        }
        else
        {
            Node* node = ParseExpression();
            if (node == nullptr)
            {
                cleanup();
                return nullptr;
            }
            if (node->nodeType == NodeType::Module)
            {
                ModuleNode* moduleNode = static_cast<ModuleNode*>(node);
                std::string moduleName = moduleNode->moduleName + ".lfn";
                std::ifstream input(moduleName, std::ios::ate);
                if (!input.is_open())
                {
                    input = std::ifstream("lib/" + moduleName, std::ios::ate);
                    if (!input.is_open())
                    {
                        OnModuleError("module " + moduleName + " not found");
                        cleanup();
                        return nullptr;
                    }
                }
                size_t size = input.tellg();
                input.seekg(0);
                std::string data;
                data.resize(size);
                input.read(&data[0], size);
                input.close();

                Parser parser;
                ProgramNode* moduleProgram = parser.ParseProgram(data, moduleName);
                if (moduleProgram != nullptr)
                {
                    moduleNode->moduleNode = moduleProgram;
                    modules.push_back(moduleNode);
                }
                else
                {
                    cleanup();
                    OnModuleError("parsing error in: " + moduleName);
                    return nullptr;
                }
            }
            else if (node->nodeType == NodeType::FunctionCall)
                    nodes.push_back(static_cast<FunctionCall*>(node));
        }
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
    m_Pos = 0;
    m_ColumnPos = 0;
    m_Text = string;

    while (m_Pos < m_Text.size())
    {
        SkipWhile(isspace);
        if (isalpha(Peek()) || Peek() == '_') // we have start of a string
        {
            std::string name = ParseFunctionName();
            SkipWhile(isspace);
            if (Peek() == '-') // look for ->
            {
                FunctionDecl* decl = ParseFunctionDecl(name);
                return decl;
            }
            else
            {
                FunctionCall* call = ParseFunctionCall(name);
                return call;
            }
        }
        else
        {
            Node* node = ParseExpression();
            if (node == nullptr)
                return nullptr;
            if (node->nodeType == NodeType::Module)
            {
                ModuleNode* moduleNode = static_cast<ModuleNode*>(node);
                std::string moduleName = moduleNode->moduleName + ".lfn";
                std::ifstream input(moduleName, std::ios::ate);
                if (!input.is_open())
                {
                    input = std::ifstream("lib/" + moduleName, std::ios::ate);
                    if (!input.is_open())
                    {
                        OnModuleError("module " + moduleName + " not found");
                        return nullptr;
                    }
                }
                size_t size = input.tellg();
                input.seekg(0);
                std::string data;
                data.resize(size);
                input.read(&data[0], size);

                Parser parser;
                ProgramNode* moduleProgram = parser.ParseProgram(data, moduleName);
                if (moduleProgram != nullptr)
                {
                    moduleNode->moduleNode = moduleProgram;
                    if (m_Filename.empty())
                        std::cout << "\e[1m" << "Module imported successfully." << "\e[0m";
                    return moduleNode;
                }
                else
                {
                    OnModuleError("parsing error in: " + moduleName);
                    delete moduleNode;
                    return nullptr;
                }
            }
            else
                return node;
        }
        SkipWhile(isspace);
    }

    // ProgramNode* programNode = new ProgramNode();
    // programNode->functionDeclarations = std::move(decls);
    // programNode->nodes = std::move(nodes);
    // programNode->modules = std::move(modules);
    // return programNode;
    return nullptr;
}

Node* Parser::ParseExpression()
{
    if (Peek() == '[') // list
    {
        SkipWhile(isspace);
        return ParseListLiteral();
    }
    else if (std::isdigit(Peek()) || Peek() == '-') // number
        return ParseNumberLiteral();
    else if (std::isalpha(Peek()) || Peek() == '_')
        return ParseFunctionCall(ParseFunctionName());
    else if (Peek() == '"')
        return ParseString();
    else if (Peek() == '#')
    {
        if (m_InsideFunctionDecl) // arg
        {
            Advance();
            std::string val;
            while (isdigit(Peek()))
            {
                val += Peek();
                Advance();
            }
            uint32_t ival = 0;
            try {
                ival = (uint32_t)std::stoul(val);
            }
            catch (const std::exception& ex) {
                OnError(std::string("invalid argument index \"") + val + "\"");
                return nullptr;
            }
            ArgId* argid = new ArgId();
            argid->value = ival;
            return argid;
        }
        else if(m_Expectation.empty()) // check for expectation, because s1 might decide to do [#library] and expectation is set for lists
        {
            Advance();
            std::string moduleName;
            while (isalnum(Peek()) || Peek() == '_')
            {
                moduleName += Peek();
                Advance();
            }
            ModuleNode* moduleNode = new ModuleNode();
            moduleNode->moduleName = moduleName;
            return moduleNode;
        }
    }
    else
    {
        SkipWhile(isspace);
        if (!m_Expectation.empty())
            OnError(std::string("unexpected symbol '") + Peek() + "'" + " expected " + m_Expectation);
        else
            OnError(std::string("unexpected symbol '") + Peek() + "'");
        Advance();
    }
    
    return nullptr;
}

ListNode* Parser::ParseListLiteral()
{
    Advance(); // skip [
    std::list<Node*> elements;
    while (m_Pos < m_Text.size() && Peek() != ']')
    {
        m_Expectation = "list element or ']'";
        Node* expr = ParseExpression();
        m_Expectation.clear();
        if (expr == nullptr)
        {
            for (Node* el : elements)
                delete el;
            return nullptr;
        }
        elements.push_back(expr);
        SkipWhile(isspace);
    }
    if (Peek() != ']')
    {
        OnError("could not find end of list, perhaps you are missing a closing ]");
        for (Node* el : elements)
            delete el;
        return nullptr;
    }
    Advance(); // skip ]
    ListNode* list = new ListNode();
    ListLiteral* literal = new ListLiteral();
    literal->elements = std::move(elements);
    list->lists.push_back(literal);
    return list;
}

Node* Parser::ParseNumberLiteral()
{
    bool negative = false;
    if (Peek() == '-')
    {
        negative = true;
        Advance();
    }

    std::string num;
    bool isInt = true;
    while (isdigit(Peek()) || Peek() == '.')
    {
        if (Peek() == '.')
            isInt = false;
        num += Peek();
        Advance();
    }

    if (!isInt)
    {
        double dval = 0;
        try {
            dval = std::stod(num) * (negative ? -1 : 1);
        } catch (const std::exception& ex) {
            OnError(std::string("invalid real number value \"") + num + "\"");
            return nullptr;
        }
        FloatNode* node = new FloatNode();
        node->value = dval;
        return node;
    }
    else
    {
        int64_t ival = 0;
        try {
            ival = std::stol(num) * (negative ? -1 : 1);
        } catch (const std::exception& ex) {
            OnError(std::string("invalid int number value \"") + num + "\"");
            return nullptr;
        }
        IntNode* node = new IntNode();
        node->value = ival;
        return node;
    }
}

FunctionCall* Parser::ParseFunctionCall(const std::string& funcName)
{
    SkipWhile(isspace);
    if (Peek() != '(') // Error
    {
        OnError("could not find '(' after function name");
        return nullptr;
    }
    Advance();
    SkipWhile(isspace);
    std::vector<Node*> args;
    while (m_Pos < m_Text.size() && Peek() != ')')
    {
        SkipWhile(isspace);
        m_Expectation = "function arg";
        Node* arg = ParseExpression();
        m_Expectation.clear();
        if (arg == nullptr)
        {
            for (Node* a : args)
                delete a;
            return nullptr;
        }
        args.push_back(arg);
        
        SkipWhile(isspace);
        if (Peek() == ',') // in not comma and not ) error here
            Advance();
        else if (Peek() != ')')
        {
            OnError("expected closing ')' or ',' followed by another arg.");
            for (Node* a : args)
                delete a;
            return nullptr;
        }
        SkipWhile(isspace);
    }
    if (Peek() != ')')
    {
        OnError("could not find matching ')' after function arguments");
        for (Node* a : args)
            delete a;
        return nullptr;
    }
    Advance();
    FunctionCall* funcCall = new FunctionCall();
    funcCall->functionName = funcName;
    funcCall->args = std::move(args);
    return funcCall;
}

std::string Parser::ParseFunctionName()
{
    std::string funcName;
    while (isalnum(Peek()) || Peek() == '_')
    {
        funcName += Peek();
        Advance();
    }
    return funcName;
}

FunctionDecl* Parser::ParseFunctionDecl(const std::string& funcName)
{
    SkipWhile(isspace);
    if (Peek() != '-') // error
    {
        OnError("could not find \"->\" for function declaration");
        return nullptr;
    }
    Advance();
    if (Peek() != '>') // error
    {
        OnError("could not find \"->\" for function declaration");
        return nullptr;
    }
    Advance();
    SkipWhile(isspace);
    FunctionDecl* funcDecl = new FunctionDecl();
    funcDecl->functionName = funcName;
    m_InsideFunctionDecl = true;
    m_Expectation = "function body";
    funcDecl->body = ParseExpression();
    m_Expectation.clear();
    m_InsideFunctionDecl = false;
    return funcDecl;
}

StringNode* Parser::ParseString()
{
    Advance();
    std::string result;
    while (m_Pos < m_Text.size() && Peek() != '"')
    {
        result += Peek();
        Advance();
    }
    if (Peek() != '"')
    {
        OnError("could not find end of string, perhaps you are missing a closing \"");
        return nullptr;
    }
    Advance();
    StringNode* string = new StringNode();
    string->string = result;
    return string;
}

void Parser::PrintNode(Node* node, uint32_t tabs)
{
    std::string t;
    for (uint32_t i = 0; i < tabs; i++)
        t += "  ";
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