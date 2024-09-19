#include <iostream>
#include <unordered_set>
#include <string>
#include <map>

enum class TokenType {
    IDENTIFIER,
    INT,
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
    OPERATOR,
    OPERATOR_EQUAL,
    END_OF_FILE,
    STRING,
    SEMICOLON,
    COLON,
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
    {"=", TokenType::OPERATOR_EQUAL}
};

const std::unordered_set<std::string> keywords = {
    "array", "boolean", "char", "else", "false", "for", "function", 
    "if", "integer", "map", "print", "return", "string", "true", 
    "void", "while"
};

const std::unordered_set<char> operators = {
    '+', '-', '*', '/', '<', '>', '!'
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
            {TokenType::KEYWORD_ARRAY, "RK_ARRAY"},
            {TokenType::KEYWORD_BOOLEAN, "RK_BOOLEAN"},
            {TokenType::KEYWORD_CHAR, "RK_CHAR"},
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
            {TokenType::OPERATOR, "OPERATOR"},
            {TokenType::OPERATOR_EQUAL, "OPERATOR_EQUAL"},
            {TokenType::END_OF_FILE, "END_OF_TOKENS"},
            {TokenType::STRING, "RK_STRING"},
            {TokenType::SEMICOLON, "SEMICOLON"},
            {TokenType::COLON, "COLON"},
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
