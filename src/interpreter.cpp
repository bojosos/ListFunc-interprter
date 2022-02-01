#include "interpreter.h"
#include "library.h"
#include "parser.h"
#include "nodes.h"

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
    m_LibraryFunctions["write"] = Library::Write;
    m_LibraryFunctions["writeln"] = Library::WriteLn;
    m_LibraryFunctions["int"] = Library::Int;
    m_LibraryFunctions["read"] = Library::Read;

    m_LibraryFunctions["add"] = Library::Add;
    m_LibraryFunctions["sub"] = Library::Sub;
    m_LibraryFunctions["mul"] = Library::Mul;
    m_LibraryFunctions["div"] = Library::Div;
    m_LibraryFunctions["mod"] = Library::Mod;
    m_LibraryFunctions["sqrt"] = Library::Sqrt;

    m_LibraryFunctions["call"] = Library::Call;
    m_LibraryFunctions["require"] = Library::Require;
}

Interpreter::~Interpreter()
{
    FreeNodes();
    for (auto kv : m_SymbolTable)
        delete kv.second;
}

Node* Interpreter::Evaluate(Node* node)
{
    std::function<void(ProgramNode*)> importModules = [&](ProgramNode* program) {
        for (ModuleNode* decls : program->modules)
        {
            for (FunctionDecl* decl : decls->moduleNode->functionDeclarations)
                m_SymbolTable[decl->functionName] = static_cast<FunctionDecl*>(decl->Clone(this));
            for (ModuleNode* module : decls->moduleNode->modules)
                importModules(decls->moduleNode);
        }
    };

    switch (node->nodeType)
    {
    case NodeType::Program:
    {
        ProgramNode* program = static_cast<ProgramNode*>(node);
        // add imported functions
        importModules(program);

        // add functions from current module
        for (FunctionDecl* decl : program->functionDeclarations)
        {
            FunctionDecl* declCopy = static_cast<FunctionDecl*>(decl->Clone(this));
            m_SymbolTable[decl->functionName] = declCopy;
        }
        // add function calls
        for (Node* node : program->nodes)
        {
            if (m_ErrorFlag)
                return nullptr;
            Evaluate(node);
        }
        return nullptr; // TODO: return status code
    }
    case NodeType::FunctionCall: // args for functions are evaluated before the function is called
    {
        FunctionCall* funcCall = static_cast<FunctionCall*>(node);        
        m_CallStack.push(funcCall->functionName);
        auto iterFind = m_SymbolTable.find(funcCall->functionName);
        if (iterFind != m_SymbolTable.end())
        {
            std::vector<Node*> newArgs;
            newArgs.resize(funcCall->args.size());
            for (uint32_t i = 0; i < funcCall->args.size(); i++)
            {
                Node* arg = Evaluate(funcCall->args[i]);
                if (arg == nullptr)
                {
                    for (Node* arg : newArgs)
                        delete arg;
                    return nullptr;
                }
                newArgs[i] = arg;
            }
            m_Arguments.push(newArgs);
            Node* result = Evaluate(iterFind->second->body);
            if (result != nullptr)
            {
                m_CallStack.pop();
                m_Arguments.pop();
            }
            return result;
        }

        auto libIter = m_LibraryFunctions.find(funcCall->functionName);
        if (libIter != m_LibraryFunctions.end())
        {
            Node* node = libIter->second(funcCall->args, this);
            if (!m_ErrorFlag)
                m_CallStack.pop();
            return node;
        }
        OnError("function " + funcCall->functionName + " not found");
        return nullptr;
    }
    case NodeType::ListNode:
    {
        ListNode* listNode = static_cast<ListNode*>(node);
        ListNode* result = new ListNode(this);
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
                result->lists.push_back(list->Clone(this));
        }
        return result;
    }
    case NodeType::IntLiteral:
    case NodeType::StringLiteral:
    case NodeType::FloatLiteral:
    {
        Node* clone = node->Clone(this);
        return clone;
    }
    case NodeType::FunctionDecl:
    {
        FunctionDecl* decl = static_cast<FunctionDecl*>(node->Clone(this));
        m_SymbolTable[decl->functionName] = decl;
        return nullptr;
    }
    case NodeType::Module:
    {
        ModuleNode* modu = static_cast<ModuleNode*>(node);
        for (FunctionDecl* decl : modu->moduleNode->functionDeclarations)
            m_SymbolTable[decl->functionName] = static_cast<FunctionDecl*>(decl->Clone(this));

        importModules(modu->moduleNode);
        return nullptr;
    }
    case NodeType::ArgId:
    {
        ArgId* argid = static_cast<ArgId*>(node);
        if (m_Arguments.top().size() <= argid->value)
        {
            OnError("argument with id " + std::to_string(argid->value) + " was not provided");
            return nullptr;
        }
        return m_Arguments.top()[argid->value];
    }
    }
    return nullptr;
}

void Interpreter::FreeNodes()
{
    for (Node* node : m_AllocatedNodes)
    {
        if (node->nodeType == NodeType::ListNode) // removel list items, so they aren't deleted twice
        {
            ListNode* listNode = static_cast<ListNode*>(node);
            for (List* list : listNode->lists)
            {
                if (list->listType == ListType::LiteralList)
                {
                    ListLiteral* l = static_cast<ListLiteral*>(list);
                    l->elements.clear();
                }
            }
        }
        else if (node->nodeType == NodeType::FunctionCall)
            static_cast<FunctionCall*>(node)->args.clear();
        else if (node->nodeType == NodeType::FunctionDecl)
            static_cast<FunctionDecl*>(node)->body = nullptr;
        delete node;
    }

    m_AllocatedNodes.clear();
    while (!m_Arguments.empty())
        m_Arguments.pop();
    while (!m_CallStack.empty())
        m_CallStack.pop();
    m_ErrorFlag = false;
}

void Interpreter::OnError(const std::string& message)
{
    // FreeNodes();
    std::cout << "\e[1;31mruntime error: \e[0m" << "\e[1m" << message << "\e[0m" << std::endl;
    if (!m_CallStack.empty())
    {
        std::cout << "Callstack:\n";
        while (!m_CallStack.empty())
        {
            std::cout << "\t" << m_CallStack.top() << std::endl;
            m_CallStack.pop();
        }
    }
    m_ErrorFlag = true;
}

bool Interpreter::IsErrorFlagSet() const
{
    return m_ErrorFlag;
}