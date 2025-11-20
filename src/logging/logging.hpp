#pragma once
#include <string>

enum class ErrType {
	Lexer,
	Parser,
	Evaluator
};

struct ErrMsg {
	std::string ctx;		//entire line of user input
	uint64_t pointerPos;	//position where to display an arrow
	std::string msg;		//error message
	std::string note;		//hopefully helpful note how to fix it
	ErrType type;			//from what part of an app the error is coming from
	
	ErrMsg(const std::string& c,const uint64_t p,const std::string& m,const std::string& n,const ErrType e): 
		ctx(c),pointerPos(p),msg(m),note(n),type(e){}
};

void printError(const ErrMsg& ErrMsg);