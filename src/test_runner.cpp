#include "test_runner.h"

#include <fstream>

void TestRunner::RecordFalse(Node* node)
{
    std::cout << "Error on test: " << m_TestCount << std::endl;
    if (m_IsLibrary)
        Parser::PrintNode(node);
    m_TestCount++;
}

void TestRunner::RecordTrue()
{
    m_SuccessfulTest += 1;
    m_TestCount++;
}

void TestRunner::BeginSession(const std::string& test_name, bool isLibrary)
{
    assert(!IsStartedUp());
    _Instance() = new TestRunner();
    IsStartedUp() = true;
    
    Instance().m_SuccessfulTest = 0;
    Instance().m_TestCount = 0;
    Instance().m_IsLibrary = isLibrary;
}

void TestRunner::EndSession()
{
    assert(IsStartedUp());

    std::cout << "Ran: " << Instance().m_TestCount << " tests." << std::endl;
    std::cout << Instance().m_SuccessfulTest << " were successful and " << Instance().m_TestCount - Instance().m_SuccessfulTest << " failed." << std::endl;
    delete _Instance();
}

TestRunner*& TestRunner::_Instance()
{
    static TestRunner* s_Instance = nullptr;
    return s_Instance;
}

TestRunner& TestRunner::Instance()
{
    assert(IsStartedUp());
    return *_Instance();
}

bool& TestRunner::IsStartedUp()
{
    static bool s_StartedUp = false;
    return s_StartedUp;
}

#ifdef TEST_LIB
int main ()
{
    TestRunner::BeginSession("Library", true);
    std::ifstream in("library_test.lfn", std::ios::ate);
    if (!in.is_open())
    {
        std::cout << "Couldn't find library_test.lfn" << std::endl;
        TestRunner::EndSession();
        return 0;
    }
    uint64_t pos = in.tellg();
    in.seekg(0);
    std::string file;
    file.resize(pos);
    in.read(&file[0], pos);
    in.close();

    Parser parser;
    ProgramNode* program = parser.ParseProgram(file, "library_test.lfn");

    try {
        Interpreter interpreter;
        interpreter.Evaluate(program);
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
    delete program;
    TestRunner::EndSession();
    return 0;
}
#endif