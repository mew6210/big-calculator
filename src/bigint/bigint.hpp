#pragma once
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <limits>

void checkBigInt();
void checkmultip();
void checkComparsions();
typedef uint64_t chunkInt;

enum class chunkDisplayMode {
	decimal,
	hex
};
struct Remainder {
	uint64_t value;
	uint64_t chunkPos;
};

class BigInt {

	std::vector<chunkInt> chunks;
	bool isPositive;

	int BigInt::compareChunks(const BigInt& bi);

	void sumUpRemainders(std::vector<Remainder>& remainders);
	void multiplyChunkInt64(chunkInt val);
	void multiplyChunkInt32(chunkInt val);

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const chunkInt& val);
	BigInt();

	//comparsions
	bool biggerThan(const BigInt& bi);
	bool smallerThan(const BigInt& bi);
	bool equals(const BigInt& bi);

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