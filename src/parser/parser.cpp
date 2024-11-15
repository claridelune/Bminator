#include "parser.h"
#include "../utils/logger.h"
#include <memory>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

bool Parser::parse() {
    Logger& logger = Logger::getInstance();
    logger.info("Iniciando el análisis del programa.");
    root = program();

    if (!failed) {
        logger.info("El input pertenece a la gramática. AST construido");
    } else {
        logger.info("El input no pertenece a la gramática.");
        root = nullptr;
    }
    return failed;
}

UnqPtr<ProgramNode> Parser::getAST() {
    if (failed) {
        Logger::getInstance().error("El análisis falló. No se construyó el AST.");
        return nullptr;
    }
    return std::unique_ptr<ProgramNode>(static_cast<ProgramNode*>(root.release()));
}

bool Parser::isAtEnd() {
    return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    Logger::getInstance().debug("Token avanzado: " + previous().value);
    return previous();
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        Logger::getInstance().debug("Coincidencia encontrada: " + previous().value);
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

UnqPtr<ASTNode> Parser::consume(TokenType type, const std::string& message) {
    Logger::getInstance().debug("Trying to consume " + tokens[current].value);
    if (!match(type)) {
        Logger::getInstance().error(message);
        synchronize();
        return nullptr;
    }
    Logger::getInstance().debug("CONSUMED and current token is " + tokens[current].value);
    return nullptr;
    //throw std::runtime_error(message);
}

void Parser::synchronize() {
    Logger::getInstance().debug("SYNC from " + tokens[current].value);
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON){
            if(tokens[current].type == TokenType::LEFT_BRACKET) advance();
            Logger::getInstance().debug("SYNC ended at " + tokens[current].value);
            program();
            return;
            }; // Retorna al encontrar un punto y coma
        Logger::getInstance().debug("SYNC advance " + tokens[current].value);
        advance();
    }
    
    program();
}

/* Program -> Declaration ProgramPrime */
UnqPtr<ProgramNode> Parser::program() {
    Logger::getInstance().debug("Analizando 'program' con: " + tokens[current].value);

    auto programNode = std::make_unique<ProgramNode>();
    while (!isAtEnd()) { // bucle elimina ProgramPrime(?
        UnqPtr<ASTNode> decl = declaration();
        if (!decl) {
            failed = true;
            return nullptr;
        }
        programNode->AddDeclaration(std::move(decl));
    }
    return programNode;
    /* failed = !(declaration() && programPrime());
    return failed; */
}

// ProgramPrime -> Declaration ProgramPrime
// ProgramPrime -> epsilon
bool Parser::programPrime() {
    Logger::getInstance().debug("Analizando 'programPrime' con: " + tokens[current].value);
    
    if (isAtEnd()) {
        Logger::getInstance().debug("'programPrime' encontrado epsilon");
        return true;
    }

    if (!checkForDeclarationStart()) {
        Logger::getInstance().error("No se encontró un tipo de dato válido en 'programPrime' para " + tokens[current].value);
        synchronize();
        return false;
    }

    if (!declaration()) {
        Logger::getInstance().error("Error en 'declaration' dentro de 'programPrime' para " + tokens[current].value);
        synchronize();
        return false;
    }

    return programPrime();
}

/*
Declaration -> Type Identifier DeclarationPrime 
*/
UnqPtr<ASTNode> Parser::declaration() {
    Logger::getInstance().debug("Analizando 'declaration' con: " + tokens[current].value);
        
    Token typeToken = peek();
    if (type()) {
        if (match(TokenType::IDENTIFIER)) {
            Token identifierToken = previous();
            Logger::getInstance().debug("Encontrado identificador en 'declaration': " + previous().value);
            return declarationPrime(typeToken, identifierToken);
        } else {
            Logger::getInstance().error("Se esperaba un identificador después del tipo en 'declaration'.");
            synchronize();
            return nullptr;
            //throw std::runtime_error("Se esperaba un identificador después del tipo.");
        }
    }

    Logger::getInstance().error("Error en 'declaration', no se encontró un tipo válido para " + tokens[current].value);
    synchronize();
    return nullptr;
}


/*
DeclarationPrime -> function || varDecl
 * */
UnqPtr<ASTNode> Parser::declarationPrime(const Token& typeToken, const Token& identifierToken) {
    Logger::getInstance().debug("Analizando 'declarationPrime' con: " + tokens[current].value);
    if (check(TokenType::LEFT_PARENTHESIS)) return function(typeToken, identifierToken);
    if (check(TokenType::OPERATOR_ASSIGN)) return varDecl(typeToken, identifierToken);
    Logger::getInstance().error("Se esperaba un '(' o un operador de asignación en lugar de: " + tokens[current].value);
    synchronize();
    return nullptr;
}

