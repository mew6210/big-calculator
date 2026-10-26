#include "stl.hpp"
#include "funcinfo.hpp"
#include "funcreturn.hpp"
#include <algorithm>
#include <fstream>
#include <cstdio>

using ExprNodes = std::vector<std::unique_ptr<ExprNode>>;

namespace helpers {

	void printFuncLabel(const std::string& name) {
		constexpr size_t totalWidth = 20;

		const size_t dashCount = totalWidth - name.size();
		const size_t left = dashCount / 2;
		const size_t right = dashCount - left;

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
		ret += '(';
		for (size_t i = 0; i < func.params.size(); i++) {
			ret += func.params[i];
			if (i != func.params.size() - 1) ret += ',';
		}
		ret += ')';
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

	funcReturn voidReturn() {
		return {.value = BigInt(0),.hasValue =false};
	}

	//void
	funcReturn inspect(const ExprNodes& args, EvalCtx& eCtx) {
		chunkDisplayMode cdm = chunkDisplayMode::decimal;
		if (args.size() > 2) throw EvalException("Too many arguments in inspect()", "Check out \"?inspect()\" to see the correct function parameters");
		if (args.empty()) throw EvalException("Expected at least 1 argument in inspect()", "Check out \"?inspect()\" to see the correct function parameters");

		if (args.size() == 2) {
			BigInt cdmVar = args[1]->eval(eCtx);
			cdm = getChunkDisplayModeFromBigInt(cdmVar);
		}

		const BigInt var = args[0]->eval(eCtx);

		var.inspectChunks(cdm);
		eCtx.shouldPrint = false; //dont print result, which is 0
		return voidReturn(); //dont return anything
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
		return voidReturn(); //dont return anything
	}

	//void
	funcReturn showVars(ExprNodes& args, EvalCtx& eCtx) {
		for (auto& [name, value] : eCtx.vars) {
			std::cout << name << " = " << value.toString() << "\n";
		}
		eCtx.shouldPrint = false; //dont print result, which is 0
		return voidReturn(); //dont return anything
	}

	funcReturn abs(const ExprNodes& args, EvalCtx& eCtx) {
		
		if (args.size() != 1) throw EvalException("Too many arguments in abs(), expected 1", "Check out \"?abs()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);
		if (var.isNegative()) {
			var.flipSign();
		}
		return funcReturn{ .value = var ,.hasValue = true};
	}

	funcReturn max(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in max(), expected 2", "Check out \"?max()\" to see the correct function parameters");

		BigInt var1 = args[0]->eval(eCtx);
		BigInt var2 = args[1]->eval(eCtx);
		
		if (var1.biggerThan(var2)) return funcReturn { var1,true };
		if (var2.biggerThan(var1)) return funcReturn{ var2, true };
		return funcReturn{ .value = var1,.hasValue = true };
	}

	funcReturn min(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in min(), expected 2", "Check out \"?min()\" to see the correct function parameters");

		const BigInt var1 = args[0]->eval(eCtx);
		const BigInt var2 = args[1]->eval(eCtx);

		if (var1.biggerThan(var2)) return funcReturn{ var2,true };
		if (var2.biggerThan(var1)) return funcReturn{ var1, true };
		return funcReturn{ .value = var1,.hasValue = true };
	}

	//void
	funcReturn cmp(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in cmp(), expected 2", "Check out \"?cmp()\" to see the correct function parameters");

