#include "stl.hpp"
#include "funcinfo.hpp"
#include "funcreturn.hpp"
#include <algorithm>
#include <fstream>
#include <cstdio>

using ExprNodes = std::vector<std::unique_ptr<ExprNode>>;

namespace helpers {

	void printFuncLabel(const std::string& name) {

		const int totalWidth = 20;

		int dashCount = totalWidth - name.size();
		int left = dashCount / 2;
		int right = dashCount - left;

		std::cout << "<" << std::string(left, '-');
		std::cout << name;
		std::cout << std::string(right, '-') << ">\n";
	}

	void printStlFuncInfo(const stlFunc& func) {
		printFuncLabel(func.funcName);

		std::cout << "-DESCRIPTION\n";
		std::cout << func.funcDesc << "\n";
		std::cout << "-PARAMETERS\n";
		std::cout << func.paramDesc << "\n";
		std::cout << "-EXAMPLE\n";
		std::cout << func.exampleDesc << "\n";
		std::cout << "<----------------->\n\n";
	}

	uint64_t levenshteinDistance(const std::string& a, const std::string& b){
		const size_t n = a.size();
		const size_t m = b.size();

		if (n == 0) return m;
		if (m == 0) return n;

		if (m > n)
			return levenshteinDistance(b, a);

		std::vector<uint64_t> prev(m + 1), curr(m + 1);

		for (size_t j = 0; j <= m; ++j)
			prev[j] = j;

		for (size_t i = 1; i <= n; ++i){
			curr[0] = i;
			for (size_t j = 1; j <= m; ++j){
				uint64_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
				curr[j] = std::min({
					prev[j] + 1,        // deletion
					curr[j - 1] + 1,    // insertion
					prev[j - 1] + cost  // substitution
					});
			}
			std::swap(prev, curr);
		}

		return prev[m];
	}

	std::string userFuncToStr(const UserFunc& func) {
		std::string ret;
		ret += func.name;
		ret += "(";
		for (size_t i = 0; i < func.params.size(); i++) {
			ret += func.params[i];
			if (i != func.params.size() - 1) ret += ",";
		}
		ret += ")";
		ret += " = " + func.definition->toString();

		return ret;
	}

}

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


/*
	implementation of stl functions, all of them based on bigint implementation
*/
namespace stlFuncs {

	//void
	funcReturn inspect(ExprNodes& args, EvalCtx& eCtx) {
		chunkDisplayMode cdm = chunkDisplayMode::decimal;
		if (args.size() > 2) throw EvalException("Too many arguments in inspect()", "Check out \"?inspect()\" to see the correct function parameters");
		if (args.size() == 0) throw EvalException("Expected at least 1 argument in inspect()", "Check out \"?inspect()\" to see the correct function parameters");

		if (args.size() == 2) {
			BigInt cdmVar = args[1]->eval(eCtx);
			cdm = getChunkDisplayModeFromBigInt(cdmVar);
		}

		BigInt var = args[0]->eval(eCtx);

		var.inspectChunks(cdm);
		eCtx.shouldPrint = false; //dont print result, which is 0
		return { BigInt(0),false }; //dont return anything
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
			"\tIf you want to check all avalible functions and their documentation, type \"showFunctions()\"\n"
			"NEXT STEPS\n"
			"\tPlay around, have fun and maybe try to break it, im curious how it behaves under user unpredictability\n"
			"\tType \"showFunctions()\" to check out other features\n"
			"\tAlso if you are unsure about some function, u can write \"?\" at the start of it to get its description,like:\n"
			"\t?help()\n"
			;
		eCtx.shouldPrint = false; //dont print result, which is 0
		return { BigInt(0),false }; //dont return anything
	}

	//void
	funcReturn showVars(ExprNodes& args, EvalCtx& eCtx) {
		for (auto& var : eCtx.vars) {
			std::cout << var.first << " = " << var.second.toString() << "\n";
		}
		eCtx.shouldPrint = false; //dont print result, which is 0
		return { BigInt(0),false }; //dont return anything
	}

	funcReturn abs(ExprNodes& args, EvalCtx& eCtx) {
		
		if (args.size() != 1) throw EvalException("Too many arguments in abs(), expected 1", "Check out \"?abs()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);
		if (var.isNegative()) {
			var.flipSign();
		}
		return funcReturn{ var ,true};
	}

	funcReturn max(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in max(), expected 2", "Check out \"?max()\" to see the correct function parameters");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);
		
