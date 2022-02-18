#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

#ifndef SMKHE_SECRET_KEY_H
#define SMKHE_SECRET_KEY_H

namespace smkhe {
    class SecretKey {
        vector<Polynomial<uint64_t>> polyQ;
        vector<Polynomial<uint64_t>> polyP;

    public:
        SecretKey(int levels, int specialLevels, int ringDegree);

        SecretKey();

        SecretKey(vector<Polynomial<uint64_t>> givenPolyQ, vector<Polynomial<uint64_t>> givenPolyP);

        Polynomial<uint64_t> &getPolyQ(int level);

        Polynomial<uint64_t> &getPolyP(int level);

        vector<Polynomial<uint64_t>> &getPolysQ();

        vector<Polynomial<uint64_t>> &getPolysP();

        void serialize(string &resultedString);

        void deserialize(string &givenString);
    };
}

#endif //SMKHE_SECRET_KEY_H