		const BigInt var1 = args[0]->eval(eCtx);
		const BigInt var2 = args[1]->eval(eCtx);


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
		return voidReturn(); //dont return anything
	}

	funcReturn sum(const ExprNodes& args, EvalCtx& eCtx) {

		BigInt sum = BigInt(0);

		for (auto& arg : args) {
			BigInt var = arg->eval(eCtx);
			sum.addBigInt(var);
		}

		return funcReturn{ .value = sum,.hasValue = true };
	}

	funcReturn prod(const ExprNodes& args, EvalCtx& eCtx) {

		BigInt sum = BigInt(1);

		for (const auto& arg : args) {
			BigInt var = arg->eval(eCtx);
			sum.multiplyBigInt(var);
		}

		return funcReturn{ .value = sum,.hasValue = true };
	}

	funcReturn cntDigits(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in cntDigits(), expected 1", "Check out \"?cntDigits()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		return funcReturn{ .value = BigInt(var.toString().size()),.hasValue = true };
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
	funcReturn execFile(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in execFile(), expected 1", "Check out \"?execFile()\" to see the correct function parameters");
		if (args[0]->type() != NodeType::Var) throw EvalException("Given argument has to be a variable", "Check out \"?execFile()\" to see the correct function parameters");

		auto varNode = dynamic_cast<VariableExprNode*>(args[0].get());

		std::string fileName = varNode->getName();
		eCtx.fileToExec = fileName;

		std::ifstream file(fileName);

		if (!file.good()) throw EvalException("No such file", fileName+" does not exist");

		eCtx.shouldPrint = false;
		return voidReturn();
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
		return voidReturn(); //file auto closed
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
		return voidReturn();
	}

	funcReturn mod(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in mod(), expected 2", "Check out \"?mod()\" to see the correct function parameters");

		BigInt a = args[0]->eval(eCtx);
		BigInt b = args[1]->eval(eCtx);

		auto [quotient, remainder] = divideUnsigned(a, b);
		
		return funcReturn{ .value = remainder,.hasValue = true};
	}

	/*
		@brief fast exponentiation algorithm
		to avoid multiplying n times, fast exponentiation algorithm is used
		source: https://math-sites.uncg.edu/sites/pauli/112/HTML/secfastexp.html
	*/
	funcReturn pow(const ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 2) throw EvalException("Wrong amount of arguments in pow(), expected 2", "Check out \"?pow()\" to see the correct function parameters");

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
		return funcReturn{ .value = a,.hasValue = true };
	}
	
	funcReturn lexerOutput(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in lexerOutput(), expected 1", "Check out \"?lexerOutput()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		if(var.equals(BigInt(1))) eCtx.showLexerOutput = true;
		if (var.isZero()) eCtx.showLexerOutput = false;

		eCtx.shouldPrint = false;
		return voidReturn();
	}

	//void
	funcReturn parserOutput(ExprNodes& args, EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in parserOutput(), expected 1", "Check out \"?parserOutput()\" to see the correct function parameters");

		BigInt var = args[0]->eval(eCtx);

		if (var.equals(BigInt(1))) eCtx.showParserOutput = true;
		if (var.isZero()) eCtx.showParserOutput = false;

		eCtx.shouldPrint = false;
		return voidReturn();
	}

	//void
	funcReturn print(ExprNodes& args,EvalCtx& eCtx) {

		if (args.size() != 1) throw EvalException("Wrong amount of arguments in print(), expected 1", "Check out \"?print()\" to see the correct function parameters");

		std::string literal = args[0]->eval(eCtx).getStringFromChunks();
		std::cout<<literal;
		std::cout<<"\n";
		eCtx.shouldPrint = false;
		return voidReturn();
	}

	funcReturn toString(ExprNodes& args,EvalCtx& eCtx) {
		BigInt ret;
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in toString(), expected 1", "Check out \"?toString()\" to see the correct function parameters");

		BigInt bi = args[0]->eval(eCtx);
		std::string strLiteral = bi.toString();
		ret.writeStringToChunks(strLiteral);

		return {.value = ret,.hasValue = true};
	}

	funcReturn strConcat(ExprNodes& args,EvalCtx& eCtx) {
		BigInt ret;
		if (args.size() != 2) throw EvalException("Wrong amount of arguments in strConcat(), expected 2", "Check out \"?strConcat()\" to see the correct function parameters");
		std::string a = args[0]->eval(eCtx).getStringFromChunks();
		a.pop_back(); //remove null character at the end
		std::string b = args[1]->eval(eCtx).getStringFromChunks();

		ret.writeStringToChunks(a+b);

		return {.value = std::move(ret),.hasValue = true};
	}

	funcReturn stoi(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in stoi(), expected 1", "Check out \"?stoi()\" to see the correct function parameters");
		std::string a = args[0]->eval(eCtx).getStringFromChunks();
		std::string parsedLiteral;

		for (const auto& c : a) {
			if (isdigit(c)) parsedLiteral+=c;
			else break;
		}
		BigInt ret = BigInt(parsedLiteral);

		return {.value = ret,.hasValue = true};
	}
	funcReturn fopen(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 2) throw EvalException("Wrong amount of arguments in fopen(), expected 2", "Check out \"?fopen()\" to see the correct function parameters");
		std::string path = args[0]->eval(eCtx).getStringFromChunks();
	    std::string mode = args[1]->eval(eCtx).getStringFromChunks();
		mode.pop_back(); //remove null terminator
		uint64_t handle = eCtx.createFileResource(path,mode);

		return {.value = BigInt(handle),.hasValue = true};
	}
	funcReturn fread(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fread(), expected 1", "Check out \"?fread()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		if (!eCtx.handles.contains(handle)) return voidReturn(); //TODO: PROBABLY MAKE IT LOOK BETTER
		auto fileHandle = reinterpret_cast<FileHandle*>(eCtx.handles[handle].get());
		std::string content = fileHandle->getContent();
		BigInt a;
		a.writeStringToChunks(content);

		return {.value = a,.hasValue = true};
	}
	funcReturn fclose(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fclose(), expected 1", "Check out \"?fclose()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		if (!eCtx.closeFileResource(handle)) {
			//TODO: NOTIFY ABOUT RESOURCE NOT EXISTING
		}

		return voidReturn();
	}
	funcReturn fgetline(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fgetline(), expected 1", "Check out \"?fgetline()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		std::string line = eCtx.getFileResourceNextLine(handle);
		BigInt ret = {};
		ret.writeStringToChunks(line);
		return {.value = ret, .hasValue = true};
	}
	funcReturn fhasline(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fhasline(), expected 1", "Check out \"?fhasline()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		bool hasNextLine = eCtx.hasFileResourceNextLine(handle);
		if (hasNextLine) {
			return {.value = BigInt(1), .hasValue = true};
		}
		else {
			return {.value = BigInt(0), . hasValue = true};
		}
	}

	funcReturn fgetchar(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fgetchar(), expected 1", "Check out \"?fgetchar()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		char c = eCtx.getFileResourceNextChar(handle);
		BigInt ret = {};
		ret.writeStringToChunks(std::string{c});
		return {.value = ret, .hasValue = true};
	}
	funcReturn fhaschar(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fhaschar(), expected 1", "Check out \"?fhaschar()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		bool hasNextChar = eCtx.hasFileResourceNextChar(handle);
		if (hasNextChar) {
			return {.value = BigInt(1), .hasValue = true};
		}
		else {
			return {.value = BigInt(0), . hasValue = true};
		}
	}
	funcReturn fresetcursor(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fresetcursor(), expected 1", "Check out \"?fresetcursor()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		eCtx.resetFileResourceCursor(handle);
		return voidReturn();
	}
	funcReturn fgetcursor(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 1) throw EvalException("Wrong amount of arguments in fgetcursor(), expected 1", "Check out \"?fgetcursor()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		uint64_t cursor = eCtx.getFileResourceCursor(handle);
		return {.value = BigInt(cursor), .hasValue = true};
	}

	funcReturn fwrite(ExprNodes& args, EvalCtx& eCtx) {
		if (args.size() != 2) throw EvalException("Wrong amount of arguments in fwrite(), expected 2", "Check out \"?fwrite()\" to see the correct function parameters");
		BigInt bHandle = args[0]->eval(eCtx);
		std::string str = args[1]->eval(eCtx).getStringFromChunks();
		str.pop_back(); //remove \0
		uint64_t handle = bHandle.getFirstChunk(); //TODO: MAKE SURE IT HAS ONE CHUNK, AND ONLY ONE
		eCtx.writeToFileResource(handle,str);

		return voidReturn();
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
	{.funcName = "inspect",
	.funcDesc = "\tDisplays detailed information about the value of an expression",
	.paramDesc = "\tOne or two expressions, if one then only the inspected number, if 2 then first one is inspected number and 2nd one is base, which can be either 10 or 16",
	.exampleDesc = "\t\"inspect(5*1000000000000000000000000)\",displays chunks of a number 5000000000000000000000000\n\t\"inspect(3*100,16), displays chunks of a number 300 in hexadecimal form\"",
	.call = stlFuncs::inspect
	},

	{.funcName = "showFunctions" ,
	.funcDesc = "\tDisplays all functions and how they work",
	.paramDesc = "\tAny parameters are ignored",
	.exampleDesc = "\tYou probably already know how to use it if you see it",
	.call = stlFuncs::showFunctions
	},

	{.funcName = "showVars" ,
	.funcDesc = "\tDisplays all declared variables",
	.paramDesc = "\tAny parameters are ignored",
	.exampleDesc = "\t \"a=5\", \t\"showVars()\" ",
	.call = stlFuncs::showVars
	},

	{.funcName = "help" ,
	.funcDesc = "\tDisplays a quick introduction",
	.paramDesc = "\tAny parameters are ignored",
	.exampleDesc = "\t \"help()\"",
	.call = stlFuncs::help
	},

	{.funcName = "abs" ,
	.funcDesc = "\tReturns an absolute value of a given expression",
	.paramDesc = "\tOnly one parameter, any expression",
	.exampleDesc = "\t\"abs(3-5)\" returns 2",
	.call = stlFuncs::abs
	},

	{.funcName = "max" ,
	.funcDesc = "\tReturns a bigger value",
	.paramDesc = "\tTwo parameters, any expressions",
	.exampleDesc = "\t\"max(100,5*3)\" returns 100",
	.call = stlFuncs::max
	},

	{.funcName = "min" ,
	.funcDesc = "\tReturns a smaller value",
	.paramDesc = "\tTwo parameters, any expressions",
	.exampleDesc = "\t\"min(-5,100)\" returns -5",
	.call = stlFuncs::min
	},

	{.funcName = "cmp" ,
	.funcDesc = "\tPrints out which value is bigger",
	.paramDesc = "\tTwo parameters, any expressions",
	.exampleDesc = "\t\"cmp(5,3)\" prints out \"5 is bigger\"",
	.call = stlFuncs::cmp
	},

	{.funcName = "sum" ,
	.funcDesc = "\tReturns a sum of all the given expressions",
	.paramDesc = "\tAny amount of expressions",
	.exampleDesc = "\t\"sum(5,-5,3,4)\" returns 7",
	.call = stlFuncs::sum
	},

	{.funcName = "prod" ,
	.funcDesc = "\tReturns a product of all the given expressions",
	.paramDesc = "\tAny amount of expressions",
	.exampleDesc = "\t\"prod(5,3,4)\" returns 60",
	.call = stlFuncs::prod
	},

	{.funcName = "cntDigits" ,
	.funcDesc = "\tReturns the amount of digits that a given value has in its decimal form",
	.paramDesc = "\tOne parameter, any expression",
	.exampleDesc = "\t\"cntDigits(150)\" returns 3",
	.call = stlFuncs::cntDigits
	},

	{.funcName = "showMyFunctions",
	.funcDesc = "\tShows user-defined functions",
	.paramDesc = "\tNo parameters, they are ignored",
	.exampleDesc = "\t\"f(x) = x\n\t\"showMyFunctions()\"\" prints f(x) and its info",
	.call = stlFuncs::showMyFunctions
	},

	{.funcName = "execFile",
	.funcDesc = "\tExecutes a given file line by line",
	.paramDesc = "\tOnly one parameter, has to be a variable, which is the name to the file, including extension",
	.exampleDesc = "\t\"execFile(test1.txt)\" executes contents of a \"test.txt\" file",
	.call = stlFuncs::execFile
	},

	{.funcName = "save",
	.funcDesc = "\tSaves variables and functions to a file, that will be loaded at the next startup",
	.paramDesc = "\tNone, any are ignored",
	.exampleDesc = "",
	.call = stlFuncs::save
	},
	{.funcName = "clearSave",
	.funcDesc = "\tDeletes the save file made by \"save()\"",
	.paramDesc = "\tNone, any are ignored",
	.exampleDesc = "",
	.call = stlFuncs::clearSave
	},
	
	{.funcName = "mod",
	.funcDesc = "\tReturns remainder of the division between 2 parameters",
	.paramDesc = "\tTwo parameters, any expressions",
	.exampleDesc = "\t\"mod(5,3)\" returns 2, because its the remainder of division 5 / 3",
	.call = stlFuncs::mod
	},
	
	{.funcName = "pow",
	.funcDesc = "\tReturns first parameter exponentiated to second parameter",
	.paramDesc = "\tTwo parameters, any expressions",
	.exampleDesc = "\t\"pow(2,3)\" returns 8 becase 2^3",
	.call = stlFuncs::pow
	},
	
	{ .funcName = "lexerOutput",
	.funcDesc = "\tToggles if lexer output should be printed to the console",
	.paramDesc = "\tOnly one parameter, any expression that evaluates to either 0 or 1, if its something else it ignores it",
	.exampleDesc = "\t\"lexerOutput(1)\" now lexer output will be shown in console",
	.call = stlFuncs::lexerOutput
	},

	{ .funcName = "parserOutput",
	.funcDesc = "\tToggles if parser output should be printed to the console",
	.paramDesc = "\tOnly one parameter, any expression that evaluates to either 0 or 1, if its something else it ignores it",
	.exampleDesc = "\t\"parserOutput(1)\" now parser output will be shown in console",
	.call = stlFuncs::parserOutput
	},
  { .funcName = "print",
  	.funcDesc = "\tPrints a string provided to the console",
  	.paramDesc = "\tOnly one parameter, if its something else it ignores it",
  	.exampleDesc = "\t\"print(\"hello\")\" will print hello to console",
  	.call = stlFuncs::print
	},
  { .funcName = "toString",
  .funcDesc = "\tConverts a BigInt to string",
  .paramDesc = "\tOnly one parameter, if its something else it ignores it",
  .exampleDesc = "\t\"a = toString(26)\" will make a = \"26\"",
  .call = stlFuncs::toString
    },
  { .funcName = "strConcat",
.funcDesc = "\tMerges 2 strings",
.paramDesc = "\tTwo parameters, first string and second string",
.exampleDesc = "\t\"a = strConcat(\"hello \",\"world\")\" will make a = \"hello world\"",
.call = stlFuncs::strConcat
	},
{ .funcName = "stoi",
.funcDesc = "\tConverts string to int",
.paramDesc = "\tOne parameter, a string",
.exampleDesc = "\t\"a = stoi(\"123\")\" will make a = 123",
.call = stlFuncs::stoi
},

{ .funcName = "fopen",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fopen
},
{ .funcName = "fread",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fread
},
{ .funcName = "fclose",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fclose
},
{ .funcName = "fhasline",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fhasline
},
{ .funcName = "fgetline",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fgetline
},
{ .funcName = "fhaschar",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fhaschar
},
{ .funcName = "fgetchar",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fgetchar
},
{ .funcName = "fresetcursor",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fresetcursor
},
{ .funcName = "fgetcursor",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fgetcursor
},
{ .funcName = "fwrite",
.funcDesc = "\tTODO: FINISH",
.paramDesc = "\tTODO: FINISH",
.exampleDesc = "\tTODO: FINISH",
.call = stlFuncs::fwrite
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
		return voidReturn();	//dont return anything
	}
}

