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

BigInt add(BigInt& a, BigInt& b) {
	a.chunks.resize(std::max(a.chunks.size(), b.chunks.size()));

	for (size_t i = 0; i < b.chunks.size(); i++) {
		a.addChunkInt(b.chunks[i], i);
	}
	return a;
}

BigInt abs(BigInt& a) {
	BigInt b = BigInt(0);
	b.chunks = a.chunks;
	b.isPositive = true;
	return b;
}

void BigInt::addBigInt(BigInt& bi) {
	
	BigInt absA = abs(*this);
	BigInt absB = abs(bi);
	if (isPositive == bi.isPositive) { 
		BigInt temp = add(absA, absB); 
		chunks = temp.chunks; 
		return; 
	}
	else {
		if (absA.biggerThan(absB)) { 
			BigInt temp = subtract(absA, absB); 
			chunks = temp.chunks;
		}
		else if (abs(*this).smallerThan(abs(bi))) {
			BigInt temp = subtract(absB, absA);
			chunks = temp.chunks;
			isPositive = bi.isPositive;
		}
		else { chunks = { 0 }; return; };
	}

}