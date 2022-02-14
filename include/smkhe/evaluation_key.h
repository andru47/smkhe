#include "smkhe/util.h"
#include "smkhe/math/polynomial.h"

#ifndef SMKHE_EVALUATION_KEY_H
#define SMKHE_EVALUATION_KEY_H

class EvaluationKey {
public:
    vector<Polynomial<uint64_t>> aP, aQ, bP, bQ;

    EvaluationKey(vector<Polynomial<uint64_t>> &aP, vector<Polynomial<uint64_t>> &aQ, vector<Polynomial<uint64_t>> &bP,
                  vector<Polynomial<uint64_t>> &bQ);
};

#endif //SMKHE_EVALUATION_KEY_H
