#include "smkhe/mk_public_key.h"
#include "smkhe/serializer_util.h"

namespace smkhe {
    MKPublicKey::MKPublicKey(vector<Polynomial<uint64_t>> &polyAQ, vector<Polynomial<uint64_t>> &polyAP,
                             vector<Polynomial<uint64_t>> &polyBQ, vector<Polynomial<uint64_t>> &polyBP) : polyAQ(polyAQ),
                             polyAP(polyAP), polyBQ(polyBQ), polyBP(polyBP) {}

    Polynomial<uint64_t> &MKPublicKey::getPolyAQ(int level) {
        return polyAQ[level];
    }

    Polynomial<uint64_t> &MKPublicKey::getPolyAP(int level) {
        return polyAP[level];
    }

    Polynomial<uint64_t> &MKPublicKey::getPolyBQ(int level) {
        return polyBQ[level];
    }

    Polynomial<uint64_t> &MKPublicKey::getPolyBP(int level) {
        return polyBP[level];
    }

    PublicKey MKPublicKey::getPublicKey() {
        return PublicKey(polyAQ, polyBQ);
    }

    MKPublicKey::MKPublicKey() {}

    void MKPublicKey::serialize(string &resultedString) {
        EvaluationKeySerializer serializer;

        for (auto it: polyAP) {
            serializer.add_ap()->CopyFrom(serializePolynomial(it));
        }
        for (auto it: polyAQ) {
            serializer.add_aq()->CopyFrom(serializePolynomial(it));
        }
        for (auto it: polyBP) {
            serializer.add_bp()->CopyFrom(serializePolynomial(it));
        }
        for (auto it: polyBQ) {
            serializer.add_bq()->CopyFrom(serializePolynomial(it));
        }

        serializer.SerializeToString(&resultedString);
    }

    void MKPublicKey::deserialize(string &givenString) {
        EvaluationKeySerializer serializer;
        serializer.ParseFromString(givenString);

        polyAP.clear();
        polyAQ.clear();
        polyBP.clear();
        polyBQ.clear();
        polyAP.resize(serializer.ap_size());
        polyAQ.resize(serializer.aq_size());
        polyBP.resize(serializer.bp_size());
        polyBQ.resize(serializer.bq_size());

        for (int index = 0; index < polyAP.size(); ++index) {
            parsePolynomial(serializer.ap(index), polyAP[index]);
        }
        for (int index = 0; index < polyAQ.size(); ++index) {
            parsePolynomial(serializer.aq(index), polyAQ[index]);
        }
        for (int index = 0; index < polyBP.size(); ++index) {
            parsePolynomial(serializer.bp(index), polyBP[index]);
        }
        for (int index = 0; index < polyBQ.size(); ++index) {
            parsePolynomial(serializer.bq(index), polyBQ[index]);
        }
    }
}
