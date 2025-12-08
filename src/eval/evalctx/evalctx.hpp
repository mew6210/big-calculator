#pragma once
#include <vector>
#include "../../bigint/bigint.hpp"
#include <string>

/*
	@brief struct for keeping information current evaluation "runtime"
*/
struct EvalCtx {
	std::vector<std::pair<std::string, BigInt>> vars;
	bool isAssignExpr;

	BigInt getVar(const std::string& name);
	bool varExists(std::string& name);
	void assignVar(std::string& name, BigInt& bi);
};