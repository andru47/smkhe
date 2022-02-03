#include "smkhe/util.h"
#include "smkhe/math/ntt_transformer.h"

#ifndef SMKHE_PARAMETERS_H
#define SMKHE_PARAMETERS_H

using namespace std;

class Parameters {
    double scale;
    int ringDegree;
    vector<uint64_t> primes;
    NTTTransformer transformer;

public:
    Parameters(double scale, int ringDegree, vector<uint64_t> primes);

    double getScale();

    void setScale(double scale);

    int getRingDegree();

    int getModulusLevels();

    uint64_t getModulus(int level);

    void setRingDegree(int ringDegree);

    bool operator==(const Parameters);

    vector<uint64_t> &getPrimes();

    NTTTransformer &getTransformer();
};

#endif //SMKHE_PARAMETERS_H
