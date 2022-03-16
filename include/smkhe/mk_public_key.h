#include "smkhe/math/polynomial.h"
#include "smkhe/public_key.h"

#ifndef SMKHE_MK_PUBLIC_KEY_H
#define SMKHE_MK_PUBLIC_KEY_H

namespace smkhe {
    class MKPublicKey {
        vector<Polynomial<uint64_t>> polyAQ, polyBQ;
        vector<Polynomial<uint64_t>> polyAP, polyBP;
    public:
        MKPublicKey(vector<Polynomial<uint64_t>> &polyAQ, vector<Polynomial<uint64_t>> &polyAP,
                    vector<Polynomial<uint64_t>> &polyBQ, vector<Polynomial<uint64_t>> &polyBP);

        MKPublicKey();

        Polynomial<uint64_t> &getPolyAQ(int level);

        Polynomial<uint64_t> &getPolyAP(int level);

        Polynomial<uint64_t> &getPolyBQ(int level);

        Polynomial<uint64_t> &getPolyBP(int level);

        PublicKey getPublicKey();

        void serialize(string &resultedString);

        void deserialize(string &givenString);

        int getLevelsQ();

        int getLevelsP();
    };
}

#endif //SMKHE_MK_PUBLIC_KEY_H
