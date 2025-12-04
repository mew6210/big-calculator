#include "astnodes.hpp"
#include <map>
#include <iostream>

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
	//throw EvalException{ "blah blah","TODO" };
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