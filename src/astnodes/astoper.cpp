#include "astoper.hpp"

/*
	@brief helper function for narrowing tokens used for operations to a specific OperatorType enum
*/
OperatorType tokenToOper(const Token& tok) {
	switch (tok.type) {
	case TokenType::assignOp: return OperatorType::assign;
	case TokenType::plusSign: return OperatorType::add;
	case TokenType::minusSign: return OperatorType::subtract;
	case TokenType::multipSign: return OperatorType::multiply;
	case TokenType::divSign: return OperatorType::divide;
	case TokenType::equalKeyword: return OperatorType::equal;
	case TokenType::notEqualKeyword: return OperatorType::notEqual;
	default: return OperatorType::undefined;
	}
}