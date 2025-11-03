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

void BigInt::sumUpRemainders(std::vector<Remainder>& remainders) {
	for (auto& remainder : remainders) {
		addChunkInt(remainder.value, remainder.chunkPos);
	}
	remainders.clear();
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