#include "astnodes.hpp"
#include <map>
#include <iostream>
#include "../eval/evalException.hpp"
#include "../eval/eval.hpp"
#include "../eval/stl/stl.hpp"

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
	BigInt retVal;
	bool set = false;
	m_EvalCtx.setParent(eCtx);
	ev.evalCtx = std::move(m_EvalCtx);
	for (size_t i = 0; i < lines.size()-1; i++) {
		ev.setASTRoot(lines[i]);	
		ev.evalCtx.shouldPrint = false;
		try {
			ev.eval();
		}catch (ReturnException& ret) {
			retVal = ret.getVal();
			set = true;
			lines[i] = std::move(ev.ASTRoot);
			break;
		}

		lines[i] = std::move(ev.ASTRoot);
	}

	if (set) {
		m_EvalCtx = std::move(ev.evalCtx);
		return retVal;
	}

	ev.setASTRoot(lines[lines.size() - 1]);
	try {
		retVal = ev.evalRet();
		eCtx.shouldPrint = ev.evalCtx.shouldPrint;
	}catch (ReturnException& ret) {
		retVal = ret.getVal();
		lines[lines.size() - 1] = std::move(ev.ASTRoot);
		m_EvalCtx = std::move(ev.evalCtx);
		return retVal;
	}

	lines[lines.size() - 1] = std::move(ev.ASTRoot);
	m_EvalCtx = std::move(ev.evalCtx);
	return retVal;

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

void IfStmtNode::print(int indent) {
	std::cout << std::string(indent, ' ') << "If statement node:\n";
	std::cout << std::string(indent, ' ') << "Condition: \n";
	cond->print(indent+4);
	std::cout << std::string(indent, ' ') << "Body: \n";
	body->print(indent+4);
	std::cout << "\n";
}
NodeType IfStmtNode::type() {return NodeType::IfStmt;}
std::string IfStmtNode::toString() {
	return "If statement, condition: "+
		cond->toString()
		+" \nbody: "
		+ body->toString();
}

void ReturnNode::print(int indent) {
	std::cout<<std::string(indent,' ')<<"Return: \n";
	val->print(indent+4);
}

std::string ReturnNode::toString() {
	return "return "+val->toString();
}

NodeType ReturnNode::type() {
	return NodeType::Return;
}

//TODO: Implement
void IfChainNode::print(int indent) {

}

//TODO: Implement
BigInt IfChainNode::eval(EvalCtx&) {
	return {0};
}

//TODO: Implement
std::string IfChainNode::toString() {
	return "";
}

NodeType IfChainNode::type() {
	return NodeType::IfChain;
}

BigInt BigIntNode::eval(EvalCtx&) {
	return val;
}

BigInt BinaryExprNode::eval(EvalCtx& evalCtx){
	BigInt a = lhs->eval(evalCtx);
	BigInt b = rhs->eval(evalCtx);

	switch (op) {
	case OperatorType::add: a.addBigInt(b); break;
	case OperatorType::subtract: a.subtractBigInt(b); break;
	case OperatorType::multiply: a.multiplyBigInt(b); break;
	case OperatorType::divide: {
		if (b.equals(BigInt("0"))) throw EvalException("Division by zero", "Dont divide by 0", a, BigInt(0), '/');
		else a.divideBigInt(b,false);
	}break;
	case OperatorType::equal: {
		if (a.equals(b)) return {1};
		else return {0};
	} break;
	case OperatorType::notEqual: {
		if (a.equals(b)) return 0;
		else return 1;
	}
	case OperatorType::assign: throw EvalException("Assignment caught in the middle of an eval", "Assignment here is only one-part like 'a=5'", a, b, '='); break;
	case OperatorType::undefined: throw EvalException("Unkown operation", "avalible operations are: +,-,*,/,^", a, b, '?'); break;
	}

	return a;
}

BigInt VariableExprNode::eval(EvalCtx& ectx) {
	std::string varName = getName();
	BigInt ret = ectx.getVar(varName);
	if (varName[0] == '-') {		//if somebody types in -g, it should be recognized as negative g, not '-g' variable
		varName.erase(0,1);
		ret = ectx.getVar(varName);
		ret.flipSign();
	}
	return ret;
}

BigInt CallExprNode::eval(EvalCtx& ectx) {

	auto var = funcDispatch(funcName, args, ectx);

	if (var) return var.value();
	else return {0};
}

BigInt IfStmtNode::eval(EvalCtx& ectx){
	if (cond->eval(ectx).equals(1)){
		return body->eval(ectx);
	}
	else return {0};
}

BigInt ReturnNode::eval(EvalCtx& ectx) {
	BigInt returnValue = val->eval(ectx);
	throw ReturnException(returnValue,toString());
}


