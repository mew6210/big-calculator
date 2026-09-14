#pragma once
#include <vector>
#include <string>
#include <iostream>

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

struct DivResult;

struct uint128Emul {
	uint64_t high;
	uint64_t low;
};

uint128Emul mult64to128(uint64_t op1, uint64_t op2);

class BigInt {

	std::vector<uChunkInt> chunks;
	bool isPositive;

	void borrow(size_t pos);
	int compareChunks(const BigInt& bi) const;
	bool areSignsDifferent(const BigInt& bi) const;
	bool compareSigns(const BigInt& bi) const;
	void trimTrailingChunks();
	void resizeBigInts(BigInt& bi);

	void sumUpRemainders(std::vector<Remainder>& remainders);
	void multiplyChunkInt64(uChunkInt val);
	
	

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const chunkInt& val);
	BigInt();
	//BigInt(BigInt&&) noexcept = default;
	BigInt(const BigInt&) = default;

	//comparsions
	bool biggerThan(const BigInt& bi) const;
	bool smallerThan(const BigInt& bi) const;
	bool equals(const BigInt& bi) const;

	//arithmetic
	void addChunkInt(uChunkInt val);
	void addChunkInt(uChunkInt val, uChunkInt startChunk);
	void addBigInt(const BigInt& bi);

	void subtractChunkInt(uChunkInt val);
	void subtractBigInt(BigInt& bi);
	
	void multiplyChunkInt(uChunkInt val);
	void multiplyBigInt(const BigInt& bi);

	void divideChunkInt(uChunkInt val, bool toMute = false);
	void divideBigInt(BigInt& bi,bool toMute);

	uChunkInt moduloChunkInt(uChunkInt val);

	[[nodiscard]] std::string toString() const;
	void inspectChunks(chunkDisplayMode cdm) const;
	void inspectChunks(chunkDisplayMode cdm, int indent) const;
	void print() const;
	void flipSign() { isPositive = !isPositive; };
	[[nodiscard]] bool isNegative() const { return !isPositive; }
	[[nodiscard]] bool isZero() const { if (chunks.empty()) return true;if (chunks.size() == 1 && chunks[0] == 0) return true; else return false;}
	friend BigInt add(BigInt& a, const BigInt& b);
	friend BigInt subtract(BigInt& a, BigInt& b);
	friend BigInt abs(const BigInt& a);
	friend DivResult divideUnsigned(BigInt a, BigInt b);

	friend bool isQuotientTooLarge(BigInt& u, BigInt& v, uint64_t qhat, size_t j);
	friend void subtractMultiple(BigInt& u, BigInt& v, uint64_t qhat, size_t j);

};

struct DivResult {
	BigInt quotient;
	BigInt remainder;
};
