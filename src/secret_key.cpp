#include "smkhe/secret_key.h"

SecretKey::SecretKey(Parameters params) : params(params), polyQ(params.getModulusLevels(),
                                                               Polynomial<uint64_t>(params.getRingDegree())), polyP(params.getSpecialPrimes().size(), Polynomial<uint64_t>(params.getRingDegree())) {}

SecretKey::SecretKey(Parameters params, vector<Polynomial<uint64_t>> givenPolyQ, vector<Polynomial<uint64_t>> givenPolyP) : params(params), polyQ(givenPolyQ),
                                                                                                                            polyP(givenPolyP){}

Polynomial<uint64_t> &SecretKey::getPolyQ(int level) {
    return polyQ[level];
}

vector<Polynomial<uint64_t>> &SecretKey::getPolysQ() {
    return polyQ;
}

vector<Polynomial<uint64_t>> &SecretKey::getPolysP() {
    return polyP;
}

Polynomial<uint64_t> &SecretKey::getPolyP(int level) {
    return polyP[level];
}
