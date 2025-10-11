#include <string>
enum class TokenType {
    openParen = 1,
    closeParen = 2,
    addOp = 3,
    SubtrOp = 4,
    MultipOp = 5,
    DivOp = 6,
    numLiteral = 7,
    identifier = 8,
    assignOp = 9,
    undefined = 10
};

struct Token {
    TokenType type;
    std::string value;
};