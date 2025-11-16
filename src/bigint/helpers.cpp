#include "bigint.hpp"
#include <iomanip>

std::numeric_limits<uChunkInt> CHUNKINTLIMIT;

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

void chunkIntToHex(uChunkInt x) {
	std::cout << std::setw(sizeof(uChunkInt) * 2) << std::setfill('0') << std::hex << x;
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

void BigInt::inspectChunks(chunkDisplayMode cdm,int indent) {
	std::cout << std::string(indent,' ') << "------------\n";
	std::cout << std::string(indent, ' ') << "sign: ";
	if (isPositive) std::cout << "+\n";
	else std::cout << "-\n";
	if (chunks.size() == 0) {
		std::cout << std::string(indent, ' ') << "no chunks to display, empty bigint\n";
		std::cout << std::string(indent, ' ') << "------------\n";
		return;
	}

	for (int i = 0; i < chunks.size(); i++) {
		std::cout << std::string(indent, ' ') << "chunk " << i << ": ";

		switch (cdm) {
		case chunkDisplayMode::hex: { std::cout<<std::string(indent, ' '); chunkIntToHex(chunks[i]); } break;
		case chunkDisplayMode::decimal: std::cout << std::string(indent, ' ') << chunks[i]; break;
		}

		std::cout << "\n";
	}
	std::cout << std::string(indent, ' ') << "------------\n";
}

void BigInt::trimTrailingChunks() {
	while (!chunks.empty() && chunks.back() == 0)
		chunks.pop_back();
}

void BigInt::resizeBigInts(BigInt& bi) {
	size_t maxSize = std::max(chunks.size(), bi.chunks.size());
	chunks.resize(maxSize, 0);
	bi.chunks.resize(maxSize, 0);
}

void checkmultip() {
	BigInt a = BigInt("999999999999999999");
	a.inspectChunks(chunkDisplayMode::decimal);
	BigInt b = BigInt("999999999999999999");
	b.inspectChunks(chunkDisplayMode::decimal);
	a.multiplyBigInt(b);
	a.inspectChunks(chunkDisplayMode::decimal);
}

void checkSubtr() {

	BigInt a = BigInt("100");
	BigInt b = BigInt("50");
	
	a.subtractBigInt(b);
	a.inspectChunks(chunkDisplayMode::decimal);
}

void checkDiv() {

	BigInt a = BigInt("55340232221128654850");
	uint64_t b = 5;
	a.divideChunkInt(b);

	a.inspectChunks(chunkDisplayMode::decimal);
}

void checkMod() {
	BigInt a = BigInt("55340232221128654851");
	uint64_t b = 234;

	uint64_t remainder = a.moduloChunkInt(b);
	std::cout << "remainder: " << remainder;
}