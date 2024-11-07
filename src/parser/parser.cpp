#include "parser.h"
#include "../utils/logger.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

bool Parser::parse() {
    Logger& logger = Logger::getInstance();
    logger.info("Iniciando el análisis del programa.");
    program();
    if (!failed) {
        logger.info("El input pertenece a la gramática.");
    } else {
        logger.info("El input no pertenece a la gramática.");
    }
    return failed;
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
    Logger::getInstance().debug(message);
    throw std::runtime_error(message);
}

void Parser::synchronize() {
    Logger::getInstance().debug("SYNC from " + tokens[current].value);
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return; // Retorna al encontrar un punto y coma
        Logger::getInstance().debug("Advanced token " + tokens[current].value);
        advance();
    }
    program();
}

/* Program -> Declaration ProgramPrime */
bool Parser::program() {
    Logger::getInstance().debug("Analizando 'program' con: " + tokens[current].value);
    failed = !(declaration() && programPrime());
    return failed;
}

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
bool Parser::declaration() {
    Logger::getInstance().debug("Analizando 'declaration' con: " + tokens[current].value);

    if (type()) {
        if (match(TokenType::IDENTIFIER)) {
            Logger::getInstance().debug("Encontrado identificador en 'declaration': " + previous().value);
            return declarationPrime();
        } else {
            Logger::getInstance().debug("Se esperaba un identificador después del tipo en 'declaration'.");
            synchronize();
            //throw std::runtime_error("Se esperaba un identificador después del tipo.");
        }
    }

    Logger::getInstance().error("Error en 'declaration', no se encontró un tipo válido para " + tokens[current].value);
    synchronize();
    return false;
}


/*
DeclarationPrime -> function || varDecl
 * */
bool Parser::declarationPrime() {
    Logger::getInstance().debug("Analizando 'declarationPrime' con: " + tokens[current].value);
    if (check(TokenType::LEFT_PARENTHESIS)) return function();
    if (check(TokenType::OPERATOR_ASSIGN)) return varDecl();
    Logger::getInstance().error("Se esperaba un '(' o un operador de asignación en lugar de: " + tokens[current].value);
    synchronize();
    return false;
}

/*
function -> ( params ) { stmtlist }
*/
bool Parser::function() {
    Logger::getInstance().debug("Analizando 'function' con: " + tokens[current].value);
    if (match(TokenType::LEFT_PARENTHESIS) && params()) {
        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' al final de los parámetros.");
        consume(TokenType::LEFT_BRACE, "Se esperaba '{' al inicio del cuerpo de la función.");
        
        if (stmtList()) {
            consume(TokenType::RIGHT_BRACE, "Se esperaba '}' al final del cuerpo de la función.");
            return true;
        }
        Logger::getInstance().error("Error en la declaración de la función.");
        // throw std::runtime_error("Error en la declaración de la función.");
    }
    Logger::getInstance().error("Se esperaba una declaración de tipo (content)");
    synchronize();
    return false;
}

