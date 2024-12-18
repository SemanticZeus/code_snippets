#include <string>
#include <array>

constexpr auto SPEC_SI = std::array{"", "k", "M", "G", "T", "P", "E", "Z", "Y", "R", "Q"};

constexpr size_t string_size(const char *s) {
    size_t len = 1;
    while (*s++) {
        len++;
    }
    return len;
}

constexpr size_t full_array_size(auto a) {
    size_t len = 0;
    for (auto e: *a) {
        len += string_size(e);
    }
    return len;
}

template<auto a>
constexpr auto append_i_to_elements() {
    auto fill_value = []() {
        std::array<char, full_array_size(a) + std::size(*a)> strings{};
        size_t out = 0;
        for (const char* e : *a) {
            while (*e) {
                strings[out++] = *e++;
            }
            strings[out++] = 'i';
            strings[out++] = '\0';
        }
        return strings;
    };
    static constexpr auto strings = fill_value();
    auto fill_indexes = []() {
        std::array<const char*, std::size(*a)> result{};
        size_t index = 0;
        size_t out = 0;
        for (const char* e : *a) {
            result[index++] = std::begin(strings) + out;
            out += string_size(e) + 1;
        }
        return result;
    };
    static constexpr auto result = fill_indexes();
    return result;
}

constexpr auto SPEC_IEC = append_i_to_elements<&SPEC_SI>();

#include <iostream>

int main(int argc, char *argv[])
{
  for (auto &s : SPEC_IEC)
    std::cout << s << std::endl;
  return 0;
}
