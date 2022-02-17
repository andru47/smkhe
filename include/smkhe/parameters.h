#include "smkhe/util.h"
#include "smkhe/math/ntt_transformer.h"

#ifndef SMKHE_PARAMETERS_H
#define SMKHE_PARAMETERS_H

using namespace std;

namespace smkhe {
    class Parameters {
        uint64_t scale;
        int ringDegree;
        vector<uint64_t> qPrimes, pPrimes;
        NTTTransformer transformerQ, transformerP;

    public:
        Parameters(uint64_t scale, int ringDegree, vector<uint64_t> qPrimes, vector<uint64_t> pPrimes);

        uint64_t getScale();

        void setScale(uint64_t scale);

        int getRingDegree();

        int getModulusLevels();

        uint64_t getModulus(int level);

        void setRingDegree(int ringDegree);

        bool operator==(const Parameters);

        vector<uint64_t> &getPrimes();

        vector<uint64_t> &getSpecialPrimes();

        int getSpecialPrimesNumber();

        NTTTransformer &getTransformerQ();

        NTTTransformer &getTransformerP();
    };
}

#endif //SMKHE_PARAMETERS_H
