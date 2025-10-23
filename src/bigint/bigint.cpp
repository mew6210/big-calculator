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

	for (int i = 0; i < bi.chunks.size(); i++) {
		addChunkInt(bi.chunks[i], i);
	}
}