#include "parser.hpp"

BigInt BigIntNode::eval() {
	return val;
}

BigInt BinaryExprNode::eval(){
	BigInt a = lhs->eval();
	BigInt b = rhs->eval();

	switch (op) {
	case OperatorType::add: a.addBigInt(b); break;
	case OperatorType::subtract: a.subtractBigInt(b); break;
	case OperatorType::multiply: a.multiplyBigInt(b); break;
	case OperatorType::divide: break;	//TODO: USE DIVISION ONCE BIGINT DIVISION IS DONE
	default: std::cout << "op not supported yet";
	}

	return a;
}

BigInt VariableExprNode::eval() {
	return BigInt(0);	//TODO: MAKE A VALUE LOOKUP IN VARIABLE VECTOR
}

BigInt CallExprNode::eval() {
	return BigInt(0);	//TODO: MAKE FUNCTION EXECUTE
}