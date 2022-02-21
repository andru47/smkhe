#include "smkhe/mk_ciphertext.h"
#include "smkhe/serializer_util.h"

namespace smkhe {
    MKCiphertext::MKCiphertext(vector<vector<Polynomial<uint64_t>>> &ciphers, vector<uint64_t> &ids, int level)
            : ciphers(ciphers), ids(ids), level(level) {}

    vector<Polynomial<uint64_t>> &MKCiphertext::getCipherForIndex(int index) {
        if (index >= ciphers.size()) {
            throw ("Index is out of ciphers range.");
        }
        return ciphers[index];
    }

    vector<uint64_t> &MKCiphertext::getIds() {
        return ids;
    }

    MKCiphertext::MKCiphertext(Ciphertext &givenCipher, uint64_t givenId, int level) : ciphers(
            {givenCipher.getPolysA(), givenCipher.getPolysB()}), ids({givenId}), level(level) {}

    void MKCiphertext::decreaseLevel(int howMany) {
        for (auto &cipher: ciphers) {
            if (howMany >= cipher.size()) {
                throw ("Cannot decrease more levels than available.");
            }
            for (int i = 0; i < howMany; ++i) {
                cipher.pop_back();
            }
        }
        level -= howMany;
    }

    int MKCiphertext::getLevel() {
        return level;
    }

    void MKCiphertext::serialize(string &resultedString) {
        MKCiphertextSerializer serializer;
        serializer.set_level(level);
        for (auto &id: ids) {
            serializer.add_ids(id);
        }
        DoubleVectorPolynomial *doubleVectorPolynomial = new DoubleVectorPolynomial();
        doubleVectorPolynomial -> set_n(ids.size() + 1);
        doubleVectorPolynomial -> set_m(level + 1);
        for (int index = 0; index <= ids.size(); ++index) {
            for (int currentLevel = 0; currentLevel <= level; ++currentLevel) {
                doubleVectorPolynomial -> add_numbers()->CopyFrom(serializePolynomial(ciphers[index][currentLevel]));
            }
        }
        serializer.set_allocated_ciphers(doubleVectorPolynomial);
        serializer.SerializeToString(&resultedString);
    }

    void MKCiphertext::deserialize(string &givenString) {
        MKCiphertextSerializer serializer;
        serializer.ParseFromString(givenString);
        level = serializer.level();
        ids.clear();
        ids.resize(serializer.ids_size());
        for (int index = 0; index < serializer.ids_size(); ++index) {
            ids[index] = serializer.ids(index);
        }
        ciphers.clear();
        ciphers.resize(ids.size() + 1);
        for (int index = 0; index < ciphers.size(); ++index) {
            ciphers[index].resize(serializer.ciphers().m());
            for (int currentLevel = 0; currentLevel <= level; ++currentLevel) {
                parsePolynomial(serializer.ciphers().numbers(index * (level + 1) + currentLevel), ciphers[index][currentLevel]);
            }
        }
    }

    MKCiphertext::MKCiphertext() {}
}
