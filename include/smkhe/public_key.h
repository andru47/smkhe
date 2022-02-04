#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

#ifndef SMKHE_PUBLIC_KEY_H
#define SMKHE_PUBLIC_KEY_H

class PublicKey {
    vector<Polynomial<uint64_t>> polyA, polyB;
    Parameters params;

public:
    PublicKey(Parameters params, vector<Polynomial<uint64_t>> polyA, vector<Polynomial<uint64_t>> polyB);

    Polynomial<uint64_t> getA(int level);

    Polynomial<uint64_t> getB(int level);
};

#endif //SMKHE_PUBLIC_KEY_H
