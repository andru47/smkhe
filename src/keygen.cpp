#include <random>
#include "smkhe/keygen.h"

namespace smkhe {
    Keygen::Keygen(Parameters params) : params(params),
                                        secretKey(params.getModulusLevels(), params.getSpecialPrimesNumber(),
                                                  params.getRingDegree()), secretKeyAttached(false) {}

    Keygen::Keygen(Parameters params, SecretKey secretKey) : params(params), secretKey(secretKey),
                                                             secretKeyAttached(true) {}

    bool Keygen::isSecretKeyAttached() {
        return secretKeyAttached;
    }

    void resetKey(SecretKey &secretKey, int levels, int specialPrimes) {
        for (int i = 0; i < levels; ++i) {
            for (int j = 0; j < secretKey.getPolysQ()[i].getDegree(); ++j) {
                secretKey.getPolysQ()[i].setCoeff(j, 0);
            }
            secretKey.getPolysQ()[i].setTransformedToNTT(false);
        }
        for (int i = 0; i < specialPrimes; ++i) {
            for (int j = 0; j < secretKey.getPolysQ()[i].getDegree(); ++j) {
                secretKey.getPolysP()[i].setCoeff(j, 0);
            }
            secretKey.getPolysP()[i].setTransformedToNTT(false);
        }
    }

    SecretKey Keygen::generateSecretKey() {
        if (secretKeyAttached) {
            resetKey(secretKey, params.getModulusLevels(), params.getSpecialPrimesNumber());
        }
        sampleHWT(secretKey.getPolysQ(), secretKey.getPolysP(), params.getPrimes(), params.getSpecialPrimes());
        for (int level = 0; level < params.getModulusLevels(); ++level) {
            params.getTransformerQ().toNTT(secretKey.getPolysQ()[level], level);
        }
        for (int level = 0; level < params.getSpecialPrimesNumber(); ++level) {
            params.getTransformerP().toNTT(secretKey.getPolysP()[level], level);
        }
        secretKeyAttached = true;

        return secretKey;
    }

    PublicKey Keygen::generatePublicKey() {
        if (!secretKeyAttached) {
            throw ("Cant generate without a secret key. Create or attach one first.");
        }
        vector<Polynomial<uint64_t>> polyA(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree())),
                polyB(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree()));

        samplePolynomial(polyB, params.getPrimes(), true);
        for (int level = 0; level < params.getModulusLevels(); ++level) {
            params.getTransformerQ().toNTT(polyB[level], level);
        }
        polyA = secretKey.getPolysQ();
        for (int level = 0; level < params.getModulusLevels(); ++level) {
            polyA[level].multiply(polyB[level], params.getPrimes()[level]);
            polyA[level].multiply(params.getPrimes()[level] - 1, params.getPrimes()[level]);
        }
        sampleErrorAndAdd(polyA, params.getPrimes(), params.getTransformerQ());

        return PublicKey(polyA, polyB);
    }

    EvaluationKey Keygen::generateEvaluationKey() {
        vector<uint64_t> pModQ(params.getPrimes().size());
        for (int i = 0; i < params.getPrimes().size(); ++i) {
            pModQ[i] = 1;
            for (int j = 0; j < params.getSpecialPrimesNumber(); ++j) {
                pModQ[i] = modMultiply(pModQ[i], params.getSpecialPrimes()[j], params.getPrimes()[i]);
            }
        }
        vector<Polynomial<uint64_t>> secretSquared(params.getModulusLevels(),
                                                   Polynomial<uint64_t>(params.getRingDegree()));
        for (int i = 0; i < params.getModulusLevels(); ++i) {
            for (int j = 0; j < params.getRingDegree(); ++j) {
                secretSquared[i].setCoeff(j, fastExp(secretKey.getPolyQ(i).getCoeff(j), 2, params.getPrimes()[i]));
                secretSquared[i].setCoeff(j,
                                          modMultiply(secretSquared[i].getCoeff(j), pModQ[i], params.getPrimes()[i]));
            }
        }
        vector<Polynomial<uint64_t>> aQ(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree())), aP(
                params.getSpecialPrimesNumber(), Polynomial<uint64_t>(params.getRingDegree()));
        sampleDoublePolynomial(aQ, aP, params.getPrimes(), params.getSpecialPrimes());

        vector<Polynomial<uint64_t>> bQ(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree())), bP(
                params.getSpecialPrimesNumber(), Polynomial<uint64_t>(params.getRingDegree()));
        for (int i = 0; i < params.getModulusLevels(); ++i) {
            aQ[i].setTransformedToNTT(true);
            bQ[i].setTransformedToNTT(true);
        }
        for (int i = 0; i < params.getSpecialPrimesNumber(); ++i) {
            aP[i].setTransformedToNTT(true);
            bP[i].setTransformedToNTT(true);
        }
        for (int elementIndex = 0; elementIndex < params.getRingDegree(); ++elementIndex) {
            for (int i = 0; i < params.getSpecialPrimesNumber(); ++i) {
                bP[i].setCoeff(elementIndex, params.getSpecialPrimes()[i] - aP[i].getCoeff(elementIndex));
                bP[i].setCoeff(elementIndex,
                               modMultiply(bP[i].getCoeff(elementIndex),
                                           secretKey.getPolysP()[i].getCoeff(elementIndex),
                                           params.getSpecialPrimes()[i]));
            }
            for (int i = 0; i < params.getModulusLevels(); ++i) {
                bQ[i].setCoeff(elementIndex, params.getPrimes()[i] - aQ[i].getCoeff(elementIndex));
                bQ[i].setCoeff(elementIndex,
                               modMultiply(bQ[i].getCoeff(elementIndex),
                                           secretKey.getPolysQ()[i].getCoeff(elementIndex),
                                           params.getPrimes()[i]));
                bQ[i].setCoeff(elementIndex,
                               modAdd(bQ[i].getCoeff(elementIndex), secretSquared[i].getCoeff(elementIndex),
                                      params.getPrimes()[i]));
            }
        }

        sampleDoubleErrorAndAdd(bQ, bP, params.getPrimes(), params.getSpecialPrimes(), params.getTransformerQ(),
                                params.getTransformerP());
        return EvaluationKey(aP, aQ, bP, bQ);
    }
}
