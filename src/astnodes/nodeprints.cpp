#include "astnodes.hpp"
#include <map>
#include <iostream>
#include "../eval/evalException.hpp"
#include "../eval/eval.hpp"

//printing
void BinaryExprNode::print(int indent = 0) {
	static std::map<OperatorType, std::string> binOpToStringMap = {
		{OperatorType::assign,"assign"},
		{OperatorType::add,"add"},
		{OperatorType::subtract,"subtract"},
		{OperatorType::multiply,"multiply"},
		{OperatorType::divide,"divide"},
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
		std::cout << std::string(indent, ' ') << "Arg" << i << ": "<<"\n";
		args[i]->print(indent + 4);
	}
}

//node types
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

char operatorTypeToChar(OperatorType& op) {
	switch (op) {
		case OperatorType::assign: return '=';
		case OperatorType::add: return '+';
		case OperatorType::subtract: return '-';
		case OperatorType::multiply: return '*';
		case OperatorType::divide: return '/';
		case OperatorType::undefined: return '?';
	}
}

//to string
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


void Block::print(int ident) {
	std::cout << std::string(ident, ' ') << "Block{ \n";

	for (size_t i = 0; i < lines.size(); i++) {
		std::cout << std::string(ident, ' ') << "line " << i << ": " << lines[i]->toString()<<";\n";
	}
	std::cout << std::string(ident, ' ') << "}\n";
}
BigInt Block::eval(EvalCtx& eCtx){
	
	Evaluator ev;
	ev.evalCtx = std::move(m_EvalCtx);
	for (size_t i = 0; i < lines.size()-1; i++) {
		ev.setASTRoot(lines[i]);	
		ev.evalCtx.shouldPrint = false;
		ev.eval();
		lines[i] = std::move(ev.ASTRoot);
	}

	ev.setASTRoot(lines[lines.size() - 1]);
	BigInt ret = ev.evalRet();
	lines[lines.size() - 1] = std::move(ev.ASTRoot);
	m_EvalCtx = std::move(ev.evalCtx);
	return ret;

}
std::string Block::toString(){
	std::string ret;
	ret+="Block: \n";

	for (size_t i = 0; i < lines.size(); i++) {
		ret += "line ";
		ret += std::to_string(i);
		ret += ": ";
		ret += lines[i]->toString();
		ret+=";\n";
	}
	return ret;
}
NodeType Block::type(){
	return NodeType::Block;
}