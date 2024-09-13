#include <iostream>
#include <unordered_set>
#include <string>

enum class TokenType {
    IDENTIFIER,
    INT,
    KEYWORD,
    OPERATOR,
    END_OF_FILE,
    ERROR
};

const std::unordered_set<std::string> keywords = {
    "if", "else", "while", "return", "int", "float", "void"
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
            case TokenType::END_OF_FILE:
                std::cout << " END OF TOKENS"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
            case TokenType::ERROR:
                std::cout << " ERROR"<< " (Line " <<line << ", Column " << column << ")\n";
                break;
        }
    }
};
