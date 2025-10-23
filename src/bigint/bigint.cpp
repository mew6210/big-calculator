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

BigInt::BigInt(const std::string& s) {
	chunks = {};
	isPositive = true;
}

BigInt::BigInt(const chunkInt& val) {
	chunks.resize(2);
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
	if (chunks.size() == 0) return;
	std::cout << "------------\n";
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

void BigInt::multiplyChunkInt(chunkInt val) {
	for (size_t i = 0; i<chunks.size();i++) {
		uint128Emul sum = mult64to128(chunks[i], val);
		chunks[i] = sum.low;
		addChunkInt(sum.high, i+1);
	}
}