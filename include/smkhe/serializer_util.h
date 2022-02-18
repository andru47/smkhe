#include "smkhe/math/polynomial.h"
#include "../../build/messages.pb.h"

#ifndef SMKHE_SERIALIZER_UTIL_H
#define SMKHE_SERIALIZER_UTIL_H

namespace smkhe {
    PolynomialSerializer serializePolynomial(Polynomial<uint64_t> &givenPolynomial);

    void parsePolynomial(const PolynomialSerializer &givenSerializer, Polynomial<uint64_t> &givenPolynomial);
}

#endif //SMKHE_SERIALIZER_UTIL_H
