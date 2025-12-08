#include "evalctx.hpp"
#include "../evalException.hpp"

//get var value by name
BigInt EvalCtx::getVar(const std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) {
			return var.second;
		}
	}

	std::string errStr = name + " is undefined";
	std::string noteStr = "Define " + name;
	throw EvalException{ errStr,noteStr };
	return BigInt(0);
}

//check if var exists by name
bool EvalCtx::varExists(std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) return true;
	}
	return false;
}

//assign var by name and value
void EvalCtx::assignVar(std::string& name, BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
		};
	}
}