		if (var1.biggerThan(var2)) return funcReturn { var1,true };
		if (var2.biggerThan(var1)) return funcReturn{ var2, true };
		return funcReturn{ var1,true };
	}

	funcReturn min(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in min(), expected 2", "Check out \"?min()\" to see the correct function parameters");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);

		if (var1.biggerThan(var2)) return funcReturn{ var2,true };
		if (var2.biggerThan(var1)) return funcReturn{ var1, true };
		return funcReturn{ var1,true };
	}

	//void
	funcReturn cmp(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in cmp(), expected 2", "Check out \"?cmp()\" to see the correct function parameters");

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

		eCtx.shouldPrint = false; //dont print result, which is 0
		return funcReturn{ var1,false }; //dont return anything
	}

	funcReturn sum(ExprNodes& args, EvalCtx& eCtx) {

		BigInt sum = BigInt(0);

		for (auto& arg : args) {
			BigInt var = arg->eval(eCtx);
			sum.addBigInt(var);
		}

		return funcReturn{ sum,true };
	}

	funcReturn prod(ExprNodes& args, EvalCtx& eCtx) {

		BigInt sum = BigInt(1);

		for (auto& arg : args) {
			BigInt var = arg->eval(eCtx);
			sum.multiplyBigInt(var);
		}

		return funcReturn{ sum,true };
	}

	funcReturn cntDigits(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in cntDigits(), expected 1", "Check out \"?cntDigits()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		return funcReturn{ BigInt(var.toString().size()),true };
	}

	//void
	funcReturn showMyFunctions(ExprNodes& args, EvalCtx& eCtx) {

		for (const auto& func : eCtx.userFunctions) {

			std::string userFuncStr = helpers::userFuncToStr(func);
			std::cout << userFuncStr;
			std::cout << "\n";
		}
		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false };	//void
	}

	//void
	funcReturn execFile(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in execFile(), expected 1", "Check out \"?execFile()\" to see the correct function parameters");
		if (args[0]->type() != NodeType::Var) throw EvalException("Given argument has to be a variable", "Check out \"?execFile()\" to see the correct function parameters");

		auto varNode = dynamic_cast<VariableExprNode*>(args[0].get());

		std::string fileName = varNode->getName();
		eCtx.fileToExec = fileName;

		std::ifstream file(fileName);

		if (!file.good()) throw EvalException("No such file", fileName+" does not exist");

		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false };
	}

	//void
	funcReturn save(ExprNodes& args, EvalCtx& eCtx) {

		std::ofstream file("start.txt",std::ios::trunc); //clear file on open

		for (const auto& var : eCtx.vars) {
			file << var.first + " = " + var.second.toString()<<"\n";
		}

		for (const auto& func : eCtx.userFunctions) {
			file<<helpers::userFuncToStr(func);
			file<< "\n";
		}

		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false }; //file auto closed
	}

	//void
	funcReturn clearSave(ExprNodes& args, EvalCtx& eCtx) {

		int status = remove("start.txt");	//remove save file

		if (status==0) {
			std::cout << "Save successfully deleted\n";
		}
		else {
			std::cout << "Error removing save file\n";
		}

		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false }; 
	}

	funcReturn mod(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in mod(), expected 2", "Check out \"?mod()\" to see the correct function parameters");

		BigInt a = args[0]->eval(eCtx);
		BigInt b = args[1]->eval(eCtx);

		auto res = divideUnsigned(a, b);
		
		return funcReturn{ res.remainder,true};
	}

	/*
		@brief fast exponentiation algorithm
		to avoid multiplying n times, fast exponentiation algorithm is used
		source: https://math-sites.uncg.edu/sites/pauli/112/HTML/secfastexp.html
	*/
	funcReturn exp(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in exp(), expected 2", "Check out \"?exp()\" to see the correct function parameters");

		BigInt b = args[0]->eval(eCtx);
		BigInt n = args[1]->eval(eCtx);

		BigInt a = BigInt(1);

		BigInt c = b;

		while (!n.isZero()) {
			BigInt r = divideUnsigned(n, BigInt(2)).remainder;

			if (r.equals(BigInt("1"))) {
				a.multiplyBigInt(c);
			}
			BigInt temp = BigInt(2);
			n.divideBigInt(temp,true);

			c.multiplyBigInt(c);
		}
		return funcReturn{ a,true };
	}
	
	funcReturn lexerOutput(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in lexerOutput(), expected 1", "Check out \"?lexerOutput()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		if(var.equals(BigInt(1))) eCtx.showLexerOutput = true;
		if (var.isZero()) eCtx.showLexerOutput = false;

		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false };
	}


	funcReturn parserOutput(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in parserOutput(), expected 1", "Check out \"?parserOutput()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		if (var.equals(BigInt(1))) eCtx.showParserOutput = true;
		if (var.isZero()) eCtx.showParserOutput = false;

		eCtx.shouldPrint = false;
		return funcReturn{ BigInt(0),false };
	}

}

