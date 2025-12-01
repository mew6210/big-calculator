#include "string"
#include "../astnodes/astnodes.hpp"

struct Evaluator {
	std::string src;
	std::unique_ptr<ExprNode> ASTRoot;

public:
	void eval();
	Evaluator(const std::string& s, std::unique_ptr<ExprNode>& astRoot) : src(s), ASTRoot(std::move(astRoot)){}
};