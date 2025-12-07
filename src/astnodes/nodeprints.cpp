#include "astnodes.hpp"
#include <map>
#include <iostream>
#include "../eval/evalException.hpp"

void BinaryExprNode::print(int indent = 0) {
	static std::map<OperatorType, std::string> binOpToStringMap = {
		{OperatorType::assign,"assign"},
		{OperatorType::add,"add"},
		{OperatorType::subtract,"subtract"},
		{OperatorType::multiply,"multiply"},
		{OperatorType::divide,"divide"},
		{OperatorType::exponentiate,"exponentiate"},
		{OperatorType::undefined,"undefined"}
	};
	std::cout << std::string(indent, ' ') << "BinaryExprNode" << " op: " << binOpToStringMap[op]<< '\n';
	lhs->print(indent + 4);
	rhs->print(indent + 4);
}

void BigIntNode::print(int indent = 0) {
	std::cout << std::string(indent, ' ') << "BigIntNode: " << '\n';
	val.inspectChunks(chunkDisplayMode::decimal, indent);
}

void VariableExprNode::print(int indent = 0) {
	std::cout << std::string(indent, ' ') << "VariableExprNode: " << name << "\n";
	val.inspectChunks(chunkDisplayMode::decimal, indent);
}

void CallExprNode::print(int indent = 0) {
	std::cout << std::string(indent, ' ') << "CallExprNode: " << funcName << "\n";
	for (size_t i = 0; i < args.size(); i++) {
		std::cout << std::string(indent, ' ') << "Arg" << i << ": ";
		args[i]->print(indent + 4);
	}
}

NodeType CallExprNode::type() {
	return NodeType::CallExpr;
}

NodeType BigIntNode::type(){
	return NodeType::BigInt;
}
NodeType BinaryExprNode::type() {
	return NodeType::BinExpr;
}
NodeType VariableExprNode::type() {
	return NodeType::Var;
}

BigInt EvalCtx::getVar(const std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) {
			return var.second;
		}
	}

	std::string errStr = name + " is undefined";
	std::string noteStr = "Define " + name;
	throw EvalException{ errStr,noteStr}; 
	return BigInt(0);
}

bool EvalCtx::varExists(std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) return true;
	}
	return false;
}

void EvalCtx::assignVar(std::string& name, BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
		};
	}
}

char operatorTypeToChar(OperatorType& op) {
	switch (op) {
		case OperatorType::assign: return '=';
		case OperatorType::add: return '+';
		case OperatorType::subtract: return '-';
		case OperatorType::multiply: return '*';
		case OperatorType::divide: return '/';
		case OperatorType::exponentiate: return '^';
		case OperatorType::undefined: return '?';
	}
}

std::string CallExprNode::toString() {
	
	std::string ret;
	ret += funcName;
	ret += '(';

	for (auto& arg : args) {

		ret +=arg->toString();
		if (&arg != &args.back()) ret += ',';
	}
	ret += ')';
	return ret;
}
std::string BigIntNode::toString() {
	return val.toString();
}
std::string BinaryExprNode::toString() {
	std::string lhsString = lhs->toString();
	std::string rhsString = rhs->toString();
	char opString = operatorTypeToChar(op);

	std::string ret = lhsString +opString+ rhsString;

	return ret;
}
std::string VariableExprNode::toString() {
	return name;
}