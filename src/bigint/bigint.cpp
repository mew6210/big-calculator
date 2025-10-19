#include "bigint.hpp"

void checkBigInt() {

	BigInt a = BigInt();
	BigInt b = BigInt("");
	BigInt c = BigInt(25);
	c.inspectChunks();
}

BigInt::BigInt(const std::string& s) {
	chunks = {};
	isPositive = true;
}

BigInt::BigInt(const uint64_t& u64Val){
	chunks.resize(2);
	chunks[0] = u64Val;
	isPositive = true;
}

BigInt::BigInt() {
	chunks = {};
	isPositive = true;
}

void BigInt::inspectChunks(){
	if(chunks.size()==0) return;
	std::cout << "------------\n";
	for(int i=0;i<chunks.size();i++){
		std::cout<<"chunk "<<i<<": "<<chunks[i]<<"\n";
	}
	std::cout << "------------\n";
}