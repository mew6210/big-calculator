#include "../bigint.hpp"
#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>


/*
 * @brief NOT MY CODE!!!
 *
 * taken from https://ridiculousfish.com/blog/posts/labor-of-division-episode-v.html.
 *
 * modified to use std::countl_zero, instead of builtin compiler function so that its compatible. all comments left as they were
 *
 * Perform a narrowing division: 128 / 64 -> 64, and 64 / 32 -> 32.
 * The dividend's low and high words are given by \p numhi and \p numlo, respectively.
 * The divisor is given by \p den.
 * \return the quotient, and the remainder by reference in \p r, if not null.
 * If the quotient would require more than 64 bits, or if denom is 0, then return the max value
 * for both quotient and remainder.
 *
 * These functions are released into the public domain, where applicable, or the CC0 license.
 */
    uint64_t divllu(uint64_t numhi, uint64_t numlo, uint64_t den, uint64_t* r)
{
    // We work in base 2**32.
    // A uint32 holds a single digit. A uint64 holds two digits.
    // Our numerator is conceptually [num3, num2, num1, num0].
    // Our denominator is [den1, den0].
    const uint64_t b = (1ull << 32);

    // The high and low digits of our computed quotient.
    uint32_t q1;
    uint32_t q0;

    // The normalization shift factor.
    int shift;

    // The high and low digits of our denominator (after normalizing).
    // Also the low 2 digits of our numerator (after normalizing).
    uint32_t den1;
    uint32_t den0;
    uint32_t num1;
    uint32_t num0;

    // A partial remainder.
    uint64_t rem;

    // The estimated quotient, and its corresponding remainder (unrelated to true remainder).
    uint64_t qhat;
    uint64_t rhat;

    // Variables used to correct the estimated quotient.
    uint64_t c1;
    uint64_t c2;

    // Check for overflow and divide by 0.
    if (numhi >= den) {
        if (r != NULL)
            *r = ~0ull;
        return ~0ull;
    }

    // Determine the normalization factor. We multiply den by this, so that its leading digit is at
    // least half b. In binary this means just shifting left by the number of leading zeros, so that
    // there's a 1 in the MSB.
    // We also shift numer by the same amount. This cannot overflow because numhi < den.
    // The expression (-shift & 63) is the same as (64 - shift), except it avoids the UB of shifting
    // by 64. The funny bitwise 'and' ensures that numlo does not get shifted into numhi if shift is 0.
    // clang 11 has an x86 codegen bug here: see LLVM bug 50118. The sequence below avoids it.
    shift = std::countl_zero(den);
    den <<= shift;
    numhi <<= shift;
    numhi |= (numlo >> (-shift & 63)) & (-(int64_t)shift >> 63);
    numlo <<= shift;

    // Extract the low digits of the numerator and both digits of the denominator.
    num1 = (uint32_t)(numlo >> 32);
    num0 = (uint32_t)(numlo & 0xFFFFFFFFu);
    den1 = (uint32_t)(den >> 32);
    den0 = (uint32_t)(den & 0xFFFFFFFFu);

    // We wish to compute q1 = [n3 n2 n1] / [d1 d0].
    // Estimate q1 as [n3 n2] / [d1], and then correct it.
    // Note while qhat may be 2 digits, q1 is always 1 digit.
    qhat = numhi / den1;
    rhat = numhi % den1;
    c1 = qhat * den0;
    c2 = rhat * b + num1;
    if (c1 > c2)
        qhat -= (c1 - c2 > den) ? 2 : 1;
    q1 = (uint32_t)qhat;

    // Compute the true (partial) remainder.
    rem = numhi * b + num1 - q1 * den;

    // We wish to compute q0 = [rem1 rem0 n0] / [d1 d0].
    // Estimate q0 as [rem1 rem0] / [d1] and correct it.
    qhat = rem / den1;
    rhat = rem % den1;
    c1 = qhat * den0;
    c2 = rhat * b + num0;
    if (c1 > c2)
        qhat -= (c1 - c2 > den) ? 2 : 1;
    q0 = (uint32_t)qhat;

    // Return remainder if requested.
    if (r != NULL)
        *r = (rem * b + num0 - q0 * den) >> shift;
    return ((uint64_t)q1 << 32) | q0;
}

