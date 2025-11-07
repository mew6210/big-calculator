#include "lexer/lexer.hpp"
#include "core/core.hpp"
#include "bigint/bigint.hpp"

int main(){

    initApp();
    Lexer lexer = Lexer();

    checkSubtr();

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