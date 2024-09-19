#include <iostream>
#include <unordered_set>
#include <string>

enum class TokenType {
    IDENTIFIER,
    INT,
    KEYWORD,
    OPERATOR,
    END_OF_FILE,
    STRING,
    SEMICOLON,
    COLON,
    ERROR
};

const std::unordered_set<std::string> keywords = {
    "array", "boolean", "char", "else", "false", "for", "function", 
    "if", "integer", "map", "print", "return", "string", "true", 
    "void", "while"
};

const std::unordered_set<char> operators = {
    '+', '-', '*', '/', '=', '<', '>', '!'
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
        switch (type) {
            case TokenType::INT:
                std::cout << " TOKEN TYPE: INT"<< " (Line " << line << ", Column " << column << ")\n";
                break;
            case TokenType::IDENTIFIER:
                std::cout << " TOKEN TYPE: IDENTIFIER"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::KEYWORD:
                std::cout << " TOKEN TYPE: KEYWORD"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            /* case TokenType::SEPARATOR:
                std::cout << "TOKEN TYPE: SEPARATOR\n";
                break; */
            case TokenType::OPERATOR:
                std::cout << " TOKEN TYPE: OPERATOR"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::STRING:
            // If string contains \n it will go to next line xd
                std::cout << " TOKEN TYPE: STRING"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::SEMICOLON:
                std::cout << " TOKEN TYPE: SEMICOLON"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::COLON:
                std::cout << " TOKEN TYPE: COLON"<< " (Line " <<line << ", Column " << column << ")\n";
                break;          
            case TokenType::END_OF_FILE:
                std::cout << " END OF TOKENS"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::ERROR:
                std::cout << " ERROR"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
        }
    }
};
