#pragma once
#include <string>
enum class TokenType {
    openParen ,
    closeParen ,
    comma,
    openCurl,
    closeCurl,
    semiColon,

    plusSign ,
    minusSign ,
    multipSign ,
    divSign ,
    assignOp ,
    smallerThan,
    biggerThan,
    smallerOrEqualThan,
    biggerOrEqualThan,
    andOp,
    orOp,

    ifKeyword,
    elseKeyword,
    whileKeyword,
    equalKeyword,
    notEqualKeyword,
    returnKeyword,

    numLiteral,
    identifier,
    
    undefined ,
    tokEOF
};

struct Token {
    TokenType type;
    std::string value;
    uint64_t startPos;
    uint64_t length;
};

