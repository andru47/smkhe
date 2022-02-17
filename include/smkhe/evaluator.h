#include "smkhe/parameters.h"
#include "smkhe/evaluation_key.h"
#include "smkhe/ciphertext.h"
#include "smkhe/plaintext.h"
#include "smkhe/math/rns_transformer.h"

#ifndef SMKHE_EVALUATOR_H
#define SMKHE_EVALUATOR_H

namespace smkhe {
    class Evaluator {
        Parameters params;
        EvaluationKey evk;
        RNSTransformer rnsTransformer;

    public:
        Evaluator(Parameters params, EvaluationKey evk);

        void rescaleInPlace(Ciphertext &cipher);

        void multiplyRelinInPlace(Ciphertext &cipherA, Ciphertext &cipherB);

        void multiplyPlainInPlace(Ciphertext &cipher, Plaintext &plaintext);

        void addPlainInPlace(Ciphertext &cipher, Plaintext &plaintext);

        void addCipherInPlace(Ciphertext &cipherA, Ciphertext &cipherB);

        void negateInPlace(Ciphertext &cipher);
    };
}

#endif //SMKHE_EVALUATOR_H
