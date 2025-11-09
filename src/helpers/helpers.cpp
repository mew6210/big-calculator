#include "helpers.hpp"
void printError(const std::string& src, const uint64_t& errorPos, const std::string& errorMsg, const std::string& errorNote) {
    std::cout << src << "\n";
    for (int j = 0; j < 3; j++) {
        for (int i = 0; i <= src.size(); i++) {
            if (j == 0 && i == errorPos) {
                std::cout << "^";
                continue;
            }
            if (i == errorPos) {
                std::cout << "|";
            }
            else std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\nLexer error: " << errorMsg << "\n";   //lexer output
    std::cout << "Note: " << errorNote << "\n";     //guiding instruction
    exit(1);
}
