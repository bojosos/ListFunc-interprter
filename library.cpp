#include "library.h"
#include "parser.h"

#include <iostream>
#include <cassert>
#include <cmath>

Node* Library::Equals(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 2);
    IntNode* node = new IntNode();
    node->value = static_cast<IntNode*>(interpreter->Evaluate(args[0]))->value == static_cast<IntNode*>(interpreter->Evaluate(args[1]))->value;
    return node;
}

Node* Library::Less(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 2);
    Node* l = interpreter->Evaluate(args[0]);
    Node* r = interpreter->Evaluate(args[1]);
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode();
            result->value = static_cast<IntNode*>(l)->value < static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<IntNode*>(l)->value < static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value < static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value < static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
}

Node* Library::Nand(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 2);
    // std::cout << "Nand" << std::endl;

    Node* l = interpreter->Evaluate(args[0]);
    Node* r = interpreter->Evaluate(args[1]);
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode();
            result->value = !static_cast<IntNode*>(l)->value || !static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = !static_cast<IntNode*>(l)->value || !static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode();
            result->value = !static_cast<FloatNode*>(l)->value || !static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = !static_cast<FloatNode*>(l)->value || !static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
}

Node* Library::Length(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 1);
    ListNode* listNode = static_cast<ListNode*>(interpreter->Evaluate(args[0]));
    IntNode* node = new IntNode();
    node->value = 0;
    for (auto* list : listNode->lists)
    {
        if (list->listType == ListType::FunctionalList)
        {
            FunctionalList* funcList = static_cast<FunctionalList*>(list);
            if (funcList->size != (uint64_t)-1)
            {
                node->value += funcList->size;
            }
            else
            {
                node->value = (uint64_t)-1;
                return node; // infinite list in chain, no need to keep going
            }
        }
        else
            node->value += static_cast<ListLiteral*>(list)->elements.size();
    }
    std::cout << "Length: " << node->value << std::endl;
    return node;
}

Node* Library::Head(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 1);
    std::cout << "Head: " << args[0]->nodeType << std::endl;
    ListNode* listNode = static_cast<ListNode*>(interpreter->Evaluate(args[0]));
    Parser::PrintNode(listNode);

    ::List* list = listNode->lists.front();
    if (list->listType == ListType::LiteralList)
        return interpreter->Evaluate(static_cast<ListLiteral*>(list)->elements.front());

    FloatNode* floatNode = new FloatNode();
    floatNode->value = static_cast<FunctionalList*>(list)->startValue;
    return floatNode;
}

Node* Library::Tail(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 1);
    ListNode* listNode = static_cast<ListNode*>(interpreter->Evaluate(args[0]));
    ListNode* listCopy = static_cast<ListNode*>(listNode->Clone());

    ::List* list = listCopy->lists.front();
    if (list->listType == ListType::FunctionalList)
    {
        FunctionalList* funcList = static_cast<FunctionalList*>(list);
        funcList->startValue += funcList->step;
        if (funcList->size != (uint64_t)-1)
            funcList->size -= 1;
        if (funcList->size == 0)
            listCopy->lists.pop_front();
    }
    else
    {
        ListLiteral* literal = static_cast<ListLiteral*>(list);
        literal->elements.pop_front();
        if (literal->elements.size() == 0)
            listCopy->lists.pop_front();
    }
    
    return listCopy;
}

Node* Library::List(const std::vector<Node*>& args, Interpreter* interpreter)
{    
    if (args.size() == 1)
    {
        std::cout << 1;
        double startValue = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value;
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = 1;
        list->size = (uint64_t)-1;

        ListNode* listNode = new ListNode();
        listNode->lists.push_back(list);
        return listNode;
    }
    else if (args.size() == 2)
    {
        std::cout << 2;
        double startValue = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value;
        double step = static_cast<FloatNode*>(interpreter->Evaluate((args[1])))->value;
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = step;
        list->size = (uint64_t)-1;

        ListNode* listNode = new ListNode();
        listNode->lists.push_back(list);
        return listNode;
    }
    else if (args.size() == 3)
    {
        double startValue = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value;
        double step = static_cast<FloatNode*>(interpreter->Evaluate(args[1]))->value;
        int64_t size = (int64_t)static_cast<FloatNode*>(interpreter->Evaluate(args[2]))->value;
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = step;
        list->size = size;

        ListNode* listNode = new ListNode();
        listNode->lists.push_back(list);
        return listNode;
    }
    return nullptr;
}

Node* Library::Concat(const std::vector<Node*>& args, Interpreter* interpreter)
{
    assert(args.size() == 2);
    ListNode* l1 = static_cast<ListNode*>(interpreter->Evaluate(args[0]));
    ListNode* l2 = static_cast<ListNode*>(interpreter->Evaluate(args[1]));
    std::cout << "Concat: ";
    Parser::PrintNode(l1);
    std::cout << " ";
    Parser::PrintNode(l2);
    std::cout << std::endl;
    
    ListNode* result = static_cast<ListNode*>(l1->Clone());
    for (::List* list : l2->lists)
        result->lists.push_back(list->Clone());
    return result;
}

