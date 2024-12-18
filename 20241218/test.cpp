#include <iostream>
#include <regex>
#include <string_view>
#include <sstream>
#include <iostream>
#include <boost/regex.hpp>
#include <string>

auto split_string(const std::string&s, char ch)
{
    std::vector<std::string> ret(1);
    for (size_t i=0; i<s.size(); i++) {
        if (s[i]!=ch) ret.back().push_back(s[i]);
        else ret.push_back(std::string{});
    }
    return ret;
}

int main() {
    std::string input = "Name: John, Age: 30";
    boost::regex pattern("Name: (?<name>[A-Za-z]+), Age: (?<age>\\d+)");
    boost::smatch matches;

    if (boost::regex_search(input, matches, pattern)) {
        std::string name = matches["name"]; // Named group
        std::string age = matches["age"];   // Named group

        std::cout << "Name: " << name << ", Age: " << age << std::endl;
    } else {
        std::cout << "No match found!" << std::endl;
    }

    std::string s = "-7.444444-543.363635.124.592726";
    std::string ss;
    std::ostringstream os{ss};
    os << "testing" << "heather mason" << std::endl;
    std::stringstream is{s};
    float n1,n2;
    is >> n1 >> n2 >> n2 >> n1;
    std::cout << n1 << ", " << n2 << std::endl;
    std::cout << ss << std::endl;
    for (auto &s : split_string("ali,aminian,heather,mason", 'J'))
        std::cout << s << std::endl;

    std::string command = "M  7.646522 -1.821967";
    std::istringstream iss{command};
    std::string f1,f2;
    char c;
    iss >> c >> f1 >> f2;
    std::cout << c << ", " << f1 << ", " << f2 << std::endl;

    std::string text = "Hello, World!";
    std::regex pattern2("(World)");

    // Define the substring using iterators
    auto begin = text.begin() + 7; // Start from index 7
    auto end = text.end();

    std::match_results<std::string::const_iterator> matchResult;
    // Perform regex_search
    if (std::regex_search(begin, end, matchResult, pattern2)) {
        std::cout << "Match found: " << matchResult.str() << "\n";
        std::cout << "Match position relative to substring: " << matchResult.position() << "\n";
    } else {
        std::cout << "No match found in the substring.\n";
    }

    return 0;
}
