#include "../bigint.hpp"

typedef std::vector<uChunkInt> Chunks;

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
/*
	---namespace for multiplication helpers---
	they are just slightly modified operations that already exist, but they dont modify the underlying chunks in the class, since they are not in the class
	its needed, because when multiplication happens, any change to the number that is being multiplied alters the result
	in anonymous namespace for extra clarity, they should not be used outside multiplication.
*/
namespace {
	//almost the same as 'addChunkInt'
	void addChunkIntExternal(Chunks& chunks, uChunkInt val, uChunkInt startChunk) {
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
	//almost the same as 'multiplyChunkInt64'
	Chunks multiplyChunkInt64External(const Chunks& chunks, uChunkInt val) {
		std::vector<Remainder> remainders = {};
		Chunks subsituteChunks = chunks;

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

/*
	@brief sums up vector of sums, like they would be in a 'slupkowym' multiplication into one result vector

	it produces the offset internally, no need to add padding to vectors to simulate how it works below

	example:
	    2 3 4
	  6 3 5
    +
	----------
      6 5 7 4
*/
Chunks sumUpMultiplicationResults(const std::vector<Chunks>& sums) {
	
	Chunks result;
	result.resize(sums.size() + sums.back().size());		//max size of a result is the `size of the last sum` + `how many sums there are` 

	int curRow = 0;	//keep track of what row we are on right now
	for (auto& sum : sums) {

		for (size_t i = 0; i < sum.size(); i++) {
			addChunkIntExternal(result, sum[i], i + curRow);	//add each chunk of a given sum to the result chunks, at a specific place accounting for the offset (i+curRow)
		}

		curRow++;
	}
	return result;
}

bool determineMultSign(bool& sign1,bool& sign2) {
	return sign1 == sign2;
}

void BigInt::multiplyBigInt(BigInt& bi) {
	std::vector<Chunks> sums;
	for (size_t i = 0; i < bi.chunks.size(); i++) {		//for every chunk in 2nd bigint
		auto result = multiplyChunkInt64External(chunks,bi.chunks[i]);	//multiply every single chunk in 1st bigint
		sums.push_back(result);
	}
	if (sums.size() == 0) { chunks = {}; return; }
	auto resultChunks = sumUpMultiplicationResults(sums);
	chunks = resultChunks;
	trimTrailingChunks();
	isPositive = determineMultSign(isPositive, bi.isPositive);
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