#include "stl.hpp"
#include "funcinfo.hpp"
#include "funcreturn.hpp"
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

	//void
	funcReturn inspect(ExprNodes& args, EvalCtx& eCtx) {
		chunkDisplayMode cdm = chunkDisplayMode::decimal;
		if (args.size() > 2) throw EvalException("Too many arguments in inspect()", "Check out \"showfunctions()\" to see the correct function parameters");
		if (args.size() == 0) throw EvalException("Expected at least 1 argument in inspect()", "Check out \"showfunctions()\" to see the correct function parameters");

		if (args.size() == 2) {
			BigInt cdmVar = args[1]->eval(eCtx);
			cdm = getChunkDisplayModeFromBigInt(cdmVar);
		}

		BigInt var = args[0]->eval(eCtx);

		var.inspectChunks(cdm);
		eCtx.shouldPrint = false;
		return { BigInt(0),false };
	}

	//void
	funcReturn showFunctions(ExprNodes& args, EvalCtx& eCtx);

	//void
	funcReturn help(ExprNodes& args, EvalCtx& eCtx) {

		std::cout << 
			"SHORT DESCRIPTION\n"
			"\tA python-like REPL(read-eval-print loop) calculator for integers of theoretically any size\n\n"
			"LONG DESCRIPTION\n"
			"\tThis app consists of 2 parts:\n"
			"\t-- Frontend layer:\n"
			"\tUser-interface that you are currently interacting with \n"
			"\tLexes, parses and interprets user input\n"
			"\tIt behaves like a frontend of a interpreter or a compiler\n"
			"\n\t-- Backend layer:\n"
			"\tBigInt implementation that handles all the calculations on arbitrary-sized integers\n" 
			"\tInternally it uses a vector of unsigned 64-bit integers, and a bool for the sign\n"
			"EXAMPLES\n"
			"\tBasic expressions are handled\n"
			"\t\"2 + 3\" returns 5\n\n"
			"\tIt also internally understands operator precedence\n"
			"\t\"2+3*6\" returns 20, not 30\n\n"
			"\tVariables are also here\n"
			"\t\"a = 5\" assigns a to 5, and can be reused later like so \"a + 2\" returns 7\n\n"
			"\tParenthesis are also supported\n"
			"\tFunctions are also supported\n" 
			"\tIf you want to check all avalible functions and their documentation, type \"showfunctions()\"\n"
			"NEXT STEPS\n"
			"\tPlay around, have fun and maybe try to break it, im curious how it behaves under user unpredictability\n"
			"\tType \"showfunctions()\" to check out other features\n"
			;
		eCtx.shouldPrint = false;
		return { BigInt(0),false };
	}

	//void
	funcReturn showVars(ExprNodes& args, EvalCtx& eCtx) {
		for (auto& var : eCtx.vars) {
			std::cout << var.first << " = " << var.second.toString() << "\n";
		}
		eCtx.shouldPrint = false;
		return { BigInt(0),false };
	}

	funcReturn abs(ExprNodes& args, EvalCtx& eCtx) {
		
		if (args.size() != 1) throw EvalException("Too many arguments in abs(), expected 1", "Check out \"showfunctions()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);
		if (var.isNegative()) {
			var.flipSign();
		}
		return funcReturn{ var ,true};
	}

	funcReturn max(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("", "");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);
		
		if (var1.biggerThan(var2)) return funcReturn { var1,true };
		if (var2.biggerThan(var1)) return funcReturn{ var2, true };
		return funcReturn{ var1,true };
	}

	funcReturn min(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("", "");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);

		if (var1.biggerThan(var2)) return funcReturn{ var2,true };
		if (var2.biggerThan(var1)) return funcReturn{ var1, true };
		return funcReturn{ var1,true };
	}

	//void
	funcReturn cmp(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("", "");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);

		
		if (var1.biggerThan(var2)){
			std::cout << args[0]->toString() << " is bigger\n";
		}
		else if (var2.biggerThan(var1)){
			std::cout << args[1]->toString() << " is bigger\n";
		}
		else if (var1.equals(var2)) {
			std::cout << args[0]->toString() << " is equal " << args[1]->toString();
		}

		eCtx.shouldPrint = false;
		return funcReturn{ var1,false };

	}
}

std::vector<stlFunc> stlFunctions = {
	{"inspect","","","",stlFuncs::inspect},
	{"showFunctions","","","",stlFuncs::showFunctions},
	{"showVars","","","",stlFuncs::showVars},
	{"help","","","",stlFuncs::help},
	{"abs","","","",stlFuncs::abs},
	{"max","","","",stlFuncs::max},
	{"min","","","",stlFuncs::min},
	{"cmp","","","",stlFuncs::cmp}

};

/*
	showfunctions needs to be added later to the namespace, since it references stlFunctions
*/
namespace stlFuncs {
	funcReturn showFunctions(ExprNodes& args, EvalCtx& eCtx) {
		return funcReturn{ BigInt(0),false };
	}
}

std::optional<BigInt> stlDispatch(std::string& funcName,ExprNodes& args, EvalCtx& eCtx) {

	//strToLower(funcName);

	for (auto& func : stlFunctions) {
		if (funcName == func.funcName) { 
			auto res = func.call(args, eCtx);
			if (res.hasValue) return res.value;
			else return std::nullopt;
		}
	}
	return std::nullopt;
}