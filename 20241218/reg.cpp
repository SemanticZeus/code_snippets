#include <iostream>
#include <regex>

int main()
{
  std::smatch m;
  std::string xml = "<path id=\"page\" class=\"classname\" some other infos />";
  std::regex_search(xml, m, std::regex{R"(\sclass=['"](.*?)['"])"});
  std::cout << m[0] << ", " << m[1] << ", " << m[1].str().size() << std::endl;
  return 0;
}
