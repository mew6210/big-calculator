#pragma once
#include <vector>
#include "../../bigint/bigint.hpp"
#include <string>

struct UserFunc;

/*
	@brief struct for keeping information current evaluation "runtime"
*/
struct EvalCtx {
	std::vector<std::pair<std::string, BigInt>> vars;
	std::string fileToExec="";
	bool shouldPrint = true;
	std::vector<UserFunc> userFunctions;
	bool showLexerOutput = false;
	bool showParserOutput = false;

	BigInt getVar(const std::string& name);
	bool varExists(std::string& name);
	void assignVar(std::string& name, BigInt& bi);

	bool funcExists(std::string& name);
	void assignFunc(UserFunc& func);
};

#include "../userfunc/userfunc.hpp"