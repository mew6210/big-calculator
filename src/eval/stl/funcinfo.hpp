#include <string>
#include<functional>
#include "../../astnodes/astnodes.hpp"
#include "../../bigint/bigint.hpp"
#include "../evalctx/evalctx.hpp"
#include "funcreturn.hpp"

struct stlFunc{
	std::string funcName;
	std::string funcDesc;
	std::string exUsage;
	std::function<funcReturn(std::vector<std::unique_ptr<ExprNode>>&,EvalCtx&)> call;
};