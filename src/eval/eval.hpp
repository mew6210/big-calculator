#include "string"
#include "../astnodes/astnodes.hpp"
#include <map>
struct Evaluator {
	std::string src;
	std::unique_ptr<ExprNode> ASTRoot;
	EvalCtx evalCtx;

public:
	void eval();
	void handleAssignRoot();
	void setSrc(std::string& src_) { src = src_; };
	void setASTRoot(std::unique_ptr<ExprNode>& root_) { ASTRoot = std::move(root_); };
	Evaluator(const std::string& s, std::unique_ptr<ExprNode>& astRoot) : src(s), ASTRoot(std::move(astRoot)){}
	Evaluator() = default;
};