#include "smkhe/math/polynomial.h"

#ifndef SMKHE_MK_EVALUATION_KEY_H
#define SMKHE_MK_EVALUATION_KEY_H

namespace smkhe {
    class MKEvaluationKey {
    public:
        vector<Polynomial<uint64_t>> d0P, d0Q;
        vector<Polynomial<uint64_t>> d1P, d1Q;
        vector<Polynomial<uint64_t>> d2P, d2Q;

        MKEvaluationKey(vector<Polynomial<uint64_t>> &d0P, vector<Polynomial<uint64_t>> &d0Q,
                        vector<Polynomial<uint64_t>> &d1P, vector<Polynomial<uint64_t>> &d1Q,
                        vector<Polynomial<uint64_t>> &d2P, vector<Polynomial<uint64_t>> &d2Q);

        MKEvaluationKey();

        void serialize(string &resultedString);

        void deserialize(string &givenString);
    };
}

#endif //SMKHE_MK_EVALUATION_KEY_H
