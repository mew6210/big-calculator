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

//lexer wrapper
void AppState::lex(){
    tokens = lexer.getTokensFromString(src);
    //lexer.printTokens();
}

//parser wrapper
void AppState::parse(){
    parser.setTokens(tokens);
    parser.setSrc(src);
    parser.parse();
    //parser.print();
}

//evaluator wrapper
void AppState::eval(){
    evaluator.setSrc(src);
    auto root = parser.getRoot();       //setASTRoot only takes lvalues, so this variable is actually necessary XD
    evaluator.setASTRoot(root);
    evaluator.eval();
}

/*
    @brief resets some variables to their original form, so that another expression can be executed
*/
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