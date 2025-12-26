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

	void execute();
	void setSrc(const std::string& s) { src = s; }
};

AppState initApp();