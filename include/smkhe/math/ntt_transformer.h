#include "smkhe/util.h"
#include "smkhe/math/polynomial.h"

using namespace std;

#ifndef SMKHE_NTT_TRANSFORMER_H
#define SMKHE_NTT_TRANSFORMER_H

namespace smkhe {
    class NTTTransformer {
        int N;
        vector<uint64_t> primes, NInvs;
        vector<vector<uint64_t>> rootPowers, inverseRootPowers;
        vector<vector<uint64_t>> psi, psiInverse;
    public:
        NTTTransformer(int ringDegree, vector<uint64_t> primes);

        void toNTT(Polynomial<uint64_t> &poly, int level);

        void fromNTT(Polynomial<uint64_t> &poly, int level);

        uint64_t getPrime(int index);
    };
}

#endif //SMKHE_NTT_TRANSFORMER_H
