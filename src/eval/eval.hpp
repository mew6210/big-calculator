#include "string"
#include "../astnodes/astnodes.hpp"
#include <map>
struct Evaluator {
	std::string src;
	std::string seed;
	std::unique_ptr<ExprNode> ASTRoot;
	EvalCtx evalCtx;

	void setSeed(const std::string_view s){seed = s;}
	[[nodiscard]] bool isSeeded() const noexcept {if (seed.empty()) return false; else return true;}
	void handleAssignVar(BinaryExprNode* ASTRootAssignNode);
	void handleAssignUserFunc(BinaryExprNode* ASTRootAssignNode);
 	[[nodiscard("Printing the result is handled outside the eval, so you should probably use the eval output")]] BigInt eval();
	void handleAssignRoot();
	void setSrc(std::string& src_) { src = src_; };
	void setASTRoot(std::unique_ptr<ExprNode>& root_) { ASTRoot = std::move(root_); };
	Evaluator(const std::string& s, std::unique_ptr<ExprNode>& astRoot) : src(s), ASTRoot(std::move(astRoot)){}
	Evaluator() = default;
};