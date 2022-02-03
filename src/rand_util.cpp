#include "smkhe/rand_util.h"

uint64_t getRandom() {
    static random_device rd;
    static mt19937 generator(rd());
    static uniform_int_distribution<uint64_t> distribution(0, 2);
    return distribution(generator);
}

long long getRandomError() {
    static random_device rd;
    static mt19937 generator(rd());
    static normal_distribution<double> distribution(0, 3.2);
    double nr = distribution(generator);
    return llround(nr);
}

void samplePolynomial(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes) {
    for (int i = 0; i < poly[0].getDegree(); ++i) {
        uint64_t generated = getRandom();
        for (int level = 0; level < primes.size(); ++level) {
            uint64_t modGenerated = generated == 0 ? (primes[level] - 1) : (generated - 1);
            poly[level].setCoeff(i, modGenerated);
        }
    }
}

void sampleErrorAndAdd(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes, NTTTransformer &transformer) {
    vector<Polynomial<uint64_t>> errors(poly.size(), Polynomial<uint64_t>(poly[0].getDegree()));
    for (int i = 0; i < poly[0].getDegree(); ++i) {
        long long error = getRandomError();
        for (int level = 0; level < primes.size(); ++level) {
            uint64_t currentError = error < 0 ? (primes[level] + error) : error;
            errors[level].setCoeff(i, currentError);
        }
    }
    for (int level = 0; level < primes.size(); ++level) {
        transformer.toNTT(errors[level], level);
        poly[level].add(errors[level], primes[level]);
    }

}