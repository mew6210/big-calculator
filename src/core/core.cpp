#include "core.hpp"
#include <iostream>

void introduce(){
    std::cout<<"Arbitrary precision integer calculator\n";
    std::cout<<"Made by Marcin Skrzypek\n";
    std::cout<<"Type 'help()' for help\n";
}

AppState initApp(){
    introduce();
    return AppState();
}

void AppState::lex(){
    tokens = lexer.getTokensFromString(src);
    //lexer.printTokens();
}

void AppState::parse(){
    parser.setTokens(tokens);
    parser.setSrc(src);
    parser.parse();
    //parser.print();
}

void AppState::eval(){
    
    evaluator.setSrc(src);
    auto root = parser.getRoot();
    evaluator.setASTRoot(root);
    evaluator.eval();
}

void AppState::cleanup() {
    lexer.cleanup();
    parser.cleanup();
}

void AppState::execute() {
    lex();
    parse();
    eval();
    cleanup();
}