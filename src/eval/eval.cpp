#include "eval.hpp"
#include "../astnodes/astnodes.hpp"
#include "../logging/logging.hpp"
#include <optional>
#include "evalException.hpp"

/*
	@brief checks if root node is a binary assignment expression
*/
bool isRootAssign(std::unique_ptr<ExprNode>& node) {
	if (node->type() == NodeType::BinExpr) {	//if node has a type of binexpr
		auto binExprNode = dynamic_cast<BinaryExprNode*>(node.get());	//cast it to binaryexprnode
		if (binExprNode->getOp() == OperatorType::assign) {	//check for assignment operator
			return true;
		}
		else return false;
	}
	else return false;
}

void Evaluator::eval() {
	try {
		if (isRootAssign(ASTRoot)) {	//if its an assignment, treat it like so
			handleAssignRoot();
		}
		else {
			BigInt res = ASTRoot->eval(evalCtx);	//otherwise treat it like a basic evaluation, no variable assigning
			res.print();
		}
	}
	catch (EvalException& e) {
		e.printEvalErr();
	}
}

bool isVarSignNegative(const std::string& s) {	
	return s[0] == '-';
}

void Evaluator::handleAssignRoot() {

	auto ASTRootAssignNode = dynamic_cast<BinaryExprNode*>(ASTRoot.get());	//it was already checked for it being an assignment binexprnode, so its safe to cast it

	auto rhs = ASTRootAssignNode->getRhs()->eval(evalCtx);	//evaluate right hand side
	auto lhs = ASTRootAssignNode->getLhs();

	if (lhs->type() != NodeType::Var) {	//check if lhs is variable, if not then its probably binexprnode, which means somebody is doing 'a=b=5' or something like that
		throw EvalException("Incorrect assignment syntax","Only one part assignments are allowed, like 'a=5'",lhs, rhs,'=');
	}

	auto lhsVar = dynamic_cast<VariableExprNode*>(lhs.get());	//treat it like varexprnode
	std::string varName = lhsVar->getName();

	if (isVarSignNegative(varName)) {	//do not allow assigning to negative variables
		//display a helpful message that converts illegal expression to a legal, and still correct one
		// '-g=5' => 'g=-5'
		std::string noteStr = "Instead, do: " + varName.erase(0, 1) + " = ";
		rhs.flipSign();
		noteStr += rhs.toString();

		throw EvalException("Do not assign values to negative variables", noteStr);
	}

	//assign variable
	if (evalCtx.varExists(varName)) {
		evalCtx.assignVar(varName, rhs);
	}
	else evalCtx.vars.push_back({ varName,std::move(rhs) });
	
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
	case OperatorType::divide: throw EvalException("division not yet supported","dont use it please",a,b,'/'); break;	//TODO: USE DIVISION ONCE BIGINT DIVISION IS DONE
	case OperatorType::exponentiate: throw EvalException("exponentiation not yet supported", "dont use it please", a, b, '^'); break;	//TODO: EXPONENTIATION
	case OperatorType::assign: throw EvalException("Assignment caught in the middle of an eval", "Assignment here is only one-part like 'a=5'", a, b, '='); break;	//TODO: ASSIGNMENT
	case OperatorType::undefined: throw EvalException("Unkown operation", "avalible operations are: +,-,*,/,^", a, b, '?'); break;
	}

	return a;
}

BigInt VariableExprNode::eval(EvalCtx& ectx) {
	std::string name = getName();
	BigInt ret = ectx.getVar(name);
	if (name[0] == '-') {		//if somebody types in -g, it should be recognized as negative g, not '-g' variable
		name.erase(0,1);
		ret = ectx.getVar(name);
		ret.flipSign();
	}
	return ret;
}

BigInt CallExprNode::eval(EvalCtx& ectx) {
	return BigInt(0);	//TODO: MAKE FUNCTION EXECUTE
}
