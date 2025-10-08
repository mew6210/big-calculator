#include "lexer/lexer.hpp"

int main(){
    Lexer lexer = Lexer();
    lexer.getTokensFromString("+=()* - //          )");
    lexer.printTokens();
    return 0;
}