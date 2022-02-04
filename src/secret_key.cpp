#include "smkhe/secret_key.h"

SecretKey::SecretKey(Parameters params) : params(params), poly(params.getModulusLevels(),
                                                               Polynomial<uint64_t>(params.getRingDegree())) {}

SecretKey::SecretKey(Parameters params, vector<Polynomial<uint64_t>> givenPoly) : params(params), poly(givenPoly) {}

Polynomial<uint64_t> &SecretKey::getPoly(int level) {
    return poly[level];
}

vector<Polynomial<uint64_t>> &SecretKey::getPolys() {
    return poly;
}
