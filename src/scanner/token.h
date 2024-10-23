#ifndef TOKEN_H
#define TOKEN_H
#include <string>
#include <unordered_map>

#include "../utils/logger.h"

enum class TokenType {
    IDENTIFIER,

    // Literals
    LITERAL_INT,
    LITERAL_CHAR,
    LITERAL_STRING,

    // Keywords
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

    // Operators
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MOD,

    OPERATOR_EQUAL,
    OPERATOR_NOT_EQUAL,
    OPERATOR_LESS_THAN,
    OPERATOR_GREATER_THAN,
    OPERATOR_LESS_EQUAL,
    OPERATOR_GREATER_EQUAL,

    OPERATOR_AND,
    OPERATOR_OR,
    OPERATOR_NOT,

    OPERATOR_ASSIGN,

    // Varied
    SEMICOLON,
    COMMA,
    COLON,

    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,

    END_OF_FILE,

    ERROR, // borrar
};

static std::unordered_map<std::string, TokenType> tokensMap = {
    // Keywords --------------------------------------------
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

    // Operators --------------------------------------------
    {"+", TokenType::OPERATOR_PLUS},
    {"-", TokenType::OPERATOR_MINUS},
    {"*", TokenType::OPERATOR_MULTIPLY},
    {"/", TokenType::OPERATOR_DIVIDE},
    {"%", TokenType::OPERATOR_MOD},

    {"==", TokenType::OPERATOR_EQUAL},
    {"!=", TokenType::OPERATOR_NOT_EQUAL},
    {"<", TokenType::OPERATOR_LESS_THAN},
    {">", TokenType::OPERATOR_GREATER_THAN},
    {"<=", TokenType::OPERATOR_LESS_EQUAL},
    {">=", TokenType::OPERATOR_GREATER_EQUAL},
    
    { "&&", TokenType::OPERATOR_AND},
    { "||", TokenType::OPERATOR_OR},
    { "!", TokenType::OPERATOR_NOT},

    { "=", TokenType::OPERATOR_ASSIGN},

    // Varied
    {";", TokenType::SEMICOLON},
    {":", TokenType::COLON},
    {",", TokenType::COMMA},

    {"{", TokenType::LEFT_BRACE},
    {"}", TokenType::RIGHT_BRACE},
    {"[", TokenType::LEFT_BRACKET},
    {"]", TokenType::RIGHT_BRACKET},
    {"(", TokenType::LEFT_PARENTHESIS},
    {")", TokenType::RIGHT_PARENTHESIS},

    {"\032", TokenType::END_OF_FILE},
};

static const std::unordered_map<TokenType, std::string> tokenTypeMap = {
    {TokenType::IDENTIFIER, "IDENTIFIER"},

    // Literals
    {TokenType::LITERAL_INT, "INT_LITERAL"},
    {TokenType::LITERAL_CHAR, "CHAR_LITERAL"},
    {TokenType::LITERAL_STRING, "STRING_LITERAL"},

    // Keywords
    {TokenType::KEYWORD_ARRAY, "KEYWORD_ARRAY"},
    {TokenType::KEYWORD_BOOLEAN, "KEYWORD_BOOLEAN"},
    {TokenType::KEYWORD_CHAR, "KEYWORD_CHAR"},
    {TokenType::KEYWORD_ELSE, "KEYWORD_ELSE"},
    {TokenType::KEYWORD_FALSE, "KEYWORD_FALSE"},
    {TokenType::KEYWORD_FOR, "KEYWORD_FOR"},
    {TokenType::KEYWORD_FUNCTION, "KEYWORD_FUNCTION"},
    {TokenType::KEYWORD_IF, "KEYWORD_IF"},
    {TokenType::KEYWORD_INTEGER, "KEYWORD_INTEGER"},
    {TokenType::KEYWORD_MAP, "KEYWORD_MAP"},
    {TokenType::KEYWORD_PRINT, "KEYWORD_PRINT"},
    {TokenType::KEYWORD_RETURN, "KEYWORD_RETURN"},
    {TokenType::KEYWORD_STRING, "KEYWORD_STRING"},
    {TokenType::KEYWORD_TRUE, "KEYWORD_TRUE"},
    {TokenType::KEYWORD_VOID, "KEYWORD_VOID"},
    {TokenType::KEYWORD_WHILE, "KEYWORD_WHILE"},

    // Operators
    {TokenType::OPERATOR_PLUS, "OPERATOR_PLUS"},
    {TokenType::OPERATOR_MINUS, "OPERATOR_MINUS"},
    {TokenType::OPERATOR_MULTIPLY, "OPERATOR_MULTIPLY"},
    {TokenType::OPERATOR_DIVIDE, "OPERATOR_DIVIDE"},
    {TokenType::OPERATOR_MOD, "OPERATOR_MOD"},

    {TokenType::OPERATOR_EQUAL, "OPERATOR_EQUAL"},
    {TokenType::OPERATOR_NOT_EQUAL, "OPERATOR_NOT_EQUAL"},
    {TokenType::OPERATOR_LESS_THAN, "OPERATOR_LESS_THAN"},
    {TokenType::OPERATOR_GREATER_THAN, "OPERATOR_GREATER_THAN"},
    {TokenType::OPERATOR_LESS_EQUAL, "OPERATOR_LESS_EQUAL"},
    {TokenType::OPERATOR_GREATER_EQUAL, "OPERATOR_GREATER_EQUAL"},

    {TokenType::OPERATOR_AND, "OPERATOR_AND"},
    {TokenType::OPERATOR_OR, "OPERATOR_OR"},
    {TokenType::OPERATOR_NOT, "OPERATOR_NOT"},

    {TokenType::OPERATOR_ASSIGN, "OPERATOR_ASSIGN"},

    // Varied
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::COLON, "COLON"},

    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
    {TokenType::LEFT_PARENTHESIS, "LEFT_PARENTHESIS"},
    {TokenType::RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"},

    {TokenType::END_OF_FILE, "END_OF_FILE"},
    {TokenType::ERROR, "ERROR"},
};

class Token {
    public:
        TokenType type;
        std::string value;
        int line;
        int column;

        Token(TokenType type, const std::string& value, int line, int column)
            : type(type), value(value), line(line), column(column) {}
        ~Token() = default;

        void print() const {
            Logger& logger = Logger::getInstance();
            
            std::string tokenTypeName = tokenTypeMap.at(type);

            std::string message = "TOKEN VALUE: '" + value + "' TOKEN TYPE: " + tokenTypeName +
                                  " (Line " + std::to_string(line) + ", Column " + std::to_string(column) + ")";
            
            logger.info(message);
        }
};

#endif

