#include "evalctx.hpp"

#include <algorithm>

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
	return {0};
}

//check if var exists by name
bool EvalCtx::varExists(std::string& name) {

	return std::ranges::any_of(vars,
		[name](const std::pair<std::string,BigInt>& p){return p.first == name;});
}

//assign var by name and value
void EvalCtx::assignVar(std::string& name, BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
		};
	}
}

bool EvalCtx::funcExists(std::string& name) {
	return std::ranges::any_of(userFunctions,[name](UserFunc& func){return func.name == name;});
}

void EvalCtx::assignFunc(UserFunc& userFunc) {

	for (auto& func : userFunctions) {
		if (func.name == userFunc.name) {
			func.params = std::move(userFunc.params);
			func.definition = std::move(userFunc.definition);
		}
	}

}