Node* Library::If(const std::vector<Node*>& args, Interpreter* interpreter)
{
    // std::cout << "If" << std::endl;
    // std::cout << args[0]->nodeType << std::endl;
    // Parser::PrintNode(args[0]);
    IntNode* condition = static_cast<IntNode*>(interpreter->Evaluate(args[0]));
    if (condition->value)
    {
        // std::cout << "True: " << condition->value << std::endl;
        Node* node = interpreter->Evaluate(args[1]);
        // std::cout << node->nodeType << std::endl;
        return node;
    }
    // std::cout << "False: " << condition->value << std::endl;
    Node* node = interpreter->Evaluate(args[2]);
    // std::cout << node->nodeType << std::endl;
    return node;
}

Node* Library::Read(const std::vector<Node*>& args, Interpreter* interpreter)
{
    double value;
    std::cin >> value;
    FloatNode* result = new FloatNode();
    result->value = value;
    return result;
}

Node* Library::WriteLn(const std::vector<Node*>& args, Interpreter* interpreter)
{
    if (args.size() > 0)
    // Parser::PrintNode(args[0]);
    Write(args, interpreter);
    std::cout << std::endl;
    return nullptr;
}

Node* Library::Write(const std::vector<Node*>& args, Interpreter* interpreter)
{
    if (args.size() == 0)
        return nullptr;
    Node* evaluated = interpreter->Evaluate(args[0]);
    if (evaluated->nodeType == NodeType::IntLiteral)
        std::cout << static_cast<IntNode*>(evaluated)->value << " ";
    else if (evaluated->nodeType == NodeType::FloatLiteral)
        std::cout << static_cast<FloatNode*>(evaluated)->value << " ";
    else if (evaluated->nodeType == NodeType::ListNode)
    {
        std::cout << '[';
        bool printed = false;
        ListNode* listNode = static_cast<ListNode*>(evaluated);
        for (::List* list : listNode->lists)
        {
            if (list->listType == ListType::LiteralList)
            {
                ListLiteral* literal = static_cast<ListLiteral*>(list);
                for (Node* element : literal->elements)
                {
                    printed = true;
                    Write({ element }, interpreter);
                }
            }
            else
            {
                FunctionalList* funcList = static_cast<FunctionalList*>(list);
                for (uint64_t i = 0; i < funcList->size; i++)
                {
                    printed = true;
                    std::cout << funcList->startValue + funcList->step * i << " ";
                }
            }
        }
        if (printed)
            std::cout << "\b] "; // move one back
        else
            std::cout << "] "; // move one back
    }
    return nullptr;
}

Node* Library::Int(const std::vector<Node*>& args, Interpreter* interpreter)
{
    IntNode* result = new IntNode();
    Node* node = interpreter->Evaluate(args[0]);
    if (node->nodeType == NodeType::FloatLiteral)
        result->value = (int64_t)static_cast<FloatNode*>(node)->value;
    else
        result->value = static_cast<IntNode*>(node)->value;
    return result;
}

Node* Library::Add(const std::vector<Node*>& args, Interpreter* interpreter)
{
    Node* l = interpreter->Evaluate(args[0]);
    Node* r = interpreter->Evaluate(args[1]);
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode();
            result->value = static_cast<IntNode*>(l)->value + static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<IntNode*>(l)->value + static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value + static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value + static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
}

Node* Library::Sub(const std::vector<Node*>& args, Interpreter* interpreter)
{
    double value = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value - static_cast<FloatNode*>(interpreter->Evaluate(args[1]))->value;
    FloatNode* result = new FloatNode();
    result->value = value;
    return result;
}

Node* Library::Mul(const std::vector<Node*>& args, Interpreter* interpreter)
{
    // std::cout << (uint32_t)args[0]->nodeType << " " << (uint32_t)args[1]->nodeType << std::endl;
    Node* l = interpreter->Evaluate(args[0]);
    Node* r = interpreter->Evaluate(args[1]);
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode();
            result->value = static_cast<IntNode*>(l)->value * static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<IntNode*>(l)->value * static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value * static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            FloatNode* result = new FloatNode();
            result->value = static_cast<FloatNode*>(l)->value * static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
}

Node* Library::Div(const std::vector<Node*>& args, Interpreter* interpreter)
{
double value = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value / static_cast<FloatNode*>(interpreter->Evaluate(args[1]))->value;
    FloatNode* result = new FloatNode();
    result->value = value;
    return result;
}

Node* Library::Mod(const std::vector<Node*>& args, Interpreter* interpreter)
{
    Node* l = interpreter->Evaluate(args[0]);
    Node* r = interpreter->Evaluate(args[1]);
    if (l->nodeType == NodeType::IntLiteral && r->nodeType == NodeType::IntLiteral)
    {
        IntNode* intNode = new IntNode();
        intNode->value = static_cast<IntNode*>(l)->value % static_cast<IntNode*>(r)->value;
        // std::cout << static_cast<IntNode*>(l)->value << " " << static_cast<IntNode*>(r)->value << " " << intNode->value << std::endl;
        return intNode;
    }
    return nullptr;
    // Error here
}

Node* Library::Sqrt(const std::vector<Node*>& args, Interpreter* interpreter)
{
    double value = static_cast<FloatNode*>(interpreter->Evaluate(args[0]))->value;
    FloatNode* result = new FloatNode();
    result->value = std::sqrt(value);
    return result;
}
