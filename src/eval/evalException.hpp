#include <string>
#include <optional>
#include "../logging/logging.hpp"
#include "../bigint/bigint.hpp"
#include "../astnodes/astnodes.hpp"

/*
	@brief helper exception for eval errors

	since all information about tokens and its position is lost during parsing stage, eval cant use printError like it would normally,
	instead it has to create src from currently evaluated numbers

	functions as kind of a wrapper around printError

	has 2 constructors, one that provides context for when it happened, whereas the other one doesnt
*/
class EvalException : public std::runtime_error {

	std::string msg;
	std::string note;
	std::optional<BigInt> a, b;
	std::optional<std::unique_ptr<ExprNode>> var1;
	std::optional<char> op;

public:
	EvalException(const std::string& _msg, const std::string& _note) :
		std::runtime_error(_msg),
		msg(_msg),
		note(_note) {
	}
	EvalException(const std::string& _msg, const std::string& _note, const BigInt& _a, const BigInt& _b, const char _op)
		: std::runtime_error(_msg),
		msg(_msg),
		note(_note),
		a(_a),
		b(_b),
		op(_op) {
	}

	EvalException(const std::string& _msg, const std::string& _note, std::unique_ptr<ExprNode>& _a, const BigInt& _b, const char _op)
		: std::runtime_error(_msg),
		msg(_msg),
		note(_note),
		var1(std::move(_a)),
		b(_b),
		op(_op) {
	}


	void printEvalErr() {
		if (a && b && op) {
			std::string src = a.value().toString() + op.value() + b.value().toString();		//src made from lhs,op,rhs

			printError(ErrMsg{
				src,
				a.value().toString().size(),
				msg,
				note,
				ErrType::Evaluator
				}
			);
		}
		else if (var1 && b && op) {

			std::string varName = var1.value()->toString();
			std::string src = varName + op.value() + b.value().toString();
			printError(ErrMsg{
				src,
				varName.size(),
				msg,
				note,
				ErrType::Evaluator
				});
		}
		else {
			std::cout << "Evaluator error: " << msg << "\n";
			std::cout << "Note: " << note << "\n";
		}
	}
};