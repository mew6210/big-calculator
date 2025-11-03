#include "bigint.hpp"
#include <iomanip>

std::numeric_limits<chunkInt> CHUNKINTLIMIT;

void checkBigInt() {

	BigInt a = BigInt(0);
	BigInt b = BigInt(0);
	BigInt c = BigInt(CHUNKINTLIMIT.max());

	for (uint64_t i = 0; i < 5000000000; i++) {
		a.addChunkInt(CHUNKINTLIMIT.max());
	}
	a.inspectChunks(chunkDisplayMode::hex);
	for (uint64_t i = 0; i < 3; i++) {
		b.addChunkInt(CHUNKINTLIMIT.max());
	}
	b.inspectChunks(chunkDisplayMode::hex);
	a.addBigInt(b);
	a.inspectChunks(chunkDisplayMode::hex);
}

BigInt::BigInt(const chunkInt& val) {
	chunks.resize(1);
	chunks[0] = val;
	isPositive = true;
}

BigInt::BigInt() {
	chunks = {};
	isPositive = true;
}

void chunkIntToHex(chunkInt x) {
	std::cout << std::setw(sizeof(chunkInt)*2) << std::setfill('0') << std::hex << x;
}

void BigInt::inspectChunks(chunkDisplayMode cdm) {
	std::cout << "------------\n";
	std::cout << "sign: ";
	if (isPositive) std::cout << "+\n";
	else std::cout << "-\n";
	if (chunks.size() == 0) {
		std::cout << "no chunks to display, empty bigint\n";
		std::cout << "------------\n";
		return;
	} 
	
	for (int i = 0; i < chunks.size(); i++) {
		std::cout << "chunk " << i << ": ";

		switch (cdm) {
		case chunkDisplayMode::hex: chunkIntToHex(chunks[i]); break;
		case chunkDisplayMode::decimal: std::cout << chunks[i]; break;
		}

		std::cout << "\n";
	}
	std::cout << "------------\n";
}

void BigInt::addChunkInt(chunkInt val) {

	chunkInt carry = val;
	size_t i = 0;

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		chunkInt sum = chunks[i] + carry;
		carry = (sum < chunks[i]) ? 1 : 0;
		chunks[i] = sum;
		i++;
	}
}

void BigInt::addChunkInt(chunkInt val, chunkInt startChunk) {
	chunkInt carry = val;
	size_t i = startChunk; //start from a different chunk than the first one

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		chunkInt sum = chunks[i] + carry;
		carry = (sum < chunks[i]) ? 1 : 0;
		chunks[i] = sum;
		i++;
	}
}

void BigInt::addBigInt(BigInt& bi) {

	chunks.resize(std::max(chunks.size(), bi.chunks.size()));

	for (size_t i = 0; i < bi.chunks.size(); i++) {
		addChunkInt(bi.chunks[i], i);
	}
}

uint128Emul mult64to128(uint64_t op1, uint64_t op2) {	//stack overflow came in clutch
	uint64_t u1 = (op1 & 0xffffffff);
	uint64_t v1 = (op2 & 0xffffffff);
	uint64_t t = (u1 * v1);
	uint64_t w3 = (t & 0xffffffff);
	uint64_t k = (t >> 32);

	op1 >>= 32;
	t = (op1 * v1) + k;
	k = (t & 0xffffffff);
	uint64_t w1 = (t >> 32);

	op2 >>= 32;
	t = (u1 * op2) + k;
	k = (t >> 32);

 	uint64_t hi = (op1 * op2) + w1 + k;
 	uint64_t lo = (t << 32) + w3;
	return uint128Emul{ hi,lo };
}

void BigInt::multiplyChunkInt64(chunkInt val) {
	std::vector<Remainder> remainders = {};

	uint64_t originalSize = chunks.size();
	for (size_t i = 0; i < originalSize; i++) {
		uint128Emul sum = mult64to128(chunks[i], val);
		chunks[i] = sum.low;
		remainders.push_back(Remainder{ sum.high,i + 1 });
	}
	sumUpRemainders(remainders);
}

void BigInt::sumUpRemainders(std::vector<Remainder>& remainders) {
	for (auto& remainder : remainders) {
		addChunkInt(remainder.value, remainder.chunkPos);
	}
	remainders.clear();
}

void BigInt::multiplyChunkInt32(chunkInt val) {
	uint64_t carry = 0;
	for (size_t i = 0; i < chunks.size(); ++i) {
		uint64_t prod = (uint64_t)chunks[i] * (uint64_t)val + carry;	//multip result
		chunks[i] = static_cast<chunkInt>(prod & 0xFFFFFFFFu);		//set current chunk to low 32 bits
		carry = prod >> 32;	//set carry to high 32 bits
	}

	if (carry) addChunkInt(static_cast<chunkInt>(carry), chunks.size());	//push high 32-bits to the next chunk
}

void BigInt::multiplyChunkInt(chunkInt val) {
	
	switch (sizeof(chunkInt)) {
	case 8: multiplyChunkInt64(val); break; //uint64_t
	case 4: multiplyChunkInt32(val); break; //uint32_t
	}

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

void checkmultip() {
	BigInt a = BigInt("999999999999999999999999999999999999999999999999");
	//a.multiplyChunkInt(5);
	a.inspectChunks(chunkDisplayMode::decimal);
}

/*
internal helper function
	- returns 1 if `this` bigint is bigger
	- returns 0 if both bigints are equal
	- returns -1 if argument bigint is bigger
*/
int BigInt::compareChunks(const BigInt& bi) {
	if (chunks.size() > bi.chunks.size()) return 1; //if chunks size vary, one is clearly bigger
	else if (chunks.size() < bi.chunks.size()) return -1;

	else {	//if not, compare values of chunks, start from most significant chunk
		for (size_t i = chunks.size(); i-- > 0;) {	//possible for loop overflow(size_t)
			if (chunks[i] > bi.chunks[i]) return 1;
			else if (chunks[i] < bi.chunks[i]) return -1;
		}
	}

	return 0; //otherwise they are equal
}

bool BigInt::biggerThan(const BigInt& bi) {
	return compareChunks(bi) == 1;
}

bool BigInt::smallerThan(const BigInt& bi) {
	return compareChunks(bi) == -1;
}

bool BigInt::equals(const BigInt& bi) {
	return compareChunks(bi) == 0;
}

void checkComparsions() {
	BigInt a = BigInt(28);
	BigInt b = BigInt(5);

	if (a.biggerThan(b)) std::cout << "a is bigger than b";
	if (a.smallerThan(b)) std::cout << "a is smaller than b";
	if (a.equals(b)) std::cout << "a equals b";

}