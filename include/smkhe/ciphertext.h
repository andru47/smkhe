#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

#ifndef SMKHE_CIPHERTEXT_H
#define SMKHE_CIPHERTEXT_H

namespace smkhe {
    class Ciphertext {
        vector<Polynomial<uint64_t>> polysA, polysB;
        int level;

    public:
        Ciphertext(int level, vector<Polynomial<uint64_t>> polysA, vector<Polynomial<uint64_t>> polysB);

        Ciphertext();

        Polynomial<uint64_t> &getPolyA(int givenLevel);

        Polynomial<uint64_t> &getPolyB(int givenLevel);

        vector<Polynomial<uint64_t>> &getPolysB();

        vector<Polynomial<uint64_t>> &getPolysA();

        int getLevel();

        void decreaseLevel(int howMany);

        void serialize(string &givenString);

        void deserialize(string &givenString);
    };
}

#endif //SMKHE_CIPHERTEXT_H
