#include <vector>
#include <map>
#include "../bigint/bigint.hpp"
#include "../lexer/token/token.hpp"
#include "../helpers/helpers.hpp"
enum class OperatorType {
	add,
	subtract,
	multiply,
	divide,
	undefined
	//exponentiate
};

OperatorType tokenToOper(const Token& tok);

/*
	@brief base class for other nodes
*/
class ExprNode {

public:
	virtual ~ExprNode() = default;
	ExprNode() = default;
	ExprNode(const ExprNode&) = default;
	virtual void print(int ident = 0) = 0;
};

/*
	@brief node for number literals like `46123512`
*/
class BigIntNode : public ExprNode {

	BigInt val;
public:
	BigIntNode(Token& tok) : val(tok.value){}
	BigIntNode(BigIntNode&&) noexcept = default;
	BigIntNode(const BigIntNode&) = default;
	void print(int indent) override;
};

/*
	@brief node for expressions like `2+3`
*/
class BinaryExprNode : public ExprNode {
	std::unique_ptr<ExprNode> lhs, rhs;
	OperatorType op;

public: 
	BinaryExprNode(std::unique_ptr<ExprNode>& lhsT, std::unique_ptr<ExprNode>& rhsT, OperatorType& opT) : lhs(std::move(lhsT)), rhs(std::move(rhsT)), op(opT) {}
	void print(int indent) override;
};

/*
	@brief node for variables like `a`
*/
class VariableExprNode : public ExprNode {

	std::string name;
	BigInt val;

public:
	VariableExprNode(){}
	VariableExprNode(std::string& name) : name(name){}
	void print(int indent) override;
};

/*
	@brief node for function calls like pow(2,3)
*/
class CallExprNode : public ExprNode {

	std::string funcName; //callee
	std::vector<std::unique_ptr<ExprNode>> args;	//not tokens or strings, cause user can input pow(2+3,3) which requires evaluation

public:
	CallExprNode(const Token& nameT,std::vector<std::unique_ptr<ExprNode>>& argsT): funcName(nameT.value),args(std::move(argsT)){}
	void print(int indent) override;
};

class Parser {
	std::vector<Token> tokens;	//token stream from lexer
	std::string src;	//for nice error messages
	Token curTok;
	size_t curTokIndex = 0;
	std::map<TokenType, int> precedenceMap = {
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
	Parser(std::vector<Token>& tokensT,const std::string& src) :curTok(tokens[0]),tokens(std::move(tokensT)),src(src){}
};