#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "../scanner/token.h"
#include "../utils/logger.h"
#include "../AST/AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    bool parse();
    UnqPtr<ProgramNode> getAST();

private:
    std::vector<Token> tokens;
    UnqPtr<ASTNode> root;

    int current;
    bool failed = false;

    bool isAtEnd();
    Token peek();
    Token advance();
    Token previous();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);

    // Recuperación de errores
    void synchronize();
    void error(const std::string& message);

    UnqPtr<ProgramNode> program();
    bool programPrime();
    UnqPtr<ExprListNode> exprList();
    bool exprListPrime(UnqPtr<ExprListNode>& exprListNode);
    UnqPtr<ASTNode> expression();
    UnqPtr<ASTNode> expressionPrime(UnqPtr<ASTNode> left);
    UnqPtr<ASTNode> orExpr();
    UnqPtr<ASTNode> orExprPrime(UnqPtr<ASTNode> left);
    UnqPtr<ASTNode> andExpr();
    UnqPtr<ASTNode> andExprPrime(UnqPtr<ASTNode> left);
    bool multOrDivOrMod();
    UnqPtr<ASTNode> unary();
    UnqPtr<ASTNode> term();
    UnqPtr<ASTNode> termPrime(UnqPtr<ASTNode> left);
    bool equalOrDifferent();
    UnqPtr<ASTNode> eqExpr();
    UnqPtr<ASTNode> eqExprPrime(UnqPtr<ASTNode> left);
    bool greaterOrLess();
    UnqPtr<ASTNode> relExpr();
    UnqPtr<ASTNode> relExprPrime(UnqPtr<ASTNode> left);
    bool sumOrRest();
    UnqPtr<ASTNode> expr();
    UnqPtr<ASTNode> exprPrime(UnqPtr<ASTNode> left);
    UnqPtr<ASTNode> factor();
    bool literal();
    UnqPtr<ASTNode> Parenthesis(UnqPtr<IdentifierNode> identifier);
    UnqPtr<ASTNode> factorPrime(UnqPtr<ASTNode> node);
    UnqPtr<ASTNode> statement();
    UnqPtr<ASTNode> ifStmt();
    UnqPtr<ASTNode> ifStmtPrime();
    UnqPtr<ASTNode> forStmt();
    UnqPtr<ASTNode> returnStmt();
    UnqPtr<ASTNode> printStmt();
    UnqPtr<ASTNode> exprStmt();
    UnqPtr<ASTNode> stmtList();
    bool stmtListPrime(UnqPtr<CompoundStatementNode>& compoundNode);
    bool type();
    bool typePrime();
    UnqPtr<ASTNode> declaration();
    UnqPtr<ASTNode> declarationPrime(const Token& typeToken, const Token& identifierToken);
    UnqPtr<FunctionDeclarationNode> function(const Token& typeToken, const Token& identifierToken);
    bool params(std::vector<UnqPtr<ParamNode>>& parameters);
    UnqPtr<VarDeclarationNode> varDecl(const Token& typeToken, const Token& identifierToken);
    bool varDeclPrime();
    bool checkForDeclarationStart();
};

#endif
