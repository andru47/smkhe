#include "smkhe/partial_ciphertext.h"
#include "smkhe/serializer_util.h"

namespace smkhe {
    PartialCiphertext::PartialCiphertext(vector<Polynomial<uint64_t>> &partiallyDecrypted) : partiallyDecrypted(
            partiallyDecrypted) {}

    PartialCiphertext::PartialCiphertext() {}

    vector<Polynomial<uint64_t>>& PartialCiphertext::getPolynomial() {
        return partiallyDecrypted;
    }

    void PartialCiphertext::serialize(string &resultedString) {
        PartialCiphertextSerializer serializer;
        for (auto &it: partiallyDecrypted) {
            serializer.add_partiallydecrypted()->CopyFrom(serializePolynomial(it));
        }
        serializer.SerializeToString(&resultedString);
    }

    void PartialCiphertext::deserialize(string &givenString) {
        PartialCiphertextSerializer serializer;
        serializer.ParseFromString(givenString);

        partiallyDecrypted.clear();
        partiallyDecrypted.resize(serializer.partiallydecrypted_size());

        for (int index = 0; index < partiallyDecrypted.size(); ++index) {
            parsePolynomial(serializer.partiallydecrypted(index), partiallyDecrypted[index]);
        }
    }
}
