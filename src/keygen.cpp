#include <random>
#include "smkhe/keygen.h"

Keygen::Keygen(Parameters params) : params(params), secretKey(params), secretKeyAttached(false) {}

Keygen::Keygen(Parameters params, SecretKey secretKey) : params(params), secretKey(secretKey),
                                                         secretKeyAttached(true) {}

bool Keygen::isSecretKeyAttached() {
    return secretKeyAttached;
}

SecretKey Keygen::generateSecretKey() {
    samplePolynomial(secretKey.getPolys(), params.getPrimes());
    for (int level = 0; level < params.getModulusLevels(); ++level) {
        params.getTransformer().toNTT(secretKey.getPolys()[level], level);
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

    samplePolynomial(polyB, params.getPrimes());
    for (int level = 0; level < params.getModulusLevels(); ++level) {
        params.getTransformer().toNTT(polyB[level], level);
    }
    polyA = secretKey.getPolys();
    for (int level = 0; level < params.getModulusLevels(); ++level) {
        polyA[level].multiply(polyB[level], params.getPrimes()[level]);
        polyA[level].multiply(params.getPrimes()[level] - 1, params.getPrimes()[level]);
    }
    sampleErrorAndAdd(polyA, params.getPrimes(), params.getTransformer());

    return PublicKey(params, polyA, polyB);
}