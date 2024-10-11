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
        return current >= tokens.size() || tokens[current].type == TokenType::EOF_TOKEN;
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
    bool program(){
        declaration();
        programPrime();
    }

};