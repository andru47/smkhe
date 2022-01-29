#include "smkhe/plaintext.h"

Plaintext::Plaintext(VectorXcd &givenCoefficients, Parameters &parameters) : parameters(parameters) {
    fmpz_poly_init2(polynomial._poly(), givenCoefficients.rows());
    for (int index = 0; index < givenCoefficients.rows(); ++index) {
        polynomial.set_coeff(index, fmpz(givenCoefficients(index, 0).real()));
    }
}

VectorXcd Plaintext::getVectorPolynomial() {
    VectorXcd result(polynomial.degree() + 1);

    for (int index = 0; index <= polynomial.degree(); ++index) {
        result(index, 0) = dcomplex(fmpz_poly_get_coeff_si(polynomial._poly(), index));
    }

    return result;
}

void Plaintext::add(Plaintext &otherPlain) {
    polynomial += otherPlain.polynomial;
}
