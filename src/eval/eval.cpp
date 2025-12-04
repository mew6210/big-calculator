#include "eval.hpp"
#include "../astnodes/astnodes.hpp"
#include "../logging/logging.hpp"
#include <optional>

std::vector<std::pair<std::string, BigInt>> vars;

/*
	@brief helper exception for eval errors

	since all information about tokens and its position is lost during parsing stage, eval cant use printError like it would normally,
	instead it has to create src from currently evaluated numbers

	functions as kind of a wrapper around printError

	has 2 constructors, one that provides context for when it happened, whereas the other one doesnt
*/
class EvalException : public std::runtime_error {

	std::string msg;
	std::string note;
	std::optional<BigInt> a,b;
	std::optional<char> op;

public:
	EvalException(const std::string& _msg,const std::string& _note): 
		std::runtime_error(_msg),
		msg(_msg),
		note(_note){}
	EvalException(const std::string& _msg,const std::string& _note,BigInt& _a,BigInt& _b, const char _op) 
		: std::runtime_error(_msg),
		msg(_msg),
		note(_note),
		a(_a),
		b(_b),
		op(_op){}

	
	void printEvalErr(){
		if (a && b && op) {
			std::string src = a.value().toString() + op.value() + b.value().toString();		//src made from lhs,op,rhs

			printError(ErrMsg{
				src,
				a.value().toString().size(),
				msg,
				note,
				ErrType::Evaluator
				}
			);
		} 
		else {
			std::cout << "Evaluator error: " << msg << "\n";
			std::cout << "Note: " << note << "\n";
		}
	}
};

bool isRootAssign(std::unique_ptr<ExprNode>& node) {
	if (node->type() == NodeType::BinExpr) {
		auto binExprNode = dynamic_cast<BinaryExprNode*>(node.get());
		if (binExprNode->getOp() == OperatorType::assign) {
			return true;
		}
		else return false;
	}
	else return false;
}

void Evaluator::eval() {
	try {
		if (isRootAssign(ASTRoot)) {
			handleAssignRoot();
		}
		else {
			BigInt res = ASTRoot->eval();
			res.print();
		}
		
	}
	catch (EvalException& e) {
		e.printEvalErr();
	}
	
}

bool varExists(std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) return true;
	}
	return false;
}

void assignVar(std::string& name,BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
		};
	}
}

BigInt getVar(const std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) {
			return var.second;
		}
		
	}
	throw EvalException{ "blah blah","TODO" };
	return BigInt(0);
}

void Evaluator::handleAssignRoot() {

	auto ASTRootAssignNode = dynamic_cast<BinaryExprNode*>(ASTRoot.get());

	auto rhs = ASTRootAssignNode->getRhs()->eval();
	auto lhs = ASTRootAssignNode->getLhs();
	if (lhs->type() != NodeType::Var) {
		throw EvalException("blah blah","TODO");
	}
	auto lhsVar = dynamic_cast<VariableExprNode*>(lhs.get());
	std::string varName = lhsVar->getName();

	if (varExists(varName)) {
		assignVar(varName, rhs);
	}
	else {
		vars.push_back({ varName,std::move(rhs) });
	}
}

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
	case OperatorType::divide: throw EvalException("division not yet supported","dont use it please",a,b,'/'); break;	//TODO: USE DIVISION ONCE BIGINT DIVISION IS DONE
	case OperatorType::exponentiate: throw EvalException("exponentiation not yet supported", "dont use it please", a, b, '^'); break;	//TODO: EXPONENTIATION
	case OperatorType::assign: throw EvalException("assignment not yet supported", "dont use it please", a, b, '='); break;	//TODO: ASSIGNMENT
	case OperatorType::undefined: throw EvalException("Unkown operation", "avalible operations are: +,-,*,/,^", a, b, '?'); break;
	}

	return a;
}

BigInt VariableExprNode::eval() {
	return getVar(getName());	//TODO: MAKE A VALUE LOOKUP IN VARIABLE VECTOR
}

BigInt CallExprNode::eval() {
	return BigInt(0);	//TODO: MAKE FUNCTION EXECUTE
}
