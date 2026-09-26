#include "eval.hpp"
#include "../astnodes/astnodes.hpp"
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

BigInt Evaluator::eval() {
	try {
		if (isRootAssign(ASTRoot)) {	//if its an assignment, treat it like so
			handleAssignRoot();
			evalCtx.shouldPrint = false;
			return {0};
		}
		else {
			BigInt res = ASTRoot->eval(evalCtx);	//otherwise treat it like a basic evaluation, no variable assigning
			return res;	
		}
	}
	catch (EvalException& e) {
		e.printEvalErr();
		evalCtx.shouldPrint = false;
		return {0};
	}
}

bool isVarSignNegative(const std::string& s) {	
	return s[0] == '-';
}

void Evaluator::handleAssignVar(BinaryExprNode* ASTRootAssignNode) {	//dont need to free this pointer, since its not owning
	auto rhs = ASTRootAssignNode->getRhs();	//evaluate right hand side
	auto rhValue = rhs->eval(evalCtx);
	auto lhs = ASTRootAssignNode->getLhs();
	auto lhsVar = dynamic_cast<VariableExprNode*>(lhs.get());	//treat it like varexprnode
	std::string varName = lhsVar->getName();

	if (isVarSignNegative(varName)) {	//do not allow assigning to negative variables
		//display a helpful message that converts illegal expression to a legal, and still correct one
		// '-g=5' => 'g=-5'
		std::string noteStr = "Instead, do: " + varName.erase(0, 1) + " = ";
		rhValue.flipSign();
		noteStr += rhValue.toString();

		throw EvalException("Do not assign values to negative variables", noteStr);
	}

	//assign variable
	if (evalCtx.varExists(varName)) {
		evalCtx.assignVar(varName, rhValue);
	}
	else evalCtx.vars.push_back({ varName,std::move(rhValue) });

	ASTRootAssignNode->rhs = std::move(rhs);
	ASTRootAssignNode->lhs = std::move(lhs);
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