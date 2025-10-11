#include <string>
#include <vector>


enum class TokenType{
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

struct Token{
    TokenType type;
    std::string value;
};

class Lexer{
    std::string source;
    int cur_index = 0;
    std::vector<Token> tokens;

    Token parseToken();
    void parseTokens();
    Token handleNumberLiteralToken(const std::string& tok);
    Token handleIdentifierToken(const std::string& tok);
    Token handleMultipleCharInstruction();
public:
    std::vector<Token> Lexer::getTokensFromString(const std::string& s);
    void printTokens();
    std::vector<Token> getTokens() { return tokens; };
};