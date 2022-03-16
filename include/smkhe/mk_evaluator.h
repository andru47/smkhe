#include "smkhe/parameters.h"
#include "smkhe/mk_ciphertext.h"
#include "smkhe/math/rns_transformer.h"
#include "smkhe/mk_public_key.h"
#include "smkhe/mk_evaluation_key.h"
#include "smkhe/plaintext.h"
#include <unordered_map>

#ifndef SMKHE_MK_EVALUATOR_H
#define SMKHE_MK_EVALUATOR_H

namespace smkhe {
    class MKEvaluator {
        Parameters params;
        RNSTransformer rnsTransformer;
        unordered_map<uint64_t, MKPublicKey> publicKeys;
        unordered_map<uint64_t, MKEvaluationKey> evaluationKeys;

    public:
        explicit MKEvaluator(Parameters &params, unordered_map<uint64_t, MKPublicKey> &publicKeys,
                             unordered_map<uint64_t, MKEvaluationKey> &evaluationKeys);

        MKCiphertext add(MKCiphertext &cipherA, MKCiphertext &cipherB);

        MKCiphertext multiplyAndRelinearize(MKCiphertext &cipherA, MKCiphertext &cipherB);

        void rescaleInPlace(MKCiphertext &cipher);

        void addPlainInPlace(MKCiphertext &cipher, Plaintext &plain);

        void multiplyPlainInPlace(MKCiphertext &cipher, Plaintext &plain);

        void negateInPlace(MKCiphertext &cipher);
    };
}

#endif //SMKHE_MK_EVALUATOR_H
