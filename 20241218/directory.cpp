#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
    fs::recursive_directory_iterator dirIter("./Videos/");
    for (const fs::directory_entry& entry : dirIter) {
        if (entry.is_regular_file()) {
            std::cout << "File: " << entry << " Size: " << entry.file_size() << " bytes\n";
        }
    }
    return 0;
}
