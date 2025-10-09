#include "bigint.hpp"


BigInt::BigInt(const std::string& s) {
	chunks = {};
	isPositive = true;
}