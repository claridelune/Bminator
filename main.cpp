#include "src/scanner/scanner.h"
#include "src/utils/logger.h"
#include "src/parser/parser.h"
#include "src/AST/AST.h"
#include "src/AST/ASTVisitor.h"
#include "src/AST/ASTPrinter.h"
#include "src/AST/ASTPrinterJson.h"
#include "src/Semantics/SemanticAnalyzer.h"
#include <fstream>

int main() {
    std::ifstream inputFile("input2.1.txt");
    std::string sourceCode((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    Scanner s(sourceCode);
    s.scan();

    const std::vector<Token>& tokens = s.getTokens();

    Logger& logger = Logger::getInstance();
    logger.setLogLevel(Logger::LogLevel::ERROR);
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

    SemanticAnalyzer sem;
    sem.Analyze(*ast);

    if (sem.HasError()) {
        std::cerr << "Analisis semantico fallo.\n";
        return 1;
    }
    std::cout << "Analisis semantico completado con exito.\n";

    sem.Render();
    
    std::string filename = "ASTTree.json";
    ASTPrinterJson printer(filename);
    ast->Accept(printer);
    return 0;
}
