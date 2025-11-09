#include <vector>
#include <map>
#include "../bigint/bigint.hpp"
#include "../lexer/token/token.hpp"

enum class OperatorType {
	add,
	subtract,
	multiply,
	divide,
	undefined
	//exponentiate
};

OperatorType tokenToOper(const Token& tok);

class ExprNode {

public:
	virtual ~ExprNode() = default;
	ExprNode() = default;
	ExprNode(const ExprNode&) = default;
	virtual void print(int ident = 0) = 0;
};

class BigIntNode : public ExprNode {

	BigInt val;
public:
	BigIntNode(Token& tok) : val(tok.value){}
	BigIntNode(BigIntNode&&) noexcept = default;
	BigIntNode(const BigIntNode&) = default;
	void print(int indent = 0) override {
		std::cout << std::string(indent, ' ') << "BigIntNode: "<<'\n';
		val.inspectChunks(chunkDisplayMode::decimal,indent);
		std::cout<< "\n";
	}
};

class BinaryExprNode : public ExprNode {
	std::unique_ptr<ExprNode> lhs, rhs;
	OperatorType op;

public: 
	BinaryExprNode(std::unique_ptr<ExprNode>& lhsT, std::unique_ptr<ExprNode>& rhsT, OperatorType& opT) : lhs(std::move(lhsT)), rhs(std::move(rhsT)), op(opT) {}
	void print(int indent = 0) override {
		std::cout << std::string(indent, ' ') << "BinaryExprNode" << " op: " << (int)op<<'\n';
		lhs->print(indent + 4);
		rhs->print(indent + 4);
	}
};

class VariableExprNode : public ExprNode {

	std::string name;
	BigInt val;

public:
	VariableExprNode(){}
	VariableExprNode(std::string& name) : name(name){}
	void print(int indent = 0) override {
		std::cout << std::string(indent, ' ') << "VariableExprNode: " << name << "\n";
		val.inspectChunks(chunkDisplayMode::decimal,indent);
		std::cout << "\n";
	}
};

class CallExprNode : public ExprNode {

	std::string funcName; //callee
	std::vector<std::unique_ptr<ExprNode>> args;

public:
	CallExprNode(const Token& nameT,std::vector<std::unique_ptr<ExprNode>>& argsT): funcName(nameT.value),args(std::move(argsT)){}
	void print(int indent = 0) override {
		std::cout << std::string(indent, ' ') << "CallExprNode: " << funcName << "\n";
		for (size_t i = 0; i < args.size(); i++) {
			std::cout << std::string(indent, ' ') << "Arg" << i << ": ";
			args[i]->print(indent+4);
		}
	}
};

class Parser {
	std::vector<Token> tokens;	//token stream from lexer
	Token curTok;
	size_t curTokIndex = 0;
	std::map<TokenType, int> precedenceMap = {
		{TokenType::plusSign,10},
		{TokenType::minusSign,10},
		{TokenType::MultipSign,20},
		{TokenType::DivSign,20}
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
	void initCurTok() {
		curTok = tokens[0];
	}
public:
	std::unique_ptr<ExprNode> parse();
	Parser(std::vector<Token>& tokensT) :curTok(tokens[0]),tokens(std::move(tokensT)){}
};