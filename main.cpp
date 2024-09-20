#include "scanner.h"
#include <fstream>

int main() {
    std::ifstream inputFile("input.txt");
    std::string sourceCode((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Scanner s(sourceCode);
    s.scan();

    const std::vector<Token>& tokens = s.getTokens();

    for (auto token : tokens) {
        token.print();
    }
    return 0;
}
