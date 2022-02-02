#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

using namespace std;

#ifndef SMKHE_PLAINTEXT_H
#define SMKHE_PLAINTEXT_H

class Plaintext {
    vector<Polynomial<uint64_t>> polynomials;
    Parameters parameters;
    int currentLevel = 0;

public:
    Plaintext(vector<vector<uint64_t>> &givenCoefficients, Parameters &parameters);

    Polynomial<uint64_t> &getPolynomial(int modIndex);

    void add(Plaintext &otherPlain, int modulusLevel);

    int getLevel();
};

#endif //SMKHE_PLAINTEXT_H
