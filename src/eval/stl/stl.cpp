#include "stl.hpp"

using ExprNodes = std::vector<std::unique_ptr<ExprNode>>;


void strToLower(std::string& s) {	//converts string to lowercase
	for (char& c : s) {
		c = tolower(c);
	}
}

chunkDisplayMode getChunkDisplayModeFromBigInt(BigInt& bi) {

	if(bi.equals(BigInt(10))) return chunkDisplayMode::decimal;
	if (bi.equals(BigInt(16))) return chunkDisplayMode::hex;
	
	return chunkDisplayMode::decimal;
}

namespace stlFuncs {

	void inspect(ExprNodes& args, EvalCtx& eCtx) {
		chunkDisplayMode cdm = chunkDisplayMode::decimal;
		if (args.size() > 2) throw EvalException("", "");
		if (args.size() == 0) throw EvalException("", "");

		if (args.size() == 2) {
			BigInt cdmVar = args[1]->eval(eCtx);
			cdm = getChunkDisplayModeFromBigInt(cdmVar);
		}

		BigInt var = args[0]->eval(eCtx);

		var.inspectChunks(cdm);
		eCtx.shouldPrint = false;
	}

	void showFunctions(ExprNodes& args, EvalCtx& eCtx) {



	}

	void help(ExprNodes& args, EvalCtx& eCtx) {

	}

	void showVars(ExprNodes& args, EvalCtx& eCtx) {
		for (auto& var : eCtx.vars) {
			std::cout << var.first << " = " << var.second.toString() << "\n";
		}
		eCtx.shouldPrint = false;
	}

	BigInt abs(ExprNodes& args, EvalCtx& eCtx) {
		
		if (args.size() != 1) throw EvalException("", "");

		BigInt var = args[0]->eval(eCtx);
		if (var.isNegative()) {
			var.flipSign();
		}
		return var;
	}
}

std::optional<BigInt> stlDispatch(std::string& funcName,ExprNodes& args, EvalCtx& eCtx) {

	strToLower(funcName);

	if (funcName == "inspect") stlFuncs::inspect(args, eCtx);
	if (funcName == "showFunctions") stlFuncs::showFunctions(args, eCtx);
	if (funcName == "showvars") stlFuncs::showVars(args, eCtx);
	if (funcName == "help") stlFuncs::help(args, eCtx);
	if (funcName == "abs") return stlFuncs::abs(args, eCtx);


	return std::nullopt;
}