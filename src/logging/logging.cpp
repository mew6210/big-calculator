#include "logging.hpp"

void printErrType(const ErrType& type) {
    switch (type) {
    case ErrType::Lexer: std::cout << "Lexer error: "; break;
    case ErrType::Parser: std::cout << "Parser error: "; break;
    case ErrType::Evaluator: std::cout << "Evaluator error: "; break;
    default: std::cout << "Unkown error: "; break;
    }
}

void printError(const ErrMsg& errMsg) {
    std::cout << errMsg.ctx << "\n";
    /*
    arrow to point where error was
    */
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i <= errMsg.ctx.size(); i++) {
            if (j == 0 && i == errMsg.pointerPos) {
                std::cout << "^";
                continue;
            }
            if (i == errMsg.pointerPos) {
                std::cout << "|";
            }
            else std::cout << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\n";

    printErrType(errMsg.type);
    std::cout << errMsg.msg << "\n";                  //error content  
    std::cout << "Note: " << errMsg.note << "\n";     //guiding instruction
    exit(1);
}