/*
Params -> Type Identifier Params
Params -> , Params
Params -> epsilon
*/
bool Parser::params() {
    Logger::getInstance().debug("Analizando 'params' con: " + tokens[current].value);
    if (type()) {
        if (match(TokenType::IDENTIFIER)) {
            Logger::getInstance().debug("Encontrado parámetro: " + previous().value);
            if (params()) {
                return true;
            }
            Logger::getInstance().error("Se esperaban parámetros válidos");
            synchronize();
            return false;
        }
        Logger::getInstance().error("Se esperaba un identificador después del tipo en 'params'.");
        synchronize();
        //throw std::runtime_error("Se esperaba un identificador después del tipo en 'params'.");
    }

    if (match(TokenType::COMMA)) {
        Logger::getInstance().debug("Encontrado ',' en 'params'" + tokens[current].value);
        if (params()) {
            return true;
        }
        Logger::getInstance().error("Se esperaban parámetros válidos después de ',' en 'params'");
        synchronize();
        return false;
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
bool Parser::varDecl() {
    Logger::getInstance().debug("Analizando 'varDecl' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Asignación en declaración de variable");
        if (!expression()) {
            Logger::getInstance().debug("Se esperaba una expresión después del '=' en 'varDecl'");
            throw std::runtime_error("Se esperaba una expresión después del '='.");
        }

        consume(TokenType::SEMICOLON, "Se esperaba ';' al final de la declaración.");
        Logger::getInstance().debug("Declaración de variable con asignación finalizada");
        return true;
    }
    if (match(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Declaración de variable con asignación finalizada");
        return true;
    }

    Logger::getInstance().error("Error en 'varDecl', se esperaba ';' o '='.");
    return false;
}

// ExprList -> Expression ExprListPrime
bool Parser::exprList() {
    Logger::getInstance().debug("Analizando 'exprList' con: " + tokens[current].value);
    if (!expression()) {
        Logger::getInstance().error("Error en 'expression' dentro de 'exprList'");
        return false;
    }
    return exprListPrime();
}

// ExprListPrime -> , ExprList
// ExprListPrime -> epsilon
bool Parser::exprListPrime() {
    Logger::getInstance().debug("Analizando 'exprListPrime' con: " + tokens[current].value);
    if (match(TokenType::COMMA)) {
        if (!exprList()) {
            Logger::getInstance().error("Error en 'exprList' después de ',' en 'exprListPrime'");
            return false;
        }
        return true;
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
bool Parser::expression() {
    Logger::getInstance().debug("Analizando 'expression' con: " + tokens[current].value);
    return orExpr() && expressionPrime();
}

/*  
ExpressionPrime -> epsilon
ExpressionPrime -> = OrExpr
*/
bool Parser::expressionPrime() {
    Logger::getInstance().debug("Analizando 'expressionPrime' con: " + tokens[current].value);

    if (match(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Se encontró operador de asignación");
        if (!orExpr()) {
            Logger::getInstance().error("Error en 'orExpr' después de operador de asignación en 'expressionPrime'");
            return false;
        }
        return true;
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON)) {
        Logger::getInstance().debug("Epsilon encontrado en 'expressionPrime'");
        return true;
    }
    Logger::getInstance().error("Error en 'expressionPrime': token inesperado");
    return false;
}

/* 
OrExpr -> AndExpr OrExprPrime
*/
bool Parser::orExpr() {
    Logger::getInstance().debug("Analizando 'orExpr' con: " + tokens[current].value);
    if (!andExpr()) return false;
    return orExprPrime();
}

/*
OrExprPrime -> || AndExpr OrExprPrime
OrExprPrime -> epsilon
*/
bool Parser::orExprPrime() {
    Logger::getInstance().debug("Analizando 'orExprPrime' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_OR)) {
        Logger::getInstance().debug("Encontrado operador '||'");
        if (!andExpr()) {
            Logger::getInstance().debug("Se esperaba una expresión después de '||'.");
            throw std::runtime_error("Se esperaba una expresión después de '||'.");
        }
        return orExprPrime();
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN)) {
        Logger::getInstance().debug("Epsilon encontrado en 'orExprPrime'");
        return true;
    }

    Logger::getInstance().error("Error en 'orExprPrime': token inesperado");
    return false;
}

/*
AndExpr -> EqExpr AndExprPrime
*/
bool Parser::andExpr() {
    Logger::getInstance().debug("Analizando 'andExpr' con: " + tokens[current].value);
    if (!eqExpr()) return false;
    return andExprPrime();
}

/*
AndExprPrime -> && EqExpr AndExprPrime
AndExprPrime -> epsilon
*/
bool Parser::andExprPrime() {
    Logger::getInstance().debug("Analizando 'andExprPrime' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_AND)) {
        Logger::getInstance().debug("Encontrado operador '&&'");
        if (!eqExpr()) {
            Logger::getInstance().debug("Se esperaba una expresión después de '&&'.");
            throw std::runtime_error("Se esperaba una expresión después de '&&'.");
        }
        return andExprPrime();
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
        return true;
    }

    Logger::getInstance().error("Error en 'andExprPrime': token inesperado");
    return false;
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
bool Parser::unary() {
    Logger::getInstance().debug("Analizando 'unary' con: " + tokens[current].value);
    if (match(TokenType::OPERATOR_NOT)) {
        Logger::getInstance().debug("Encontrado operador '!'");
        return unary();
    } else if (match(TokenType::OPERATOR_MINUS)) {
        Logger::getInstance().debug("Encontrado operador '-'");
        return unary();
    }
    return factor();
}

/* 
 * Factor -> Identifier Parenthesis FactorPrime | Literal FactorPrime | ( Expression ) FactorPrime 
 */
bool Parser::factor() {
    Logger::getInstance().debug("Analizando 'factor' con: " + tokens[current].value);
    if (literal()) {
        Logger::getInstance().debug("Encontrado literal: " + previous().value);
        return factorPrime();
    }
    if (match(TokenType::IDENTIFIER)) {
        Logger::getInstance().debug("Encontrado identificador: " + previous().value);
        return Parenthesis() && factorPrime();
    }
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        if (!expression()) {
            Logger::getInstance().debug("Se esperaba una expresión después de '('");
            return false;
        }
        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");
        return factorPrime();
    }
    return false;
}

/* Parenthesis -> ( ExprList ) | epsilon */
bool Parser::Parenthesis() {
    Logger::getInstance().debug("Analizando 'Parenthesis' con: " + tokens[current].value);
    if (match(TokenType::LEFT_PARENTHESIS)) {
        Logger::getInstance().debug("Encontrado '('");
        if (!exprList()) {
          Logger::getInstance().debug(
              "Se esperaba una exprList después de '('");
          return false;
        }
        consume(TokenType::RIGHT_PARENTHESIS,
                "Se esperaba '}' después de exprList.");
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
        return true;
    }
    return false;
}

/* FactorPrime -> [ Expression ] FactorPrime | epsilon */
bool Parser::factorPrime() {
    Logger::getInstance().debug("Analizando 'factorPrime': " + tokens[current].value);
    if (match(TokenType::LEFT_BRACKET)) {
        Logger::getInstance().debug("Encontrado '['");
        if (!expression()) {
            Logger::getInstance().debug("Se esperaba una expresión después de '['");
            return false;  
        }
        consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de la expresión.");
        return factorPrime();  
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
        return true;
    }
    return false;  
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
bool Parser::term() {
    Logger::getInstance().debug("Analizando 'term' con: " + tokens[current].value);
    if (!unary()) return false;
    return termPrime();
}

/*
TermPrime -> MultOrDivOrMod Unary TermPrime
TermPrime -> epsilon
*/
bool Parser::termPrime() {
    Logger::getInstance().debug("Analizando 'termPrime' con: " + tokens[current].value);
    if (multOrDivOrMod()) {
        Logger::getInstance().debug("Encontrado operador de multiplicación/división/módulo");
        if (!unary()) {
            Logger::getInstance().error("Error en 'unary' después de operador en 'termPrime'");
            return false;
        }
        return termPrime();
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
        return true;
    }

    Logger::getInstance().error("Error en 'termPrime': token inesperado");
    return false;
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
bool Parser::eqExpr() {
    Logger::getInstance().debug("Analizando 'eqExpr' con:" + tokens[current].value);
    if (!relExpr()) return false;
    return eqExprPrime();
}

/*
EqExprPrime -> EqualOrDifferent RelExpr EqExprPrime
EqExprPrime -> epsilon
*/
bool Parser::eqExprPrime() {
    Logger::getInstance().debug("Analizando 'eqExprPrime' con: " + tokens[current].value);
    if (equalOrDifferent()) {
        Logger::getInstance().debug("Encontrado operador de igualdad/desigualdad");
        if (!relExpr()) {
            Logger::getInstance().error("Error en 'relExpr' después de operador en 'eqExprPrime'");
            return false;
        }
        return eqExprPrime();
    }
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_AND)) {
        Logger::getInstance().debug("Epsilon encontrado en 'eqExprPrime'");
        return true;
    }
    Logger::getInstance().error("Error en 'eqExprPrime': token inesperado");
    return false;
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
bool Parser::relExpr() {
    Logger::getInstance().debug("Analizando 'relExpr'");
    if (!expr()) return false;
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
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL)) {
        Logger::getInstance().debug("Epsilon encontrado en 'relExprPrime'");
        return true;
    }

    Logger::getInstance().error("Error en 'relExprPrime': token inesperado");
    return false;
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
    Logger::getInstance().debug("Analizando 'expr' con: " + tokens[current].value);
    if (!term()) return false;
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
    if (check(TokenType::RIGHT_PARENTHESIS) || check(TokenType::RIGHT_BRACKET) ||
        check(TokenType::COMMA) || check(TokenType::SEMICOLON) ||
        check(TokenType::OPERATOR_ASSIGN) || check(TokenType::OPERATOR_OR) ||
        check(TokenType::OPERATOR_AND) || check(TokenType::OPERATOR_EQUAL) ||
        check(TokenType::OPERATOR_NOT_EQUAL) || check(TokenType::OPERATOR_LESS_THAN) ||
        check(TokenType::OPERATOR_GREATER_THAN) || check(TokenType::OPERATOR_LESS_EQUAL) ||
        check(TokenType::OPERATOR_GREATER_EQUAL)) {
        Logger::getInstance().debug("Epsilon encontrado en 'exprPrime'");
        return true;
    }
    Logger::getInstance().error("Error en 'exprPrime': token inesperado");
    return false;
}

// Statement -> VarDecl | IfStmt | ForStmt | ReturnStmt | ExprStmt | PrintStmt | {StmtList}
bool Parser::statement() {
    Logger::getInstance().debug("Analizando 'statement' con: " + tokens[current].value);
    if (type() && match(TokenType::IDENTIFIER)) {
        Logger::getInstance().debug("Encontrado identificador en 'statement': " + previous().value);
        return varDecl();
    } else if (ifStmt()) {
        return true;
    } else if (forStmt()) {
        return true;
    } else if (returnStmt()) {
        return true;
    } else if (printStmt()) {
        return true;
    } else if (exprStmt()) {
        return true;
    } else if (match(TokenType::LEFT_BRACE)) {
        if (!stmtList()) {
            Logger::getInstance().error("Se espera stmtList despues de { en 'statement'");
            return false;
        }
        consume(TokenType::RIGHT_BRACE, "Se espera } despues de stmtList");
    }

    Logger::getInstance().debug("Ninguna coincidencia encontrada en 'statement'");
    return false;
}

// IfStmt -> if ( Expression ) { Statement } ifStmtPrime
bool Parser::ifStmt() {
    Logger::getInstance().debug("Analizando 'ifStmt'");
    if (!match(TokenType::KEYWORD_IF)) {
        return false;
    }
    Logger::getInstance().debug("Analizando 'ifStmt' - IF ENCONTRADO");

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'if'.");
    if (!expression()) {
        Logger::getInstance().debug("Se esperaba una expresión dentro del 'if'.");
        throw std::runtime_error("Se esperaba una expresión dentro del 'if'.");
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");

    Logger::getInstance().debug("Analizando 'ifStmt' - ENCONTRADA CONDICIÓN ENTRE PARENTESIS");

    consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de la expresión.");

    if (!stmtList()) {
        Logger::getInstance().error("Error en 'statement' dentro de 'ifStmt'");
        return false;
    }

    consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'if'.");
    Logger::getInstance().debug("Analizando 'ifStmt' - ENCONTRADO BLOQUE ENTRE CORCHETES");
    return ifStmtPrime();
}

// IfStmtPrime -> else { Statement } | epsilon
bool Parser::ifStmtPrime() {
    Logger::getInstance().debug("Analizando 'ifStmtPrime' con: " + tokens[current].value);
    if (match(TokenType::KEYWORD_ELSE)) {
        Logger::getInstance().debug("Encontrado 'else'");
        consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de 'else'.");
        if (!statement()) {
            Logger::getInstance().error("Error en 'statement' dentro de 'ifStmtPrime'");
            return false;
        }
        consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'else'.");
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
        return true;
    }

    Logger::getInstance().error("Error en 'ifStmtPrime': token inesperado");
    return false;
}

// ForStmt -> for ( ExprStmt Expression ; ExprStmt ) Statement
bool Parser::forStmt() {
    Logger::getInstance().debug("Analizando 'forStmt': " + tokens[current].value);
    if (!match(TokenType::KEYWORD_FOR)) {
        return false;
    }

    consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'for'.");
    if (!exprStmt()) {
        Logger::getInstance().error("Error en 'exprStmt' dentro de 'forStmt'");
        return false;
    }

    if (!expression()) {
        Logger::getInstance().debug("Se esperaba una expresión en la condición del 'for'.");
        throw std::runtime_error("Se esperaba una expresión en la condición del 'for'.");
    }

    consume(TokenType::SEMICOLON, "Se esperaba ';' después de la condición del 'for'.");
    if (!exprStmt()) {
        Logger::getInstance().error("Error en 'exprStmt' después de ';' en 'forStmt'");
        return false;
    }

    consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la condición del 'for'.");

    if (!statement()) {
      Logger::getInstance().debug("Error en 'statement' después del 'for'.");
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
        Logger::getInstance().debug("Se esperaba una expresión después de 'return'.");
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

    if (!expression()) return false;

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
