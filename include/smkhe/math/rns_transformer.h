#include "smkhe/util.h"

#ifndef SMKHE_RNS_TRANSFORMER_H
#define SMKHE_RNS_TRANSFORMER_H

class RNSTransformer {
    int numberPrimesP, numberPrimesQ;
    vector<uint64_t> primesP, primesQ;
    vector<vector<uint64_t>> pHatModQ, qHatInverseModQ;
    vector<uint64_t> pHatInverseModP, pInverseModQ;
    vector<vector<vector<uint64_t>>> qHatModP;

public:
    RNSTransformer(vector<uint64_t> pPrimes, vector<uint64_t> qPrimes);

    vector<vector<uint64_t>> modUp(vector<vector<uint64_t>> &vectorInQBasis);

    vector<vector<uint64_t>> modDown(vector<vector<uint64_t>> &vectorInPQBasis);
};

#endif //SMKHE_RNS_TRANSFORMER_H
