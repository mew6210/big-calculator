#include "eval.hpp"
#include "../astnodes/astnodes.hpp"
#include "../logging/logging.hpp"
#include <optional>

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
			std::string src = a.value().toString() + op.value() + b.value().toString();

			printError(ErrMsg{
				src,
				a.value().toString().size(),
				msg,
				note,
				ErrType::Evaluator
				});
		} 
		else {
			std::cout << "Evaluator error: " << msg << "\n";
			std::cout << "Note: " << note << "\n";
		}
	}
};

void Evaluator::eval() {
	try {
		BigInt res = ASTRoot->eval();
		res.print();
	}
	catch (EvalException& e) {
		e.printEvalErr();
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
	return BigInt(0);	//TODO: MAKE A VALUE LOOKUP IN VARIABLE VECTOR
}

BigInt CallExprNode::eval() {
	return BigInt(0);	//TODO: MAKE FUNCTION EXECUTE
}