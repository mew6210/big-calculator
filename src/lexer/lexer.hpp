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
    std::string cur_token;
    int cur_index = 0;
    std::vector<Token> tokens;

    Token parseToken();
    std::vector<Token> parseTokens();
    Token handleNumberLiteralToken(const std::string& tok);
    Token handleIdentifierToken(const std::string& tok);
    Token handleMultipleCharInstruction();
public:
    void getTokensFromString(const std::string& s);
    void printTokens();
};