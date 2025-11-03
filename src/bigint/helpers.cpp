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

void checkmultip() {
	BigInt a = BigInt("999999999999999999999999999999999999999999999999");
	//a.multiplyChunkInt(5);
	a.inspectChunks(chunkDisplayMode::decimal);
}
