#pragma once
#include "../bigint/bigint.hpp"
#include "../lexer/token/token.hpp"
#include "astoper.hpp"
#include <map>
#include <string>
#include <utility>
#include "../eval/evalctx/evalctx.hpp"

/*
	@brief enum for "reflection" of Node Types
*/
enum class NodeType {
	BigInt,
	Var,
	BinExpr,
	CallExpr,
	Block,
	IfStmt,
	IfChain,
	Return,
	WhileLoop
};

/*
	@brief base class for other nodes
*/
class ExprNode {

public:
	virtual ~ExprNode() = default;
	ExprNode() = default;
	ExprNode(const ExprNode&) = default;
	virtual void print(int ident = 0) = 0;	//each node should be printable
	virtual BigInt eval(EvalCtx&) = 0;	//each node shold be able to be evaled
	virtual std::string toString() = 0;	//each node should have a toString method
	virtual NodeType type() = 0;	//each node should have a type	
};

/*
	@brief node for number literals like `46123512`
*/
class BigIntNode : public ExprNode {

	BigInt val;
public:
	BigIntNode(Token& tok) : val(tok.value) {}
	BigIntNode(BigIntNode&&) noexcept = default;
	BigIntNode(const BigIntNode&) = default;
	void print(int indent) override;
	std::string toString() override;
	BigInt eval(EvalCtx&) override;
	NodeType type() override;
};

/*
	@brief node for expressions like `2+3`
*/
class BinaryExprNode : public ExprNode {
public:
	std::unique_ptr<ExprNode> lhs, rhs;
	OperatorType op;


	BinaryExprNode(std::unique_ptr<ExprNode>&& lhsT, std::unique_ptr<ExprNode>&& rhsT, OperatorType& opT) : lhs(std::move(lhsT)), rhs(std::move(rhsT)), op(opT) {}
	void print(int indent) override;
	OperatorType getOp() { return op; };
	std::unique_ptr<ExprNode> getLhs() { return std::move(lhs); };
	std::unique_ptr<ExprNode> getRhs() { return std::move(rhs); };
	NodeType lhsType(){ return lhs->type(); }
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

/*
	@brief node for variables like `a`
*/
class VariableExprNode : public ExprNode {

	std::string name;
	BigInt val;

public:
	VariableExprNode() {}
	VariableExprNode(std::string& name) : name(name) {}
	void print(int indent) override;
	std::string getName() { return name; };
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

/*
	@brief node for function calls like pow(2,3)
*/
class CallExprNode : public ExprNode {

	std::string funcName; //callee
	std::vector<std::unique_ptr<ExprNode>> args;	//not tokens or strings, cause user can input pow(2+3,3) which requires evaluation

public:
	CallExprNode(const Token& nameT, std::vector<std::unique_ptr<ExprNode>>& argsT) : funcName(nameT.value), args(std::move(argsT)) {}
	std::string getName() { return funcName; }
	std::vector<std::unique_ptr<ExprNode>> getArgs() { return std::move(args); }
	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

class Block : public ExprNode {
public:
	std::vector<std::unique_ptr<ExprNode>> lines;	//roots
	EvalCtx m_EvalCtx;

	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

class IfStmtNode : public ExprNode{

public:
	enum class IfStmtNodeType {
		If,
		ElseIf,
		Else
	};

private:
	std::unique_ptr<ExprNode> cond;
	std::unique_ptr<ExprNode> body;
	IfStmtNodeType ifType;

public:
	IfStmtNode(std::unique_ptr<ExprNode> cond,
		std::unique_ptr<ExprNode> body,
		IfStmtNodeType type
		):
	cond(std::move(cond)),
	body(std::move(body)),
	ifType(type){}

	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;

	BigInt evalCond(EvalCtx&) const;
	BigInt evalBody(EvalCtx&) const;
	[[nodiscard]] bool isConditionNull() const {return !cond;}
	[[nodiscard]] IfStmtNodeType getIfType() const {return ifType;}
	void printBody(int indent) const {body->print(indent);}
	void printCond(int indent) const;
	[[nodiscard]] std::string toStringBody() const noexcept {return body->toString();}
	[[nodiscard]] std::string toStringCond() const noexcept;
};

class ReturnNode : public ExprNode {
	std::unique_ptr<ExprNode> val;
public:
	ReturnNode(std::unique_ptr<ExprNode> val):val(std::move(val)){}

	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

class ReturnException : std::exception {
	BigInt value;
	std::string src;

public:
	ReturnException(const BigInt& v,std::string s) : value(v),src(std::move(s)) {}
	[[nodiscard]] std::string getSrc() const noexcept {return src;}
	[[nodiscard]] BigInt getVal() const noexcept {return value;}
};

class IfChainNode : public ExprNode {
	using IfChain = std::vector<std::unique_ptr<IfStmtNode>>;

	IfChain branches;

public:
	IfChainNode(IfChain chain): branches(std::move(chain)){}
	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};

class WhileLoopNode : public ExprNode {

	std::unique_ptr<ExprNode> cond;
	std::unique_ptr<ExprNode> body;

public:

	WhileLoopNode(std::unique_ptr<ExprNode> cond, std::unique_ptr<ExprNode> body):
	cond(std::move(cond)),
	body(std::move(body)){}

	void print(int indent) override;
	BigInt eval(EvalCtx&) override;
	std::string toString() override;
	NodeType type() override;
};
