#include "smkhe/plaintext.h"

Plaintext::Plaintext(vector<vector<uint64_t>> &givenCoefficients, Parameters &parameters) : parameters(parameters), currentLevel(parameters.getPrimes().size() - 1) {
    for (int index = 0; index < givenCoefficients.size(); ++index) {
        Polynomial<uint64_t> newPoly = Polynomial<uint64_t>(givenCoefficients[index].size());
        for (int coeffIndex = 0; coeffIndex < givenCoefficients[index].size(); ++coeffIndex) {
            newPoly.setCoeff(coeffIndex, givenCoefficients[index][coeffIndex]);
        }
        polynomials.push_back(newPoly);
    }
}

Polynomial<uint64_t> &Plaintext::getPolynomial(int modIndex) {
    return polynomials[modIndex];
}

void Plaintext::add(Plaintext &otherPlain, int modulusLevel) {
    polynomials[modulusLevel] += otherPlain.polynomials[modulusLevel];
}

int Plaintext::getLevel() {
    return currentLevel;
}

Plaintext::Plaintext(vector<Polynomial<uint64_t>> polynomials, Parameters params, int level) : polynomials(polynomials),
                                                                                               parameters(params),
                                                                                               currentLevel(level) {}

void Plaintext::decreaseLevel(int howMany) {
    if (!howMany) {
        return;
    }
    if (howMany > currentLevel + 1) {
        throw ("Cannot decrease more levels than remaining");
    }
    currentLevel -= howMany;
    while (howMany--) {
        polynomials.pop_back();
    }
}
