#pragma once
#include "../bigint/bigint.hpp"
#include "../lexer/token/token.hpp"
#include "astoper.hpp"

/*
	@brief base class for other nodes
*/
class ExprNode {

public:
	virtual ~ExprNode() = default;
	ExprNode() = default;
	ExprNode(const ExprNode&) = default;
	virtual void print(int ident = 0) = 0;
	virtual BigInt eval() = 0;
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
	BigInt eval() override;
};

/*
	@brief node for expressions like `2+3`
*/
class BinaryExprNode : public ExprNode {
	std::unique_ptr<ExprNode> lhs, rhs;
	OperatorType op;

public:
	BinaryExprNode(std::unique_ptr<ExprNode>&& lhsT, std::unique_ptr<ExprNode>&& rhsT, OperatorType& opT) : lhs(std::move(lhsT)), rhs(std::move(rhsT)), op(opT) {}
	void print(int indent) override;
	BigInt eval() override;
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
	BigInt eval() override;
};

/*
	@brief node for function calls like pow(2,3)
*/
class CallExprNode : public ExprNode {

	std::string funcName; //callee
	std::vector<std::unique_ptr<ExprNode>> args;	//not tokens or strings, cause user can input pow(2+3,3) which requires evaluation

public:
	CallExprNode(const Token& nameT, std::vector<std::unique_ptr<ExprNode>>& argsT) : funcName(nameT.value), args(std::move(argsT)) {}
	void print(int indent) override;
	BigInt eval() override;
};