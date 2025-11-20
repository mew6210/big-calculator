#include "logging.hpp"
void printError(const ErrMsg& ErrMsg) {
    std::cout << ErrMsg.ctx << "\n";
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i <= ErrMsg.ctx.size(); i++) {
            if (j == 0 && i == ErrMsg.pointerPos) {
                std::cout << "^";
                continue;
            }
            if (i == ErrMsg.pointerPos) {
                std::cout << "|";
            }
            else std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\nLexer error: " << ErrMsg.msg << "\n";   //lexer output
    std::cout << "Note: " << ErrMsg.note << "\n";     //guiding instruction
    exit(1);
}
