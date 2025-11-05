#include "../bigint.hpp"

std::numeric_limits<uChunkInt> CHUNKINTLIMIT2;

void BigInt::borrow(size_t pos) {
	
	if (chunks[pos] != 0) {
		chunks[pos]--;	//if chunk is not 0, we can borrow from it
	}
	else {
		borrow(pos + 1);	//if it is 0, we have to try borrowing further
		chunks[pos] = CHUNKINTLIMIT2.max();
	}
}

void BigInt::subtractChunkInt(uChunkInt val) {
	
	if (chunks.size() == 0) return;

	if (val <= chunks[0]) {
		chunks[0] -= val;	//if val is smaller than first chunk, we can easily subtract it
	}
	else {
		borrow(1);	//if its not, we have to borrow
		uint64_t diff = CHUNKINTLIMIT2.max() - val+1;	//+1, because chunkintlimit.max returns max HOLDABLE amount, but my base is 2^64, not 2^64 -1 
		chunks[0] += diff;
	}
	trimTrailingChunks();
}