#include "lexer.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>
#include "../logging/logging.hpp"

/*
    @brief all tokens that are single chars
*/
std::unordered_map<char,TokenType> singleOpsToEnumMap = {
    {'(',TokenType::openParen},
    {')',TokenType::closeParen},
    {'{',TokenType::openCurl},
    {'}',TokenType::closeCurl},
    {',',TokenType::comma},
    {';',TokenType::semiColon},

    {'+',TokenType::plusSign},
    {'-',TokenType::minusSign},
    {'*',TokenType::multipSign},
    {'/',TokenType::divSign},
    {'=',TokenType::assignOp},
    {'<',TokenType::smallerThan},
    {'>',TokenType::biggerThan}
    
};

std::unordered_map<std::string, TokenType> reservedKeywords = {
    {"if",TokenType::ifKeyword},
    {"else",TokenType::elseKeyword},
    {"equals",TokenType::equalKeyword},
    {"notEquals",TokenType::notEqualKeyword},
    {"return",TokenType::returnKeyword},
    {"while",TokenType::whileKeyword},

    {"==",TokenType::equalKeyword},
    {"!=",TokenType::notEqualKeyword},
    {"&&",TokenType::andOp},
    {"||",TokenType::orOp},
    {">=",TokenType::biggerOrEqualThan},
    {"<=",TokenType::smallerOrEqualThan}
};

/*
    @brief functions that are used only by lexer
*/
namespace {

    bool isSpace(const char& c) {

        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') return true;
        else return false;
    }

    enum class TokenCharCount {
        singleChar,
        doubleChar,
        keyword,
    };

    TokenCharCount isSingleCharInstruction(const char& currentChar, std::optional<char> nextChar) {
        if (!nextChar.has_value()) {
            if (singleOpsToEnumMap.contains(currentChar)) return TokenCharCount::singleChar;
            else return TokenCharCount::keyword; //idk about that lol
        };

        if (currentChar == '!') return TokenCharCount::doubleChar; //kind of an exception
        if (!singleOpsToEnumMap.contains(currentChar)) return TokenCharCount::keyword;

        if (singleOpsToEnumMap.contains(currentChar) && (
            nextChar.value() == '=' ||
            nextChar.value() == '<' ||
            nextChar.value() == '>'
            )) return TokenCharCount::doubleChar;

        return TokenCharCount::singleChar;
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
        return singleOpsToEnumMap[cur_index] == TokenType::minusSign && (lastTok != TokenType::numLiteral && lastTok != TokenType::identifier && lastTok!=TokenType::closeParen&& lastTok!=TokenType::closeCurl);
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
    return Token{
        .type = TokenType::identifier,
        .value = tok,
        .startPos = startPos,
        .length = tok.size()
    };
}

Token handleKeywordIdentifier(const std::string& nameBuf, const uint64_t& startingPos) {
    return Token{ reservedKeywords[nameBuf],nameBuf,startingPos,nameBuf.size() };
}

/*
    @brief handles any strings that arent single-char tokens

    if it starts with a digit, then lexes it as if it was a number literal

    otherwise handles it as an identifier
*/
Token Lexer::handleMultipleCharInstruction() {

    std::string nameBuf;
    uint64_t startingPos = cur_index;
    //load name into nameBuf
    while (
        isSingleCharInstruction(source[cur_index],peekNextToken()) != TokenCharCount::singleChar &&
        !isSpace(source[cur_index]) && cur_index != source.size()) {
        nameBuf += source[cur_index];
        cur_index++;
    }

    //if last char was a single instruction, then go back it needs to be evaluated, not skipped
    if (isSingleCharInstruction(source[cur_index],peekNextToken()) == TokenCharCount::singleChar) {
        cur_index--;
    }

    if (isDigit(nameBuf[0])) {
        return handleNumberLiteralToken(nameBuf,startingPos);
    }
    else {
        if (reservedKeywords.contains(nameBuf)) {
            return handleKeywordIdentifier(nameBuf,startingPos);
        }
        else return handleIdentifierToken(nameBuf,startingPos);
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

        Token token = Token{
            .type = singleOpsToEnumMap[source[cur_index]],
            .value = std::string{source[cur_index]},
            .startPos = cur_index,
            .length = 1
        };
        cur_index++;
        lastTokenType = token.type;
        return token;
    }

}

std::optional<char> Lexer::peekNextToken() {
    if (cur_index != source.size()) return source[cur_index + 1];
    else return std::nullopt;
}

Token Lexer::handleDoubleCharInstruction() {
    TokenType type = {};
    if (source[cur_index] == '=') type = TokenType::equalKeyword;
    if (source[cur_index] == '!') type = TokenType::notEqualKeyword;
    if (source[cur_index] == '>') type = TokenType::biggerOrEqualThan;
    if (source[cur_index] == '<') type = TokenType::smallerOrEqualThan;

    Token token = Token{
        .type = type,
        .value = std::string{source[cur_index],source[cur_index+1]},
        .startPos = cur_index,
        .length = 2
    };

    return token;
}

Token Lexer::parseToken(){

    //skip spaces
    while(isSpace(source[cur_index])){
        cur_index++;
    }

    TokenCharCount tokenCharType = isSingleCharInstruction(source[cur_index],peekNextToken());

    if(tokenCharType == TokenCharCount::singleChar){
        Token token = handleSingleCharInstruction();
        return token;
    } else if(tokenCharType == TokenCharCount::keyword){
        Token token = handleMultipleCharInstruction();
        cur_index++;
        lastTokenType = token.type;
        return token;
    } else if (tokenCharType == TokenCharCount::doubleChar) {
        Token token = handleDoubleCharInstruction();
        cur_index +=2;
        lastTokenType = token.type;
        return token;
    }
    else return Token{};
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
    tokens.push_back(Token{
        .type = TokenType::tokEOF,
        .value = "",
        .startPos = cur_index-1,
        .length = 1
    });
    lastTokenType = TokenType::undefined;
    return tokens;
}

/*
    @brief helper function for assembling a string with tokens position and length
*/
std::string printTokenPosAndLength(const Token& tok){
    std::string tokString;
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
        case TokenType::tokEOF: std::cout << "end of file token\n";                                         break;
        case TokenType::closeCurl: std::cout << "close curl " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::openCurl: std::cout << "open curl " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::semiColon: std::cout << "semicolon " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::ifKeyword: std::cout << "ifKeyword " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::elseKeyword: std::cout << "elseKeyword " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::equalKeyword: std::cout << "equalKeyword " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::notEqualKeyword: std::cout<<"notEqualKeyword"<<printTokenPosAndLength(token)<<"\n";break;
        case TokenType::returnKeyword: std::cout<<"returnKeyword"<<printTokenPosAndLength(token)<<"\n";break;
        case TokenType::whileKeyword: std::cout<<"whileKeyword"<<printTokenPosAndLength(token)<<"\n"; break;
        case TokenType::smallerThan: std::cout << "smallerThan " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::biggerThan: std::cout << "biggerThan " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::smallerOrEqualThan: std::cout << "smallerOrEqualThan " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::biggerOrEqualThan: std::cout << "biggerOrEqualThan " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::andOp: std::cout << "andOp " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::orOp: std::cout << "orOp " << printTokenPosAndLength(token) << "\n"; break;
        case TokenType::undefined:  std::cout << "I DONT KNOW T_T\n";break;
        }
    }
}

void Lexer::cleanup() {
    source = "";
    cur_index = 0;
    tokens = {};
    lastTokenType = TokenType::undefined;
}