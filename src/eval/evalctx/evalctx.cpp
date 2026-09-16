#include "evalctx.hpp"

#include <algorithm>

#include "../evalException.hpp"


std::optional<BigInt> EvalCtx::findVar(const std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) {
			return var.second;
		}
	}

	if (parentCtx) {
		return parentCtx->findVar(name);
	}

	return std::nullopt;
}

//get var value by name
BigInt EvalCtx::getVar(const std::string& name) {
	auto biVar = findVar(name);
	if (biVar.has_value())
		return biVar.value();


	std::string errStr = name + " is undefined";
	std::string noteStr = "Define " + name;
	throw EvalException{ errStr,noteStr };
	return {0};
}

//check if var exists by name
bool EvalCtx::varExists(std::string& name) {
	bool isLocal = std::ranges::any_of(vars,
		[name](const std::pair<std::string,BigInt>& p){return p.first == name;});

	if (isLocal) return isLocal;
	if (parentCtx) parentCtx->varExists(name);

	return false;
}

//assign var by name and value
void EvalCtx::assignVar(std::string& name, BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
			return;
		};
	}

	if (parentCtx) parentCtx->assignVar(name,bi);
}

//TODO: implement parentCtx recursion, as in functions above
bool EvalCtx::funcExists(std::string& name) {
	return std::ranges::any_of(userFunctions,[name](UserFunc& func){return func.name == name;});
}

//TODO: implement parentCtx recursion, as in functions above
void EvalCtx::assignFunc(UserFunc& userFunc) {

	for (auto& func : userFunctions) {
		if (func.name == userFunc.name) {
			func.params = std::move(userFunc.params);
			func.definition = std::move(userFunc.definition);
		}
	}

}

void EvalCtx::setParent(EvalCtx& eCtx) {
	parentCtx = &eCtx;
}
