#include "smkhe/util.h"
#include "smkhe/public_key.h"
#include "smkhe/secret_key.h"
#include "smkhe/parameters.h"
#include "smkhe/rand_util.h"
#include "smkhe/evaluation_key.h"

#ifndef SMKHE_KEYGEN_H
#define SMKHE_KEYGEN_H

class Keygen {
    Parameters params;
    SecretKey secretKey;
    bool secretKeyAttached = false;

public:
    Keygen(Parameters params);

    Keygen(Parameters params, SecretKey secretKey);

    bool isSecretKeyAttached();

    SecretKey generateSecretKey();

    PublicKey generatePublicKey();

    EvaluationKey generateEvaluationKey();
};

#endif //SMKHE_KEYGEN_H