/*
function -> ( params ) { stmtlist }
*/
UnqPtr<FunctionDeclarationNode> Parser::function(const Token& typeToken, const Token& identifierToken) {
    Logger::getInstance().debug("Analizando 'function' con: " + tokens[current].value);
    auto functionNode = std::make_unique<FunctionDeclarationNode>(typeToken, identifierToken, std::vector<UnqPtr<ParamNode>>{}, nullptr);
    
    if (match(TokenType::LEFT_PARENTHESIS) && params(functionNode->parameters)) {
        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            Logger::getInstance().error("Se esperaba ')' al final de los parámetros.");
            synchronize();
            return nullptr;
        }
        if (!match(TokenType::LEFT_BRACE)) {
            Logger::getInstance().error("Se esperaba '{' al inicio del cuerpo de la función.");
            synchronize();
            return nullptr;
        }

        functionNode->body = stmtList();
        
        if (functionNode->body) {
            if (!match(TokenType::RIGHT_BRACE)) {
                Logger::getInstance().error("Se esperaba '}' al final del cuerpo de la función.");
                synchronize();
                return nullptr;
            }
            return functionNode;
        }

        Logger::getInstance().error("Error en la declaración de la función.");
        // throw std::runtime_error("Error en la declaración de la función.");
    }
    Logger::getInstance().error("Se esperaba una declaración de tipo (content)");
    synchronize();
    return nullptr;
}

/*
Params -> Type Identifier Params
Params -> , Params
Params -> epsilon
*/
bool Parser::params(std::vector<UnqPtr<ParamNode>>& parameters) {
    Logger::getInstance().debug("Analizando 'params' con: " + tokens[current].value);
    if (type()) {
        Token typeToken = previous();
        if (match(TokenType::IDENTIFIER)) {
            Token identifierToken = previous();
            Logger::getInstance().debug("Encontrado parámetro: " + previous().value);

            parameters.push_back(std::make_unique<ParamNode>(typeToken, identifierToken));

            if (match(TokenType::COMMA)) {
                return params(parameters);
            }
            return true;
            /* Logger::getInstance().error("Se esperaban parámetros válidos");
            synchronize();
            return false; */
        }
        Logger::getInstance().error("Se esperaba un identificador después del tipo en 'params'.");
        synchronize();
        //throw std::runtime_error("Se esperaba un identificador después del tipo en 'params'.");
    }

    if (check(TokenType::RIGHT_PARENTHESIS)) {
        Logger::getInstance().debug("Epsilon encontrado en 'params'");
        return true;
    }
    Logger::getInstance().error("Error en 'params': token inesperado: " + tokens[current].value);
    synchronize();
    return false;
}

