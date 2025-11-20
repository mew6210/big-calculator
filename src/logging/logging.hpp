#pragma once
#include <string>
#include <iostream>

enum class ErrType {
	Lexer,
	Parser,
	Evaluator
};

struct ErrMsg {
	std::string ctx;
	uint64_t pointerPos;
	std::string msg;
	std::string note;
	ErrType type;
	ErrMsg(const std::string& c,const uint64_t p,const std::string& m,const std::string& n,const ErrType e): 
		ctx(c),pointerPos(p),msg(m),note(n),type(e){}
};

void printError(const ErrMsg& ErrMsg);