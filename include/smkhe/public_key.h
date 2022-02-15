#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

#ifndef SMKHE_PUBLIC_KEY_H
#define SMKHE_PUBLIC_KEY_H

class PublicKey {
    vector<Polynomial<uint64_t>> polyA, polyB;

public:
    PublicKey(vector<Polynomial<uint64_t>> polyA, vector<Polynomial<uint64_t>> polyB);

    PublicKey();

    int getLevels();

    Polynomial<uint64_t> getA(int level);

    Polynomial<uint64_t> getB(int level);

    void serialize(string &givenString);

    void deserialize(string &givenString);
};

#endif //SMKHE_PUBLIC_KEY_H
