#pragma once
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <limits>

void checkBigInt();
void checkmultip();
typedef uint64_t chunkInt;

enum class chunkDisplayMode {
	decimal,
	hex
};

class BigInt {

	std::vector<chunkInt> chunks;
	bool isPositive;

	void multiplyChunkInt64(chunkInt val);
	void multiplyChunkInt32(chunkInt val);

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const chunkInt& val);
	BigInt();

	//arithmetic
	void addChunkInt(chunkInt val);
	void addChunkInt(chunkInt val, chunkInt startChunk);
	void addBigInt(BigInt& bi);

	void multiplyChunkInt(chunkInt val);

	void inspectChunks(chunkDisplayMode cdm);
};

struct uint128Emul {
	uint64_t high;
	uint64_t low;
};