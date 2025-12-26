#include <string>
#include <vector>


/*
//this class needs to be forward declared, because this whole file is included in evalCtx, 
and if u try to include astnodes before that it leads to a big mess, where astnodes includes evalctx and evalctx includes astnodes
*/
class ExprNode;	

struct UserFunc {

	std::string name;
	std::vector<std::string> params;
	std::unique_ptr<ExprNode> definition;

};
#include "../../astnodes/astnodes.hpp"