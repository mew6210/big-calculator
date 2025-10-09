#include "lexer/lexer.hpp"

int main(){
    Lexer lexer = Lexer();
    lexer.getTokensFromString("variable=(5*3)/3");
    lexer.printTokens();
    return 0;
}