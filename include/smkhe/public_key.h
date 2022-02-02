//
// Created by Andru Stefanescu on 31.01.2022.
//
#include "smkhe/math/polynomial.h"
#include "smkhe/parameters.h"

#ifndef SMKHE_PUBLIC_KEY_H
#define SMKHE_PUBLIC_KEY_H


class PublicKey {
    Polynomial<uint64_t> polyA, polyB;
    Parameters params;

public:
    PublicKey(Parameters params);
};


#endif //SMKHE_PUBLIC_KEY_H
