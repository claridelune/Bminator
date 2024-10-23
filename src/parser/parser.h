#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "../scanner/token.h"
#include "../utils/logger.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    void parse();

private:
    std::vector<Token> tokens;
    int current;

    bool isAtEnd();
    Token peek();
    Token advance();
    Token previous();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);

    bool program();
    bool programPrime();
    bool exprList();
    bool exprListPrime();
    bool expression();
    bool expressionPrime();
    bool orExpr();
    bool orExprPrime();
    bool andExpr();
    bool andExprPrime();
    bool multOrDivOrMod();
    bool unary();
    bool term();
    bool termPrime();
    bool equalOrDifferent();
    bool eqExpr();
    bool eqExprPrime();
    bool greaterOrLess();
    bool relExpr();
    bool relExprPrime();
    bool sumOrRest();
    bool expr();
    bool exprPrime();
    bool factor();
    bool literal();
    bool Parenthesis();
    bool factorPrime();
    bool statement();
    bool ifStmt();
    bool ifStmtPrime();
    bool forStmt();
    bool returnStmt();
    bool printStmt();
    bool exprStmt();
    bool stmtList();
    bool stmtListPrime();
    bool type();
    bool typePrime();
    bool declaration();
    bool declarationPrime();
    bool function();
    bool params();
    bool varDecl();
    bool varDeclPrime();
    bool checkForDeclarationStart();
};

#endif
