#include "scanner.h"

int main() {
    std::string sourceCode = "int x = 10; while (x > 0) { x = x - 1; }";
    Scanner s(sourceCode);
    s.scan();
    return 0;
}
