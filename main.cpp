#include "scanner.h"
#include <fstream>

int main() {
    std::ifstream inputFile("input.txt");
    std::string sourceCode((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Scanner s(sourceCode);
    s.scan();
    return 0;
}
