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
	BigInt(const std::string& s);
	BigInt(const uint64_t& u64Val);
	BigInt();
	void addUint64(uint64_t val);
	void inspectChunks(chunkDisplayMode cdm);
};