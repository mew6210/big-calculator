#include <string>
#include <vector>
#include "../../astnodes/astnodes.hpp"
#include "../evalctx/evalctx.hpp"
#include "../../bigint/bigint.hpp"
#include "../evalException.hpp"
#include <optional>

std::optional<BigInt> stlDispatch(std::string& funcName, std::vector<std::unique_ptr<ExprNode>>& args, EvalCtx& eCtx);