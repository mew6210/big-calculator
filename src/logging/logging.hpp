#pragma once
#include <string>
#include <iostream>

struct ErrMsg {
	const std::string ctx;
	const uint64_t pointerPos;
	const std::string msg;
	const std::string& note;
};

void printError(const ErrMsg& ErrMsg);