#include "bigint.hpp"

BigInt::BigInt(const chunkInt& val) {
	chunks.resize(1);
	chunks[0] = abs(val);
	if (val > 0) isPositive = true;
	else if (val < 0) isPositive = false;
	else isPositive = true;
}

BigInt::BigInt() {
	chunks = {};
	isPositive = true;
}

/*
	- input correctness is checked by lexer, so no need to check if everything is a digit here
*/
BigInt::BigInt(const std::string& s) {
	chunks = {}; 
	isPositive = true; 
	bool toSkip = false; 
	if (s[0] == '-') { isPositive = false; toSkip = true; }; 
	for (char c : s) { //slow, should be replaced with a chunk-based approach later, better for bigger strings
		if (toSkip) { toSkip = false; continue; } 
		int d = c - '0'; //get numerical vaue of c 
		multiplyChunkInt64(10);
		addChunkInt(d);
	} 
}

/*

	TODO: DOESNT WORK YET FOR NEGATIVE NUMBERS
*/
std::string BigInt::toString() {
	std::string ret = "";
	std::vector<uChunkInt> zero = { 0 };
	BigInt selfCopy = BigInt();
	selfCopy.chunks = chunks;
	selfCopy.isPositive = isPositive;
	size_t i = 0;
	while (!selfCopy.chunks.empty()) {
		uChunkInt mod = selfCopy.moduloChunkInt(10);
		selfCopy.divideChunkInt(10,true);
		char toAdd = mod + '0';
		ret.push_back(toAdd);
	}
	std::reverse(ret.begin(), ret.end());
	return ret;
}