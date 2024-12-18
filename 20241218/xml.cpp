#include <iostream>
#include <vector>
#include <vector>
#include <filesystem>
#include <map>
#include <regex>
#include <memory>
#include <string>


namespace fs = std::filesystem;
using namespace std::literals::string_literals;



class xmlnode {
public:
    xmlnode(const std::string& s);
    std::string operator[](const std::string& name);
    bool is_valid();
private:
    xmlnode(std::shared_ptr<std::string> xml, int start);
    void parse(int start);
    int start = 0;
    int end = 0;
    bool valid = true;
    std::string tag;
    std::shared_ptr<std::string> raw_xml;
    std::weak_ptr<xmlnode> host;
    std::vector<std::unique_ptr<xmlnode>> nodes;
    std::map<std::string,std::string> attributes;
};

bool xmlnode::is_valid()
{
    return valid;
}

xmlnode::xmlnode(const std::string& s)
{
    raw_xml = std::make_shared<std::string>(s);
    parse(0);
}

xmlnode::xmlnode(std::shared_ptr<std::string> xml, int start)
{
    raw_xml = xml;
    parse(start);
}


void xmlnode::parse(int start)
{
    std::string_view xml{raw_xml->begin()+start, raw_xml->end()};
    while (std::isspace(xml[start])) ++start;
    this->start = start;
    if (xml[start]!='<') {
        valid = false;
        return;
    }
    std::cmatch match;
    std::regex_search(xml.begin(), xml.end(),match,std::regex{R"(^<([^\s]+))"});
    if (match.empty()) {valid = false; return; }
    tag = match[0].str();
    for (int i=0;i<xml->size();i++) {
      char ch = xml[0];
    }
}

