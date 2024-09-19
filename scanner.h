#include <string>

#include "token.h"

//agregar una lista de TOKENS -> imprimir desde esa lista
//leer archivo y generar buffer de lectura
//SEPARATOR

class Scanner {
public:
    Scanner(const std::string& source) : source(source), pos(0), line(1), column(1) {}

    Token getToken() {
        skipWhitespace();
        if (isAtEOF()) return Token(TokenType::END_OF_FILE, "", line, column);

        char currentChar = peekChar();

        if (isAlpha(currentChar)) {
            return identifierOrKeyword();
        }

        if (isDigit(currentChar)) {
            return number();
        }

        if (isQuotationMark(currentChar)){
            return string();
        }

        if (operators.count(currentChar)) {
            return makeOperator();
        }

        if (currentChar == ':') {
            return Token(TokenType::COLON, std::string(1, getChar()), line, column - 1);
        }

        if (currentChar == ';') {
            return Token(TokenType::SEMICOLON, std::string(1, getChar()), line, column - 1);
        }

        if (currentChar == '=') {
            return Token(TokenType::OPERATOR_EQUAL, std::string(1, getChar()), line, column - 1);
        }

        // Si no se reconoce el carácter, es un error.
        return Token(TokenType::ERROR, std::string(1, getChar()), line, column++);
    }

    void scan() {
        Token token = getToken();
        while (token.type != TokenType::END_OF_FILE) {
            token.print();
            token = getToken();
        }
        token.print();
    }

  private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peekChar() {
        return source[pos];
    }

    char getChar() {
        pos++;
        column++;
        return source[pos - 1];
    }

    bool isAtEOF() const {
        return pos >= source.length();
    }

    void skipWhitespace() {
        while (!isAtEOF() && isWhitespace(peekChar())) {
            if (peekChar() == '\n') {
                line++;
                column = 1;
            }
            getChar();
        }
    }

    bool isAlpha(char c) const {
        return std::isalpha(c) || c == '_';
    }

    bool isQuotationMark(char c) const {
        return c == '\"';
    }

    bool isDigit(char c) const {
        return std::isdigit(c);
    }

    bool isAlnum(char c) const {
        return isAlpha(c) || isDigit(c);
    }

    bool isWhitespace(char c) const {
        return std::isspace(c);
    }

    Token identifierOrKeyword() {
        int startColumn = column;
        std::string lexeme;

        while (!isAtEOF() && isAlnum(peekChar())) {
            lexeme += getChar();
        }

        auto it = keywordMap.find(lexeme);
        if (it != keywordMap.end()) {
            return Token(it->second, lexeme, line, startColumn);
        }

        return Token(TokenType::IDENTIFIER, lexeme, line, startColumn);
    }


    Token number() {
        int startColumn = column;
        std::string lexeme;

        while (!isAtEOF() && isDigit(peekChar())) {
            lexeme += getChar();
        }

        return Token(TokenType::INT, lexeme, line, startColumn);
    }

    Token string() {
        int startColumn = column;
        std::string lexeme;

        getChar();

        while (!isAtEOF() && peekChar() != '\"') {
            char currentChar = getChar();

            if (currentChar == '\n') {
                return Token(TokenType::ERROR, "Unterminated string", line, startColumn);
            }

            // Manejar caracteres de escape
            if (currentChar == '\\') {
                if (!isAtEOF()) {
                    char nextChar = getChar();
                    switch (nextChar) {
                        case 'n':
                            lexeme += '\n';
                            break;
                        case 't':
                            lexeme += '\t';
                            break;
                        case '\\':
                        case '\"':
                            lexeme += nextChar;
                            break;
                        default:
                            lexeme += '\\';
                            lexeme += nextChar;
                            break;
                    }
                }
            } else {
                lexeme += currentChar;
            }
        }

        if (isAtEOF()) {
            return Token(TokenType::ERROR, "Unterminated string", line, startColumn);
        }

        getChar();
        // Returns only string content without quotation marks
        return Token(TokenType::STRING, lexeme, line, startColumn);
    }

    Token makeOperator() {
        char op = getChar();
        return Token(TokenType::OPERATOR, std::string(1, op), line, column - 1);
    }
};
