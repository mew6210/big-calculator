#include "../bigint.hpp"

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

void BigInt::multiplyChunkInt64(uChunkInt val) {
	std::vector<Remainder> remainders = {};

	uint64_t originalSize = chunks.size();
	for (size_t i = 0; i < originalSize; i++) {
		uint128Emul sum = mult64to128(chunks[i], val);
		chunks[i] = sum.low;
		remainders.push_back(Remainder{ sum.high,i + 1 });
	}
	sumUpRemainders(remainders);
}
namespace {

	void addChunkIntExternal(std::vector<uChunkInt>& chunks, uChunkInt val, uChunkInt startChunk) {
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

	std::vector<uChunkInt> multiplyChunkInt64External(const std::vector<uChunkInt>& chunks, uChunkInt val) {
		std::vector<Remainder> remainders = {};
		std::vector<uChunkInt> subsituteChunks = chunks;

		uint64_t originalSize = chunks.size();
		for (size_t i = 0; i < originalSize; i++) {
			uint128Emul sum = mult64to128(chunks[i], val);
			subsituteChunks[i] = sum.low;
			remainders.push_back(Remainder{ sum.high,i + 1 });
		}

		for (auto& remainder : remainders) {
			addChunkIntExternal(subsituteChunks, remainder.value, remainder.chunkPos);
		}
		return subsituteChunks;
	}

}

void BigInt::sumUpRemainders(std::vector<Remainder>& remainders) {
	for (auto& remainder : remainders) {
		addChunkInt(remainder.value, remainder.chunkPos);
	}
	remainders.clear();
}

std::vector<uChunkInt> sumUpMultiplicationResults(const std::vector<std::vector<uChunkInt>>& sums) {
	
	std::vector<uChunkInt> result;
	result.resize(sums.size() + sums.back().size());

	int curRow = 0;
	for (auto& sum : sums) {

		for (size_t i = 0; i < sum.size(); i++) {
			addChunkIntExternal(result, sum[i], i + curRow);
		}

		curRow++;
	}
	return result;
}

void BigInt::multiplyBigInt(BigInt& bi) {
	std::vector<std::vector<uChunkInt>> sums;
	for (size_t i = 0; i < bi.chunks.size(); i++) {
		auto result = multiplyChunkInt64External(chunks,bi.chunks[i]);
		sums.push_back(result);
	}

	auto resultChunks = sumUpMultiplicationResults(sums);
	chunks = resultChunks;
	trimTrailingChunks();
}

void BigInt::multiplyChunkInt32(uChunkInt val) {
	uint64_t carry = 0;
	for (size_t i = 0; i < chunks.size(); ++i) {
		uint64_t prod = (uint64_t)chunks[i] * (uint64_t)val + carry;	//multip result
		chunks[i] = static_cast<uChunkInt>(prod & 0xFFFFFFFFu);		//set current chunk to low 32 bits
		carry = prod >> 32;	//set carry to high 32 bits
	}

	if (carry) addChunkInt(static_cast<uChunkInt>(carry), chunks.size());	//push high 32-bits to the next chunk
}

void BigInt::multiplyChunkInt(uChunkInt val) {

	switch (sizeof(uChunkInt)) {
	case 8: multiplyChunkInt64(val); break; //uint64_t
	case 4: multiplyChunkInt32(val); break; //uint32_t
	}

}