#include <vector>
#include <utility>
#include <string>
#include <iostream>

void checkBigInt();

enum class chunkDisplayMode {
	decimal,
	hex
};

class BigInt {

	std::vector<uint64_t> chunks;
	bool isPositive;

public:
	//constructors
	BigInt(const std::string& s);
	BigInt(const uint64_t& u64Val);
	BigInt();

	//arithmetic
	void addUint64(uint64_t val);
	void addUint64(uint64_t val, uint64_t startChunk);
	void addBigInt(BigInt& bi);


	void inspectChunks(chunkDisplayMode cdm);
};