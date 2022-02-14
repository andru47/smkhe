#include "smkhe/evaluation_key.h"
#include "smkhe/serializer_util.h"

EvaluationKey::EvaluationKey(vector<Polynomial<uint64_t>> &aP, vector<Polynomial<uint64_t>> &aQ,
                             vector<Polynomial<uint64_t>> &bP, vector<Polynomial<uint64_t>> &bQ) : aP(aP), aQ(aQ), bP(bP), bQ(bQ) {}

EvaluationKey::EvaluationKey() {}

void EvaluationKey::serialize(string &givenString) {
    EvaluationKeySerializer serializer;

    for (auto it: aP) {
        serializer.add_ap()->CopyFrom(serializePolynomial(it));
    }
    for (auto it: aQ) {
        serializer.add_aq()->CopyFrom(serializePolynomial(it));
    }
    for (auto it: bP) {
        serializer.add_bp()->CopyFrom(serializePolynomial(it));
    }
    for (auto it: bQ) {
        serializer.add_bq()->CopyFrom(serializePolynomial(it));
    }

    serializer.SerializeToString(&givenString);
}

void EvaluationKey::deserialize(string &givenString) {
    EvaluationKeySerializer serializer;
    serializer.ParseFromString(givenString);

    aP.clear();
    aQ.clear();
    bP.clear();
    bQ.clear();
    aP.resize(serializer.ap_size());
    aQ.resize(serializer.aq_size());
    bP.resize(serializer.bp_size());
    bQ.resize(serializer.bq_size());

    for (int index = 0; index < aP.size(); ++index) {
        parsePolynomial(serializer.ap(index), aP[index]);
    }
    for (int index = 0; index < aQ.size(); ++index) {
        parsePolynomial(serializer.aq(index), aQ[index]);
    }
    for (int index = 0; index < bP.size(); ++index) {
        parsePolynomial(serializer.bp(index), bP[index]);
    }
    for (int index = 0; index < bQ.size(); ++index) {
        parsePolynomial(serializer.bq(index), bQ[index]);
    }
}
