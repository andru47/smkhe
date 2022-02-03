#include <cstdint>
#include <complex>
#include <vector>

using namespace std;

#ifndef SMKHE_UTIL_H
#define SMKHE_UTIL_H

uint64_t modMultiply(uint64_t a, uint64_t b, uint64_t mod);

uint64_t modAdd(uint64_t a, uint64_t b, uint64_t mod);

uint64_t fastExp(uint64_t base, uint64_t exponent, uint64_t mod);

void bitReverse(vector<uint64_t> &vec);

#endif //SMKHE_UTIL_H
