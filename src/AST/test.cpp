#include "ASTVisitor.h"
#include "ASTPrinter.h"
#include "AST.h"
#include "../scanner/token.h"

void TestAST() {
    Token intToken(TokenType::KEYWORD_INTEGER, "integer", 1, 1);
    Token varName(TokenType::IDENTIFIER, "x", 1, 5);
    Token numberLiteral(TokenType::LITERAL_INT, "5", 1, 10);
    Token funcToken(TokenType::KEYWORD_FUNCTION, "myFunction", 1, 1);
    Token plusToken(TokenType::OPERATOR_PLUS, "+", 1, 15);
    Token assignToken(TokenType::OPERATOR_ASSIGN, "=", 1, 20);
    Token andToken(TokenType::OPERATOR_AND, "&&", 1, 25);
    Token orToken(TokenType::OPERATOR_OR, "||", 1, 30);
    Token lessThanToken(TokenType::OPERATOR_LESS_THAN, "<", 1, 35);

    auto literalNode = std::make_unique<LiteralNode>(numberLiteral);

    auto typeNode = std::make_unique<TypeNode>(intToken);

    auto identifierNode = std::make_unique<IdentifierNode>(varName);

    auto varDeclarationNode = std::make_unique<VarDeclarationNode>(intToken, varName, std::move(literalNode));

    // (5 + 3)
    Token numberLiteral3(TokenType::LITERAL_INT, "3", 1, 15);
    auto leftExpr = std::make_unique<LiteralNode>(numberLiteral);
    auto rightExpr = std::make_unique<LiteralNode>(numberLiteral3);
    auto binaryOpNode = std::make_unique<BinaryOperationNode>(std::move(leftExpr), plusToken, std::move(rightExpr));

    // ( x = 5 + 3)
    auto assignmentNode = std::make_unique<AssignmentNode>(std::make_unique<IdentifierNode>(varName), assignToken, std::move(binaryOpNode));

    // (x < 10 && x > 5)
    auto leftRelExpr = std::make_unique<RelationalNode>(
        std::make_unique<IdentifierNode>(varName), lessThanToken, std::make_unique<LiteralNode>(Token(TokenType::LITERAL_INT, "10", 1, 35))
    );
    auto rightRelExpr = std::make_unique<RelationalNode>(
        std::make_unique<IdentifierNode>(varName), lessThanToken, std::make_unique<LiteralNode>(Token(TokenType::LITERAL_INT, "5", 1, 40))
    );

    auto logicalAndNode = std::make_unique<LogicalAndNode>(std::move(leftRelExpr), andToken, std::move(rightRelExpr));

    // ((x < 10 && x > 5) || y == 0)
    Token yToken(TokenType::IDENTIFIER, "y", 1, 50);
    auto equalityNode = std::make_unique<EqualityNode>(
        std::make_unique<IdentifierNode>(yToken), Token(TokenType::OPERATOR_EQUAL, "==", 1, 55), std::make_unique<LiteralNode>(Token(TokenType::LITERAL_INT, "0", 1, 55))
    );
    auto logicalOrNode = std::make_unique<LogicalOrNode>(std::move(logicalAndNode), orToken, std::move(equalityNode));

    // funcion 
    auto functionBody = std::make_unique<CompoundStatementNode>();
    functionBody->AddStatement(std::move(varDeclarationNode));
    functionBody->AddStatement(std::move(assignmentNode));

    auto functionDeclarationNode = std::make_unique<FunctionDeclarationNode>(intToken, funcToken, std::vector<UnqPtr<ParamNode>>(), std::move(functionBody));

    // (if ((x < 10 && x > 5) || y == 0) { x = 5; })
    auto ifStatement = std::make_unique<IfStatementNode>(
        std::move(logicalOrNode),
        std::make_unique<CompoundStatementNode>()  // Cuerpo del if (bloque vacío en este caso)
    );

    auto programNode = std::make_unique<ProgramNode>();
    programNode->AddDeclaration(std::move(functionDeclarationNode));
    programNode->AddDeclaration(std::move(ifStatement));

    ASTPrinter printer;
    programNode->Accept(printer);
}

int main() {
    TestAST();
    return 0;
}
