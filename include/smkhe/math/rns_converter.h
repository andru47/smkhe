#include "smkhe/parameters.h"
#include "smkhe/util.h"
#include "../lib/BigInt.h"

#ifndef SMKHE_RNS_CONVERTER_H
#define SMKHE_RNS_CONVERTER_H

class RNSConverter {
    vector<BigInt> qPerLevel;
    vector<vector<BigInt>> crtTransformation;
    Parameters params;
public:
    explicit RNSConverter(Parameters &params);

    void scaleDownFromCRT(vector<vector<uint64_t>> &coeffsInCRT, vector<complex<double>> &destination);
};

#endif //SMKHE_RNS_CONVERTER_H
