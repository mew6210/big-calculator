#include "bigint.hpp"


BigInt::BigInt(const std::string& s) {
	chunks = {};
	isPositive = true;
}

void BigInt::inspectChunks(){
	if(chunks.size()==0) return;
	for(int i=0;i<chunks.size();i++){
		std::cout<<"chunk "<<i<<": "<<chunks[i]<<"\n";
	}
}