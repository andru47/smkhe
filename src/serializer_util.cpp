#include "smkhe/serializer_util.h"

namespace smkhe {
    PolynomialSerializer serializePolynomial(Polynomial<uint64_t> &givenPolynomial) {
        PolynomialSerializer result;
        result.set_degree(givenPolynomial.getDegree());
        result.set_transformertontt(givenPolynomial.isTransformedToNTT());
        for (auto &coeff: givenPolynomial.getCoeffs()) {
            result.add_coeffs(coeff);
        }
        return result;
    }

    void parsePolynomial(const PolynomialSerializer &givenSerializer, Polynomial<uint64_t> &givenPolynomial) {
        vector<uint64_t> coeffs(givenSerializer.coeffs().begin(), givenSerializer.coeffs().end());
        givenPolynomial.setCoeffs(coeffs);
        givenPolynomial.setTransformedToNTT(givenSerializer.transformertontt());
    }
}
