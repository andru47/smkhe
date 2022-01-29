#include "flint/fmpz_mod_polyxx.h"
#include "Eigen/Core"
#include "smkhe/parameters.h"

#ifndef SMKHE_PLAINTEXT_H
#define SMKHE_PLAINTEXT_H

using namespace flint;
using namespace Eigen;

class Plaintext {
    fmpz_polyxx polynomial;
    Parameters parameters;

public:
    Plaintext(VectorXcd &givenCoefficients, Parameters &parameters);

    VectorXcd getVectorPolynomial();

    void add(Plaintext &otherPlain);
};


#endif //SMKHE_PLAINTEXT_H
