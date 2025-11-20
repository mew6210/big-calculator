#include "../bigint.hpp"
void BigInt::exponentiateChunkInt(uChunkInt val) {
	naiveExponentiation(val);
}

void BigInt::naiveExponentiation(uChunkInt val) {
	BigInt b = BigInt();
	b.chunks = chunks;

	if (val == 0) {
		chunks = { 1 };
		return;
	}
	for (size_t i = 0; i < val-1; i++) {
		multiplyBigInt(b);
	}
}
