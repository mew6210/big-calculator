#include "lexer/lexer.hpp"
#include "core/core.hpp"
#include "bigint/bigint.hpp"
#include "parser/parser.hpp"

int main(){

    initApp();
    Lexer lexer = Lexer();

    std::string line = "";
    std::vector<Token> curTokens;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);
        
        curTokens = lexer.getTokensFromString(line);
        Parser p = Parser(curTokens, line);
        auto root = p.parse();
        auto res = root->eval();
        res.print();
    }
    return 0;
}