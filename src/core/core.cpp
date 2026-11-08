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
    return {};
}

void AppState::handleFileLoad() {
    cleanup();
    std::string fileName = evaluator.evalCtx.fileToExec;
    evaluator.evalCtx.fileToExec = "";

    if (fileName.empty()) return; //if no file to execute, just leave

    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cout << "Error: Could not open \"" << fileName << "\" \n";
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    for (const auto& lineFromLines : lines) {
        setSrc(lineFromLines);
        execute();
    }
    fileName = "";
}

void AppState::printOutput(const BigInt& ret) {

    if (!isSeeded()) {
        if (evaluator.evalCtx.shouldPrint) ret.print();
        evaluator.evalCtx.shouldPrint = true;
    }
    else {
        if (evaluator.evalCtx.shouldPrint) ret.print();
        evaluator.evalCtx.shouldPrint = true;
        std::cout<<"Done! Seed: "<<seed<<"\n";
    }


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
    evaluator.setSeed(seed);
    auto root = parser.getRoot();       //setASTRoot only takes lvalues, so this variable is actually necessary XD
    evaluator.setASTRoot(root);
    try {
        const BigInt ret = evaluator.eval();
        printOutput(ret);
    }catch (ReturnException& ret) {
        printError(ErrMsg{ret.getSrc(),0,"Return outside block statement","Only blocks can return something, like so: {return 3;}",ErrType::Evaluator});
        return;
    }
    
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
    if (src.empty()) return;
    lex();
    parse();
    eval();
    cleanup();
}

void checkForStart(AppState& state) {
    if (std::ifstream file("start.txt"); file.good()) {
        state.setFileToExec("start.txt");
        state.executeFile();
        std::cout << "file with previous vars and funcs found and loaded successfully\n";
    }
    else return;
}

void handleInteractiveMode() {
    AppState state = initApp();
    checkForStart(state);   //check for saved things
    std::string line;
    while (true) {
        std::cout << ">";
        std::getline(std::cin, line);

        state.setSrc(line);
        state.execute();
    }
}

void handleInteractiveModeWithSeed(const char* seed) {
    AppState state = initApp();
    state.setSeed(seed);
    checkForStart(state);   //check for saved things
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        state.setSrc(line);
        state.execute();
    }
}


bool isStrFile(const char* c){
    if (!c) return false;
    return std::strchr(c, '.') != nullptr;
}

bool isSeeded(const char* c) {
    if (strcmp(c,"--seeded") == 0 || strcmp(c,"-s") == 0) return true;
    else return false;
}

void handleFileExecution(const char* c) {

    AppState state = {};
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

    AppState state = {};
    checkForStart(state);
    state.setSrc(code);
    state.execute();
}

void handleCmdArgs(int argc, char** argv) {

    switch (argc) {
    case 1: handleInteractiveMode(); return;
    case 2: {
        if (isStrFile(argv[1])) {
            handleFileExecution(argv[1]);
            return;
        }
        else break;
    
    }break;
    case 3: {
        if (isSeeded(argv[1])) {
            handleInteractiveModeWithSeed(argv[2]);
        }
    }break;
    default: break;
    }
    
    handleCodeExecution(argc, argv);
}