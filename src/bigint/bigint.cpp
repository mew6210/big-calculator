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

std::string BigInt::toString() const {
    if (chunks.empty())
        return "0";

    BigInt tmp;
    tmp.chunks = chunks;
    tmp.isPositive = true;

    static constexpr uChunkInt DEC_BASE = 1000000000000000000ULL; // 10^18
    static constexpr int DEC_DIGITS = 18;

    std::vector<uChunkInt> blocks;
    blocks.reserve(chunks.size());

    while (!tmp.chunks.empty()) {
        uChunkInt rem = tmp.moduloChunkInt(DEC_BASE);
        tmp.divideChunkInt(DEC_BASE, true);
        blocks.push_back(rem);
    }

    std::string result;

    result = std::to_string(blocks.back());
    for (int i = (int)blocks.size() - 2; i >= 0; --i) {
        std::string part = std::to_string(blocks[i]);
        result.append(DEC_DIGITS - part.length(), '0');
        result += part;
    }

    if (!isPositive)
        result.insert(result.begin(), '-');

    return result;
}

void BigInt::print() {
	std::cout << this->toString() << "\n";
}