/*
    @brief divides bigint by uChunkInt

    divides each chunk by val, and handles carrying remainders

    spits out a warning if there is a reminder, this calc only supports whole numbers
*/
void BigInt::divideChunkInt(uChunkInt val, bool toMute) {

    uint64_t remainder = 0;
    uint64_t* remptr = &remainder;
    std::vector<uChunkInt> chunksStored = {};

    for (size_t i = chunks.size(); i-- > 0;) {

        uint64_t res = divllu(remainder, chunks[i], val, remptr);       //function for dividing 128bit number by 64bit number
        chunksStored.push_back(res);
    }
    if (!toMute) {
        if (remainder != 0) {
            std::cout << "Warning: division did not produce a whole number. Remainder: " << remainder << "\n";
        }
    }
    std::reverse(chunksStored.begin(), chunksStored.end());
    chunks = chunksStored;
    trimTrailingChunks();
}

/*
    @brief returns remainder, basically % operation

    divides number n times by val, and only return remainder

    slimmed down version of divideChunkInt
*/
uChunkInt BigInt::moduloChunkInt(uChunkInt val) {
    uint64_t remainder = 0;
    uint64_t* remptr = &remainder;

    for (size_t i = chunks.size(); i-- > 0;) {
        divllu(remainder, chunks[i], val, remptr);       //function for dividing 128bit number by 64bit number
    }

    return remainder;
}

bool isQuotientTooLarge(BigInt& u, BigInt& v, uint64_t qhat, size_t j) {
    size_t n = v.chunks.size();

    // Simple test: if qhat == 0, it can't be too large
    if (qhat == 0) return false;

    // Create temp = v × qhat, shifted by j
    BigInt temp = v;
    temp.multiplyChunkInt(qhat);

    // Shift left by j digits (multiply by b^j)
    temp.chunks.insert(temp.chunks.begin(), j, 0);

    // Ensure temp has same size as u for comparison
    while (temp.chunks.size() < u.chunks.size()) {
        temp.chunks.push_back(0);
    }

    // Test if temp > u
    return temp.biggerThan(u);
}

// Subtract v × qhat from u, starting at position j
void subtractMultiple(BigInt& u, BigInt& v, uint64_t qhat, size_t j) {
    if (qhat == 0) return;

    // Create temp = v × qhat
    BigInt temp = v;
    temp.multiplyChunkInt(qhat);

    // Shift left by j digits
    temp.chunks.insert(temp.chunks.begin(), j, 0);

    // Ensure temp has same size as u
    while (temp.chunks.size() < u.chunks.size()) {
        temp.chunks.push_back(0);
    }
    while (temp.chunks.size() > u.chunks.size()) {
        u.chunks.push_back(0);
    }

    // Subtract temp from u
    u.subtractBigInt(temp);

    // Trim leading zeros
    u.trimTrailingChunks();
}


namespace divHelpers {
    uint64_t computeNormalizationFactor(uint64_t vn_1) {
        if (vn_1 >= (1ULL << 63)) {
            return 1;
        }
        uint64_t d = (1ULL << 63) / vn_1;
        if ((vn_1 * d) < (1ULL << 63)) {
            d++;
        }
        return d;
    }

