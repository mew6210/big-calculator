#pragma once
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <limits>

void checkBigInt();

typedef uint64_t chunkInt;

enum class chunkDisplayMode {
	decimal,
	hex
};

class BigInt {

	std::vector<chunkInt> chunks;
	bool isPositive;

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const chunkInt& u64Val);
	BigInt();

	//arithmetic
	void addChunkInt(chunkInt val);
	void addChunkInt(chunkInt val, chunkInt startChunk);
	void addBigInt(BigInt& bi);


	void inspectChunks(chunkDisplayMode cdm);
};