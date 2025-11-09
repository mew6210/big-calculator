#include "parser.hpp"

using std::unique_ptr;
using std::make_unique;

OperatorType tokenToOper(const Token& tok) {
	switch (tok.type) {
	case TokenType::plusSign: return OperatorType::add;
	case TokenType::minusSign: return OperatorType::subtract;
	case TokenType::MultipSign: return OperatorType::multiply;
	case TokenType::DivSign: return OperatorType::divide;
	default: return OperatorType::undefined;
	}
}

Token Parser::getNextToken() {
	if (curTokIndex == tokens.size()-1) {
		return curTok;
	}
	curTokIndex++;
	return curTok = tokens[curTokIndex];
}

unique_ptr<ExprNode> Parser::parseNumberExpr() {
	auto result = make_unique<BigIntNode>(curTok);
	getNextToken(); //eat token
	return std::move(result);
}
/*
	@brief handles `( expression)`

*/
unique_ptr<ExprNode> Parser::parseParenExpr() {

	getNextToken(); //eat (
	
	auto v = parseExpression();		//handle expression inside ()
	if (!v) return nullptr;
	
	if (curTok.type != TokenType::closeParen) std::cout << "TODO ERROR `EXPECTED ) `";

	getNextToken(); //eat )
	return v;
}

/*
	@brief handles either `identifier` or `identifier(expression)`

*/
unique_ptr<ExprNode> Parser::parseIdentifierExpr() {

	Token idName = curTok;
	getNextToken(); //eat name

	if (curTok.type != TokenType::openParen) //if next token is not (
		return make_unique<VariableExprNode>(idName.value);	//then its a variable

	//else its a function call

	getNextToken(); //eat (
	std::vector<Token> args;
	//TODO: FINISH FUNCTION PARSING WHEN ADDED COMMA TO A LEXER
	getNextToken();	//eat )

	return make_unique<CallExprNode>(idName, args);
}

unique_ptr<ExprNode> Parser::parsePrimary() {
	switch (curTok.type) {
	case TokenType::identifier: return parseIdentifierExpr();
	case TokenType::numLiteral: return parseNumberExpr();
	case TokenType::openParen: return parseParenExpr();
	default: std::cout << "TODO MAKE ERROR FOR UNKOWN TOKEN";
	}
}

int Parser::getTokPrecedence() {

	int tokPrecedence = precedenceMap[curTok.type];
	if (tokPrecedence <= 0) return -1;
	return tokPrecedence;
}

unique_ptr<ExprNode> Parser::parseExpression() {
	auto lhs = parsePrimary();
	if (!lhs) return nullptr;

	return parseBinOpRHS(0, std::move(lhs));
}

unique_ptr<ExprNode> Parser::parseBinOpRHS(int exprPrec, unique_ptr<ExprNode> lhs){

	while (true) {
		int tokPrec = getTokPrecedence();

		if (tokPrec < exprPrec) {
			return lhs;
		}

		OperatorType binOp = tokenToOper(curTok);
		getNextToken();
		auto rhs = parsePrimary();
		if (!rhs) return nullptr;

		int nextPrec = getTokPrecedence();
		if (tokPrec < nextPrec) {
			rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
			if (!rhs) return nullptr;
		}
		lhs = make_unique<BinaryExprNode>(std::move(lhs), std::move(rhs), binOp);
	}
}

unique_ptr<ExprNode> Parser::parseTopLevelExpr() {
	auto e = parseExpression();
	return e;
}

unique_ptr<ExprNode> Parser::parse() {
	
	while (true) {
		switch (curTok.type) {
		default: return parseTopLevelExpr();
		}
	}
}