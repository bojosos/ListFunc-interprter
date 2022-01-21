#include "interpreter.h"
#include "library.h"
#include "parser.h"

#include <iostream>

Interpreter::Interpreter()
{
    m_LibraryFunctions["eq"] = Library::Equals;
    m_LibraryFunctions["le"] = Library::Less;
    m_LibraryFunctions["nand"] = Library::Nand;
    m_LibraryFunctions["length"] = Library::Length;
    m_LibraryFunctions["head"] = Library::Head;
    m_LibraryFunctions["tail"] = Library::Tail;
    m_LibraryFunctions["list"] = Library::List;
    m_LibraryFunctions["concat"] = Library::Concat;
    m_LibraryFunctions["if"] = Library::If;
    m_LibraryFunctions["read"] = Library::Read;
    m_LibraryFunctions["write"] = Library::Write;
    m_LibraryFunctions["writeln"] = Library::WriteLn;
    m_LibraryFunctions["int"] = Library::Int;

    m_LibraryFunctions["add"] = Library::Add;
    m_LibraryFunctions["sub"] = Library::Sub;
    m_LibraryFunctions["mul"] = Library::Mul;
    m_LibraryFunctions["div"] = Library::Div;
    m_LibraryFunctions["mod"] = Library::Mod;
    m_LibraryFunctions["sqrt"] = Library::Sqrt;
}

Node* Interpreter::Evaluate(Node* node, uint32_t elementCount)
{
    switch (node->nodeType)
    {
    case NodeType::Program:
    {
        ProgramNode* program = static_cast<ProgramNode*>(node);
        for (FunctionDecl* decl : program->functionDeclarations)
            m_SymbolTable[decl->functionName] = decl;
        for (Node* node : program->nodes)
            Evaluate(node);
        return nullptr; // TODO: return status code
    }
    case NodeType::FunctionCall: // args for functions are evaluated before the function is called
    {
        FunctionCall* funcCall = static_cast<FunctionCall*>(node);
        std::string t;
        for (int i = 0; i < m_Indent; i++)
            t += "  ";
        // std::cout << t << "Call: " << funcCall->functionName << " (";
        // for (uint8_t i = 0; i < funcCall->args.size(); i++)
        // {
        //     Node* node = funcCall->args[i];
        //     if (i != funcCall->args.size() - 1)
        //         std::cout << node->nodeType << ", ";
        //     else
        //         std::cout << node->nodeType;
        // }
        // std::cout << ")" << std::endl;

        // Parser::PrintNode(funcCall);
        auto iterFind = m_SymbolTable.find(funcCall->functionName);
        if (iterFind != m_SymbolTable.end())
        {
            std::unordered_map<uint8_t, Node*> args;
            for (uint8_t i = 0; i < funcCall->args.size(); i++)
                args[i] = Evaluate(funcCall->args[i]);
                // args[i] = funcCall->args[i];
            m_Arguments.push(args);
            m_ArgumentNames.push(funcCall->functionName);
            m_Indent += 1;
            Node* result = Evaluate(iterFind->second->body);
            // std::cout << t << result->nodeType << " ";
            // Parser::PrintNode(result);
            std::cout <<  std::endl;
            m_Indent -= 1;
            m_Arguments.pop();
            m_ArgumentNames.pop();
            return result;
        }

        auto libIter = m_LibraryFunctions.find(funcCall->functionName);
        if (libIter != m_LibraryFunctions.end())
            return libIter->second(funcCall->args, this);
        std::cout << "Function " << funcCall->functionName << " not found" << std::endl;
        return nullptr;
    }
    case NodeType::ListNode:
    {
        ListNode* listNode = static_cast<ListNode*>(node);
        ListNode* result = new ListNode();
        for (List* list : listNode->lists)
        {
            if (list->listType == ListType::LiteralList)
            {
                ListLiteral* lit = static_cast<ListLiteral*>(list);
                ListLiteral* newL = new ListLiteral();
                for (Node* node : lit->elements)
                    newL->elements.push_back(Evaluate(node));
                result->lists.push_back(newL);
            }
            else
                result->lists.push_back(list->Clone());
        }
        return result;
    }
    case NodeType::IntLiteral:
    case NodeType::FloatLiteral: return node;
    case NodeType::ArgId:
    {
        ArgId* argid = static_cast<ArgId*>(node);
        // std::cout << "Argid: " << argid->value << std::endl;
        std::cout << "Current layer: " << m_ArgumentNames.top() << std::endl;
        return m_Arguments.top()[argid->value];
        // return Evaluate(m_Arguments.top()[argid->value]);
    }
    }
}

ListNode* Interpreter::EvaluateList(Node* node, uint32_t size, uint32_t startIdx)
{/*
    switch (node->nodeType)
    {
    case NodeType::Program:
    {
        ProgramNode* program = static_cast<ProgramNode*>(node);
        for (FunctionDecl* decl : program->functionDeclarations)
            m_SymbolTable[decl->functionName] = decl;
        for (Node* node : program->nodes)
            Evaluate(node);
        return nullptr; // TODO: return status code
    }
    case NodeType::FunctionCall: // args for functions are evaluated before the function is called
    {
        FunctionCall* funcCall = static_cast<FunctionCall*>(node);
        // std::cout << "Call: " << funcCall->functionName << std::endl;
        auto iterFind = m_SymbolTable.find(funcCall->functionName);
        if (iterFind != m_SymbolTable.end())
        {
            std::unordered_map<uint8_t, Node*> args;
            for (uint8_t i = 0; i < funcCall->args.size(); i++)
                // args[i] = Evaluate(funcCall->args[i]);
                args[i] = funcCall->args[i];
            m_Arguments.push(args);
            Node* result = Evaluate(iterFind->second->body);
            m_Arguments.pop();
            return result;
        }

        auto libIter = m_LibraryFunctions.find(funcCall->functionName);
        if (libIter != m_LibraryFunctions.end())
            return libIter->second(funcCall->args, this);
        std::cout << "Function " << funcCall->functionName << " not found" << std::endl;
        return nullptr;
    }
    case NodeType::ListNode:
    case NodeType::IntLiteral:
    case NodeType::FloatLiteral: return node;
    case NodeType::ArgId:
    {
        ArgId* argid = static_cast<ArgId*>(node);
        // std::cout << "Argid: " << argid->value << std::endl;
        // return m_Arguments.top()[argid->value];
        return Evaluate(m_Arguments.top()[argid->value]);
    }
    }*/
}