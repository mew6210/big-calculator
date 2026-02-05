#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../eval/eval.hpp"

class AppState {

	Lexer lexer;
	Parser parser;
	Evaluator evaluator;
	std::string src;
	std::vector<Token> tokens;

	void lex();
	void parse();
	void eval();
	void cleanup();
	void handleFileLoad();

public:
	AppState(){}
	
	//could be std::filesystem::path to make it more secure
	void setFileToExec(const std::string& s) { evaluator.evalCtx.fileToExec = s; }
	
	void execute();
	void executeFile() { handleFileLoad(); }
	void setSrc(const std::string& s) { src = s; }
};

AppState initApp();

void checkForStart(AppState& a);

void handleCmdArgs(int argc, char** argv);