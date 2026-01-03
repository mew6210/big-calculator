#include "bigint.hpp"
#include <iomanip>

std::numeric_limits<uChunkInt> CHUNKINTLIMIT;

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