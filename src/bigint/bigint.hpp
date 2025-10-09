#include <vector>
#include <utility>
#include <string>
#include <iostream>

class BigInt {

	std::vector<uint64_t> chunks;
	bool isPositive;

public:
	BigInt(const std::string&);
};