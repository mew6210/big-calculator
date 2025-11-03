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