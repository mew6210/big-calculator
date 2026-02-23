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

    ifKeyword,
    elseKeyword,

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

