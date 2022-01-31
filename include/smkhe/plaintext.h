#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include <complex>

#ifndef SMKHE_PLAINTEXT_H
#define SMKHE_PLAINTEXT_H

class Plaintext {
    Polynomial<long long> polynomial;
    Parameters parameters;

public:
    Plaintext(vector<complex<double>> &givenCoefficients, Parameters &parameters);

    Polynomial<long long> getPolynomial();

    void add(Plaintext &otherPlain);
};

#endif //SMKHE_PLAINTEXT_H
