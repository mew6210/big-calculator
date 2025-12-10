#include "core/core.hpp"
#include "bigint/bigint.hpp"

int main(){

    AppState state = initApp();
    
    
    std::string line = "";
    std::vector<Token> curTokens;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);
        
        state.setSrc(line);
        state.execute();
    }
    return 0;
}