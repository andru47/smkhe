#include "smkhe/mk_keygen.h"
#include "smkhe/rand_util.h"


namespace smkhe {
    MKKeygen::MKKeygen(smkhe::Parameters params, uint64_t seed) : params(params), seed(seed),
                                                                  secretKey(params.getModulusLevels(),
                                                                            params.getSpecialPrimesNumber(),
                                                                            params.getRingDegree()) {}

    void resetKey(SecretKey &secretKey, bool &isSecretAttached, Parameters &params) {
        secretKey.getPolysQ().clear();
        secretKey.getPolysQ().clear();
        secretKey.getPolysQ().resize(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree()));
        secretKey.getPolysP().resize(params.getSpecialPrimesNumber(), Polynomial<uint64_t>(params.getRingDegree()));
        isSecretAttached = false;
    }

    SecretKey MKKeygen::generateSecretKey() {
        resetKey(secretKey, isSecretAttached, params);
        sampleHWT(secretKey.getPolysQ(), secretKey.getPolysP(), params.getPrimes(), params.getSpecialPrimes());

        for (int level = 0; level < params.getModulusLevels(); ++level) {
            params.getTransformerQ().toNTT(secretKey.getPolysQ()[level], level);
        }
        for (int level = 0; level < params.getSpecialPrimesNumber(); ++level) {
            params.getTransformerP().toNTT(secretKey.getPolysP()[level], level);
        }

        isSecretAttached = true;

        return secretKey;
    }

    MKPublicKey MKKeygen::generatePublicKey() {
        if (!isSecretAttached) {
            throw runtime_error("Cant generate without a secret key. Create or attach one first.");
        }
        vector<Polynomial<uint64_t>> polyAQ(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree())),
                polyBQ(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree())),
                polyAP(params.getSpecialPrimesNumber(), Polynomial<uint64_t>(params.getRingDegree())),
                polyBP(params.getSpecialPrimesNumber(), Polynomial<uint64_t>(params.getRingDegree()));

        sampleDoublePolynomialWithSeed(polyBQ, polyBP, params.getPrimes(), params.getSpecialPrimes(), seed);

        for (int level = 0; level < params.getModulusLevels(); ++level) {
            params.getTransformerQ().toNTT(polyBQ[level], level);
        }
        for (int level = 0; level < params.getSpecialPrimesNumber(); ++level) {
            params.getTransformerP().toNTT(polyBP[level], level);
        }

        polyAQ = polyBQ;
        polyAP = polyBP;

        for (int level = 0; level < params.getModulusLevels(); ++level) {
            polyAQ[level].negate(params.getModulus(level));
            polyAQ[level].multiply(secretKey.getPolysQ()[level], params.getModulus(level));
        }
        for (int level = 0; level < params.getSpecialPrimesNumber(); ++level) {
            polyAP[level].negate(params.getSpecialPrimes()[level]);
            polyAP[level].multiply(secretKey.getPolysP()[level], params.getSpecialPrimes()[level]);
        }

        sampleDoubleErrorAndAdd(polyAQ, polyAP, params.getPrimes(), params.getSpecialPrimes(), params.getTransformerQ(),
                                params.getTransformerP());

        return MKPublicKey(polyAQ, polyAP, polyBQ, polyBP);
    }

    MKEvaluationKey MKKeygen::generateEvaluationKey(MKPublicKey &publicKey) {
        if (!isSecretAttached) {
            throw runtime_error("Cant generate without a secret key. Create or attach one first.");
        }

        vector<Polynomial<uint64_t>> d0P(params.getSpecialPrimesNumber(),
                                         Polynomial<uint64_t>(params.getRingDegree())), d0Q(params.getModulusLevels(),
                                                                                            Polynomial<uint64_t>(params.getRingDegree()));
        vector<Polynomial<uint64_t>> d1P(params.getSpecialPrimesNumber(),
                                         Polynomial<uint64_t>(params.getRingDegree())), d1Q(params.getModulusLevels(),
                                                                                            Polynomial<uint64_t>(params.getRingDegree()));
        vector<Polynomial<uint64_t>> d2P(params.getSpecialPrimesNumber(),
                                         Polynomial<uint64_t>(params.getRingDegree())), d2Q(params.getModulusLevels(),
                                                                                            Polynomial<uint64_t>(params.getRingDegree()));


        sampleDoublePolynomial(d1Q, d1P, params.getPrimes(), params.getSpecialPrimes());
        vector<Polynomial<uint64_t>> rP(params.getSpecialPrimesNumber(),
                                        Polynomial<uint64_t>(params.getRingDegree())), rQ(params.getModulusLevels(),
                                                                                          Polynomial<uint64_t>(params.getRingDegree()));
        sampleHWT(rQ, rP, params.getPrimes(), params.getSpecialPrimes());

        vector<uint64_t> pModQ(params.getPrimes().size());
        for (int i = 0; i < params.getPrimes().size(); ++i) {
            pModQ[i] = 1;
            for (int j = 0; j < params.getSpecialPrimesNumber(); ++j) {
                pModQ[i] = modMultiply(pModQ[i], params.getSpecialPrimes()[j], params.getPrimes()[i]);
            }
        }

        for (int level = 0; level < params.getModulusLevels(); ++level) {
            params.getTransformerQ().toNTT(rQ[level], level);
            params.getTransformerQ().toNTT(d1Q[level], level);

            d0Q[level] = d1Q[level];
            d0Q[level].negate(params.getModulus(level));
            d0Q[level].multiply(secretKey.getPolysQ()[level], params.getModulus(level));

            Polynomial<uint64_t> pMultR = rQ[level];
            pMultR.multiply(pModQ[level], params.getModulus(level));

            d0Q[level].add(pMultR, params.getModulus(level));

            d2Q[level] = rQ[level];
            d2Q[level].multiply(publicKey.getPolyBQ(level), params.getModulus(level));
            Polynomial<uint64_t> pMultS = secretKey.getPolyQ(level);
            pMultS.multiply(pModQ[level], params.getModulus(level));
            d2Q[level].add(pMultS, params.getModulus(level));
        }

        for (int level = 0; level < params.getSpecialPrimesNumber(); ++level) {
            params.getTransformerP().toNTT(rP[level], level);
            params.getTransformerP().toNTT(d1P[level], level);

            d0P[level] = d1P[level];
            d0P[level].negate(params.getSpecialPrimes()[level]);
            d0P[level].multiply(secretKey.getPolysP()[level], params.getSpecialPrimes()[level]);

            d2P[level] = rP[level];
            d2P[level].multiply(publicKey.getPolyBP(level), params.getSpecialPrimes()[level]);
        }

        sampleDoubleErrorAndAdd(d0Q, d0P, params.getPrimes(), params.getSpecialPrimes(), params.getTransformerQ(),
                                params.getTransformerP());
        sampleDoubleErrorAndAdd(d2Q, d2P, params.getPrimes(), params.getSpecialPrimes(), params.getTransformerQ(),
                                params.getTransformerP());

        return MKEvaluationKey(d0P, d0Q, d1P, d1Q, d2P, d2Q);
    }
}
