#ifndef SCANNER_H
#define SCANNER_H

#include <vector>
#include "token.h"

class Scanner {
    public:
        Scanner(const std::string& source);
        ~Scanner() = default;
        const std::vector<Token>& getTokens() const;
        Token getToken();
        void scan();

        void Consume(const TokenType tokenType);

    private:
        std::string source;
        size_t pos;
        int line;
        int column;
        std::vector<Token> tokens;

        static constexpr char DOUBLE_QUOTE = '\"';
        static constexpr char SINGLE_QUOTE = '\'';
        static constexpr char LEFT_BRACE = '{';
        static constexpr char RIGHT_BRACE = '}';
        static constexpr char LEFT_BRACKET = '[';
        static constexpr char RIGHT_BRACKET = ']';
        static constexpr char LEFT_PARENTHESIS = '(';
        static constexpr char RIGHT_PARENTHESIS = ')';
        static constexpr char COLON = ':';
        static constexpr char SEMICOLON = ';';
        static constexpr char COMMA = ',';
        static constexpr char EQUAL_SIGN = '=';

        inline char peekChar() const;
        inline char getChar();
        inline bool isAtEOF() const;
        inline bool isAlpha(char c) const;
        inline bool isQuotationMark(char c) const;
        inline bool isSingleQuote(char c) const;
        inline bool isDigit(char c) const;
        inline bool isAlnum(char c) const;
        inline bool isWhitespace(char c) const;
        inline bool isOperatorStart(char c) const;
        inline bool isDelimiter(char c) const;

        void skipWhitespaceAndComments();
        void skipComment();
        Token identifierOrKeyword();
        Token number();
        Token string();
        char handleEscapeSequence();
        Token character();
        Token extractOperator();
        Token extractDelimiter();
        Token handleSingleCharacterTokens(char currentChar);
        Token createEOFToken() const;
};

#endif
