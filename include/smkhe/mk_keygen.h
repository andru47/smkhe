#include "smkhe/secret_key.h"
#include "smkhe/mk_public_key.h"
#include "smkhe/mk_evaluation_key.h"

#ifndef SMKHE_MK_KEYGEN_H
#define SMKHE_MK_KEYGEN_H

namespace smkhe {
    class MKKeygen {
        bool isSecretAttached = false;
        SecretKey secretKey;
        Parameters params;
        uint64_t seed;
    public:
        MKKeygen(Parameters params, uint64_t seed);

        SecretKey generateSecretKey();

        MKPublicKey generatePublicKey();

        MKEvaluationKey generateEvaluationKey(MKPublicKey &publicKey);
    };
}

#endif //SMKHE_MK_KEYGEN_H