// VarDecl -> ;
// VarDecl -> = Expression ;
UnqPtr<VarDeclarationNode> Parser::varDecl(const Token& typeToken, const Token& identifierToken) {
    Logger::getInstance().debug("Analizando 'varDecl' con: " + tokens[current].value);

    auto varDeclNode = std::make_unique<VarDeclarationNode>(typeToken, identifierToken);

    if (match(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Asignación en declaración de variable");
        varDeclNode->expression = expression();
        if (!varDeclNode->expression) {
            Logger::getInstance().debug("Se esperaba una expresión después del '=' en 'varDecl'");
            synchronize();
        }

        if (!match(TokenType::SEMICOLON)) {
            Logger::getInstance().error("Se esperaba ';' al final de la declaración.");
            synchronize();
            return nullptr;
        }
        Logger::getInstance().debug("Declaración de variable con asignación finalizada");
        return varDeclNode;
    }
    if (match(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Declaración de variable con asignación finalizada");
        return varDeclNode;
    }

    Logger::getInstance().error("Error en 'varDecl', se esperaba ';' o '='.");
    return nullptr;
}

// ExprList -> Expression ExprListPrime
UnqPtr<ExprListNode> Parser::exprList() {
    
    auto exprListNode = std::make_unique<ExprListNode>();
    UnqPtr<ASTNode> exprNode = expression();
    Logger::getInstance().debug("Analizando 'exprList' con: " + tokens[current].value);
    if (!exprNode) {
        Logger::getInstance().error("Error en 'expression' dentro de 'exprList'");
        return nullptr;
    }
    exprListNode->AddExpression(std::move(exprNode));

    if (!exprListPrime(exprListNode)) {
        return nullptr;
    }
    return exprListNode;
}

// ExprListPrime -> , ExprList
// ExprListPrime -> epsilon
bool Parser::exprListPrime(UnqPtr<ExprListNode>& exprListNode) {
    Logger::getInstance().debug("Analizando 'exprListPrime' con: " + tokens[current].value);
    if (match(TokenType::COMMA)) {
        UnqPtr<ASTNode> exprNode = expression();
        if (!exprNode) {
            Logger::getInstance().error("Error en 'exprList' después de ',' en 'exprListPrime'");
            return false;
        }
        exprListNode->AddExpression(std::move(exprNode));
        return exprListPrime(exprListNode);
    }
    if (check(TokenType::RIGHT_PARENTHESIS)) {
        Logger::getInstance().debug("Epsilon encontrado en 'exprListPrime'");
        return true;
    }
    Logger::getInstance().error("Error en 'exprListPrime': token inesperado");
    return false;
}

/*  
Expression -> OrExpr expressionPrime
*/
UnqPtr<ASTNode> Parser::expression() {
    Logger::getInstance().debug("Analizando 'expression' con: " + tokens[current].value);

    UnqPtr<ASTNode> left = orExpr();
    if (!left) return nullptr;
    return expressionPrime(std::move(left));
}

/*  
ExpressionPrime -> epsilon
ExpressionPrime -> = OrExpr
*/
UnqPtr<ASTNode> Parser::expressionPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'expressionPrime' con: " + tokens[current].value);

    if (match(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Se encontró operador de asignación");

        UnqPtr<ASTNode> right = orExpr();
        if (!right) {
            Logger::getInstance().error("Error en 'orExpr' después de operador de asignación en 'expressionPrime'");
            return nullptr;
        }
        return std::make_unique<AssignmentNode>(std::move(left), previous(), std::move(right));;
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Epsilon encontrado en 'expressionPrime'");
        return left;
    }
    Logger::getInstance().error("Error en 'expressionPrime': token inesperado");
    return nullptr;
}

/* 
OrExpr -> AndExpr OrExprPrime
*/
UnqPtr<ASTNode> Parser::orExpr() {
    Logger::getInstance().debug("Analizando 'orExpr' con: " + tokens[current].value);
    UnqPtr<ASTNode> left = andExpr();
    if (!left) return nullptr;
    return orExprPrime(std::move(left));
}

/*
OrExprPrime -> || AndExpr OrExprPrime
OrExprPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::orExprPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'orExprPrime' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_OR)) {
        Logger::getInstance().debug("Encontrado operador '||'");
        UnqPtr<ASTNode> right = andExpr();
        if (!right) {
            Logger::getInstance().error("Se esperaba una expresión después de '||'.");
            synchronize();
        }
        auto logicalOrNode = std::make_unique<LogicalOrNode>(std::move(left), previous(), std::move(right));
        return orExprPrime(std::move(logicalOrNode));
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Epsilon encontrado en 'orExprPrime'");
        return left;
    }

    Logger::getInstance().error("Error en 'orExprPrime': token inesperado");
    return nullptr;
}

/*
AndExpr -> EqExpr AndExprPrime
*/
UnqPtr<ASTNode> Parser::andExpr() {
    Logger::getInstance().debug("Analizando 'andExpr' con: " + tokens[current].value);

    UnqPtr<ASTNode> left = eqExpr();
    if (!left) return nullptr;
    return andExprPrime(std::move(left));
}

/*
AndExprPrime -> && EqExpr AndExprPrime
AndExprPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::andExprPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'andExprPrime' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_AND)) {
        Logger::getInstance().debug("Encontrado operador '&&'");
        UnqPtr<ASTNode> right = eqExpr();
        if (!right) {
            Logger::getInstance().error("Se esperaba una expresión después de '&&'.");
            synchronize();
            //throw std::runtime_error("Se esperaba una expresión después de '&&'.");
        }
        auto logicalAndNode = std::make_unique<LogicalAndNode>(std::move(left), previous(), std::move(right));
        return andExprPrime(std::move(logicalAndNode));
    }
    if (check(TokenType::OPERATOR_NOT) || check(TokenType::OPERATOR_MINUS) ||
        check(TokenType::IDENTIFIER) || check(TokenType::LITERAL_INT) ||
        check(TokenType::LITERAL_CHAR) || check(TokenType::LITERAL_STRING) ||
        check(TokenType::KEYWORD_TRUE) || check(TokenType::KEYWORD_FALSE) ||
        check(TokenType::LEFT_PARENTHESIS) || check(TokenType::OPERATOR_ASSIGN) ||
        check(TokenType::RIGHT_BRACKET) || check(TokenType::RIGHT_PARENTHESIS) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON ) ||
        check(TokenType::OPERATOR_OR)) {
        Logger::getInstance().debug("Epsilon encontrado en 'andExprPrime'");
        return left;
    }

    Logger::getInstance().error("Error en 'andExprPrime': token inesperado");
    return nullptr;
}

/*
MultOrDivOrMod -> * | / | %
*/
bool Parser::multOrDivOrMod() {
    Logger::getInstance().debug("Analizando 'multOrDivOrMod' con " + tokens[current].value);
    if (match(TokenType::OPERATOR_MULTIPLY) || 
        match(TokenType::OPERATOR_DIVIDE) || 
        match(TokenType::OPERATOR_MOD)) {
        Logger::getInstance().debug("Operador de multiplicación/división/módulo encontrado");
        return true;
    }
    return false;
}

/*
Unary -> ! Unary
Unary -> - Unary
Unary -> Factor
*/
UnqPtr<ASTNode> Parser::unary() {
    Logger::getInstance().debug("Analizando 'unary' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_NOT) || match(TokenType::OPERATOR_MINUS)) {
        Token op = previous();
        Logger::getInstance().debug("Encontrado operador unario: " + op.value);
        UnqPtr<ASTNode> expr =unary();
        if (!expr) return nullptr;
        return std::make_unique<UnaryOperationNode>(op, std::move(expr));
    }
    return factor();
}

/* 
 * Factor -> Identifier Parenthesis FactorPrime | Literal FactorPrime | ( Expression ) FactorPrime 
 */
UnqPtr<ASTNode> Parser::factor() {
    Logger::getInstance().debug("Analizando 'factor' con: " + tokens[current].value);
    if (literal()) {
        Logger::getInstance().debug("Encontrado literal: " + previous().value);
        Token litToken = previous();
        auto literalNode = std::make_unique<LiteralNode>(litToken);
        return factorPrime(std::move(literalNode));
    }
    if (match(TokenType::IDENTIFIER)) {
        Token identifier = previous();
        Logger::getInstance().debug("Encontrado identificador: " + previous().value);
        auto identifierNode = std::make_unique<IdentifierNode>(identifier);

        UnqPtr<ASTNode> nodeAfterParenthesis = Parenthesis(std::move(identifierNode));
        if (!nodeAfterParenthesis) return nullptr;
        return factorPrime(std::move(nodeAfterParenthesis));
    }
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        UnqPtr<ASTNode> exprNode = expression();
        if (!exprNode) {
            Logger::getInstance().debug("Se esperaba una expresión después de '('");
            return nullptr;
        }
        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            Logger::getInstance().error("Se esperaba ')' después de la expresión.");
            synchronize();
            return nullptr;
        }
        return factorPrime(std::move(exprNode));
    }
    return nullptr;
}

