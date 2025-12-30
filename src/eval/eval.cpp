#include "eval.hpp"
#include "../astnodes/astnodes.hpp"
#include "../logging/logging.hpp"
#include <optional>
#include "evalException.hpp"
#include "stl/stl.hpp"

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
			if(evalCtx.shouldPrint) res.print();
			evalCtx.shouldPrint = true;
		}
	}
	catch (EvalException& e) {
		e.printEvalErr();
	}
}

bool isVarSignNegative(const std::string& s) {	
	return s[0] == '-';
}

void Evaluator::handleAssignVar(BinaryExprNode* ASTRootAssignNode) {	//dont need to free this pointer, since its not owning
	auto rhs = ASTRootAssignNode->getRhs()->eval(evalCtx);	//evaluate right hand side
	auto lhs = ASTRootAssignNode->getLhs();
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

void Evaluator::handleAssignUserFunc(BinaryExprNode* ASTRootAssignNode) {

	auto lhs = ASTRootAssignNode->getLhs();
	//by this point lhs has to be CallExpr
	auto lhsCall = dynamic_cast<CallExprNode*>(lhs.get());
	std::string name = lhsCall->getName();
	auto args = lhsCall->getArgs();

	std::vector<std::string> params;

	//each arg has to be a variable
	for (auto& arg : args) {
		if (arg->type() != NodeType::Var) throw EvalException("Function argument has to be a variable", "You provided: "+arg->toString());
		auto argVar = dynamic_cast<VariableExprNode*>(arg.get());
		params.push_back(argVar->getName());
	}
	auto definition = ASTRootAssignNode->getRhs();
	UserFunc eFunc = { name,params,std::move(definition) };

	if (evalCtx.funcExists(name)) {
		evalCtx.assignFunc(eFunc);
	}
	else evalCtx.userFunctions.push_back(std::move(eFunc));		//need to std::move eFunc cause it has std::unique_ptr
	
}

void Evaluator::handleAssignRoot() {

	auto ASTRootAssignNode = dynamic_cast<BinaryExprNode*>(ASTRoot.get());	//it was already checked for it being an assignment binexprnode, so its safe to cast it

	auto lhsType = ASTRootAssignNode->lhsType();

	if (lhsType != NodeType::Var && lhsType != NodeType::CallExpr) {	//its only legal to assign variables or functions to something
		auto lhs = ASTRootAssignNode->getLhs();
		auto rhs = ASTRootAssignNode->getRhs()->eval(evalCtx);	//evaluate right hand side
		throw EvalException("Incorrect assignment syntax","Only one part assignments are allowed, like 'a=5'",lhs, rhs,'=');
	}

	if (lhsType == NodeType::Var) {
		handleAssignVar(ASTRootAssignNode);
	}
	if (lhsType == NodeType::CallExpr) {
		handleAssignUserFunc(ASTRootAssignNode);
	}
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
	
	auto var = funcDispatch(funcName, args, ectx);

	if (var) return var.value();
	else return BigInt(0);

}
