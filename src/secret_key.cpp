#include "smkhe/secret_key.h"
#include "smkhe/serializer_util.h"

SecretKey::SecretKey(int levels, int specialLevels, int ringDegree) : polyQ(levels,
                                                               Polynomial<uint64_t>(ringDegree)), polyP(specialLevels, Polynomial<uint64_t>(ringDegree)) {}

SecretKey::SecretKey(vector<Polynomial<uint64_t>> givenPolyQ, vector<Polynomial<uint64_t>> givenPolyP) : polyQ(givenPolyQ), polyP(givenPolyP){}

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

void SecretKey::serialize(string &resultedString) {
    SecretKeySerializer serializer;

    for (auto &it: polyP) {
        serializer.add_polyp()->CopyFrom(serializePolynomial(it));
    }
    for (auto &it: polyQ) {
        serializer.add_polyq()->CopyFrom(serializePolynomial(it));
    }

    serializer.SerializeToString(&resultedString);
}

void SecretKey::deserialize(string &givenString) {
    this -> polyP.clear();
    this -> polyQ.clear();

    SecretKeySerializer serializer;
    serializer.ParseFromString(givenString);

    this -> polyP.resize(serializer.polyp_size());
    this -> polyQ.resize(serializer.polyq_size());

    for (int index = 0; index < polyP.size(); ++index) {
        parsePolynomial(serializer.polyp(index), polyP[index]);
    }
    for (int index = 0; index < polyQ.size(); ++index) {
        parsePolynomial(serializer.polyq(index), polyQ[index]);
    }
}

SecretKey::SecretKey() {}
