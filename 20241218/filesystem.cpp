#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
  fs::path d = "/Volumes/MacBook_1/porn";
  fs::directory_entry entry{d};
  std::cout << entry << std::endl;
  return 0;
}
