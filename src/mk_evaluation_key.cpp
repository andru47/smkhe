#include "smkhe/mk_evaluation_key.h"
#include "smkhe/serializer_util.h"

namespace smkhe {
    MKEvaluationKey::MKEvaluationKey(vector<Polynomial<uint64_t>> &d0P, vector<Polynomial<uint64_t>> &d0Q,
                                     vector<Polynomial<uint64_t>> &d1P, vector<Polynomial<uint64_t>> &d1Q,
                                     vector<Polynomial<uint64_t>> &d2P, vector<Polynomial<uint64_t>> &d2Q) : d0P(d0P), d0Q(d0Q),
                                                                                                             d1P(d1P), d1Q(d1Q),
                                                                                                             d2P(d2P), d2Q(d2Q) {}

    MKEvaluationKey::MKEvaluationKey() {}

    void MKEvaluationKey::serialize(string &resultedString) {
        MKEvaluationKeySerializer serializer;
        for (auto &it: d0P) {
            serializer.add_d0p()->CopyFrom(serializePolynomial(it));
        }
        for (auto &it: d0Q) {
            serializer.add_d0q()->CopyFrom(serializePolynomial(it));
        }
        for (auto &it: d1P) {
            serializer.add_d1p()->CopyFrom(serializePolynomial(it));
        }
        for (auto &it: d1Q) {
            serializer.add_d1q()->CopyFrom(serializePolynomial(it));
        }
        for (auto &it: d2P) {
            serializer.add_d2p()->CopyFrom(serializePolynomial(it));
        }
        for (auto &it: d2Q) {
            serializer.add_d2q()->CopyFrom(serializePolynomial(it));
        }

        serializer.SerializeToString(&resultedString);
    }

    void MKEvaluationKey::deserialize(string &givenString) {
        MKEvaluationKeySerializer serializer;
        serializer.ParseFromString(givenString);
        d0P.clear(); d0P.resize(serializer.d0p_size());
        d1P.clear(); d1P.resize(serializer.d1p_size());
        d2P.clear(); d2P.resize(serializer.d2p_size());
        d0Q.clear(); d0Q.resize(serializer.d0q_size());
        d1Q.clear(); d1Q.resize(serializer.d1q_size());
        d2Q.clear(); d2Q.resize(serializer.d2q_size());

        for (int index = 0; index < d0P.size(); ++index) {
            parsePolynomial(serializer.d0p(index), d0P[index]);
        }
        for (int index = 0; index < d0Q.size(); ++index) {
            parsePolynomial(serializer.d0q(index), d0Q[index]);
        }
        for (int index = 0; index < d1P.size(); ++index) {
            parsePolynomial(serializer.d1p(index), d1P[index]);
        }
        for (int index = 0; index < d1Q.size(); ++index) {
            parsePolynomial(serializer.d1q(index), d1Q[index]);
        }
        for (int index = 0; index < d2P.size(); ++index) {
            parsePolynomial(serializer.d2p(index), d2P[index]);
        }
        for (int index = 0; index < d2Q.size(); ++index) {
            parsePolynomial(serializer.d2q(index), d2Q[index]);
        }
    }
}
