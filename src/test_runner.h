#include "interpreter.h"
#include "parser.h"

#include <string>
#include <iostream>

class TestRunner
{
public:

    static TestRunner& Instance();
    /**
     * @brief Record an unsuccessful test. If this is library module print node info.
     */
    void RecordFalse(Node* node);

    /**
     * @brief Record a successful test.
     * 
     */
    void RecordTrue();
    
    /**
     * @brief Reset test stats.
     */
    static void BeginSession(const std::string& test_name, bool isLibrary = false);

    /**
     * @brief Print test stats.
     */
    static void EndSession();

private:
    static bool& IsStartedUp();

    static TestRunner*& _Instance();

    TestRunner() = default;
    TestRunner(TestRunner&&) = delete;
    TestRunner(const TestRunner&) = delete;
    TestRunner& operator=(TestRunner&&) = delete;
    TestRunner& operator=(const TestRunner&) = delete;

    bool m_IsLibrary;
    uint32_t m_SuccessfulTest;
    uint32_t m_TestCount;
};