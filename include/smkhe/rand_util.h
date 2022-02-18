#include "smkhe/util.h"
#include "smkhe/math/polynomial.h"
#include "smkhe/math/ntt_transformer.h"
#include <random>

#ifndef SMKHE_RAND_UTIL_H
#define SMKHE_RAND_UTIL_H

namespace smkhe {
    uint64_t getRandom(uint64_t end);

    void sampleHWT(vector<Polynomial<uint64_t>> &poly, vector<Polynomial<uint64_t>> &polyP, vector<uint64_t> &primes,
                   vector<uint64_t> &specialPrimes);

    long long getRandomError();

    void samplePolynomial(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes, bool isForPublic);

    void sampleDoublePolynomial(vector<Polynomial<uint64_t>> &polyQ, vector<Polynomial<uint64_t>> &polyP,
                                vector<uint64_t> &primesQ, vector<uint64_t> &primesP);

    void sampleErrorAndAdd(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes, NTTTransformer &transformer);

    void sampleDoubleErrorAndAdd(vector<Polynomial<uint64_t>> &polyQ, vector<Polynomial<uint64_t>> &polyP,
                                 vector<uint64_t> &primesQ, vector<uint64_t> &primesP, NTTTransformer &transformerQ,
                                 NTTTransformer &transformerP);
}

#endif //SMKHE_RAND_UTIL_H
