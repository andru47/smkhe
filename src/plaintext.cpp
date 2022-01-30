#include "smkhe/plaintext.h"

Plaintext::Plaintext(vector<complex<double>> &givenCoefficients, Parameters &parameters) : parameters(parameters),
                    polynomial(givenCoefficients.size()) {
    for (int index = 0; index < givenCoefficients.size(); ++index) {
        polynomial.setCoeff(index, givenCoefficients[index].real());
    }
}

Polynomial<long long> Plaintext::getPolynomial() {
    return polynomial;
}

void Plaintext::add(Plaintext &otherPlain) {
    polynomial += otherPlain.polynomial;
}
