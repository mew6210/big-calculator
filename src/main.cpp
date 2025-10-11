#include "lexer/lexer.hpp"
#include <iostream>

int main(){


    Lexer lexer = Lexer();
    
    std::string line = "";
    std::vector<Token> curTokens;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);
        
        curTokens = lexer.getTokensFromString(line);
        lexer.printTokens();
    }
    return 0;
}