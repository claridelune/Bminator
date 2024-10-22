#include "parser.h"
#include "../utils/logger.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

void Parser::parse() {
    Logger& logger = Logger::getInstance();
    logger.info("Iniciando el análisis del programa.");
    if (program()) {
        logger.info("El input pertenece a la gramática.");
    } else {
        logger.info("El input no pertenece a la gramática.");
    }
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

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    Logger::getInstance().error(message);
    throw std::runtime_error(message);
}

/* Program -> Declaration ProgramPrime */
bool Parser::program() {
    Logger::getInstance().debug("Analizando 'program'");
    return declaration() && programPrime();
}

/* ProgramPrime -> Declaration ProgramPrime */
bool Parser::programPrime() {
    Logger::getInstance().debug("Analizando 'programPrime'");
    if (isAtEnd() || !checkForDeclarationStart()) {
        Logger::getInstance().debug("'programPrime' encontrado epsilon");
        return true;
    }
    
    if (!declaration()) {
        Logger::getInstance().error("Error en 'declaration' dentro de 'programPrime'");
        return false;
    }

    return programPrime();
}

// ExprList -> Expression ExprListPrime
bool Parser::exprList() {
    Logger::getInstance().debug("Analizando 'exprList'");
    if (!expression()) {
        Logger::getInstance().error("Error en 'expression' dentro de 'exprList'");
        return false;
    }
    return exprListPrime();
}

// ExprListPrime -> , ExprList
// ExprListPrime -> epsilon
bool Parser::exprListPrime() {
    Logger::getInstance().debug("Analizando 'exprListPrime'");
    if (match(TokenType::COMMA)) {
        if (!exprList()) {
            Logger::getInstance().error("Error en 'exprList' después de ',' en 'exprListPrime'");
            return false;
        }
        return true;
    }
    Logger::getInstance().debug("'exprListPrime' encontrado epsilon");
    return true;
}

// ExprListPrime -> , ExprList
// ExprListPrime -> epsilon
bool Parser::expression() {
    Logger::getInstance().debug("Analizando 'expression'");
    if (match(TokenType::IDENTIFIER)) {
        Logger::getInstance().debug("Encontrado identificador: " + previous().value);
        if (match(TokenType::OPERATOR_EQUAL)) {
            Logger::getInstance().debug("Encontrado operador '='");
            if (!expression()) {
                Logger::getInstance().error("Se esperaba una expresión después de '='.");
                throw std::runtime_error("Se esperaba una expresión después de '='.");
            }
            return true;
        } else {
            Logger::getInstance().error("Se esperaba '=' después del identificador.");
            throw std::runtime_error("Se esperaba '=' después del identificador.");
        }
    }
    return orExpr();
}

/* 
OrExpr -> AndExpr OrExprPrime
*/
bool Parser::orExpr() {
    Logger::getInstance().debug("Analizando 'orExpr'");
    if (!andExpr()) {
        Logger::getInstance().error("Error en 'andExpr' dentro de 'orExpr'");
        return false;
    }
    return orExprPrime();
}

/*
OrExprPrime -> || AndExpr OrExprPrime
OrExprPrime -> epsilon
*/
bool Parser::orExprPrime() {
    Logger::getInstance().debug("Analizando 'orExprPrime'");
    if (match(TokenType::OPERATOR_OR)) {
        Logger::getInstance().debug("Encontrado operador '||'");
        if (!andExpr()) {
            Logger::getInstance().error("Se esperaba una expresión después de '||'.");
            throw std::runtime_error("Se esperaba una expresión después de '||'.");
        }
        return orExprPrime();
    }
    Logger::getInstance().debug("'orExprPrime' encontrado epsilon");
    return true;
}

/*
AndExpr -> EqExpr AndExprPrime
*/
bool Parser::andExpr() {
    Logger::getInstance().debug("Analizando 'andExpr'");
    if (!eqExpr()) {
        Logger::getInstance().error("Error en 'eqExpr' dentro de 'andExpr'");
        return false;
    }
    return andExprPrime();
}

