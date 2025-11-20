#include "logging.hpp"
#include <iostream>

void printErrType(const ErrType& type) {
    switch (type) {
    case ErrType::Lexer: std::cout << "Lexer error: "; break;
    case ErrType::Parser: std::cout << "Parser error: "; break;
    case ErrType::Evaluator: std::cout << "Evaluator error: "; break;
    default: std::cout << "Unkown error: "; break;
    }
}

void printError(const ErrMsg& errMsg) {
    std::cout << errMsg.ctx << "\n";    //print entire line
    
    for (int j = 0; j < 3; j++) {   // arrow to point where error was
        for (int i = 0; i <= errMsg.ctx.size(); i++) {
            if (j == 0 && i == errMsg.pointerPos) {     //if its first row, then print arrow
                std::cout << "^";
                continue;
            }
            if (i == errMsg.pointerPos) {               //if column is where position of error is, print arrow base
                std::cout << "|";
            }
            else std::cout << " ";
        }
        std::cout << "\n";  //next row
    }

    std::cout << "\n";

    printErrType(errMsg.type);
    std::cout << errMsg.msg << "\n";                  //error content  
    std::cout << "Note: " << errMsg.note << "\n";     //guiding instruction
    exit(1);
}
