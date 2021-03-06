#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"
#include "smkhe/util.h"

using namespace std;

#ifndef SMKHE_PLAINTEXT_H
#define SMKHE_PLAINTEXT_H

namespace smkhe {
    class Plaintext {
        vector<Polynomial<uint64_t>> polynomials;
        Parameters parameters;
        int currentLevel;

    public:
        Plaintext(vector<vector<uint64_t>> &givenCoefficients, Parameters &parameters);

        Plaintext(vector<Polynomial<uint64_t>> polynomials, Parameters params, int level);

        Polynomial<uint64_t> &getPolynomial(int modIndex);

        void add(Plaintext &otherPlain, int modulusLevel);

        int getLevel();

        void decreaseLevel(int howMany);
    };
}

#endif //SMKHE_PLAINTEXT_H