/*
AndExprPrime -> && EqExpr AndExprPrime
AndExprPrime -> epsilon
*/
bool Parser::andExprPrime() {
    Logger::getInstance().debug("Analizando 'andExprPrime'");
    if (match(TokenType::OPERATOR_AND)) {
        Logger::getInstance().debug("Encontrado operador '&&'");
        if (!eqExpr()) {
            Logger::getInstance().error("Se esperaba una expresión después de '&&'.");
            throw std::runtime_error("Se esperaba una expresión después de '&&'.");
        }
        return andExprPrime();
    }
    Logger::getInstance().debug("'andExprPrime' encontrado epsilon");
    return true;
}

/*
MultOrDivOrMod -> * | / | %
*/
bool Parser::multOrDivOrMod() {
    Logger::getInstance().debug("Analizando 'multOrDivOrMod'");
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
bool Parser::unary() {
    Logger::getInstance().debug("Analizando 'unary'");
    if (match(TokenType::OPERATOR_NOT)) {
        Logger::getInstance().debug("Encontrado operador '!'");
        return unary();
    } else if (match(TokenType::OPERATOR_MINUS)) {
        Logger::getInstance().debug("Encontrado operador '-'");
        return unary();
    }
    return factor();
}

/* Factor -> Identifier Parentheses FactorPrime | Literal FactorPrime | ( Expression ) FactorPrime */
bool Parser::factor() {
    Logger::getInstance().debug("Analizando 'factor'");
    if (literal()) {
        Logger::getInstance().debug("Encontrado literal: " + previous().value);
        return factorPrime();
    }
    if (match(TokenType::IDENTIFIER)) {
        Logger::getInstance().debug("Encontrado identificador: " + previous().value);
        return factorPrime();
    }
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        if (!expression()) {
            Logger::getInstance().error("Se esperaba una expresión después de '('");
            return false;
        }
        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");
        return true;
    }
    Logger::getInstance().error("Se esperaba un factor (literal, identificador o expresión entre paréntesis).");
    throw std::runtime_error("Se esperaba un factor (literal, identificador o expresión entre paréntesis).");
}

/* FactorPrime -> [ Expression ] FactorPrime | epsilon */
bool Parser::factorPrime() {
    Logger::getInstance().debug("Analizando 'factorPrime'");
    if (match(TokenType::LEFT_BRACKET)) {
        Logger::getInstance().debug("Encontrado '('");
        if (!expression()) {
            Logger::getInstance().error("Se esperaba una expresión después de '('");
            return false;  
        }
        consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de la expresión.");
        return factorPrime();  
    }
    return true;  
}


/* Literal -> IntegerLiteral | CharLiteral | BooleanLiteral | StringLiteral */
bool Parser::literal() {
    Logger::getInstance().debug("Analizando 'literal'");
    return match(TokenType::LITERAL_INT) ||
           match(TokenType::LITERAL_CHAR) ||
           match(TokenType::KEYWORD_FALSE) || match(TokenType::KEYWORD_TRUE) ||
           match(TokenType::LITERAL_STRING);
}

/*
Term -> Unary TermPrime
*/
bool Parser::term() {
    Logger::getInstance().debug("Analizando 'term'");
    if (!unary()) {
        Logger::getInstance().error("Error en 'unary' dentro de 'term'");
        return false;
    }
    return termPrime();
}

/*
TermPrime -> MultOrDivOrMod Unary TermPrime
TermPrime -> epsilon
*/
bool Parser::termPrime() {
    Logger::getInstance().debug("Analizando 'termPrime'");
    if (multOrDivOrMod()) {
        Logger::getInstance().debug("Encontrado operador de multiplicación/división/módulo");
        if (!unary()) {
            Logger::getInstance().error("Error en 'unary' después de operador en 'termPrime'");
            return false;
        }
        return termPrime();
    }
    Logger::getInstance().debug("'termPrime' encontrado epsilon");
    return true;
}

