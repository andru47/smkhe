#include "smkhe/util.h"
#include "smkhe/math/polynomial.h"
#include "smkhe/math/ntt_transformer.h"

#ifndef SMKHE_RNS_TRANSFORMER_H
#define SMKHE_RNS_TRANSFORMER_H

namespace smkhe {
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

        vector<Polynomial<uint64_t>> raisePolys(vector<Polynomial<uint64_t>> polys, NTTTransformer transformerQ, NTTTransformer transformerP);

        vector<Polynomial<uint64_t>>downPolys(vector<Polynomial<uint64_t>> polys, NTTTransformer transformerQ,
                   NTTTransformer transformerP, int numberPrimesP);
    };
}

#endif //SMKHE_RNS_TRANSFORMER_H
