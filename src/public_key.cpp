#include "smkhe/public_key.h"
#include "smkhe/serializer_util.h"

namespace smkhe {
    PublicKey::PublicKey(vector<Polynomial<uint64_t>> polyA, vector<Polynomial<uint64_t>> polyB)
            : polyA(polyA), polyB(polyB) {}

    Polynomial<uint64_t> PublicKey::getA(int level) {
        return polyA[level];
    }

    Polynomial<uint64_t> PublicKey::getB(int level) {
        return polyB[level];
    }

    void PublicKey::serialize(string &givenString) {
        PublicKeySerializer serializer;

        for (auto &polya: polyA) {
            serializer.add_polya()->CopyFrom(serializePolynomial(polya));
        }
        for (auto &polyb: polyB) {
            serializer.add_polyb()->CopyFrom(serializePolynomial(polyb));
        }

        serializer.SerializeToString(&givenString);
    }

    void PublicKey::deserialize(string &givenString) {
        polyA.clear();
        polyB.clear();

        PublicKeySerializer serializer;
        serializer.ParseFromString(givenString);

        polyA.resize(serializer.polya_size());
        polyB.resize(serializer.polyb_size());

        for (int index = 0; index < polyA.size(); ++index) {
            parsePolynomial(serializer.polya(index), polyA[index]);
        }
        for (int index = 0; index < polyB.size(); ++index) {
            parsePolynomial(serializer.polyb(index), polyB[index]);
        }
    }

    PublicKey::PublicKey() {}

    int PublicKey::getLevels() {
        return polyA.size();
    }
}
