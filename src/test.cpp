// Example program
#include <iostream>
#include <string>

int acc = 0;

void test()
{
    acc += 1;
    for (int i = 0; i < 10000; i++)
        test();
}

int main()
{
  test();
  std::cout << acc << std::endl;
}

