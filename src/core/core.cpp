#include "core.hpp"
#include <iostream>
#include <fstream>

void introduce(){
    std::cout<<"Arbitrary precision integer calculator\n";
    std::cout<<"Made by Marcin Skrzypek\n";
    std::cout<<"Type 'help()' for help\n";
}

AppState initApp(){
    introduce();
    return AppState();
}

void AppState::handleFileLoad() {
    cleanup();
    std::string fileName = evaluator.evalCtx.fileToExec;
    evaluator.evalCtx.fileToExec = "";

    if (fileName == "") return; //if no file to execute, just leave

    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cout << "Error: Could not open \"" << fileName << "\" \n";
    }

    std::vector<std::string> lines;
    std::string line = "";
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    for (const auto& line : lines) {
        setSrc(line);
        execute();
    }
    fileName = "";
}

//lexer wrapper
void AppState::lex(){
    tokens = lexer.getTokensFromString(src);
    if(evaluator.evalCtx.showLexerOutput) lexer.printTokens();
}

//parser wrapper
void AppState::parse(){
    parser.setTokens(tokens);
    parser.setSrc(src);
    parser.parse();
    if(evaluator.evalCtx.showParserOutput) parser.print();
}

//evaluator wrapper
void AppState::eval(){
    evaluator.setSrc(src);
    auto root = parser.getRoot();       //setASTRoot only takes lvalues, so this variable is actually necessary XD
    evaluator.setASTRoot(root);
    evaluator.eval();
    
    handleFileLoad();
}

/*
    @brief resets some variables to their original form, so that another expression can be executed
*/
void AppState::cleanup() {
    lexer.cleanup();
    parser.cleanup();
}

void AppState::execute() {
    if (src == "") return;
    lex();
    parse();
    eval();
    cleanup();
}

void checkForStart(AppState& state) {

    std::ifstream file("start.txt");

    if (file.good()) {
        state.setFileToExec("start.txt");
        state.executeFile();
        std::cout << "file with previous vars and funcs found and loaded successfully\n";
    }
    else return;
}

void handleInteractiveMode() {
    AppState state = initApp();
    checkForStart(state);   //check for saved things
    std::string line = "";
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);

        state.setSrc(line);
        state.execute();
    }
}


bool isStrFile(const char* c){
    if (!c) return false;
    return std::strchr(c, '.') != nullptr;
}

void handleFileExecution(const char* c) {

    AppState state = AppState();
    checkForStart(state);   //check for saved things
    
    state.setFileToExec(c);
    state.executeFile();

}

void handleCodeExecution(int argc, char** args) {

    std::string code;

    int i = 1;
    while (i != argc) {
        code.append(args[i]);
        code.append(" ");
        i++;
    }

    AppState state = AppState();
    checkForStart(state);
    state.setSrc(code);
    state.execute();

}

void handleCmdArgs(int argc, char** argv) {

    switch (argc) {
    case 1: handleInteractiveMode(); return;
    case 2: {
        if (isStrFile(argv[1])) { handleFileExecution(argv[1]); return; }
        else break;
    
    }break;
    default: break;
    }
    
    handleCodeExecution(argc, argv);

}