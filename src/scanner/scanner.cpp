#include "scanner.h"
#include "token.h"
#include <cctype>

Scanner::Scanner(const std::string& source) : source(source), pos(0), line(1), column(1) {}

const std::vector<Token>& Scanner::getTokens() const {
    return tokens;
}

Token Scanner::getToken() {
    skipWhitespaceAndComments();
    if (isAtEOF()) return createEOFToken();

    char currentChar = peekChar();

    if (isAlpha(currentChar)) return identifierOrKeyword();
    if (isDigit(currentChar)) return number();
    if (isQuotationMark(currentChar)) return string();
    if (isSingleQuote(currentChar)) return character();
    if (isOperatorStart(currentChar)) return extractOperator();
    if (isDelimiter(currentChar)) return extractDelimiter();

    return handleSingleCharacterTokens(currentChar);
}

void Scanner::scan() {
    Token token = getToken();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = getToken();
    }
    tokens.push_back(token);
}

// Métodos auxiliares pequeños marcados como inline
inline char Scanner::peekChar() const {
    return source[pos];
}

inline char Scanner::getChar() {
    pos++;
    column++;
    return source[pos - 1];
}

inline bool Scanner::isAtEOF() const {
    return pos >= source.length();
}

inline bool Scanner::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

inline bool Scanner::isQuotationMark(char c) const {
    return c == DOUBLE_QUOTE;
}

inline bool Scanner::isSingleQuote(char c) const {
    return c == SINGLE_QUOTE;
}

inline bool Scanner::isDigit(char c) const {
    return std::isdigit(c);
}

inline bool Scanner::isAlnum(char c) const {
    return isAlpha(c) || isDigit(c);
}

inline bool Scanner::isWhitespace(char c) const {
    return std::isspace(c);
}

inline bool Scanner::isOperatorStart(char c) const {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '!' || c == '%' || c == '|' || c == '&';
}

inline bool Scanner::isDelimiter(char c) const {
    return c == LEFT_BRACE || c == RIGHT_BRACE || c == LEFT_BRACKET || c == RIGHT_BRACKET ||
           c == LEFT_PARENTHESIS || c == RIGHT_PARENTHESIS;
}

void Scanner::skipWhitespaceAndComments() {
    while (!isAtEOF()) {
        char currentChar = peekChar();
        if (isWhitespace(currentChar)) {
            if (currentChar == '\n') {
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

void Scanner::skipComment() {
    while (!isAtEOF() && peekChar() != '\n') {
        getChar();
    }
}

Token Scanner::identifierOrKeyword() {
    int startColumn = column;
    size_t start = pos;

    while (!isAtEOF() && isAlnum(peekChar())) {
        getChar();
    }

    std::string lexeme(source.substr(start, pos - start));

    auto it = tokensMap.find(lexeme);
    if (it != tokensMap.end()) {
        return Token(it->second, lexeme, line, startColumn);
    }

    return Token(TokenType::IDENTIFIER, lexeme, line, startColumn);
}

Token Scanner::number() {
    int startColumn = column;
    size_t start = pos;

    while (!isAtEOF() && isDigit(peekChar())) {
        getChar();
    }

    std::string lexeme(source.substr(start, pos - start));
    return Token(TokenType::LITERAL_INT, lexeme, line, startColumn);
}

Token Scanner::string() {
    int startColumn = column;
    std::string lexeme;

    getChar(); // Consume the initial quote

    while (!isAtEOF() && peekChar() != DOUBLE_QUOTE) {
        char currentChar = getChar();

        if (currentChar == '\n') {
            return Token(TokenType::ERROR, "Unterminated string", line, startColumn);
        }

        if (currentChar == '\\') {
            lexeme += handleEscapeSequence();
        } else {
            lexeme += currentChar;
        }
    }

    if (isAtEOF()) {
        return Token(TokenType::ERROR, "Unterminated string", line, startColumn);
    }

    getChar(); // Consume the closing quote
    return Token(TokenType::LITERAL_STRING, lexeme, line, startColumn);
}

char Scanner::handleEscapeSequence() {
    char nextChar = getChar();
    switch (nextChar) {
        case 'n': return '\n';
        case 't': return '\t';
        case '\\': case DOUBLE_QUOTE: case SINGLE_QUOTE: return nextChar;
        default: return '\\';
    }
}

Token Scanner::character() {
    int startColumn = column;
    getChar(); // Consume the initial single quote
    char charValue;

    if (peekChar() == '\\') {
        getChar();
        charValue = handleEscapeSequence();
    } else {
        charValue = getChar();
    }

    if (peekChar() != SINGLE_QUOTE) {
        return Token(TokenType::ERROR, "Unterminated character literal", line, startColumn);
    }
    getChar(); // Consume the closing single quote
    
    return Token(TokenType::LITERAL_CHAR, std::string(1, charValue), line, startColumn);
}

Token Scanner::extractOperator() {
    int startColumn = column;
    char firstChar = getChar();
    std::string lexeme(1, firstChar);
    if (!isAtEOF() && (firstChar == '=' || firstChar == '<' || firstChar == '>' || firstChar == '!') && peekChar() == '=') {
        lexeme += getChar();
    } else if (!isAtEOF() && firstChar == '|' && peekChar() == '|') {
        lexeme += getChar();
    } else if (!isAtEOF() && firstChar == '&' && peekChar() == '&') {
        lexeme += getChar();
    }

    auto it = tokensMap.find(lexeme);
    if (it != tokensMap.end()) {
        return Token(it->second, lexeme, line, startColumn);
    }

    return Token(TokenType::ERROR, lexeme, line, startColumn);
}

Token Scanner::extractDelimiter() {
    char currentChar = getChar();
    switch (currentChar) {
        case LEFT_BRACE: return Token(TokenType::LEFT_BRACE, "{", line, column - 1);
        case RIGHT_BRACE: return Token(TokenType::RIGHT_BRACE, "}", line, column - 1);
        case LEFT_BRACKET: return Token(TokenType::LEFT_BRACKET, "[", line, column - 1);
        case RIGHT_BRACKET: return Token(TokenType::RIGHT_BRACKET, "]", line, column - 1);
        case LEFT_PARENTHESIS: return Token(TokenType::LEFT_PARENTHESIS, "(", line, column - 1);
        case RIGHT_PARENTHESIS: return Token(TokenType::RIGHT_PARENTHESIS, ")", line, column - 1);
        default:  return Token(TokenType::ERROR, std::string(1, currentChar), line, column - 1);
    }
}

Token Scanner::handleSingleCharacterTokens(char currentChar) {
    switch (currentChar) {
        case COLON: return Token(TokenType::COLON, std::string(1, getChar()), line, column - 1);
        case SEMICOLON: return Token(TokenType::SEMICOLON, std::string(1, getChar()), line, column - 1);
        case COMMA: return Token(TokenType::COMMA, std::string(1, getChar()), line, column - 1);
        case EQUAL_SIGN: return Token(TokenType::OPERATOR_EQUAL, std::string(1, getChar()), line, column - 1);
        default: return Token(TokenType::ERROR, std::string(1, getChar()), line, column++);
    }
}

Token Scanner::createEOFToken() const {
    return Token(TokenType::END_OF_FILE, "", line, column);
}