/* Parenthesis -> ( ExprList ) | epsilon */
UnqPtr<ASTNode> Parser::Parenthesis(UnqPtr<IdentifierNode> identifier) {
    Logger::getInstance().debug("Analizando 'Parenthesis' con: " + tokens[current].value);
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        UnqPtr<ExprListNode> arguments = nullptr;
        if (!check(TokenType::RIGHT_PARENTHESIS)) {
            arguments = exprList();
            if (!arguments) {
                Logger::getInstance().debug("Se esperaba una exprList después de '('");
                return nullptr;
            }
        }
        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            Logger::getInstance().error("Se esperaba ')' después de la expresión.");
            synchronize();
            return nullptr;
        }
        auto functionCallNode = std::make_unique<FunctionCallNode>(std::move(identifier), std::move(arguments));
        return functionCallNode;
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::LEFT_BRACKET) ||
        check(TokenType::RIGHT_BRACKET) || check(TokenType::COMMA) ||
        check(TokenType::SEMICOLON) || check(TokenType::OPERATOR_ASSIGN) ||
        check(TokenType::OPERATOR_OR) || check(TokenType::OPERATOR_AND) ||
        check(TokenType::OPERATOR_EQUAL) || check(TokenType::OPERATOR_NOT_EQUAL) ||
        check(TokenType::OPERATOR_LESS_THAN) || check(TokenType::OPERATOR_GREATER_THAN) ||
        check(TokenType::OPERATOR_LESS_EQUAL) || check(TokenType::OPERATOR_GREATER_EQUAL) ||
        check(TokenType::OPERATOR_PLUS) || check(TokenType::OPERATOR_MINUS) ||
        check(TokenType::OPERATOR_MULTIPLY) || check(TokenType::OPERATOR_DIVIDE) ||
        check(TokenType::OPERATOR_MOD)) {
        Logger::getInstance().debug("Epsilon encontrado en Parenthesis");
        return identifier;
    }
    return nullptr;
}

/* FactorPrime -> [ Expression ] FactorPrime | epsilon */
UnqPtr<ASTNode> Parser::factorPrime(UnqPtr<ASTNode> node) {
    Logger::getInstance().debug("Analizando 'factorPrime': " + tokens[current].value);
    if (match(TokenType::LEFT_BRACKET)) {
        Logger::getInstance().debug("Encontrado '['");
        UnqPtr<ASTNode> index = expression();
        if (!index) {
            Logger::getInstance().debug("Se esperaba una expresión después de '['");
            return nullptr;  
        }
        consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de la expresión.");
        auto indexingNode = std::make_unique<IndexingNode>(std::move(node), std::move(index));
        return factorPrime(std::move(indexingNode));  
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL) || check(TokenType::OPERATOR_LESS_THAN) ||
        check(TokenType::OPERATOR_GREATER_THAN) || check(TokenType::OPERATOR_LESS_EQUAL) ||
        check(TokenType::OPERATOR_GREATER_EQUAL) || check(TokenType::OPERATOR_PLUS) ||
        check(TokenType::OPERATOR_MINUS) || check(TokenType::OPERATOR_MULTIPLY) ||
        check(TokenType::OPERATOR_DIVIDE) || check(TokenType::OPERATOR_MOD)) {
        Logger::getInstance().debug("Epsilon encontrado en factorPrime");
        return node;
    }
    return nullptr;  
}


