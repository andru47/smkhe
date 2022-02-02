#include <vector>
#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

#ifndef SMKHE_NTT_TRANSFORMER_H
#define SMKHE_NTT_TRANSFORMER_H

using namespace std;

class NTTTransformer {
    Parameters parameters;
    vector<uint64_t> NInvs;
    vector<vector<uint64_t>> rootPowers, inverseRootPowers;
    vector<vector<uint64_t>> psi, psiInverse;
public:
    explicit NTTTransformer(Parameters parameters);

    void toNTT(Polynomial<uint64_t> &poly, int level);

    void fromNTT(Polynomial<uint64_t> &poly, int level);

    uint64_t getPrime(int index);
};

#endif //SMKHE_NTT_TRANSFORMER_H
