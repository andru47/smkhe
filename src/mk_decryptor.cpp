#include "smkhe/mk_decryptor.h"
#include "smkhe/rand_util.h"

namespace smkhe {
    MKDecryptor::MKDecryptor(Parameters &params) : params(params) {}

    PartialCiphertext
    MKDecryptor::partialDecryption(MKCiphertext &givenCipher, uint64_t id, SecretKey secretKey) {
        vector<Polynomial<uint64_t>> result(givenCipher.getCipherForIndex(id).size());

        for (int level = 0; level < givenCipher.getCipherForIndex(id).size(); ++level) {
            result[level] = givenCipher.getCipherForIndex(id)[level];
            result[level].multiply(secretKey.getPolysQ()[level], params.getModulus(level));
            sampleSingleErrorAndAdd(result[level], level, params);
        }

        return PartialCiphertext(result);
    }

    Plaintext MKDecryptor::mergeDecryptions(MKCiphertext &givenCipher,
                                            vector<PartialCiphertext> &givenPartialDecryptions) {
        Polynomial<uint64_t> decryptedPolynomial = givenCipher.getCipherForIndex(0)[0];
        for (auto &currentPartial: givenPartialDecryptions) {
            decryptedPolynomial.add(currentPartial.getPolynomial()[0], params.getModulus(0));
        }
        params.getTransformerQ().fromNTT(decryptedPolynomial, 0);

        return Plaintext({decryptedPolynomial}, params, 0);
    }
}
