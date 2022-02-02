//
// Created by Andru Stefanescu on 31.01.2022.
//
#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

#ifndef SMKHE_SECRET_KEY_H
#define SMKHE_SECRET_KEY_H


class SecretKey {
    Polynomial<uint64_t> poly;
    Parameters params;
public:
    SecretKey(Parameters params);
    SecretKey(Parameters params, Polynomial<uint64_t> givenPoly);
};

#endif //SMKHE_SECRET_KEY_H
