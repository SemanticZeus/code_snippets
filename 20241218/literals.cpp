#include <iostream>
#include <chrono>
#include <string>

using namespace std::literals::chrono_literals;
using namespace std::literals::string_literals;

int main(int argc, char *argv[])
{
  auto s = "The C++ Programming Language!"s;
  std::cout << s << std::endl;
  std::cout << 10ms << std::endl;
  return 0;
}