/* Literal -> IntegerLiteral | CharLiteral | BooleanLiteral | StringLiteral */
bool Parser::literal() {
    Logger::getInstance().debug("Analizando 'literal' con: " + tokens[current].value);
    return match(TokenType::LITERAL_INT) ||
           match(TokenType::LITERAL_CHAR) ||
           match(TokenType::KEYWORD_FALSE) || match(TokenType::KEYWORD_TRUE) ||
           match(TokenType::LITERAL_STRING);
}

/*
Term -> Unary TermPrime
*/
UnqPtr<ASTNode> Parser::term() {
    Logger::getInstance().debug("Analizando 'term' con: " + tokens[current].value);
    UnqPtr<ASTNode> left = unary();
    if (!left) return nullptr;
    return termPrime(std::move(left));
}

/*
TermPrime -> MultOrDivOrMod Unary TermPrime
TermPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::termPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'termPrime' con: " + tokens[current].value);
    if (multOrDivOrMod()) {
        Logger::getInstance().debug("Encontrado operador de multiplicación/división/módulo");

        Token op = previous();
        UnqPtr<ASTNode> right = unary();
        if (!right) {
            Logger::getInstance().error("Error en 'unary' después de operador en 'termPrime'");
            return nullptr;
        }
        auto binOpNode = std::make_unique<BinaryOperationNode>(std::move(left), op, std::move(right));
        return termPrime(std::move(binOpNode));
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL) || check(TokenType::OPERATOR_LESS_THAN) ||
        check(TokenType::OPERATOR_GREATER_THAN) || check(TokenType::OPERATOR_LESS_EQUAL) ||
        check(TokenType::OPERATOR_GREATER_EQUAL) || check(TokenType::OPERATOR_PLUS) ||
        check(TokenType::OPERATOR_MINUS)) {
        Logger::getInstance().debug("Epsilon encontrado en 'termPrime'");
        return left;
    }

    Logger::getInstance().error("Error en 'termPrime': token inesperado");
    return nullptr;
}

/*
EqualOrDifferent -> ==
EqualOrDifferent -> !=
*/
bool Parser::equalOrDifferent() {
    Logger::getInstance().debug("Analizando 'equalOrDifferent' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_EQUAL)) {
        Logger::getInstance().debug("Encontrado '=='");
        return true; 
    }
    if (match(TokenType::OPERATOR_NOT_EQUAL)) {
        Logger::getInstance().debug("Encontrado '!='");
        return true;
    }
    return false;
}

/*
EqExpr -> RelExpr EqExprPrime
*/
UnqPtr<ASTNode> Parser::eqExpr() {
    Logger::getInstance().debug("Analizando 'eqExpr' con:" + tokens[current].value);

    UnqPtr<ASTNode> left = relExpr();

    if (!left) return nullptr;
    return eqExprPrime(std::move(left));
}

/*
EqExprPrime -> EqualOrDifferent RelExpr EqExprPrime
EqExprPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::eqExprPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'eqExprPrime' con: " + tokens[current].value);

    if (equalOrDifferent()) {
        Logger::getInstance().debug("Encontrado operador de igualdad/desigualdad");
        Token op = previous();
        UnqPtr<ASTNode> right = relExpr();
        if (!right) {
            Logger::getInstance().error("Error en 'relExpr' después de operador en 'eqExprPrime'");
            return nullptr;
        }
        auto equalityNode = std::make_unique<EqualityNode>(std::move(left), op, std::move(right));

        return eqExprPrime(std::move(equalityNode));
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_AND)) {
        Logger::getInstance().debug("Epsilon encontrado en 'eqExprPrime'");
        return left;
    }
    Logger::getInstance().error("Error en 'eqExprPrime': token inesperado");
    return nullptr;
}

/*
GreaterOrLess -> <
GreaterOrLess -> >
GreaterOrLess -> <=
GreaterOrLess -> >=
*/
bool Parser::greaterOrLess() {
    Logger::getInstance().debug("Analizando 'greaterOrLess' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_LESS_THAN) ||
        match(TokenType::OPERATOR_GREATER_THAN) ||
        match(TokenType::OPERATOR_LESS_EQUAL) ||
        match(TokenType::OPERATOR_GREATER_EQUAL)) {
        return true;
    }
    return false;
}

