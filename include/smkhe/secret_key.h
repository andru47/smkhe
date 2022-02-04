#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

#ifndef SMKHE_SECRET_KEY_H
#define SMKHE_SECRET_KEY_H


class SecretKey {
    vector<Polynomial<uint64_t>> poly;
    Parameters params;
public:
    explicit SecretKey(Parameters params);

    SecretKey(Parameters params, vector<Polynomial<uint64_t>> givenPoly);

    Polynomial<uint64_t> &getPoly(int level);

    vector<Polynomial<uint64_t>> &getPolys();
};

#endif //SMKHE_SECRET_KEY_H
