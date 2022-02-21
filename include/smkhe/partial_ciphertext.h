#include "smkhe/math/polynomial.h"

#ifndef SMKHE_PARTIAL_CIPHERTEXT_H
#define SMKHE_PARTIAL_CIPHERTEXT_H

namespace smkhe {
    class PartialCiphertext {
        vector<Polynomial<uint64_t>> partiallyDecrypted;
    public:
        explicit PartialCiphertext(vector<Polynomial<uint64_t>> &partiallyDecrypted);

        PartialCiphertext();

        vector<Polynomial<uint64_t>> &getPolynomial();

        void serialize(string &resultedString);

        void deserialize(string &givenString);
    };
}

#endif //SMKHE_PARTIAL_CIPHERTEXT_H
