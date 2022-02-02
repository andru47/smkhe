//
// Created by Andru Stefanescu on 31.01.2022.
//

#include "smkhe/secret_key.h"

SecretKey::SecretKey(Parameters params) : params(params), poly(params.getRingDegree()) {}

SecretKey::SecretKey(Parameters params, Polynomial<uint64_t> givenPoly) : params(params), poly(givenPoly) {}
