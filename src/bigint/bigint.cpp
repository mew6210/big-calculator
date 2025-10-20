#include "bigint.hpp"
#include <iomanip>

void checkBigInt() {

	BigInt a = BigInt(0);
	BigInt b = BigInt(0);
	BigInt c = BigInt(UINT64_MAX);

	for (uint64_t i = 0; i < 5; i++) {
		a.addUint64(UINT64_MAX);
	}
	a.inspectChunks(chunkDisplayMode::decimal);
	for (uint64_t i = 0; i < 3; i++) {
		b.addUint64(UINT64_MAX);
	}
	b.inspectChunks(chunkDisplayMode::decimal);
	a.addBigInt(b);
	a.inspectChunks(chunkDisplayMode::decimal);
}

BigInt::BigInt(const std::string& s) {
	chunks = {};
	isPositive = true;
}

BigInt::BigInt(const uint64_t& u64Val){
	chunks.resize(2);
	chunks[0] = u64Val;
	isPositive = true;
}

BigInt::BigInt() {
	chunks = {};
	isPositive = true;
}

void uint64ToHex(uint64_t x) {
	std::cout << std::setw(16) << std::setfill('0') << std::hex << x;
}

void BigInt::inspectChunks(chunkDisplayMode cdm){
	if(chunks.size()==0) return;
	std::cout << "------------\n";
	for(int i=0;i<chunks.size();i++){
		std::cout << "chunk " << i << ": ";

		switch (cdm) {
		case chunkDisplayMode::hex: uint64ToHex(chunks[i]); break;
		case chunkDisplayMode::decimal: std::cout << chunks[i]; break;
		}

		std::cout << "\n";
	}
	std::cout << "------------\n";
}

void BigInt::addUint64(uint64_t val) {

	uint64_t carry = val;
	size_t i = 0;

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		uint64_t sum = chunks[i] + carry;
		carry = (sum < chunks[i]) ? 1 : 0;
		chunks[i] = sum;
		i++;
	}
}

void BigInt::addUint64(uint64_t val, uint64_t startChunk) {
	uint64_t carry = val;
	size_t i = startChunk; //start from a different chunk than the first one

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		uint64_t sum = chunks[i] + carry;
		carry = (sum < chunks[i]) ? 1 : 0;
		chunks[i] = sum;
		i++;
	}
}

void BigInt::addBigInt(BigInt& bi) {

	chunks.resize(std::max(chunks.size(), bi.chunks.size()));

	for (int i = 0; i < bi.chunks.size(); i++) {
		addUint64(bi.chunks[i], i);
	}
}