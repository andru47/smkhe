#include "smkhe/parameters.h"
#include "smkhe/public_key.h"
#include "smkhe/plaintext.h"
#include "smkhe/ciphertext.h"
#include "smkhe/secret_key.h"
#include "smkhe/util.h"
#include "smkhe/rand_util.h"

#ifndef SMKHE_ENCRYPTOR_H
#define SMKHE_ENCRYPTOR_H

class Encryptor {
    Parameters params;
public:
    explicit Encryptor(Parameters params);

    Ciphertext encrypt(Plaintext &plain, PublicKey &pub);

    Plaintext decrypt(Ciphertext &cipher, SecretKey &secret);

};

#endif //SMKHE_ENCRYPTOR_H
