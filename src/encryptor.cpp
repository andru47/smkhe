#include "smkhe/math/polynomial.h"
#include "smkhe/encryptor.h"

Encryptor::Encryptor(Parameters params) : params(params) {}

Ciphertext Encryptor::encrypt(Plaintext &plain, PublicKey &pub) {
    vector<Polynomial<uint64_t>> encryptedPolysA(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree()));
    vector<Polynomial<uint64_t>> encryptedPolysB(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree()));
    vector<Polynomial<uint64_t>> v(params.getModulusLevels(), Polynomial<uint64_t>(params.getRingDegree()));
    samplePolynomial(v, params.getPrimes());

    for (int level = 0; level < params.getModulusLevels(); ++level) {
        params.getTransformer().toNTT(plain.getPolynomial(level), level);
        params.getTransformer().toNTT(v[level], level);
        encryptedPolysA[level] = pub.getA(level);
        encryptedPolysA[level].multiply(v[level], params.getPrimes()[level]);
        encryptedPolysA[level].add(plain.getPolynomial(level), params.getPrimes()[level]);

        encryptedPolysB[level] = pub.getB(level);
        encryptedPolysB[level].multiply(v[level], params.getPrimes()[level]);
    }

    sampleErrorAndAdd(encryptedPolysA, params.getPrimes(), params.getTransformer());
    sampleErrorAndAdd(encryptedPolysB, params.getPrimes(), params.getTransformer());
    return Ciphertext(params, 0, encryptedPolysA, encryptedPolysB);
}

Plaintext Encryptor::decrypt(Ciphertext &cipher, SecretKey &secret) {
    vector<Polynomial<uint64_t>> decryptedPolynomial = cipher.getPolysB();

    for (int level = 0; level < params.getModulusLevels(); ++level) {
        decryptedPolynomial[level].multiply(secret.getPoly(level), params.getPrimes()[level]);
        decryptedPolynomial[level].add(cipher.getPolyA(level), params.getPrimes()[level]);
        params.getTransformer().fromNTT(decryptedPolynomial[level], level);
    }

    return Plaintext(decryptedPolynomial, params, cipher.getLevel());
}
