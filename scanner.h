#include <string>
#include <vector>

#include "token.h"

class Scanner {
public:
    Scanner(const std::string& source) : source(source), pos(0), line(1), column(1) {}

    const std::vector<Token>& getTokens() const {
        return tokens;
    }

    Token getToken() {
        skipWhitespaceAndComments();
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

        if (isSingleQuote(currentChar)) {
            return character();
        }

        if (isOperatorStart(currentChar)) {
            return extractOperator();
        }

        if (isDelimiter(currentChar)) {
            return extractDelimiter();
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

        if (currentChar == '%') {
            return Token(TokenType::OPERATOR_MOD, std::string(1, getChar()), line, column - 1);
        }

        if (currentChar == ',') {
            return Token(TokenType::COMMA, std::string(1, getChar()), line, column - 1);
        }

        // Si no se reconoce el carácter, es un error.
        return Token(TokenType::ERROR, std::string(1, getChar()), line, column++);
    }

    void scan() {
        Token token = getToken();
        while (token.type != TokenType::END_OF_FILE) {
            tokens.push_back(token);
            token = getToken();
        }
        tokens.push_back(token);
    }

  private:
    std::string source;
    size_t pos;
    int line;
    int column;
    std::vector<Token> tokens;

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

    void skipWhitespaceAndComments() {
        while (!isAtEOF()) {
            char currentChar = peekChar();
            if (isWhitespace(currentChar)) {
                if (peekChar() == '\n') {
                    line++;
                    column = 1;
                }
                getChar();
            } else if (currentChar == '/' && source[pos + 1] == '/') {
                skipComment();
            }
            else {
                break;
            }
        }
    }

    void skipComment() {
        while (!isAtEOF() && peekChar() != '\n') {
            getChar();
        }
    }

    bool isAlpha(char c) const {
        return std::isalpha(c) || c == '_';
    }

    bool isQuotationMark(char c) const {
        return c == '\"';
    }

    bool isSingleQuote(char c) const {
        return c == '\'';
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

    bool isOperatorStart(char c) const {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '!' || c == '|' || c == '&';
    }

    bool isDelimiter(char c) const {
        return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')';
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

        return Token(TokenType::LITERAL_INT, lexeme, line, startColumn);
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
                lexeme += handleEscapeSequence();
            } else {
                lexeme += currentChar;
            }
        }

        if (isAtEOF()) {
            return Token(TokenType::ERROR, "Unterminated string", line, startColumn);
        }

        getChar();
        // Returns only string content without quotation marks
        return Token(TokenType::LITERAL_STRING, lexeme, line, startColumn);
    }
    
    char handleEscapeSequence() {
        char nextChar = getChar();
        switch (nextChar) {
            case 'n': return '\n';
            case 't': return '\t';
            case '\\': case '\"': case '\'': return nextChar;
            default: return '\\';
        }
    }

    Token character() {
        int startColumn = column;
        getChar();
        char charValue;

        if (peekChar() == '\\') {
            getChar();
            charValue = handleEscapeSequence();
        } else {
            charValue = getChar();
        }

        if (peekChar() != '\'') {
            return Token(TokenType::ERROR, "Unterminated character literal", line, startColumn);
        }
        getChar();
        
        return Token(TokenType::LITERAL_CHAR, std::string(1, charValue), line, startColumn);
    }

    Token extractOperator() {
        int startColumn = column;
        char firstChar = getChar();
        std::string lexeme(1, firstChar);
        if (!isAtEOF() && (firstChar == '=' || firstChar == '<' || firstChar == '>' || firstChar == '!' || firstChar == '|' || firstChar == '&') && peekChar() == '=') {
            lexeme += getChar();
        }

        auto it = keywordMap.find(lexeme);
        if (it != keywordMap.end()) {
            return Token(it->second, lexeme, line, startColumn);
        }

        return Token(TokenType::ERROR, lexeme, line, startColumn);
    }

    Token extractDelimiter() {
        char currentChar = getChar();
        switch (currentChar) {
            case '{': return Token(TokenType::LEFT_BRACE, "{", line, column - 1);
            case '}': return Token(TokenType::RIGHT_BRACE, "}", line, column - 1);
            case '[': return Token(TokenType::LEFT_BRACKET, "[", line, column - 1);
            case ']': return Token(TokenType::RIGHT_BRACKET, "]", line, column - 1);
            case '(': return Token(TokenType::LEFT_PARENTHESIS, "(", line, column - 1);
            case ')': return Token(TokenType::RIGHT_PARENTHESIS, ")", line, column - 1);
            default:  return Token(TokenType::ERROR, std::string(1, currentChar), line, column - 1);
        }
    }
};
