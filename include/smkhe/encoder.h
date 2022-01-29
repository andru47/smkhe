#include "smkhe/parameters.h"
#include "smkhe/plaintext.h"
#include "Eigen/Core"

#ifndef SMKHE_ENCODER_H
#define SMKHE_ENCODER_H

using namespace Eigen;
using namespace std;

class Encoder {
    Parameters parameters;
    vector<dcomplex> rootAtPower;
    vector<int> indexHash;

public:
    Encoder(Parameters &givenParameters);

    Plaintext encode(vector<dcomplex> toEncode);

    Plaintext encode(vector<double> toEncode);

    void setParameters(Parameters &givenParameters);

    Parameters getParameters();

    vector<dcomplex> decode(Plaintext &givenPlaintext);
};


#endif //SMKHE_ENCODER_H