    uint64_t estimateQuotient(uint64_t u2, uint64_t u1, uint64_t u0,
        uint64_t v1, uint64_t v0) {
        if (u2 == v1) {
            return 0xFFFFFFFFFFFFFFFFULL;
        }

        // Use divllu for 128-by-64 division
        uint64_t remainder;
        uint64_t qhat = divllu(u2, u1, v1, &remainder);

        // Test if qhat is too large
        uint128Emul product = mult64to128(qhat, v0);
        uint128Emul test = { remainder, u0 };

        if (product.high > test.high ||
            (product.high == test.high && product.low > test.low)) {
            qhat--;
            remainder += v1;
            if (remainder < v1) {  // Check for overflow
                return qhat;
            }

            // Test again
            product = mult64to128(qhat, v0);
            if (product.high > remainder ||
                (product.high == remainder && product.low > u0)) {
                qhat--;
            }
        }

        return qhat;
    }

    // Test if qhat × v > current portion of u
    
}

/*
    @brief implements knuth's algorithm d for long division of any base

    each step is marked as Dx where x is the number of a step in knuth's algorithm d specification, that can be found here: 
    https://www.haio.ir/app/uploads/2022/01/The-art-of-computer-programming.-Vol.2.-Seminumerical-algorithms-by-Knuth-Donald-E-z-lib.org_.pdf
    at about page 273 (pdf's 289) chapter 4.3.1

*/
void BigInt::divideBigInt(BigInt& divisor) {
    
    // Handle trivial cases using existing methods
    if (divisor.biggerThan(*this) && (isPositive && divisor.isPositive)) {
        chunks = { 0 };
        isPositive = true;
        return;
    }

    //fast path for divisor chunk's <2, since knuth's algorithm d doesnt allow divisor's with less digits than 2
    if (divisor.chunks.size() == 1) {
        uint64_t remainder = moduloChunkInt(divisor.chunks[0]);
        divideChunkInt(divisor.chunks[0],true);
        isPositive = !(isPositive ^ divisor.isPositive);
        if(remainder!=0) std::cout << "Division did not produce a whole number, remainder: " << remainder << "\n";
        
        return;
    }

    // Store signs
    bool resultSign = !(isPositive ^ divisor.isPositive);
    isPositive = true;
    divisor.isPositive = true;


    // D1[Normalize]
    uint64_t vn_1 = divisor.chunks.back();
    uint64_t d = divHelpers::computeNormalizationFactor(vn_1);

    // D0 [Define]
    BigInt u = *this;      // Dividend (will become remainder)
    BigInt v = divisor;    // Divisor

    //D1[Normalize]
    // Normalize if needed
    if (d > 1) {
        u.multiplyChunkInt(d);
        v.multiplyChunkInt(d);
    }

    //D0 [Define]
    size_t m = u.chunks.size() - v.chunks.size();
    size_t n = v.chunks.size();

    //D0 [Define]
    // Create quotient
    BigInt quotient;
    quotient.chunks.resize(m + 1, 0);
    quotient.isPositive = true;

    // Get the two most significant digits of divisor
    uint64_t v1 = v.chunks[n - 1];
    uint64_t v0 = (n > 1) ? v.chunks[n - 2] : 0;

    //D1 [Normalize]
    // Add an extra zero digit to u as required by Algorithm D
    u.chunks.push_back(0);

    // D2 [Initialize j]
    // Main division loop - Knuth's Algorithm D
    for (int j = m; j >= 0; j--) {
        // Get three high digits of current window in u
        uint64_t u2 = 0, u1 = 0, u0 = 0;

        size_t idx = j + n;
        if (idx < u.chunks.size()) u2 = u.chunks[idx];
        if (idx - 1 < u.chunks.size()) u1 = u.chunks[idx - 1];
        if (idx - 2 < u.chunks.size()) u0 = u.chunks[idx - 2];

        //D3 [Calculate qhat]
        // Estimate quotient digit using helper function
        uint64_t qhat = divHelpers::estimateQuotient(u2, u1, u0, v1, v0);

        //D5 [Test qhat] //knuth says at most 2 adjustments needed
        // Adjust qhat if necessary (at most 2 times)
        for (int adjust = 0; adjust < 2; adjust++) {
            if (isQuotientTooLarge(u, v, qhat, j)) {
                if (qhat > 0) {
                    qhat--;     //D6[Add back] 
                }
            }
            else {
                break;
            }
        }

        //D4 [Multiply and subtract]
        // Subtract v × qhat from u using helper function
        subtractMultiple(u, v, qhat, j);

        //D5 [Store qj]
        // Store quotient digit
        quotient.chunks[j] = qhat;

        // Early exit if remainder is zero
        if (u.chunks.size() == 1 && u.chunks[0] == 0) {
            // Fill remaining quotient digits with 0
            for (int k = j - 1; k >= 0; k--) {
                quotient.chunks[k] = 0;
            }
            break;
        }
    }   //D7 [Loop on j]

    // Remove any extra zero we added
    u.trimTrailingChunks();
    if (u.chunks.empty()) {
        u.chunks.push_back(0);
    }

    //D8 [Unnormalize]
    // Denormalize remainder if needed
    if (d > 1) {
        u.divideChunkInt(d, true);
    }

    // Set result (quotient)
    chunks = std::move(quotient.chunks);
    isPositive = resultSign;

    // Restore divisor's original sign
    divisor.isPositive = !divisor.isPositive;

    // Handle zero result
    if (chunks.size() == 1 && chunks[0] == 0) {
        isPositive = true;
    }

    if (!u.equals(BigInt("0"))) {
        std::cout << "Division did not produce a whole number, remainder: " << u.toString()<<"\n";
    }
}

