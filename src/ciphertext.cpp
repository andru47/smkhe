#include "smkhe/ciphertext.h"
#include "smkhe/serializer_util.h"

Ciphertext::Ciphertext(int level, vector<Polynomial<uint64_t>> polysA, vector<Polynomial<uint64_t>> polysB) : level(level), polysA(polysA), polysB(polysB) {}

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

Ciphertext::Ciphertext() : level(0) {}

void Ciphertext::serialize(string &givenString) {
    CiphertextSerializer serializer;
    serializer.set_level(level);

    for (auto &polyA: polysA) {
       serializer.add_polysa()->CopyFrom(serializePolynomial(polyA));
    }
    for (auto &polyB: polysB) {
        serializer.add_polysb()->CopyFrom(serializePolynomial(polyB));
    }

    serializer.SerializeToString(&givenString);
}

void Ciphertext::deserialize(string &givenString) {
    CiphertextSerializer serializer;
    serializer.ParseFromString(givenString);

    level = serializer.level();
    polysA.clear();
    polysB.clear();
    polysA.resize(serializer.polysa_size());
    polysB.resize(serializer.polysb_size());

    for (int index = 0; index < polysA.size(); ++index) {
        parsePolynomial(serializer.polysa(index), polysA[index]);
    }
    for (int index = 0; index < polysB.size(); ++index) {
        parsePolynomial(serializer.polysb(index), polysB[index]);
    }
}
