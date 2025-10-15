#include "lexer.hpp"
#include <iostream>
#include <unordered_map>


std::unordered_map<char,TokenType> singleOpsToEnumMap = {
    {'(',TokenType::openParen},
    {')',TokenType::closeParen},
    {'+',TokenType::plusSign},
    {'-',TokenType::minusSign},
    {'*',TokenType::MultipSign},
    {'/',TokenType::DivSign},
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

    if (
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
    else return false;
}

Token Lexer::handleNumberLiteralToken(const std::string& tok,const uint64_t& startPos) {

    //check if everything is a digit
    for (auto& c : tok) {
        if (!isDigit(c)) {
            throw std::runtime_error("not every character in a number literal are digits");
        }
    }
    
    return Token{ TokenType::numLiteral, tok,startPos,tok.size()};
}

Token Lexer::handleIdentifierToken(const std::string& tok,const uint64_t& startPos) {
    return Token{ TokenType::identifier,tok,startPos,tok.size()};
}

Token Lexer::handleMultipleCharInstruction() {

    std::string nameBuf = "";
    uint64_t startingPos = cur_index;
    //load name into buffer
    while (!isSingleCharInstruction(source[cur_index]) && !isSpace(source[cur_index]) && cur_index != source.size()) {
        nameBuf += source[cur_index];
        cur_index++;
    }

    //if last char was a single instruction, then go back it needs to be evaluated, not skipped
    if (isSingleCharInstruction(source[cur_index])) {
        cur_index--;
    }

    if (isDigit(nameBuf[0])) {
        return handleNumberLiteralToken(nameBuf,startingPos);
    }
    else {
        return handleIdentifierToken(nameBuf,startingPos);
    }
}

Token Lexer::parseToken(){

    //skip spaces
    while(isSpace(source[cur_index])){
        cur_index++;
    }

    if(isSingleCharInstruction(source[cur_index])){
        Token token = Token{singleOpsToEnumMap[source[cur_index]]," ",cur_index,1};
        cur_index++;
        return token;
    }
    else{
        Token token = handleMultipleCharInstruction();
        cur_index++;
        return token;
    }
}

void Lexer::parseTokens(){

    while(cur_index < source.size()){
        auto token = parseToken();
        tokens.push_back(token);
    }
}

std::vector<Token> Lexer::getTokensFromString(const std::string& s){
    tokens.clear();
    cur_index = 0;
    source = s;
    parseTokens();
    return tokens;
}



void Lexer::printTokens(){

    for(auto& token: tokens){
        switch(token.type){
        case TokenType::openParen:  std::cout << "open paren"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";        break;
        case TokenType::closeParen: std::cout << "close paren"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";       break;
        case TokenType::plusSign:   std::cout << "addition"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";          break;
        case TokenType::minusSign:  std::cout << "subtraction"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";       break;
        case TokenType::MultipSign: std::cout << "multiplication"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";    break;
        case TokenType::DivSign:    std::cout << "division"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";          break;
        case TokenType::numLiteral: std::cout << "some number with a value of: "<<token.value<<""<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";       break;
        case TokenType::identifier: std::cout << "some variable named: "<<token.value<<""<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";     break;
        case TokenType::assignOp:   std::cout << "assign operation"<<" position: "<<token.startPos<<" length: "<<token.length<<"\n";  break;
        case TokenType::undefined:  std::cout << "I DONT KNOW T_T\n";   break;
        }
    }
}