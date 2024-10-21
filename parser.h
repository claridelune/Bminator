#include <string>
#include <vector>
#include "Token.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    // Esto se llama desde el main
    void parse() {
        program();
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

    /* Declaration -> */
    bool declaration(){
        return 0;
    }

    /* ExprList -> */
    bool exprList(){
        return 0;
    }

    /* Expression -> */
    bool expression(){
        return 0;
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


};