/*
EqualOrDifferent -> ==
EqualOrDifferent -> !=
*/
bool Parser::equalOrDifferent() {
    Logger::getInstance().debug("Analizando 'equalOrDifferent'");
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
bool Parser::eqExpr() {
    Logger::getInstance().debug("Analizando 'eqExpr'");
    if (!relExpr()) {
        Logger::getInstance().error("Error en 'relExpr' dentro de 'eqExpr'");
        return false;
    }
    return eqExprPrime();
}

/*
EqExprPrime -> EqualOrDifferent RelExpr EqExprPrime
EqExprPrime -> epsilon
*/
bool Parser::eqExprPrime() {
    Logger::getInstance().debug("Analizando 'eqExprPrime'");
    if (equalOrDifferent()) {
        Logger::getInstance().debug("Encontrado operador de igualdad/desigualdad");
        if (!relExpr()) {
            Logger::getInstance().error("Error en 'relExpr' después de operador en 'eqExprPrime'");
            return false;
        }
        return eqExprPrime();
    }
    Logger::getInstance().debug("'eqExprPrime' encontrado epsilon");
    return true;
}

/*
GreaterOrLess -> <
GreaterOrLess -> >
GreaterOrLess -> <=
GreaterOrLess -> >=
*/
bool Parser::greaterOrLess() {
    Logger::getInstance().debug("Analizando 'greaterOrLess'");
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
bool Parser::relExpr() {
    Logger::getInstance().debug("Analizando 'relExpr'");
    if (!expr()) {
        Logger::getInstance().error("Error en 'expr' dentro de 'relExpr'");
        return false;
    }
    return relExprPrime();
}

/*
RelExprPrime -> GreaterOrLess Expr RelExprPrime
RelExprPrime -> epsilon
*/
bool Parser::relExprPrime() {
    Logger::getInstance().debug("Analizando 'relExprPrime'");
    if (greaterOrLess()) {
        Logger::getInstance().debug("Encontrado operador de comparación");
        if (!expr()) {
            Logger::getInstance().error("Error en 'expr' después de operador en 'relExprPrime'");
            return false;
        }
        return relExprPrime();
    }
    Logger::getInstance().debug("'relExprPrime' encontrado epsilon");
    return true;
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
bool Parser::expr() {
    Logger::getInstance().debug("Analizando 'expr'");
    if (!term()) {
        Logger::getInstance().error("Error en 'term' dentro de 'expr'");
        return false;
    }
    return exprPrime();
}

/*
ExprPrime -> SumOrRest Term ExprPrime
ExprPrime -> epsilon
*/
bool Parser::exprPrime() {
    Logger::getInstance().debug("Analizando 'exprPrime'");
    if (sumOrRest()) {
        Logger::getInstance().debug("Encontrado operador de suma/resta");
        if (!term()) {
            Logger::getInstance().error("Error en 'term' después de operador en 'exprPrime'");
            return false;
        }
        return exprPrime();
    }
    Logger::getInstance().debug("'exprPrime' encontrado epsilon");
    return true;
}

/* Parentheses -> ( ExprList ) | epsilon */
bool Parser::parentheses() {
    Logger::getInstance().debug("Analizando 'parentheses'");
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        if (!exprList()) {
            Logger::getInstance().error("Error en 'exprList' después de '(' en 'parentheses'");
            return false;
        }
        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la lista de expresiones.");
        return true;
    }
    Logger::getInstance().debug("'parentheses' encontrado epsilon");
    return true;
}

// Statement -> VarDecl | IfStmt | ForStmt | ReturnStmt | ExprStmt | PrintStmt | StmtList
bool Parser::statement() {
    Logger::getInstance().debug("Analizando 'statement'");
    if (varDecl()) {
        return true;
    } else if (ifStmt()) {
        return true;
    } else if (forStmt()) {
        return true;
    } else if (returnStmt()) {
        return true;
    } else if (exprStmt()) {
        return true;
    } else if (printStmt()) {
        return true;
    } else if (stmtList()) {
        return true;
    }

    Logger::getInstance().error("Ninguna coincidencia encontrada en 'statement'");
    return false;
}

// IfStmt -> if ( Expression ) { Statement IfStmtPrime }
bool Parser::ifStmt() {
    Logger::getInstance().debug("Analizando 'ifStmt'");
    if (!match(TokenType::KEYWORD_IF)) {
        return false;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'if'.");
    if (!expression()) {
        Logger::getInstance().error("Se esperaba una expresión dentro del 'if'.");
        throw std::runtime_error("Se esperaba una expresión dentro del 'if'.");
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");
    consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de la expresión.");

    if (!statement()) {
        Logger::getInstance().error("Error en 'statement' dentro de 'ifStmt'");
        return false;
    }

    consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'if'.");
    return ifStmtPrime();
}

// IfStmtPrime -> else { Statement } | epsilon
bool Parser::ifStmtPrime() {
    Logger::getInstance().debug("Analizando 'ifStmtPrime'");
    if (match(TokenType::KEYWORD_ELSE)) {
        Logger::getInstance().debug("Encontrado 'else'");
        consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de 'else'.");
        if (!statement()) {
            Logger::getInstance().error("Error en 'statement' dentro de 'ifStmtPrime'");
            return false;
        }
        consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'else'.");
    }
    Logger::getInstance().debug("'ifStmtPrime' encontrado epsilon");
    return true;
}

// ForStmt -> for ( ExprStmt Expression ; ExprStmt ) Statement
bool Parser::forStmt() {
    Logger::getInstance().debug("Analizando 'forStmt'");
    if (!match(TokenType::KEYWORD_FOR)) {
        return false;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'for'.");
    if (!exprStmt()) {
        Logger::getInstance().error("Error en 'exprStmt' dentro de 'forStmt'");
        return false;
    }

    if (!expression()) {
        Logger::getInstance().error("Se esperaba una expresión en la condición del 'for'.");
        throw std::runtime_error("Se esperaba una expresión en la condición del 'for'.");
    }

    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la condición del 'for'.");
    if (!exprStmt()) {
        Logger::getInstance().error("Error en 'exprStmt' después de ';' en 'forStmt'");
        return false;
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la condición del 'for'.");
    if (!statement()) {
        Logger::getInstance().error("Error en 'statement' después del 'for'.");
        return false;
    }

    return true;
}

// ReturnStmt -> return Expression ;
bool Parser::returnStmt() {
    Logger::getInstance().debug("Analizando 'returnStmt'");
    if (!match(TokenType::KEYWORD_RETURN)) {
        return false;
    }

    if (!expression()) {
        Logger::getInstance().error("Se esperaba una expresión después de 'return'.");
        throw std::runtime_error("Se esperaba una expresión después de 'return'.");
    }

    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la expresión de retorno.");
    return true;
}

// PrintStmt -> print ( ExprList ) ;
bool Parser::printStmt() {
    Logger::getInstance().debug("Analizando 'printStmt'");
    if (!match(TokenType::KEYWORD_PRINT)) {
        return false;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'print'.");
    if (!exprList()) {
        Logger::getInstance().error("Error en 'exprList' dentro de 'printStmt'");
        return false;
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la lista de expresiones.");
    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la instrucción 'print'.");
    return true;
}

// ExprStmt -> ; | Expression ;
bool Parser::exprStmt() {
    Logger::getInstance().debug("Analizando 'exprStmt'");
    if (match(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Encontrado ';' en 'exprStmt'");
        return true;
    }

    if (!expression()) {
        Logger::getInstance().error("Error en 'expression' dentro de 'exprStmt'");
        return false;
    }

    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la expresión.");
    return true;
}

// StmtList -> Statement StmtListPrime
bool Parser::stmtList() {
    Logger::getInstance().debug("Analizando 'stmtList'");
    if (!statement()) {
        Logger::getInstance().error("Error en 'statement' dentro de 'stmtList'");
        return false;
    }

    return stmtListPrime();
}

// stmtlistprime -> statement stmtlistprime | epsilon
bool Parser::stmtListPrime() {
    Logger::getInstance().debug("Analizando 'stmtListPrime'");
    if (statement()) {
        Logger::getInstance().debug("Encontrado 'statement' en 'stmtListPrime'");
        return stmtListPrime();
    }

    Logger::getInstance().debug("'stmtListPrime' encontrado epsilon");
    return true;
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

    Logger::getInstance().error("No se encontró un tipo válido en 'type'");
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

/*
Declaration -> Function
Declaration -> VarDecl
*/
bool Parser::declaration() {
    Logger::getInstance().debug("Analizando 'declaration'");
    if (function()) {
        return true;
    }

    if (varDecl()) {
        return true;
    }

    Logger::getInstance().error("No se encontró declaración válida en 'declaration'");
    return false;
}

/*
function -> type identifier ( params ) { stmtlist }
*/
bool Parser::function() {
    Logger::getInstance().debug("Analizando 'function'");
    if (type()) {
        if (match(TokenType::IDENTIFIER)) {
            Logger::getInstance().debug("Encontrado identificador de función: " + previous().value);
            consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' al inicio de los parámetros.");
            if (params()) {
                consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' al final de los parámetros.");
                consume(TokenType::LEFT_BRACE, "Se esperaba '{' al inicio del cuerpo de la función.");
                
                if (stmtList()) {
                    consume(TokenType::RIGHT_BRACE, "Se esperaba '}' al final del cuerpo de la función.");
                    return true;
                }
            }
        }
        Logger::getInstance().error("Error en la declaración de la función.");
        throw std::runtime_error("Error en la declaración de la función.");
    }

    return false;
}

/*
Params -> Type Identifier Params
Params -> , Params
Params -> epsilon
*/
bool Parser::params() {
    Logger::getInstance().debug("Analizando 'params'");
    if (type()) {
        if (match(TokenType::IDENTIFIER)) {
            Logger::getInstance().debug("Encontrado parámetro: " + previous().value);
            if (params()) {
                return true;
            }
            Logger::getInstance().error("Error en los parámetros de la función.");
            return false;
        }
        Logger::getInstance().error("Se esperaba un identificador después del tipo en 'params'.");
        throw std::runtime_error("Se esperaba un identificador después del tipo en 'params'.");
    }

    if (match(TokenType::COMMA)) {
        Logger::getInstance().debug("Encontrado ',' en 'params'");
        if (params()) {
            return true;
        }
        Logger::getInstance().error("Error en los parámetros después de ',' en 'params'");
        return false;
    }

    Logger::getInstance().debug("'params' encontrado epsilon");
    return true;
}

// VarDecl -> Type Identifier VarDeclPrime
bool Parser::varDecl() {
    Logger::getInstance().debug("Analizando 'varDecl'");
    if (!type()) {
        Logger::getInstance().error("No se encontró un tipo válido en 'varDecl'");
        return false;
    }

    if (!match(TokenType::IDENTIFIER)) {
        Logger::getInstance().error("Se esperaba un identificador después del tipo en 'varDecl'");
        throw std::runtime_error("Se esperaba un identificador después del tipo.");
    }

    Logger::getInstance().debug("Declaración de variable con identificador: " + previous().value);
    return varDeclPrime();
}

    // VarDeclPrime -> ;
    // VarDeclPrime -> = Expression ;
bool Parser::varDeclPrime() {
    Logger::getInstance().debug("Analizando 'varDeclPrime'");
    if (match(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Declaración de variable finalizada con ';'");
        return true;
    }

    if (match(TokenType::OPERATOR_EQUAL)) {
        Logger::getInstance().debug("Asignación en declaración de variable");
        if (!expression()) {
            Logger::getInstance().error("Se esperaba una expresión después del '=' en 'varDeclPrime'");
            throw std::runtime_error("Se esperaba una expresión después del '='.");
        }

        consume(TokenType::SEMICOLON, "Se esperaba ';' al final de la declaración.");
        Logger::getInstance().debug("Declaración de variable con asignación finalizada");
        return true;
    }

    Logger::getInstance().error("Error en 'varDeclPrime', no se encontró ';' o '='.");
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
