//
// Created by Andru Stefanescu on 09.02.2022.
//

#include "smkhe/evaluation_key.h"

EvaluationKey::EvaluationKey(vector<Polynomial<uint64_t>> &aP, vector<Polynomial<uint64_t>> &aQ,
                             vector<Polynomial<uint64_t>> &bP, vector<Polynomial<uint64_t>> &bQ) : aP(aP), aQ(aQ), bP(bP), bQ(bQ) {}
