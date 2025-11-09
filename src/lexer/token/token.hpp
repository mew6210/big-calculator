#pragma once
#include <string>
enum class TokenType {
    openParen = 1,
    closeParen = 2,
    plusSign = 3,
    minusSign = 4,
    MultipSign = 5,
    DivSign = 6,
    numLiteral = 7,
    identifier = 8,
    assignOp = 9,
    undefined = 10
};

struct Token {
    TokenType type;
    std::string value;
    uint64_t startPos;
    uint64_t length;
};