//alternative function for getting the remainder, 
//no documentation, but it works the same way as the function above
DivResult divideUnsigned(BigInt a, BigInt b) {
    DivResult result;

    BigInt u = a;
    BigInt v = b;

    u.isPositive = true;
    v.isPositive = true;

    if (v.biggerThan(u)) {
        result.quotient.chunks = { 0 };
        result.quotient.isPositive = true;
        result.remainder = u;
        return result;
    }

    if (v.chunks.size() == 1) {
        uChunkInt rem = u.moduloChunkInt(v.chunks[0]);
        u.divideChunkInt(v.chunks[0],true);
        result.quotient = u;
        result.remainder.chunks = { rem };
        result.remainder.isPositive = true;
        return result;
    }

    uint64_t vn_1 = v.chunks.back();
    uint64_t d = divHelpers::computeNormalizationFactor(vn_1);

    if (d > 1) {
        u.multiplyChunkInt(d);
        v.multiplyChunkInt(d);
    }

    size_t m = u.chunks.size() - v.chunks.size();
    size_t n = v.chunks.size();

    result.quotient.chunks.resize(m + 1, 0);
    result.quotient.isPositive = true;

    u.chunks.push_back(0);

    uint64_t v1 = v.chunks[n - 1];
    uint64_t v0 = (n > 1) ? v.chunks[n - 2] : 0;

    for (int j = m; j >= 0; j--) {
        uint64_t u2 = 0, u1 = 0, u0 = 0;
        size_t idx = j + n;
        if (idx < u.chunks.size()) u2 = u.chunks[idx];
        if (idx - 1 < u.chunks.size()) u1 = u.chunks[idx - 1];
        if (idx - 2 < u.chunks.size()) u0 = u.chunks[idx - 2];

        uint64_t qhat = divHelpers::estimateQuotient(u2, u1, u0, v1, v0);

        for (int adjust = 0; adjust < 2; adjust++) {
            if (isQuotientTooLarge(u, v, qhat, j)) {
                if (qhat > 0) qhat--;
            }
            else {
                break;
            }
        }
        subtractMultiple(u, v, qhat, j);
        result.quotient.chunks[j] = qhat;
        if (u.chunks.size() == 1 && u.chunks[0] == 0) {
            for (int k = j - 1; k >= 0; k--) {
                result.quotient.chunks[k] = 0;
            }
            break;
        }
    }

    u.trimTrailingChunks();
    if (u.chunks.empty()) {
        u.chunks.push_back(0);
    }

    if (d > 1) {
        u.divideChunkInt(d, true);
    }

    result.remainder = u;
    result.remainder.isPositive = true;

    return result;
}