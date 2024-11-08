#include "src/scanner/scanner.h"
#include "src/utils/logger.h"
#include "src/parser/parser.h"
#include "src/AST/AST.h"
#include "src/AST/ASTVisitor.h"
#include "src/AST/ASTPrinter.h"
#include <fstream>

int main() {
    std::ifstream inputFile("input2.txt");
    std::string sourceCode((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Scanner s(sourceCode);
    s.scan();

    const std::vector<Token>& tokens = s.getTokens();

    Logger& logger = Logger::getInstance();
    logger.setLogLevel(Logger::LogLevel::DEBUG);
    logger.setActive(true);

    for (auto token : tokens) {
        token.print();
    }

    Parser p(tokens);
    p.parse();

    UnqPtr<ProgramNode> ast = p.getAST();
    if (!ast) {
        return 1;
    }
    ASTPrinter printer;
    ast->Accept(printer);
    return 0;
}
