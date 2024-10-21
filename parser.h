#include <string>
#include <vector>
#include "Token.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    // Esto se llama desde el main
    void parse() {
        if(program()){
            std::cout << "Input belongs to grammar\n";
        }
        else{
            std::cout << "Input does not belong to grammar\n";
        }
    }

private:
    std::vector<Token> tokens;
    int current;

    bool isAtEnd() {
        return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
    }

    Token peek() {
        return tokens[current];
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Token previous() {
        return tokens[current - 1];
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw std::runtime_error(message);
    }

    /* Program -> Declaration ProgramPrime */
    bool program() {
        return declaration() && programPrime();
    }

    // ExprList -> Expression ExprListPrime
    bool exprList() {
        if (!expression()) {
            return false;
        }
        return exprListPrime();
    }

    // ExprListPrime -> , ExprList
    // ExprListPrime -> epsilon
    bool exprListPrime() {
        if (match(TokenType::COMMA)) {
            if (!exprList()) {
                return false;
            }
            return true;
        }
        return true;
    }

    /*  Expression -> Identifier = Expression
        Expression -> OrExpr */
    bool expression() {
        if (match(TokenType::IDENTIFIER)) {
            if (match(TokenType::OPERATOR_EQUAL)) {
                if (!expression()) {
                    throw std::runtime_error("Se esperaba una expresión después de '='.");
                }
                return true;
            } else {
                throw std::runtime_error("Se esperaba '=' después del identificador.");
            }
        }
        return orExpr();
    }

    /* 
    OrExpr -> AndExpr OrExprPrime
    */
    bool orExpr() {
        if (!andExpr()) {
            return false;
        }
        return orExprPrime();
    }

    /*
    OrExprPrime -> || AndExpr OrExprPrime
    OrExprPrime -> epsilon
    */
    bool orExprPrime() {
        if (match(TokenType::OPERATOR_OR)) {
            if (!andExpr()) {
                throw std::runtime_error("Se esperaba una expresión después de '||'.");
            }
            return orExprPrime();
        }
        return true;
    }

    /*
    AndExpr -> EqExpr AndExprPrime
    */
    bool andExpr() {
        if (!eqExpr()) {
            return false;
        }

        return andExprPrime();
    }

    /*
    AndExprPrime -> && EqExpr AndExprPrime
    AndExprPrime -> epsilon
    */
    bool andExprPrime() {
        if (match(TokenType::OPERATOR_AND)) {
            if (!eqExpr()) {
                throw std::runtime_error("Se esperaba una expresión después de '&&'.");
            }
            return andExprPrime();
        }

        //epsilon
        return true;
    }

    /*
    MultOrDivOrMod -> * | / | %
    */
    bool multOrDivOrMod() {
        if (match(TokenType::OPERATOR_MULTIPLY) || 
            match(TokenType::OPERATOR_DIVIDE) || 
            match(TokenType::OPERATOR_MOD)) {
            return true;
        }
        return false;
    }

    /*
    Unary -> ! Unary
    Unary -> - Unary
    Unary -> Factor
    */
    bool unary() {
        if (match(TokenType::OPERATOR_NOT)) {  // `! Unary`
            return unary();
        } else if (match(TokenType::OPERATOR_MINUS)) {  // `- Unary`
            return unary();
        }

        return factor();
    }

    /* ProgramPrime -> Declaration ProgramPrime */
    bool programPrime() {
        /* ProgramPrime -> epsilon */
        if (isAtEnd() || !checkForDeclarationStart()) {
            return true;  
        }
        
        if (!declaration()) {  
            return false;
        }

        return programPrime();  
    }

    // Auxiliar function para verificar el inicio de una declaración
    bool checkForDeclarationStart() {
        return check(TokenType::KEYWORD_INTEGER) ||
            check(TokenType::KEYWORD_BOOLEAN) || 
            check(TokenType::KEYWORD_CHAR) ||
            check(TokenType::KEYWORD_STRING) ||
            check(TokenType::KEYWORD_VOID);
    }

    /*
    Term -> Unary TermPrime
    */
    bool term() {
        if (!unary()) {
            return false;
        }
        return termPrime();
    }

    /*
    TermPrime -> MultOrDivOrMod Unary TermPrime
    TermPrime -> epsilon
    */
    bool termPrime() {
        if (multOrDivOrMod()) {
            if (!unary()) {
                return false; 
            }

            return termPrime();
        }

        return true;
    }

    /*
    EqualOrDifferent -> ==
    EqualOrDifferent -> !=
    */
    bool equalOrDifferent() {
        if (match(TokenType::OPERATOR_EQUAL_EQUAL)) {
            return true; 
        }
        if (match(TokenType::OPERATOR_NOT_EQUAL)) {
            return true;
        }
        return false;
    }

    /*
    EqExpr -> RelExpr EqExprPrime
    */
    bool eqExpr() {
        if (!relExpr()) {
            return false;
        }
        
        return eqExprPrime();
    }

    /*
    EqExprPrime -> EqualOrDifferent RelExpr EqExprPrime
    EqExprPrime -> epsilon
    */
    bool eqExprPrime() {
        if (equalOrDifferent()) {
            if (!relExpr()) {
                return false;
            }
            return eqExprPrime();
        }
        return true;
    }

    /*
    GreaterOrLess -> <
    GreaterOrLess -> >
    GreaterOrLess -> <=
    GreaterOrLess -> >=
    */
    bool greaterOrLess() {
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
    bool relExpr() {
        if (!expr()) {
            return false;
        }
        return relExprPrime();
    }

    /*
    RelExprPrime -> GreaterOrLess Expr RelExprPrime
    RelExprPrime -> epsilon
    */
    bool relExprPrime() {
        if (greaterOrLess()) {
            if (!expr()) {
                return false;
            }
            return relExprPrime();
        }
        return true;
    }

    /*
    SumOrRest -> +
    SumOrRest -> -
    */
    bool sumOrRest() {
        if (match(TokenType::OPERATOR_PLUS) || match(TokenType::OPERATOR_MINUS)) {
            return true;
        }
        return false;
    }

    /*
    Expr -> Term ExprPrime
    */
    bool expr() {
        if (!term()) {
            return false;
        }
        return exprPrime();
    }

    /*
    ExprPrime -> SumOrRest Term ExprPrime
    ExprPrime -> epsilon
    */
    bool exprPrime() {
        if (sumOrRest()) {
            if (!term()) {
                return false;
            }
            return exprPrime();
        }

        return true;
    }


    /* Parentheses -> ( ExprList ) | epsilon */
    bool parentheses() {
        if (match(TokenType::LEFT_PARENTHESIS)) {
            consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' al comienzo de la lista de expresiones.");
            
            if (!exprList()) {
                return false;  
            }

            consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la lista de expresiones.");
        }
        return true;  
    }

    /* Factor -> Identifier Parentheses FactorPrime | Literal FactorPrime | ( Expression ) FactorPrime */
    bool factor() {
        if (literal()) {
            return factorPrime();  
        }
        
        if (match(TokenType::IDENTIFIER)) {
            return factorPrime();  
        }
        
        if (match(TokenType::LEFT_PARENTHESIS)) {
            if (!expression()) {
                return false;  
            }
            consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");
            return true;  
        }

        throw std::runtime_error("Se esperaba un factor (literal, identificador o expresión entre paréntesis).");
    }

    /* Literal -> IntegerLiteral | CharLiteral | BooleanLiteral | StringLiteral */
    bool literal() {
        return match(TokenType::LITERAL_INT) ||
            match(TokenType::LITERAL_CHAR) ||
            match(TokenType::KEYWORD_FALSE) || match(TokenType::KEYWORD_TRUE) ||
            match(TokenType::LITERAL_STRING);
    }

    /* FactorPrime -> [ Expression ] FactorPrime | epsilon */
    bool factorPrime() {
        if (match(TokenType::LEFT_BRACKET)) {
            if (!expression()) {
                return false;  
            }
            consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de la expresión.");
            return factorPrime();  
        }
        return true;  
    }
    // Statement -> VarDecl | IfStmt | ForStmt | ReturnStmt | ExprStmt | PrintStmt | StmtList
    bool statement() {
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

        return false;
    }

    // IfStmt -> if ( Expression ) { Statement IfStmtPrime }
    bool ifStmt() {
        if (!match(TokenType::KEYWORD_IF)) {
            return false;
        }

        consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'if'.");
        
        if (!expression()) {
            throw std::runtime_error("Se esperaba una expresión dentro del 'if'.");
        }

        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la expresión.");
        
        consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de la expresión.");
        
        if (!statement()) {
            return false;
        }
        
        consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'if'.");
        
        return ifStmtPrime();
    }
    
    // IfStmtPrime -> else { Statement } | epsilon
    bool ifStmtPrime() {
        if (match(TokenType::KEYWORD_ELSE)) {
            consume(TokenType::LEFT_BRACE, "Se esperaba '{' después de 'else'.");
            
            if (!statement()) {
                return false;
            }
            
            consume(TokenType::RIGHT_BRACE, "Se esperaba '}' después del bloque de 'else'.");
        }

        return true;  // epsilon
    }

    // ForStmt -> for ( ExprStmt Expression ; ExprStmt ) Statement
    bool forStmt() {
        if (!match(TokenType::KEYWORD_FOR)) {
            return false;
        }

        consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'for'.");
        
        if (!exprStmt()) {
            return false;
        }

        if (!expression()) {
            throw std::runtime_error("Se esperaba una expresión en la condición del 'for'.");
        }

        consume(TokenType::SEMICOLON, "Se esperaba ';' después de la condición del 'for'.");
        
        if (!exprStmt()) {
            return false;
        }

        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la condición del 'for'.");
        
        if (!statement()) {
            return false;
        }

        return true;
    }
    
    // ReturnStmt -> return Expression ;
    bool returnStmt() {
        if (!match(TokenType::KEYWORD_RETURN)) {
            return false;
        }

        if (!expression()) {
            throw std::runtime_error("Se esperaba una expresión después de 'return'.");
        }

        consume(TokenType::SEMICOLON, "Se esperaba ';' después de la expresión de retorno.");
        
        return true;
    }

    // PrintStmt -> print ( ExprList ) ;
    bool printStmt() {
        if (!match(TokenType::KEYWORD_PRINT)) {
            return false;
        }

        consume(TokenType::LEFT_PARENTHESIS, "Se esperaba '(' después de 'print'.");
        
        if (!exprList()) {
            return false;
        }

        consume(TokenType::RIGHT_PARENTHESIS, "Se esperaba ')' después de la lista de expresiones.");
        
        consume(TokenType::SEMICOLON, "Se esperaba ';' después de la instrucción 'print'.");
        
        return true;
    }
    
    // ExprStmt -> ; | Expression ;
    bool exprStmt() {
        if (match(TokenType::SEMICOLON)) {
            return true;
        }

        if (!expression()) {
            return false;
        }

        consume(TokenType::SEMICOLON, "Se esperaba ';' después de la expresión.");
        
        return true;
    }

    // StmtList -> Statement StmtListPrime
    bool stmtList() {
        if (!statement()) {
            return false;
        }

        return stmtListPrime();
    }   

    // StmtListPrime -> Statement StmtListPrime | epsilon
    bool stmtListPrime() {
        if (statement()) {
            return stmtListPrime();
        }

        return true;
    }
    
    /* Type -> IntType TypePrime
    Type -> BoolType TypePrime
    Type -> CharType TypePrime
    Type -> StringType TypePrime
    Type -> Void TypePrime */
    bool type() {
        if (match(TokenType::KEYWORD_INTEGER) || 
            match(TokenType::KEYWORD_BOOLEAN) ||
            match(TokenType::KEYWORD_CHAR) ||
            match(TokenType::KEYWORD_STRING) ||
            match(TokenType::KEYWORD_VOID)) {

            return typePrime();
        }

        return false;
    }

    // TypePrime -> [] TypePrime
    bool typePrime() {
        if (match(TokenType::LEFT_BRACKET)) {
            consume(TokenType::RIGHT_BRACKET, "Se esperaba ']' después de '['.");
            return typePrime();
        }

        return true;
    }

    /*
    Declaration -> Function
    Declaration -> VarDecl
    */
    bool declaration() {
        if (function()) {
            return true;
        }

        if (varDecl()) {
            return true;
        }

        return false;
    }

    /*
    Function -> Type Identifier ( Params ) { StmtList }
    */
    bool function() {
        if (type()) {
            if (match(TokenType::IDENTIFIER)) {
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
            throw std::runtime_error("Error en la declaración de la función.");
        }

        return false;
    }

    /*
    Params -> Type Identifier Params
    Params -> , Params
    Params -> epsilon
    */
    bool params() {
        if (type()) {
            if (match(TokenType::IDENTIFIER)) {
                if (params()) {
                    return true;
                }
                return false;
            }
            throw std::runtime_error("Se esperaba un identificador después del tipo.");
        }

        if (match(TokenType::COMMA)) {
            if (params()) {
                return true;
            }
            return false;
        }
        return true;
    }
    
    // VarDecl -> Type Identifier VarDeclPrime
    bool varDecl() {
        if (!type()) {
            return false;
        }

        if (!match(TokenType::IDENTIFIER)) {
            throw std::runtime_error("Se esperaba un identificador después del tipo.");
        }

        return varDeclPrime();
    }

    // VarDeclPrime -> ;
    // VarDeclPrime -> = Expression ;
    bool varDeclPrime() {
        if (match(TokenType::SEMICOLON)) {
            return true;
        }

        if (match(TokenType::OPERATOR_EQUAL)) {
            if (!expression()) {
                throw std::runtime_error("Se esperaba una expresión después del '='.");
            }

            consume(TokenType::SEMICOLON, "Se esperaba ';' al final de la declaración.");
            return true;
        }

        return false;
    }

};
