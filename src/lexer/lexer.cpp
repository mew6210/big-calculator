#include "lexer.hpp"
#include <iostream>
#include <unordered_map>
#include "../logging/logging.hpp"

/*
    @brief all tokens that are single chars
*/
std::unordered_map<char,TokenType> singleOpsToEnumMap = {
    {'(',TokenType::openParen},
    {')',TokenType::closeParen},
    {'+',TokenType::plusSign},
    {'-',TokenType::minusSign},
    {'*',TokenType::multipSign},
    {'/',TokenType::divSign},
    {'=',TokenType::assignOp},
    {',',TokenType::comma},
    {'^',TokenType::exponentSign}
};

/*
    @brief functions that are used only by lexer
*/
namespace {

    bool isSpace(const char& c) {

        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') return true;
        else return false;
    }

    bool isSingleCharInstruction(const char& c) {

        if (
            singleOpsToEnumMap.count(c)
            ) return true;
        else return false;
    }

    bool isDigit(const char& c) {

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

    /*
        @brief returns 1 if minus sign represents a negative sign, and 0 if it represents subtraction
    */
    bool isNegativeSign(const char cur_index, const TokenType& lastTok) { 
        return singleOpsToEnumMap[cur_index] == TokenType::minusSign && (lastTok != TokenType::numLiteral && lastTok != TokenType::identifier);
    }
}

/*
    @brief converts strings like `5` to their respective numLiteral tokens

    checks if inside a number there are not-digit chars, and displays an error if there are
*/
Token Lexer::handleNumberLiteralToken(const std::string& tok,const uint64_t& startPos) {
    uint64_t errorPos = startPos;
    //check if everything is a digit
    for (auto& c : tok) {
        if (!isDigit(c)) {
            if (c != '.' && c != ',') {
                printError(ErrMsg{ 
                    source, 
                    errorPos, 
                    "Found a character in a digit, invalid number literal",
                    "Fix the number",
                    ErrType::Lexer
                    });
            }
            else {
                printError(ErrMsg{ 
                    source, 
                    errorPos, 
                    "Found a comma or a dot, this calculator only supports integers",
                    "Use whole numbers only",
                    ErrType::Lexer
                    });
            }
        }
        errorPos++;
    }
    
    return Token{ TokenType::numLiteral, tok,startPos,tok.size()};
}

/*
    @brief converts identifiers to their respective identifier tokens
*/
Token Lexer::handleIdentifierToken(const std::string& tok,const uint64_t& startPos) {
    return Token{ TokenType::identifier,tok,startPos,tok.size()};
}

/*
    @brief handles any strings that arent single-char tokens

    if it starts with a digit, then lexes it as if it was a number literal

    otherwise handles it as an identifier
*/
Token Lexer::handleMultipleCharInstruction() {

    std::string nameBuf = "";
    uint64_t startingPos = cur_index;
    //load name into nameBuf
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

Token Lexer::handleSingleCharInstruction() {
    if (isNegativeSign(source[cur_index], lastTokenType)) {  //if its a -, and previous token wasnt a number then it indicates a negativeness of a number, not subtraction
        cur_index++;    //eat -
        Token token = handleMultipleCharInstruction();
        token.value.insert(0, 1, '-'); //add - at the beginning of a number
        cur_index++;
        lastTokenType = token.type;
        return token;
    }
    else {

        Token token = Token{ singleOpsToEnumMap[source[cur_index]],std::string{source[cur_index]},cur_index,1 };
        cur_index++;
        lastTokenType = token.type;
        return token;
    }

}

Token Lexer::parseToken(){

    //skip spaces
    while(isSpace(source[cur_index])){
        cur_index++;
    }

    if(isSingleCharInstruction(source[cur_index])){
        Token token = handleSingleCharInstruction();
        return token;
    }
    else{
        Token token = handleMultipleCharInstruction();
        cur_index++;
        lastTokenType = token.type;
        return token;
    }
}

void Lexer::parseTokens(){

    while(cur_index < source.size()){
        auto token = parseToken();
        tokens.push_back(token);
    }
}

/*
    @brief creates tokens from a given string, and also appends End Of File token at the end of the tokens
*/
std::vector<Token> Lexer::getTokensFromString(const std::string& s){
    tokens.clear();
    cur_index = 0;
    source = s;
    parseTokens();
    tokens.push_back(Token{ TokenType::tokEOF,"",cur_index-1,1});
    lastTokenType = TokenType::undefined;
    return tokens;
}

/*
    @brief helper function for assembling a string with tokens position and length
*/
std::string printTokenPosAndLength(const Token& tok){
    std::string tokString = "";
    tokString.append("position: ");
    tokString.append(std::to_string(tok.startPos));
    tokString.append(" length: ");
    tokString.append(std::to_string(tok.length));
    return tokString;
}

void Lexer::printTokens(){

    for(auto& token: tokens){
        switch(token.type){
        case TokenType::openParen:  std::cout << "open paren "<<printTokenPosAndLength(token)<<"\n";        break;
        case TokenType::closeParen: std::cout << "close paren "<<printTokenPosAndLength(token)<<"\n";       break;
        case TokenType::plusSign:   std::cout << "addition "<<printTokenPosAndLength(token)<<"\n";          break;
        case TokenType::minusSign:  std::cout << "subtraction "<<printTokenPosAndLength(token)<<"\n";       break;
        case TokenType::multipSign: std::cout << "multiplication "<<printTokenPosAndLength(token)<<"\n";    break;
        case TokenType::divSign:    std::cout << "division "<<printTokenPosAndLength(token)<<"\n";          break;
        case TokenType::numLiteral: std::cout << "some number with a value of: "<<token.value<<" "<<printTokenPosAndLength(token)<<"\n";       break;
        case TokenType::identifier: std::cout << "some variable named: "<<token.value<<" "<<printTokenPosAndLength(token)<<"\n";     break;
        case TokenType::assignOp:   std::cout << "assign operation "<<printTokenPosAndLength(token)<<"\n";  break;
        case TokenType::comma:      std::cout << "comma " << printTokenPosAndLength(token) << "\n";         break;
        case TokenType::exponentSign: std::cout << "exponent sign" << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::tokEOF: std::cout << "end of file token\n";                                         break;
        case TokenType::undefined:  std::cout << "I DONT KNOW T_T\n";                                       break;
        }
    }
}

void Lexer::cleanup() {
    source = "";
    cur_index = 0;
    tokens = {};
    lastTokenType = TokenType::undefined;
}