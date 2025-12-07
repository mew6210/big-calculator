
#include <string>
#include <optional>
#include "../logging/logging.hpp"
#include "../bigint/bigint.hpp"
#include "../astnodes/astnodes.hpp"

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