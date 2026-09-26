#pragma once
#include "../lexer/token/token.hpp"

enum class OperatorType {
	assign,
	add,
	subtract,
	multiply,
	divide,
	undefined,
	equal,
	notEqual,
	smallerThan,
	biggerThan,
	biggerOrEqualThan,
	smallerOrEqualThan,
	andOp,
	orOp
};

/*
	@brief helper function for narrowing tokens used for operations to a specific OperatorType enum
*/
OperatorType tokenToOper(const Token& tok);