/*
	@brief functions for this app's standard library
	it contains:
	- name
	- description
	- parameters description
	- examplary usage
	- call to the corresponding function in stlFuncs namespace

*/
std::vector<stlFunc> stlFunctions = {
	{"inspect"
	,
	"\tDisplays detailed information about the value of an expression",
	"\tOne or two expressions, if one then only the inspected number, if 2 then first one is inspected number and 2nd one is base, which can be either 10 or 16",
	"\t\"inspect(5*1000000000000000000000000)\",displays chunks of a number 5000000000000000000000000\n\t\"inspect(3*100,16), displays chunks of a number 300 in hexadecimal form\"",
	stlFuncs::inspect
	},

	{"showFunctions" ,
	"\tDisplays all functions and how they work",
	"\tAny parameters are ignored",
	"\tYou probably already know how to use it if you see it",
	stlFuncs::showFunctions
	},

	{"showVars" ,
	"\tDisplays all declared variables",
	"\tAny parameters are ignored",
	"\t \"a=5\", \t\"showVars()\" ",
	stlFuncs::showVars
	},

	{"help" ,
	"\tDisplays a quick introduction",
	"\tAny parameters are ignored",
	"\t \"help()\"",
	stlFuncs::help
	},

	{"abs" ,
	"\tReturns an absolute value of a given expression",
	"\tOnly one parameter, any expression",
	"\t\"abs(3-5)\" returns 2",
	stlFuncs::abs
	},

	{"max" ,
	"\tReturns a bigger value",
	"\tTwo parameters, any expressions",
	"\t\"max(100,5*3)\" returns 100",
	stlFuncs::max
	},

	{"min" ,
	"\tReturns a smaller value",
	"\tTwo parameters, any expressions",
	"\t\"min(-5,100)\" returns -5",
	stlFuncs::min
	},

	{"cmp" ,
	"\tPrints out which value is bigger",
	"\tTwo parameters, any expressions",
	"\t\"cmp(5,3)\" prints out \"5 is bigger\"",
	stlFuncs::cmp
	},

	{"sum" ,
	"\tReturns a sum of all the given expressions",
	"\tAny amount of expressions",
	"\t\"sum(5,-5,3,4)\" returns 7",
	stlFuncs::sum
	},

	{"prod" ,
	"\tReturns a product of all the given expressions",
	"\tAny amount of expressions",
	"\t\"prod(5,3,4)\" returns 60",
	stlFuncs::prod
	},

	{"cntDigits" ,
	"\tReturns the amount of digits that a given value has in its decimal form",
	"\tOne parameter, any expression",
	"\t\"cntDigits(150)\" returns 3",
	stlFuncs::cntDigits
	},

	{"showMyFunctions",
	"\tShows user-defined functions",
	"\tNo parameters, they are ignored",
	"\t\"f(x) = x\n\t\"showMyFunctions()\"\" prints f(x) and its info",
	stlFuncs::showMyFunctions
	},

	{"execFile",
	"\tExecutes a given file line by line",
	"\tOnly one parameter, has to be a variable, which is the name to the file, including extension",
	"\t\"execFile(test1.txt)\" executes contents of a \"test.txt\" file",
	stlFuncs::execFile
	},

	{"save",
	"\tSaves variables and functions to a file, that will be loaded at the next startup",
	"\tNone, any are ignored",
	"",
	stlFuncs::save
	},
	{"clearSave",
	"\tDeletes the save file made by \"save()\"",
	"\tNone, any are ignored",
	"",
	stlFuncs::clearSave
	},
	
	{"mod",
	"\tReturns remainder of the division between 2 parameters",
	"\tTwo parameters, any expressions",
	"\t\"mod(5,3)\" returns 2, because its the remainder of division 5 / 3",
	stlFuncs::mod
	},
	
	{"exp",
	"\tReturns first parameter exponentiated to second parameter",
	"\tTwo parameters, any expressions",
	"\t\"exp(2,3)\" returns 8 becase 2^3",
	stlFuncs::exp
	},
	
	{ "lexerOutput",
	"\tToggles if lexer output should be printed to the console",
	"\tOnly one parameter, any expression that evaluates to either 0 or 1, if its something else it ignores it",
	"\t\"lexerOutput(1)\" now lexer output will be shown in console",
	stlFuncs::lexerOutput
	},

	{ "parserOutput",
	"\tToggles if parser output should be printed to the console",
	"\tOnly one parameter, any expression that evaluates to either 0 or 1, if its something else it ignores it",
	"\t\"parserOutput(1)\" now parser output will be shown in console",
	stlFuncs::parserOutput
	}

};

