#include "library.h"
#include "parser.h"
#include "utils.h"

#include "test_runner.h"

#include <iostream>
#include <cassert>
#include <sstream>
#include <cmath>

#define ASSERT_ARGC(func, argc, provided_argc) \
    if (provided_argc < argc) {\
        interpreter->OnError(#func + std::string(" requries at least ") + std::to_string(argc) + " arg but only " + std::to_string(provided_argc) + " provided"); \
        return nullptr; \
    }

#define ASSERT_INT_NODE_TYPE(func, type) \
    if (type != NodeType::IntLiteral) {\
        interpreter->OnError(#func + std::string(" expects Int as argument but got ") + Utils::NodeTypeToString(type) + " instead."); \
        return nullptr; \
    }

#define ASSERT_NUM_NODE_TYPE(func, type) \
    if (type != NodeType::IntLiteral && type != NodeType::FloatLiteral) {\
        interpreter->OnError(#func + std::string(" expects Int/Float as argument but got ") + Utils::NodeTypeToString(type) + " instead."); \
        return nullptr; \
    }

#define ASSERT_FUNC_NODE_TYPE(func, type) \
    if (type != NodeType::FunctionCall) {\
        interpreter->OnError(#func + std::string(" expects FunctionCall as argument but got ") + Utils::NodeTypeToString(type) + " instead."); \
        return nullptr; \
    }

#define ASSERT_LIST_NODE_TYPE(func, type) \
    if (type != NodeType::ListNode) {\
        interpreter->OnError(#func + std::string(" expects List as argument but got ") + Utils::NodeTypeToString(type) + " instead."); \
        return nullptr; \
    }

Node* Library::Equals(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(eq, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(eq, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(eq, r->nodeType);
    if (interpreter->IsErrorFlagSet())
        return nullptr;

    if (l == nullptr)
    {
        if (r == nullptr)
        {
            IntNode* result = new IntNode();
            result->value = true;
            return result;
        }
        else
        {
            IntNode* result = new IntNode();
            result->value = false;
            return result;
        }
    }

    IntNode* total = nullptr;
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value == static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value == static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value == static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value == static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    if (args.size() > 2 && total != nullptr && total->value == 1)
    {
        auto vec = args;
        vec.erase(vec.begin());
        Node* res = Equals(vec, interpreter);
        if (res != nullptr)
        {
            IntNode* intNode = static_cast<IntNode*>(res);
            if (intNode->value == 0)
                return intNode;
        }
        else
            return nullptr;
    }
    return total;
}

Node* Library::Less(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(less, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(less, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(less, r->nodeType);
    if (interpreter->IsErrorFlagSet())
        return nullptr;

    IntNode* total = nullptr;
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value < static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value < static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value < static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value < static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    
    if (args.size() > 2 && total != nullptr && total->value == 1)
    {
        auto vec = args;
        vec.erase(vec.begin());
        Node* res = Less(vec, interpreter);
        if (res != nullptr)
        {
            IntNode* intNode = static_cast<IntNode*>(res);
            if (intNode->value == 0)
                return intNode;
        }
        else
            return nullptr;
    }
    return total;
}

Node* Library::Nand(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(nand, 2, args.size());
    
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(nand, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(nand, r->nodeType);

    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = !static_cast<IntNode*>(l)->value || !static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = !static_cast<IntNode*>(l)->value || !static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = !static_cast<FloatNode*>(l)->value || !static_cast<IntNode*>(r)->value;
            return result;
        }
        else
        {
            IntNode* result = new IntNode(interpreter);
            result->value = !static_cast<FloatNode*>(l)->value || !static_cast<FloatNode*>(r)->value;
            return result;
        }
    }
}

Node* Library::Length(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(length, 1, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_LIST_NODE_TYPE(length, l->nodeType);
    ListNode* listNode = static_cast<ListNode*>(l);
    IntNode* node = new IntNode(interpreter);
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
    return node;
}

Node* Library::Head(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(head, 1, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_LIST_NODE_TYPE(Head, l->nodeType);

    ListNode* listNode = static_cast<ListNode*>(l);

    ::List* list = listNode->lists.front();
    if (list->listType == ListType::LiteralList)
        return interpreter->Evaluate(static_cast<ListLiteral*>(list)->elements.front());

    FloatNode* floatNode = new FloatNode(interpreter);
    floatNode->value = static_cast<FunctionalList*>(list)->startValue;
    return floatNode;
}

Node* Library::Tail(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(tail, 1, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_LIST_NODE_TYPE(tail, l->nodeType);

    ListNode* listNode = static_cast<ListNode*>(l);
    ListNode* listCopy = static_cast<ListNode*>(listNode->Clone(interpreter));

    ::List* list = listCopy->lists.front();
    if (list->listType == ListType::FunctionalList)
    {
        FunctionalList* funcList = static_cast<FunctionalList*>(list);
        funcList->startValue += funcList->step;
        if (funcList->size != (uint64_t)-1)
            funcList->size -= 1;
        if (funcList->size == 0)
        {
            delete listCopy->lists.front();
            listCopy->lists.pop_front();
        }
    }
    else
    {
        ListLiteral* literal = static_cast<ListLiteral*>(list);
        literal->elements.pop_front();
        if (literal->elements.size() == 0)
        {
            delete listCopy->lists.front();
            listCopy->lists.pop_front();
        }
    }
    
    return listCopy;
}

Node* Library::List(const std::vector<Node*>& args, Interpreter* interpreter)
{    
    ASSERT_ARGC(list, 1, args.size());
    Node* arg0 = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_INT_NODE_TYPE(list, arg0->nodeType);

    double startValue = NodeConverter::ToFloat(arg0, interpreter)->value;
    if (args.size() == 1)
    {
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = 1;
        list->size = (uint64_t)-1;

        ListNode* listNode = new ListNode(interpreter);
        listNode->lists.push_back(list);
        return listNode;
    }
    Node* arg1 = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_INT_NODE_TYPE(list, arg1->nodeType);

    double step = NodeConverter::ToFloat(arg1, interpreter)->value;
    if (args.size() == 2)
    {
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = step;
        list->size = (uint64_t)-1;

        ListNode* listNode = new ListNode(interpreter);
        listNode->lists.push_back(list);
        return listNode;
    }
    Node* arg2 = interpreter->Evaluate(args[2]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_INT_NODE_TYPE(list, arg2->nodeType);

    uint64_t size = NodeConverter::ToInt(arg2, interpreter)->value;
    if (args.size() >= 3)
    {
        FunctionalList* list = new FunctionalList();
        list->startValue = startValue;
        list->step = step;
        list->size = size;

        ListNode* listNode = new ListNode(interpreter);
        listNode->lists.push_back(list);
        return listNode;
    }
    return nullptr;
}

Node* Library::Concat(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(concat, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_LIST_NODE_TYPE(caoncat, l->nodeType);


    ListNode* l1 = static_cast<ListNode*>(l);

    ListNode* result = static_cast<ListNode*>(l1->Clone(interpreter));
    for (uint32_t i = 1; i < args.size(); i++)
    {
        Node* ni = interpreter->Evaluate(args[i]);
        if (interpreter->IsErrorFlagSet())
            return nullptr;
        ASSERT_LIST_NODE_TYPE(concat, ni->nodeType);

        ListNode* ri = static_cast<ListNode*>(ni);
        
        for (::List* list : ri->lists)
            result->lists.push_back(list->Clone(interpreter));
    }
    return result;
}

Node* Library::If(const std::vector<Node*>& args, Interpreter* interpreter)
{
    Node* node = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(if, node->nodeType);

    bool value = false;
    if (node->nodeType == NodeType::IntLiteral)
        value = static_cast<IntNode*>(node)->value;
    else
        value = static_cast<FloatNode*>(node)->value;
    if (value)
    {
        Node* node = interpreter->Evaluate(args[1]);
        return node;
    }
    return interpreter->Evaluate(args[2]);
}

Node* Library::Read(const std::vector<Node*>& args, Interpreter* interpreter)
{
    double value;
    std::cout << "read: ";
    std::cin >> value;
    std::cin.ignore();
    FloatNode* result = new FloatNode(interpreter);
    result->value = value;
    return result;
}

Node* Library::Int(const std::vector<Node*>& args, Interpreter* interpreter)
{
    IntNode* result = new IntNode(interpreter);
    Node* node = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(int, node->nodeType);

    if (node->nodeType == NodeType::FloatLiteral)
        result->value = (int64_t)static_cast<FloatNode*>(node)->value;
    else
        result->value = static_cast<IntNode*>(node)->value;
    return result;
}

Node* Library::Add(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(add, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(add, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(add, r->nodeType);

    Node* total = nullptr;

    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value + static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value + static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value + static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value + static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    if (args.size() > 2)
    {
        auto vec = args;
        vec.erase(vec.begin());
        vec.erase(vec.begin());
        vec.insert(vec.begin(), total);
        return Add(vec, interpreter);
    }
    return total;
}

Node* Library::Sub(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(sub, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(sub, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(sub, r->nodeType);

    Node* total = nullptr;
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value - static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value - static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value - static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value - static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    if (args.size() > 2)
    {
        auto vec = args;
        vec.erase(vec.begin());
        vec.erase(vec.begin());
        vec.insert(vec.begin(), total);
        return Sub(vec, interpreter);
    }
    return total;
}

Node* Library::Mul(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(mul, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(mul, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(mul, r->nodeType);

    Node* total = nullptr;
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            IntNode* result = new IntNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value * static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value * static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value * static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value * static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    if (args.size() > 2)
    {
        auto vec = args;
        vec.erase(vec.begin());
        vec.erase(vec.begin());
        vec.insert(vec.begin(), total);
        return Mul(vec, interpreter);
    }
    return total;
}

Node* Library::Div(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(div, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(div, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(div, r->nodeType);

    Node* total = nullptr;
    if (l->nodeType == NodeType::IntLiteral)
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = (float)static_cast<IntNode*>(l)->value / static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<IntNode*>(l)->value / static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    else
    {
        if (r->nodeType == NodeType::IntLiteral)
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value / static_cast<IntNode*>(r)->value;
            total = result;
        }
        else
        {
            FloatNode* result = new FloatNode(interpreter);
            result->value = static_cast<FloatNode*>(l)->value / static_cast<FloatNode*>(r)->value;
            total = result;
        }
    }
    if (args.size() > 2)
    {
        auto vec = args;
        vec.erase(vec.begin());
        vec.erase(vec.begin());
        vec.insert(vec.begin(), total);
        return Div(vec, interpreter);
    }
    return total;
}

Node* Library::Mod(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(mod, 2, args.size());
    Node* l = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_INT_NODE_TYPE(mod, l->nodeType);
    Node* r = interpreter->Evaluate(args[1]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_INT_NODE_TYPE(mod, r->nodeType);

    IntNode* intNode = new IntNode(interpreter);
    intNode->value = static_cast<IntNode*>(l)->value % static_cast<IntNode*>(r)->value;
    return intNode;
}

Node* Library::Sqrt(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(sqrt, 1, args.size());
    Node* node = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(sqrt, node->nodeType);

    FloatNode* result = new FloatNode(interpreter);
    if (node->nodeType == NodeType::FloatLiteral)
    {
        double value = static_cast<FloatNode*>(node)->value;
        result->value = std::sqrt(value);
    }
    else if (node->nodeType == NodeType::IntLiteral)
    {
        int64_t value = static_cast<IntNode*>(node)->value;
        result->value = std::sqrt(value);
    }
    return result;
}

Node* Library::Call(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(call, 1, args.size());

    for (Node* arg : args)
    {
        ASSERT_FUNC_NODE_TYPE(call, arg->nodeType);
        Node* node = interpreter->Evaluate(arg);
        if (interpreter->IsErrorFlagSet())
            return nullptr;
    }
    return nullptr;
}

Node* Library::Require(const std::vector<Node*>& args, Interpreter* interpreter)
{
    ASSERT_ARGC(require, 1, args.size());
    Node* node = interpreter->Evaluate(args[0]);
    if (interpreter->IsErrorFlagSet())
        return nullptr;
    ASSERT_NUM_NODE_TYPE(require, node->nodeType);

    if (node == nullptr || node->nodeType != NodeType::IntLiteral)
    {
        TestRunner::Instance().RecordFalse(args[0]);
        return nullptr;
    }
    if (static_cast<IntNode*>(node)->value)
        TestRunner::Instance().RecordTrue();
    else
        TestRunner::Instance().RecordFalse(args[0]);
    return nullptr;
}

Node* Library::WriteLn(const std::vector<Node*>& args, Interpreter* interpreter)
{
    Write(args, interpreter);
    std::cout << std::endl;
    return nullptr;
}

void Library::WriteUtil(Node* evaluated, Interpreter* interpreter)
{
    if (evaluated->nodeType == NodeType::IntLiteral)
        std::cout << static_cast<IntNode*>(evaluated)->value;// << " ";
    else if (evaluated->nodeType == NodeType::FloatLiteral)
        std::cout << static_cast<FloatNode*>(evaluated)->value;// << " ";
    else if (evaluated->nodeType == NodeType::StringLiteral)
        std::cout << static_cast<StringNode*>(evaluated)->string;// << " ";
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
                    WriteUtil(element, interpreter);
                    std::cout << " ";
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
            std::cout << "\b]"; // move one back
        else
            std::cout << "] ";
    }
}

Node* Library::Write(const std::vector<Node*>& args, Interpreter* interpreter)
{
    if (args.size() == 0)
        return nullptr;
    for (Node* arg : args)
    {
        if (arg == nullptr)
            continue;
        Node* evaluated = interpreter->Evaluate(arg);
        if (evaluated == nullptr)
            return nullptr;
        WriteUtil(evaluated, interpreter);
    }
    return nullptr;
}
