#include <vector>
#include <map>
#include "../bigint/bigint.hpp"
#include "../lexer/token/token.hpp"
#include "../logging/logging.hpp"
#include "../astnodes/astnodes.hpp"

class Parser {
	std::vector<Token> tokens;	//token stream from lexer
	std::string src;	//for nice error messages
	Token curTok;
	size_t curTokIndex = 0;
	std::map<TokenType, int> precedenceMap = {
		{TokenType::assignOp,1},
		{TokenType::plusSign,10},
		{TokenType::minusSign,10},
		{TokenType::multipSign,20},
		{TokenType::divSign,20},
		{TokenType::exponentSign,30}
	};

	Token getNextToken();

	std::unique_ptr<ExprNode> parseNumberExpr();
	std::unique_ptr<ExprNode> parseParenExpr();
	std::unique_ptr<ExprNode> parseIdentifierExpr();

	std::unique_ptr<ExprNode> parsePrimary();
	int getTokPrecedence();
	std::unique_ptr<ExprNode> parseBinOpRHS(int exprPrec,std::unique_ptr<ExprNode> lhs);
	std::unique_ptr<ExprNode> parseTopLevelExpr();
	std::unique_ptr<ExprNode> parseExpression();

	std::unique_ptr<ExprNode> parseErrorLog(const std::string& msg,const std::string& note);
public:
	std::unique_ptr<ExprNode> parse();
	Parser(std::vector<Token>& tokensT,const std::string& src) :tokens(std::move(tokensT)),src(src), curTok(tokens[0]){}
};