#include <string>
#include <vector>

#include "token/token.hpp"

class Lexer{
    std::string source;
    uint64_t cur_index = 0;
    TokenType lastToken = TokenType::undefined;
    std::vector<Token> tokens;

    Token parseToken();
    void parseTokens();
    Token handleNumberLiteralToken(const std::string& tok,const uint64_t& startPos);
    Token handleIdentifierToken(const std::string& tok,const uint64_t& startPos);
    Token handleMultipleCharInstruction();
public:
    std::vector<Token> getTokensFromString(const std::string& s);
    void printTokens();
    std::vector<Token> getTokens() { return tokens; };
};