void handleFuncNotFound(const std::string& funcName) {

	using Distance = std::pair<std::string, uint64_t>;
	std::vector<Distance> distances;

	//check how similiar current stl function names are to user's input
	for (auto& func : stlFunctions) {
		distances.emplace_back( func.funcName,helpers::levenshteinDistance(funcName,func.funcName) );
	}

	//sort so that first element is the most likely
	std::ranges::sort(distances, [](const Distance& distance1, const Distance& distance2) {
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
	for (auto& [name, params, definition] : eCtx.userFunctions) {

		if (funcName == name) {
			
			if (params.size() != args.size())
				throw EvalException("Argument mismatch, " "function " + name
					+ " expects " + std::to_string(params.size())
					+ " arguments, but " + std::to_string(args.size())
					+ " were provided","Check out ?"+name+"()");

			EvalCtx newEvalCtx{};
			std::vector<std::pair<std::string,BigInt>> vars;
			
			for (size_t i = 0; i < params.size();i++) {
				vars.emplace_back( params[i],args[i]->eval(eCtx) );
			}
			
			newEvalCtx.vars = vars;
			newEvalCtx.userFunctions = std::move(eCtx.userFunctions);
			newEvalCtx.handles = std::move(eCtx.handles);

			BigInt val = BigInt(0);

			if (definition->type() != NodeType::Block) {
				val = definition->eval(newEvalCtx);
			}
			else {
				auto def = reinterpret_cast<Block*>(definition.get());
				def->m_EvalCtx = std::move(newEvalCtx);
				val = def->eval(newEvalCtx); //this newEvalCtx doesnt matter
				newEvalCtx.userFunctions = std::move(def->m_EvalCtx.userFunctions);
				newEvalCtx.handles = std::move(def->m_EvalCtx.handles); //this is getting ridiculous
			}

			eCtx.userFunctions = std::move(newEvalCtx.userFunctions);
			eCtx.handles = std::move(newEvalCtx.handles); //this is getting ridiculous p2
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