/*
RelExpr -> Expr RelExprPrime
*/
UnqPtr<ASTNode> Parser::relExpr() {
    Logger::getInstance().debug("Analizando 'relExpr'");

    UnqPtr<ASTNode> left = expr();
    if (!left) return  nullptr;
    return relExprPrime(std::move(left));
}

/*
RelExprPrime -> GreaterOrLess Expr RelExprPrime
RelExprPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::relExprPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'relExprPrime'");
    if (greaterOrLess()) {
        Logger::getInstance().debug("Encontrado operador de comparación");
        Token op = previous();

        UnqPtr<ASTNode> right = expr();
        if (!right) {
            Logger::getInstance().error("Error en 'expr' después de operador en 'relExprPrime'");
            return nullptr;
        }

        auto relationalNode = std::make_unique<RelationalNode>(std::move(left), op, std::move(right));
        return relExprPrime(std::move(relationalNode));
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL)) {
        Logger::getInstance().debug("Epsilon encontrado en 'relExprPrime'");
        return left;
    }

    Logger::getInstance().error("Error en 'relExprPrime': token inesperado");
    return nullptr;
}

/*
SumOrRest -> +
SumOrRest -> -
*/
bool Parser::sumOrRest() {
    Logger::getInstance().debug("Analizando 'sumOrRest'");
    if (match(TokenType::OPERATOR_PLUS) || match(TokenType::OPERATOR_MINUS)) {
        return true;
    }
    return false;
}

/*
Expr -> Term ExprPrime
*/
UnqPtr<ASTNode> Parser::expr() {
    Logger::getInstance().debug("Analizando 'expr' con: " + tokens[current].value);
    UnqPtr<ASTNode> left = term();
    if (!left) return nullptr;
    return exprPrime(std::move(left));
}

/*
ExprPrime -> SumOrRest Term ExprPrime
ExprPrime -> epsilon
*/
UnqPtr<ASTNode> Parser::exprPrime(UnqPtr<ASTNode> left) {
    Logger::getInstance().debug("Analizando 'exprPrime'");
    if (sumOrRest()) {
        Logger::getInstance().debug("Encontrado operador de suma/resta");
        Token op = previous();
        UnqPtr<ASTNode> right = term();
        if (!right) {
            Logger::getInstance().error("Error en 'term' después de operador en 'exprPrime'");
            return nullptr;
        }
        auto binOpNode = std::make_unique<BinaryOperationNode>(std::move(left), op, std::move(right));
        return exprPrime(std::move(binOpNode));
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL) || check(TokenType::OPERATOR_LESS_THAN) ||
        check(TokenType::OPERATOR_GREATER_THAN) || check(TokenType::OPERATOR_LESS_EQUAL) ||
        check(TokenType::OPERATOR_GREATER_EQUAL)) {
        Logger::getInstance().debug("Epsilon encontrado en 'exprPrime'");
        return left;
    }
    Logger::getInstance().error("Error en 'exprPrime': token inesperado");
    return nullptr;
}

// Statement -> VarDecl | IfStmt | ForStmt | ReturnStmt | ExprStmt | PrintStmt | {StmtList}
UnqPtr<ASTNode> Parser::statement() {
    Logger::getInstance().debug("Analizando 'statement' con: " + tokens[current].value);
    if (type()) {
        Token typeToken = previous();
        if (match(TokenType::IDENTIFIER)) {
            Token idToken = previous();
            Logger::getInstance().debug("Encontrado identificador en 'statement': " + previous().value);
            return varDecl(typeToken, idToken);
        }
    } else if (UnqPtr<ASTNode> ifNode = ifStmt()) {
        return ifNode;
    } else if (UnqPtr<ASTNode> forNode = forStmt()) {
        return forNode;
    } else if (UnqPtr<ASTNode> returnNode = returnStmt()) {
        return returnNode;
    } else if (UnqPtr<ASTNode> printNode = printStmt()) {
        return printNode;
    } else if (UnqPtr<ASTNode> exprStmtNode = exprStmt()) {
        return exprStmtNode;
    } else if (match(TokenType::LEFT_BRACE)) {
        UnqPtr<ASTNode> blockNode = stmtList();
        if (!blockNode) {
            Logger::getInstance().error("Se espera stmtList despues de { en 'statement'");
            return nullptr;
        }
        consume(TokenType::RIGHT_BRACE, "Se espera } despues de stmtList");
        return blockNode;
    }

    Logger::getInstance().debug("Ninguna coincidencia encontrada en 'statement'");
    return nullptr;
}

