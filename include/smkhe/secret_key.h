#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

#ifndef SMKHE_SECRET_KEY_H
#define SMKHE_SECRET_KEY_H

class SecretKey {
    vector<Polynomial<uint64_t>> polyQ;
    vector<Polynomial<uint64_t>> polyP;
    Parameters params;
public:
    explicit SecretKey(Parameters params);

    SecretKey(Parameters params, vector<Polynomial<uint64_t>> givenPolyQ, vector<Polynomial<uint64_t>> givenPolyP);

    Polynomial<uint64_t> &getPolyQ(int level);

    Polynomial<uint64_t> &getPolyP(int level);

    vector<Polynomial<uint64_t>> &getPolysQ();

    vector<Polynomial<uint64_t>> &getPolysP();
};

#endif //SMKHE_SECRET_KEY_H
