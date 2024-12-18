#include <format>
#include <iostream>
#include <string>

using namespace std::literals;

int main(int argc, char *argv[])
{
  auto s = std::format("Hello, World! : {}, {}, {}", "Heather Mason", 12, 12.3);
  std::cout << s << std::endl;
  return 0;
}