// IfStmt -> if ( Expression ) { Statement } ifStmtPrime
UnqPtr<ASTNode> Parser::ifStmt() {
    Logger::getInstance().debug("Analizando 'ifStmt'");
    if (!match(TokenType::KEYWORD_IF)) {
        return nullptr;
    }
    Logger::getInstance().debug("Analizando 'ifStmt' - IF ENCONTRADO");

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'if'.");

    UnqPtr<ASTNode> condition = expression();
    if (!condition) {
        Logger::getInstance().debug("Se esperaba una expresión dentro del 'if'.");
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");

    Logger::getInstance().debug("Analizando 'ifStmt' - ENCONTRADA CONDICIÓN ENTRE PARENTESIS");

    consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de la expresión.");

    UnqPtr<ASTNode> ifBody = stmtList();

    if (!ifBody) {
        Logger::getInstance().error("Error en 'statement' dentro de 'ifStmt'");
        return nullptr;
    }

    consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'if'.");
    Logger::getInstance().debug("Analizando 'ifStmt' - ENCONTRADO BLOQUE ENTRE CORCHETES");

    UnqPtr<ASTNode> elseBody = ifStmtPrime();
    return std::make_unique<IfStatementNode>(std::move(condition), std::move(ifBody), std::move(elseBody));
}

// IfStmtPrime -> else { Statement } | epsilon
UnqPtr<ASTNode> Parser::ifStmtPrime() {
    Logger::getInstance().debug("Analizando 'ifStmtPrime' con: " + tokens[current].value);
    if (match(TokenType::KEYWORD_ELSE)) {
        Logger::getInstance().debug("Encontrado 'else'");
        consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de 'else'.");

        UnqPtr<ASTNode> elseBody = stmtList();
        if (!elseBody) {
            Logger::getInstance().error("Error en 'statement' dentro de 'ifStmtPrime'");
            return nullptr;
        }
        consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'else'.");
        return std::move(elseBody);
    }
    if (check(TokenType::RIGHT_BRACE) || check(TokenType::LEFT_BRACE) ||
        check(TokenType::KEYWORD_INTEGER) || check(TokenType::KEYWORD_BOOLEAN) ||
        check(TokenType::KEYWORD_CHAR) || check(TokenType::KEYWORD_STRING) ||
        check(TokenType::KEYWORD_VOID) || check(TokenType::KEYWORD_FOR) ||
        check(TokenType::KEYWORD_IF) || check(TokenType::KEYWORD_RETURN) ||
        check(TokenType::KEYWORD_PRINT) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_NOT) || check(TokenType::OPERATOR_MINUS) ||
        check(TokenType::IDENTIFIER) || check(TokenType::LITERAL_INT) ||
        check(TokenType::LITERAL_CHAR) || check(TokenType::LITERAL_STRING) ||
        check(TokenType::KEYWORD_TRUE) || check(TokenType::KEYWORD_FALSE) ||
        check(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Epsilon encontrado en 'ifStmtPrime' - NO SE ENCONTRÓ ELSE");
        Logger::getInstance().debug("Epsilon encontrado en 'ifStmtPrime'");
        return nullptr;
    }

    Logger::getInstance().error("Error en 'ifStmtPrime': token inesperado");
    return nullptr;
}

// ForStmt -> for ( ExprStmt Expression ; ExprStmt ) Statement
UnqPtr<ASTNode> Parser::forStmt() {
    Logger::getInstance().debug("Analizando 'forStmt': " + tokens[current].value);
    if (!match(TokenType::KEYWORD_FOR)) {
        return nullptr;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'for'.");

    Logger::getInstance().debug("TEST1");

    UnqPtr<ASTNode> init = exprStmt();

    Logger::getInstance().debug("TEST2");
    if (!init) {
        Logger::getInstance().error("Error en 'exprStmt' dentro de 'forStmt'");
        return nullptr;
    }
    
    UnqPtr<ASTNode> condition = expression();
    if (!condition) {
        Logger::getInstance().debug("Se esperaba una expresión en la condición del 'for'.");
        synchronize();
    }

    if (!match(TokenType::SEMICOLON)) {
        Logger::getInstance().error("Se esperaba ';' después de la condición del 'for'.");
        synchronize();
        return nullptr;
    }

    UnqPtr<ASTNode> increment = exprStmt();
    if (!increment) {
        Logger::getInstance().error("Error en 'exprStmt' después de ';' en 'forStmt'");
        return nullptr;
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la condición del 'for'.");


    UnqPtr<ASTNode> body = statement();
    if (!body) {
      Logger::getInstance().debug("Error en 'statement' después del 'for'.");
      return nullptr;
    }

    return std::make_unique<ForStatementNode>(std::move(init), std::move(condition), std::move(increment), std::move(body));;
}

// ReturnStmt -> return Expression ;
UnqPtr<ASTNode> Parser::returnStmt() {
    Logger::getInstance().debug("Analizando 'returnStmt'");
    if (!match(TokenType::KEYWORD_RETURN)) {
        return nullptr;
    }
    
    UnqPtr<ASTNode> returnExpr = expression();
    if (!returnExpr) {
        Logger::getInstance().debug("Se esperaba una expresión después de 'return'.");
        synchronize();
        // throw std::runtime_error("Se esperaba una expresión después de 'return'.");
    }

    if (!match(TokenType::SEMICOLON)) {
        Logger::getInstance().error("Se esperaba una expresioon despues de 'return'");
        return nullptr;
    }
    return std::make_unique<ReturnStatementNode>(std::move(returnExpr));
}

// PrintStmt -> print ( ExprList ) ;
UnqPtr<ASTNode> Parser::printStmt() {
    Logger::getInstance().debug("Analizando 'printStmt'");
    if (!match(TokenType::KEYWORD_PRINT)) {
        return nullptr;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'print'.");

    UnqPtr<ExprListNode> exprListNode = exprList();
    if (!exprListNode) {
        Logger::getInstance().error("Error en 'exprList' dentro de 'printStmt'");
        return nullptr;
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la lista de expresiones.");
    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la instrucción 'print'.");
    return std::make_unique<PrintStatementNode>(std::move(exprListNode));;
}

// ExprStmt -> ; | Expression ;
UnqPtr<ASTNode> Parser::exprStmt() {
    Logger::getInstance().debug("Analizando 'exprStmt'");
    if (match(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Encontrado ';' en 'exprStmt'");
        return std::make_unique<ExpressionStatementNode>(nullptr);
    }
    UnqPtr<ASTNode> exprNode = expression();
    if (!exprNode) return nullptr;

    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la expresión.");
    return std::make_unique<ExpressionStatementNode>(std::move(exprNode));
}

// StmtList -> Statement StmtListPrime
UnqPtr<ASTNode> Parser::stmtList() {
    Logger::getInstance().debug("Analizando 'stmtList'");
    auto compoundNode = std::make_unique<CompoundStatementNode>();
    UnqPtr<ASTNode> stmtNode = statement();
    if (!stmtNode) {
        Logger::getInstance().error("Error en 'statement' dentro de 'stmtList'");
        return nullptr;
    }
    compoundNode->AddStatement(std::move(stmtNode));
    if (!stmtListPrime(compoundNode)) return nullptr;

    return compoundNode;
}

// stmtlistprime -> statement stmtlistprime | epsilon
bool Parser::stmtListPrime(UnqPtr<CompoundStatementNode>& compoundNode) {
    Logger::getInstance().debug("Analizando 'stmtListPrime'");

    UnqPtr<ASTNode> stmtNode = statement();
    if (stmtNode) {
        Logger::getInstance().debug("Encontrado 'statement' en 'stmtListPrime'");
        compoundNode->AddStatement(std::move(stmtNode));
        return stmtListPrime(compoundNode);
    }
    if (check(TokenType::RIGHT_BRACE)) {
        Logger::getInstance().debug("Epsilon encontrado en 'stmtListPrime'");
        return true;
    }

    Logger::getInstance().error("Error en 'stmtListPrime': token inesperado");
    return false;
}

/* Type -> IntType TypePrime
Type -> BoolType TypePrime
Type -> CharType TypePrime
Type -> StringType TypePrime
Type -> Void TypePrime */
bool Parser::type() {
    Logger::getInstance().debug("Analizando 'type'");
    if (match(TokenType::KEYWORD_INTEGER) || 
        match(TokenType::KEYWORD_BOOLEAN) ||
        match(TokenType::KEYWORD_CHAR) ||
        match(TokenType::KEYWORD_STRING) ||
        match(TokenType::KEYWORD_VOID)) {
        Logger::getInstance().debug("Encontrado tipo: " + previous().value);
        return typePrime();
    }

    Logger::getInstance().debug("No se encontró un tipo válido en 'type'");
    return false;
}

// TypePrime -> [] TypePrime
bool Parser::typePrime() {
    Logger::getInstance().debug("Analizando 'typePrime'");
    if (match(TokenType::LEFT_BRACKET)) {
        Logger::getInstance().debug("Encontrado '[' en 'typePrime'");
        consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de '['.");
        return typePrime();
    }

    if (check(TokenType::IDENTIFIER)) {
        Logger::getInstance().debug("'typePrime' encontrado epsilon -> IDENTIFIER");
        return true;
    }

    Logger::getInstance().error("Error en 'typePrime', no se encontró un token válido.");
    return false;
}

// Auxiliar function para verificar el inicio de una declaración
bool Parser::checkForDeclarationStart() {
    Logger::getInstance().debug("Verificando inicio de una declaración");
    return check(TokenType::KEYWORD_INTEGER) ||
           check(TokenType::KEYWORD_BOOLEAN) ||
           check(TokenType::KEYWORD_CHAR) ||
           check(TokenType::KEYWORD_STRING) ||
           check(TokenType::KEYWORD_VOID);
}