/*
	showfunctions needs to be added later to the namespace, since it references stlFunctions
*/
namespace stlFuncs {
	//void
	funcReturn showFunctions(ExprNodes& args, EvalCtx& eCtx) {
		
		for (auto& func : stlFunctions) helpers::printStlFuncInfo(func);
		eCtx.shouldPrint = false;	//dont print result, which is 0
		return funcReturn{ BigInt(0),false };	//dont return anything
	}
}

void handleFuncNotFound(std::string& funcName) {

	using Distance = std::pair<std::string, uint64_t>;
	std::vector<Distance> distances;

	//check how similiar current stl function names are to user's input
	for (auto& func : stlFunctions) {
		distances.push_back({ func.funcName,helpers::levenshteinDistance(funcName,func.funcName) });
	}

	//sort so that first element is the most likely
	std::sort(distances.begin(), distances.end(), [](Distance& distance1, Distance& distance2) {
		return distance1.second < distance2.second;
		});


	std::string errNote;
	if (distances[0].second < 3) {	//if its pretty similiar, tell user the most probable match
		errNote = "Did you mean " + distances[0].first + "()?";
	}
	//if it isnt that similiar, dont tell user the closest match
	else errNote = "Use \"showFunctions()\" for all avalible functions";

	throw EvalException("\"" + funcName + "()\" function not found", errNote);
}

std::optional<BigInt> funcDispatch(std::string& funcName,ExprNodes& args, EvalCtx& eCtx) {
	
	//check for stl functions
	for (auto& func : stlFunctions) {
		/*
		look through each stl function, and if found return its value
		if func doesnt return a value, return std::nullopt so that <optional> is empty
		*/
		if (funcName == func.funcName) { 
			auto res = func.call(args, eCtx);
			if (res.hasValue) return res.value;
			else return std::nullopt;
		}

		/*
		if name of the function starts with "?",
		user is asking about func info, not evaluation
		*/
		else if (funcName == "?" + func.funcName) {
			helpers::printStlFuncInfo(func);
			eCtx.shouldPrint = false;
			return std::nullopt;
		}
	}

	//check for user functions
	for (auto& func : eCtx.userFunctions) {

		if (funcName == func.name) {
			
			if (func.params.size() != args.size()) throw EvalException("Argument mismatch, " "function " + func.name + " expects " + std::to_string(func.params.size()) + " arguments, but " + std::to_string(args.size()) + " were provided","Check out ?"+func.name+"()");

			EvalCtx newEvalCtx{};
			std::vector<std::pair<std::string,BigInt>> vars;
			
			for (size_t i = 0; i < func.params.size();i++) {
				vars.push_back({ func.params[i],args[i]->eval(eCtx) });
			}
			
			newEvalCtx.vars = vars;
			newEvalCtx.userFunctions = std::move(eCtx.userFunctions);
			auto val = func.definition->eval(newEvalCtx);
			eCtx.userFunctions = std::move(newEvalCtx.userFunctions);
			return val;
		}

	}
	for (auto& func : eCtx.userFunctions) {
		if (funcName == "?" + func.name) {
			std::cout << func.definition->toString()<<"\n";
			eCtx.shouldPrint = false;
			return std::nullopt;
		}
	}


	//by this point, stl function was not found
	handleFuncNotFound(funcName);

	return std::nullopt;
}