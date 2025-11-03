#include "../bigint.hpp"

void BigInt::addChunkInt(uChunkInt val) {

	uChunkInt carry = val;
	size_t i = 0;

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		uChunkInt sum = chunks[i] + carry;
		carry = (sum < chunks[i]) ? 1 : 0;
		chunks[i] = sum;
		i++;
	}
}

void BigInt::addChunkInt(uChunkInt val, uChunkInt startChunk) {
	uChunkInt carry = val;
	size_t i = startChunk; //start from a different chunk than the first one

	while (carry != 0) {
		if (i == chunks.size())
			chunks.push_back(0);

		uChunkInt sum = chunks[i] + carry;
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