#include "lexer/lexer.hpp"
#include "core/core.hpp"
#include "bigint/bigint.hpp"
#include "parser/parser.hpp"
#include "eval/eval.hpp"

int main(){

    initApp();
    Lexer lexer = Lexer();
    Evaluator ev = Evaluator();
    std::string line = "";
    std::vector<Token> curTokens;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);
        
        curTokens = lexer.getTokensFromString(line);
        Parser p = Parser(curTokens, line);
        auto root = p.parse();
        ev.setSrc(line);
        ev.setASTRoot(root);
        ev.eval();
    }
    return 0;
}