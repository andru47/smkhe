#include "smkhe/parameters.h"
#include "smkhe/math/polynomial.h"
#include "smkhe/mk_ciphertext.h"
#include "smkhe/secret_key.h"
#include "smkhe/plaintext.h"
#include "smkhe/partial_ciphertext.h"

#ifndef SMKHE_MK_DECRYPTOR_H
#define SMKHE_MK_DECRYPTOR_H

namespace smkhe {
    class MKDecryptor {
        Parameters params;
    public:
        explicit MKDecryptor(Parameters &params);

        PartialCiphertext partialDecryption(MKCiphertext &givenCipher, uint64_t id, SecretKey secretKey);

        Plaintext mergeDecryptions(MKCiphertext &givenCipher, vector<PartialCiphertext> &givenPartialDecryptions);
    };
}

#endif //SMKHE_MK_DECRYPTOR_H
