#include <iostream>
#include <unordered_set>
#include <string>
#include <map>

enum class TokenType {
    IDENTIFIER,
    INT,
    CHAR,
    KEYWORD_ARRAY,
    KEYWORD_BOOLEAN,
    KEYWORD_CHAR,
    KEYWORD_ELSE,
    KEYWORD_FALSE,
    KEYWORD_FOR,
    KEYWORD_FUNCTION,
    KEYWORD_IF,
    KEYWORD_INTEGER,
    KEYWORD_MAP,
    KEYWORD_PRINT,
    KEYWORD_RETURN,
    KEYWORD_STRING,
    KEYWORD_TRUE,
    KEYWORD_VOID,
    KEYWORD_WHILE,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_NOT,
    OPERATOR_EQUAL,
    OPERATOR_EQUAL_EQUAL,
    OPERATOR_LESS_EQUAL,
    OPERATOR_GREATER_EQUAL,
    OPERATOR_NOT_EQUAL,
    OPERATOR_LESS_THAN,
    OPERATOR_GREATER_THAN,
    END_OF_FILE,
    STRING,
    SEMICOLON,
    COLON,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    ERROR
};

const std::map<std::string, TokenType> keywordMap = {
    {"array", TokenType::KEYWORD_ARRAY},
    {"boolean", TokenType::KEYWORD_BOOLEAN},
    {"char", TokenType::KEYWORD_CHAR},
    {"else", TokenType::KEYWORD_ELSE},
    {"false", TokenType::KEYWORD_FALSE},
    {"for", TokenType::KEYWORD_FOR},
    {"function", TokenType::KEYWORD_FUNCTION},
    {"if", TokenType::KEYWORD_IF},
    {"integer", TokenType::KEYWORD_INTEGER},
    {"map", TokenType::KEYWORD_MAP},
    {"print", TokenType::KEYWORD_PRINT},
    {"return", TokenType::KEYWORD_RETURN},
    {"string", TokenType::KEYWORD_STRING},
    {"true", TokenType::KEYWORD_TRUE},
    {"void", TokenType::KEYWORD_VOID},
    {"while", TokenType::KEYWORD_WHILE},

    // operadores
    {"!", TokenType::OPERATOR_NOT},
    {"+", TokenType::OPERATOR_PLUS},
    {"-", TokenType::OPERATOR_MINUS},
    {"*", TokenType::OPERATOR_MULTIPLY},
    {"/", TokenType::OPERATOR_DIVIDE},

    {"=", TokenType::OPERATOR_EQUAL},
    {"==", TokenType::OPERATOR_EQUAL_EQUAL},
    {"<=", TokenType::OPERATOR_LESS_EQUAL},
    {">=", TokenType::OPERATOR_GREATER_EQUAL},
    {"!=", TokenType::OPERATOR_NOT_EQUAL},
    {"<", TokenType::OPERATOR_LESS_THAN},
    {">", TokenType::OPERATOR_GREATER_THAN},

    // delimitadores
    {";", TokenType::SEMICOLON},
    {":", TokenType::COLON},
    {"{", TokenType::LEFT_BRACE},
    {"}", TokenType::RIGHT_BRACE},
    {"[", TokenType::LEFT_BRACKET},
    {"]", TokenType::RIGHT_BRACKET},
    {"(", TokenType::LEFT_PARENTHESIS},
    {")", TokenType::RIGHT_PARENTHESIS},

};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
        
    void print() const {
        std::cout << "TOKEN VALUE: '" << value << "' ";
        static std::map<TokenType, std::string> tokenTypeMap = {
            {TokenType::IDENTIFIER, "IDENTIFIER"},
            {TokenType::INT, "RK_INT"},
            {TokenType::CHAR, "RK_CHAR"},
            {TokenType::KEYWORD_ARRAY, "RK_ARRAY"},
            {TokenType::KEYWORD_BOOLEAN, "RK_BOOLEAN"},
            {TokenType::KEYWORD_CHAR, "RK_KEYWORD_CHAR"},
            {TokenType::KEYWORD_ELSE, "RK_ELSE"},
            {TokenType::KEYWORD_FALSE, "RK_FALSE"},
            {TokenType::KEYWORD_FOR, "RK_FOR"},
            {TokenType::KEYWORD_FUNCTION, "RK_FUNCTION"},
            {TokenType::KEYWORD_IF, "RK_IF"},
            {TokenType::KEYWORD_INTEGER, "RK_INTEGER"},
            {TokenType::KEYWORD_MAP, "RK_MAP"},
            {TokenType::KEYWORD_PRINT, "RK_PRINT"},
            {TokenType::KEYWORD_RETURN, "RK_RETURN"},
            {TokenType::KEYWORD_STRING, "RK_STRING"},
            {TokenType::KEYWORD_TRUE, "RK_TRUE"},
            {TokenType::KEYWORD_VOID, "RK_VOID"},
            {TokenType::KEYWORD_WHILE, "RK_WHILE"},

            {TokenType::OPERATOR_NOT, "OPERATOR_NOT"},
            {TokenType::OPERATOR_PLUS, "OPERATOR_PLUS"},
            {TokenType::OPERATOR_MINUS, "OPERATOR_MINUS"},
            {TokenType::OPERATOR_MULTIPLY, "OPERATOR_MULTIPLY"},
            {TokenType::OPERATOR_DIVIDE, "OPERATOR_DIVIDE"},

            {TokenType::OPERATOR_EQUAL, "OPERATOR_EQUAL"},
            {TokenType::OPERATOR_EQUAL_EQUAL, "OPERATOR_EQUAL_EQUAL"},
            {TokenType::OPERATOR_LESS_EQUAL, "OPERATOR_LESS_EQUAL"},
            {TokenType::OPERATOR_GREATER_EQUAL, "OPERATOR_GREATER_EQUAL"},
            {TokenType::OPERATOR_LESS_THAN, "OPERATOR_LESS_THAN"},
            {TokenType::OPERATOR_GREATER_THAN, "OPERATOR_GREATER_THAN"},
            {TokenType::OPERATOR_NOT_EQUAL, "OPERATOR_NOT_EQUAL"},

            {TokenType::END_OF_FILE, "END_OF_TOKENS"},
            {TokenType::STRING, "RK_STRING"},
            {TokenType::SEMICOLON, "SEMICOLON"},
            {TokenType::COLON, "COLON"},
            {TokenType::LEFT_BRACE, "LEFT_BRACE"},
            {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
            {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
            {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
            {TokenType::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"},
            {TokenType::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"},
            {TokenType::ERROR, "ERROR"}
        };

        auto it = tokenTypeMap.find(type);
        if (it != tokenTypeMap.end()) {
            std::cout << "TOKEN TYPE: " << it->second << " (Line " << line << ", Column " << column << ")\n";
        } else {
            std::cout << "UNKNOWN TOKEN TYPE (Line " << line << ", Column " << column << ")\n";
        }
    }
};
