#include "smkhe/ciphertext.h"

Ciphertext::Ciphertext(Parameters params, int level, vector<Polynomial<uint64_t>> polysA,
                       vector<Polynomial<uint64_t>> polysB) : params(params), level(level), polysA(polysA),
                                                              polysB(polysB) {}

Polynomial<uint64_t> &Ciphertext::getPolyA(int givenLevel) {
    return polysA[givenLevel];
}

Polynomial<uint64_t> &Ciphertext::getPolyB(int givenLevel) {
    return polysB[givenLevel];
}

int Ciphertext::getLevel() {
    return level;
}

vector<Polynomial<uint64_t>> &Ciphertext::getPolysB() {
    return polysB;
}
