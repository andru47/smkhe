#include "smkhe/rand_util.h"

namespace smkhe {
    uint64_t getRandom(uint64_t end) {
        static random_device rd;
        static mt19937 generator(rd());
        uniform_int_distribution<uint64_t> distribution(0, end);
        return distribution(generator);
    }

    uint64_t getRandomWithSeed(uint64_t end, uint64_t seed) {
        mt19937 generator(seed);
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

    void sampleDoublePolynomial(vector<Polynomial<uint64_t>> &polyQ, vector<Polynomial<uint64_t>> &polyP,
                                vector<uint64_t> &primesQ, vector<uint64_t> &primesP) {
        for (int i = 0; i < polyQ[0].getDegree(); ++i) {
            uint64_t generated = getRandom(primesQ.back() - 1);
            for (int level = 0; level < primesQ.size(); ++level) {
                uint64_t modGenerated = generated % primesQ[level];
                polyQ[level].setCoeff(i, modGenerated);
            }
            for (int level = 0; level < primesP.size(); ++level) {
                uint64_t modGenerated = generated % primesP[level];
                polyP[level].setCoeff(i, modGenerated);
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

    void sampleDoubleErrorAndAdd(vector<Polynomial<uint64_t>> &polyQ, vector<Polynomial<uint64_t>> &polyP,
                                 vector<uint64_t> &primesQ, vector<uint64_t> &primesP, NTTTransformer &transformerQ,
                                 NTTTransformer &transformerP) {
        vector<Polynomial<uint64_t>> errorsQ(polyQ.size(), Polynomial<uint64_t>(polyQ[0].getDegree())), errorsP(
                polyP.size(), Polynomial<uint64_t>(polyP[0].getDegree()));
        for (int i = 0; i < polyQ[0].getDegree(); ++i) {
            long long error = getRandomError();
            for (int level = 0; level < primesQ.size(); ++level) {
                uint64_t currentError = error < 0 ? (primesQ[level] + error) : error;
                errorsQ[level].setCoeff(i, currentError);
            }
            for (int level = 0; level < primesP.size(); ++level) {
                uint64_t currentError = error < 0 ? (primesP[level] + error) : error;
                errorsP[level].setCoeff(i, currentError);
            }
        }
        for (int level = 0; level < primesQ.size(); ++level) {
            transformerQ.toNTT(errorsQ[level], level);
            polyQ[level].add(errorsQ[level], primesQ[level]);
        }
        for (int level = 0; level < primesP.size(); ++level) {
            transformerP.toNTT(errorsP[level], level);
            polyP[level].add(errorsP[level], primesP[level]);
        }
    }

    void sampleHWT(vector<Polynomial<uint64_t>> &poly, vector<Polynomial<uint64_t>> &polyP, vector<uint64_t> &primes,
                   vector<uint64_t> &specialPrimes) {
        int howManySampled = 0;
        int hammingDistance = 128;
        while (howManySampled < hammingDistance) {
            uint64_t position = getRandom(poly[0].getDegree() - 1);
            if (!poly[0].getCoeff(position)) {
                ++howManySampled;
                uint64_t generated = getRandom(1);
                for (int level = 0; level < primes.size(); ++level) {
                    uint64_t modGeneratedQ = !generated ? (primes[level] - 1) : generated;
                    poly[level].setCoeff(position, modGeneratedQ);
                }
                for (int level = 0; level < specialPrimes.size(); ++level) {
                    uint64_t modGeneratedP = !generated ? (specialPrimes[level] - 1) : generated;
                    polyP[level].setCoeff(position, modGeneratedP);
                }
            }
        }
    }

    void sampleSingleErrorAndAdd(Polynomial<uint64_t>& poly, int level, Parameters& givenParams) {
        Polynomial<uint64_t> errorPoly(poly.getDegree());
        for (int index = 0; index < poly.getDegree(); ++index) {
            long long error = getRandomError();
            uint64_t positiveError = error < 0 ? (error + givenParams.getModulus(level)) : error;
            positiveError %= givenParams.getModulus(level);
            errorPoly.setCoeff(index, positiveError);
        }
        givenParams.getTransformerQ().toNTT(errorPoly, level);
        poly.add(errorPoly, givenParams.getModulus(level));
    }

    void sampleDoublePolynomialWithSeed(vector<Polynomial<uint64_t>> &polyQ, vector<Polynomial<uint64_t>> &polyP,
                                        vector<uint64_t> &primesQ, vector<uint64_t> &primesP, uint64_t seed) {
        for (int i = 0; i < polyQ[0].getDegree(); ++i) {
            uint64_t generated = getRandomWithSeed(primesQ.back() - 1, seed);
            for (int level = 0; level < primesQ.size(); ++level) {
                uint64_t modGenerated = generated % primesQ[level];
                polyQ[level].setCoeff(i, modGenerated);
            }
            for (int level = 0; level < primesP.size(); ++level) {
                uint64_t modGenerated = generated % primesP[level];
                polyP[level].setCoeff(i, modGenerated);
            }
        }
    }
}
