#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../eval/eval.hpp"

class AppState {

	Lexer lexer;
	Parser parser;
	Evaluator evaluator;
	std::string src;
	std::vector<Token> tokens;
	std::string seed;

	void lex();
	void parse();
	void eval();
	void cleanup();
	void handleFileLoad();
	void printOutput(const BigInt&);

	void executeTxtFile(std::ifstream&);
	void executeMbcFile(std::ifstream&);
public:
	AppState(){}
	
	//could be std::filesystem::path to make it more secure
	void setFileToExec(const std::string& s) { evaluator.evalCtx.fileToExec = s; }
	void setSeed(const std::string& s){seed = s;}
	[[nodiscard]] bool isSeeded() const noexcept {if (seed.empty()) return false; else return true;}

	void execute();
	void executeFile() { handleFileLoad(); }
	void setSrc(const std::string& s) { src = s; }
};

AppState initApp();

void checkForStart(AppState& a);

void handleCmdArgs(int argc, char** argv);