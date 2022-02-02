#include "smkhe/parameters.h"
#include "smkhe/plaintext.h"
#include <cstdint>
#include <complex>

#ifndef SMKHE_ENCODER_H
#define SMKHE_ENCODER_H

using namespace std;

class Encoder {
    Parameters parameters;
    vector<complex<double>> rootAtPower;
    vector<int> indexHash;

public:
    Encoder(Parameters &givenParameters);

    Plaintext encode(vector<complex<double>> toEncode);

    Plaintext encode(vector<double> toEncode);

    void setParameters(Parameters &givenParameters);

    Parameters getParameters();

    vector<complex<double>> decode(Plaintext &givenPlaintext);
};

#endif //SMKHE_ENCODER_H
