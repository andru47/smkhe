#include "smkhe/math/polynomial.h"
#include "smkhe/ciphertext.h"

#ifndef SMKHE_MK_CIPHERTEXT_H
#define SMKHE_MK_CIPHERTEXT_H

namespace smkhe {
    class MKCiphertext {
        int level;
        vector<vector<Polynomial<uint64_t>>> ciphers;
        vector<uint64_t> ids;

    public:
        MKCiphertext(vector<vector<Polynomial<uint64_t>>> &ciphers, vector<uint64_t> &ids, int level);

        MKCiphertext(Ciphertext &givenCipher, uint64_t givenId, int level);

        MKCiphertext();

        vector<Polynomial<uint64_t>> &getCipherForIndex(int index);

        vector<uint64_t> &getIds();

        void decreaseLevel(int howMany);

        int getLevel();

        void serialize(string &resultedString);

        void deserialize(string &givenString);
    };
}

#endif //SMKHE_MK_CIPHERTEXT_H
