#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

#ifndef SMKHE_CIPHERTEXT_H
#define SMKHE_CIPHERTEXT_H

class Ciphertext {
    vector<Polynomial<uint64_t>> polysA, polysB;
    int level;
    Parameters params;

public:
    Ciphertext(Parameters params, int level, vector<Polynomial<uint64_t>> polysA, vector<Polynomial<uint64_t>> polysB);

    Polynomial<uint64_t> &getPolyA(int givenLevel);

    Polynomial<uint64_t> &getPolyB(int givenLevel);

    vector<Polynomial<uint64_t>> &getPolysB();

    int getLevel();
};

#endif //SMKHE_CIPHERTEXT_H
