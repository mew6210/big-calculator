#include "bigint.hpp"

/*
	@brief internal helper function
	- returns 1 if `this` bigint is bigger
	- returns 0 if both bigints are equal
	- returns -1 if argument bigint is bigger
	@param BigInt - other number
*/
int BigInt::compareChunks(const BigInt& bi) {
	int currentBigger = 1;
	int currentSmaller = -1;
	if (!isPositive && !bi.isPositive) { //if both numbers are negative, those values should be reversed
		currentBigger = -1; 
		currentSmaller = 1;
	}
	
	if (chunks.size() > bi.chunks.size()) return currentBigger; //if chunks size vary, one is clearly bigger
	else if (chunks.size() < bi.chunks.size()) return currentSmaller;

	else {	//if not, compare values of chunks, start from most significant chunk
		for (size_t i = chunks.size(); i-- > 0;) {	//possible for loop overflow(size_t)
			if (chunks[i] > bi.chunks[i]) return currentBigger;
			else if (chunks[i] < bi.chunks[i]) return currentSmaller;
		}
	}

	return 0; //otherwise they are equal
}

/*
* @brief internal helper function
* 
* returns true if signs of two numbers are different, false otherwise
*/
bool BigInt::areSignsDifferent(const BigInt& bi) {
	return isPositive != bi.isPositive;
}

/*
* @brief internal helper function 
* 
*- returns 1 if sign comparsion indicates that `this` is bigger
*
*- returns 0 if sign comparsion indicates that parameter bigint is bigger
*/
bool BigInt::compareSigns(const BigInt& bi) {

	if (isPositive && !bi.isPositive) {
		return 1;
	}
	else if (!isPositive && bi.isPositive) {
		return 0;
	}
	else {
		std::cout << "something went wrong\n";
		return 0;
	}
}

/*
	@brief Compares two numbers

	First checks signs, then looks at values

*/
bool BigInt::biggerThan(const BigInt& bi) {

	if (areSignsDifferent(bi)) {
		return compareSigns(bi);
	}
	else return compareChunks(bi) == 1;
}

bool BigInt::smallerThan(const BigInt& bi) {
	if (areSignsDifferent(bi)) {
		return !compareSigns(bi);
	}
	else return compareChunks(bi) == -1;
}

bool BigInt::equals(const BigInt& bi) {
	if (areSignsDifferent(bi)) return false;
	else return compareChunks(bi) == 0;
}

void checkComparsions() {
	BigInt a = BigInt(-5);
	BigInt b = BigInt(-5);

	if (a.biggerThan(b)) std::cout << "a is bigger than b";
	if (a.smallerThan(b)) std::cout << "a is smaller than b";
	if (a.equals(b)) std::cout << "a equals b";

}