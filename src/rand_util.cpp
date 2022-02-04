#include "smkhe/rand_util.h"

uint64_t getRandom(uint64_t end) {
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<uint64_t> distribution(0, end);
    return distribution(generator);
}

long long getRandomError() {
    static random_device rd;
    static mt19937 generator(rd());
    normal_distribution<double> distribution(0, 3.2);
    double nr = distribution(generator);
    return llround(nr);
}

void samplePolynomial(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes, bool isForPublic) {
    for (int i = 0; i < poly[0].getDegree(); ++i) {
        uint64_t generated = getRandom(isForPublic ? primes.back() - 1 : 2);
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

void sampleHWT(vector<Polynomial<uint64_t>> &poly, vector<uint64_t> &primes) {
    int howManySampled = 0;
    int hammingDistance = 128;
    while (howManySampled < hammingDistance) {
        uint64_t position = getRandom(poly[0].getDegree() - 1);
        if (!poly[0].getCoeff(position)) {
            ++howManySampled;
            uint64_t generated = getRandom(1);
            for (int level = 0; level < primes.size(); ++level) {
                uint64_t modGenerated = !generated ? (primes[level] - 1) : generated;
                poly[level].setCoeff(position, modGenerated);
            }
        }
    }
}
