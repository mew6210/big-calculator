#pragma once
#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <limits>

void checkBigInt();
void checkmultip();
void checkComparsions();
void checkSubtr();
void checkDiv();
typedef uint64_t uChunkInt;
typedef int64_t chunkInt;

enum class chunkDisplayMode {
	decimal,
	hex
};
struct Remainder {
	uint64_t value;
	uint64_t chunkPos;
};

class BigInt {

	std::vector<uChunkInt> chunks;
	bool isPositive;

	void borrow(size_t pos);
	int compareChunks(const BigInt& bi);
	bool areSignsDifferent(const BigInt& bi);
	bool compareSigns(const BigInt& bi);
	void trimTrailingChunks();
	void resizeBigInts(BigInt& bi);

	void sumUpRemainders(std::vector<Remainder>& remainders);
	void multiplyChunkInt64(uChunkInt val);
	void multiplyChunkInt32(uChunkInt val);

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const chunkInt& val);
	BigInt();
	BigInt(BigInt&&) noexcept = default;
	BigInt(const BigInt&) = default;

	//comparsions
	bool biggerThan(const BigInt& bi);
	bool smallerThan(const BigInt& bi);
	bool equals(const BigInt& bi);

	//arithmetic
	void addChunkInt(uChunkInt val);
	void addChunkInt(uChunkInt val, uChunkInt startChunk);
	void addBigInt(BigInt& bi);

	void subtractChunkInt(uChunkInt val);
	void subtractBigInt(BigInt& bi);
	
	void multiplyChunkInt(uChunkInt val);
	void multiplyBigInt(BigInt& bi);

	void divideChunkInt(uChunkInt val);

	void inspectChunks(chunkDisplayMode cdm);
	void inspectChunks(chunkDisplayMode cdm, int indent);
};

struct uint128Emul {
	uint64_t high;
	uint64_t low;
};