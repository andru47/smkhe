#include "smkhe/util.h"

uint64_t modMultiply(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;

    while (b) {
        if (b % 2) {
            result = (result + a) % mod;
            b -= 1;
        }
        a = (a + a) % mod;
        b >>= 1;
    }

    return result;
}

uint64_t modAdd(uint64_t a, uint64_t b, uint64_t mod) {
    return (a + b) % mod;
}

uint64_t fastExp(uint64_t base, uint64_t exponent, uint64_t mod) {
    if (!exponent) {
        return 1;
    }
    if (exponent == 1) {
        return base;
    }
    if (exponent % 2) {
        return modMultiply(base, fastExp(modMultiply(base, base, mod), exponent / 2, mod), mod);
    }

    return fastExp(modMultiply(base, base, mod), exponent / 2, mod);
}

void bitReverse(vector<uint64_t> &vec) {
    int n = vec.size();
    int bits = log2(n);
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t ci = i;
        uint32_t bitReversedI = 0;
        uint32_t currentPos = bits - 1;
        while (ci) {
            bitReversedI |= ((ci & 1) << currentPos);
            ci >>= 1;
            currentPos--;
        }
        if (i < bitReversedI) {
            swap(vec[i], vec[bitReversedI]);
        }
    }
}
