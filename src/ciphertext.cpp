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

vector<Polynomial<uint64_t>> &Ciphertext::getPolysA() {
    return polysA;
}

void Ciphertext::decreaseLevel(int howMany) {
    if (!howMany) {
        return;
    }
    if (howMany > level + 1) {
        throw ("Cannot decrease more levels than remaining");
    }
    level -= howMany;
    while (howMany--) {
        polysA.pop_back();
        polysB.pop_back();
    }
}
