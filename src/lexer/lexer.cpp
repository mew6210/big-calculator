#include "lexer.hpp"
#include <iostream>
#include <unordered_map>


std::unordered_map<char,TokenType> singleOpsToEnumMap = {
    {'(',TokenType::openParen},
    {')',TokenType::closeParen},
    {'+',TokenType::addOp},
    {'-',TokenType::SubtrOp},
    {'*',TokenType::MultipOp},
    {'/',TokenType::DivOp},
    {'=',TokenType::assignOp}
};

bool isSpace(const char& c){

    if(c == ' ' || c == '\n' || c == '\r' || c == '\t') return true;
    else return false;

}

bool isSingleCharInstruction(const char& c){

    if(
        singleOpsToEnumMap.count(c)
    ) return true;
    else return false;

}

bool isDigit(const char& c){

    if(
        c == '0' ||
        c == '1' ||
        c == '2' ||
        c == '3' ||
        c == '4' ||
        c == '5' ||
        c == '6' ||
        c == '7' ||
        c == '8' ||
        c == '9'
    )
    return true;
    else return false'

}

void Lexer::handleMultipleCharInstruction(){

    if(source[cur_index])

}

Token Lexer::parseToken(){

    //skip spaces
    while(isSpace(source[cur_index])){
        cur_index++;
    }

    if(isSingleCharInstruction(source[cur_index])){
        Token token = Token{singleOpsToEnumMap[source[cur_index]]," "};
        cur_index++;
        return token;
    }
    else{
        handleMultipleCharInstruction();
    }


    return Token{TokenType::varName,"  "};
}

std::vector<Token> Lexer::parseTokens(){

    while(cur_index != source.size()){
    auto token = parseToken();
    tokens.push_back(token);
    }
    return {};
}

void Lexer::getTokensFromString(const std::string& s){

    source = s;
    parseTokens();

}

void Lexer::printTokens(){

    for(auto& token: tokens){

        switch(token.type){

        case TokenType::openParen:  std::cout<<" open paren\n";break;
        case TokenType::closeParen: std::cout<<" close paren\n";break;
        case TokenType::addOp:      std::cout<<" addition\n";break;
        case TokenType::SubtrOp:    std::cout<<" subtraction\n";break;
        case TokenType::MultipOp:   std::cout<<" multiplication\n";break;
        case TokenType::DivOp:      std::cout<<" division\n";break;
        case TokenType::numLiteral: std::cout<<" some number\n";break;
        case TokenType::varName:    std::cout<<" some variable\n";break;
        case TokenType::assignOp:   std::cout<<" assign operation\n";break;



        }

    }


}