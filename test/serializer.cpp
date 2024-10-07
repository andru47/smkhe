#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/keygen.h"
#include "smkhe/ciphertext.h"
#include "smkhe/encryptor.h"
#include "smkhe/mk_keygen.h"
#include "smkhe/mk_ciphertext.h"
#include "smkhe/mk_public_key.h"
#include "smkhe/mk_decryptor.h"

#define PRIMES {1152921504606748673, 576460752308273153, 576460752302473217}
#define SPECIAL_PRIMES {0x7fffffffe0001, 0x80000001c0001, 0x80000002c0001, 0x7ffffffd20001}

using namespace std;
using namespace smkhe;

Parameters serializerParams(1ULL << 59, 16384, PRIMES, SPECIAL_PRIMES);
Keygen serializerKeygen(serializerParams);
SecretKey serializerSecretKey = serializerKeygen.generateSecretKey();
PublicKey serializerPublicKey = serializerKeygen.generatePublicKey();
EvaluationKey serializerEvaluationKey = serializerKeygen.generateEvaluationKey();
MKKeygen serializerMKKeygen(serializerParams, 1231234);
SecretKey serializerMKSecretKey = serializerMKKeygen.generateSecretKey();
MKPublicKey serializerMKPublicKey = serializerMKKeygen.generatePublicKey();
MKEvaluationKey serializedMKEvalutionKey = serializerMKKeygen.generateEvaluationKey(serializerMKPublicKey);

TEST(Serializer, SecretKey) {
    string serializedString;
    serializerSecretKey.serialize(serializedString);
    SecretKey otherKey;
    otherKey.deserialize(serializedString);

    ASSERT_EQ(serializerSecretKey.getPolysP().size(), otherKey.getPolysP().size());
    ASSERT_EQ(serializerSecretKey.getPolysQ().size(), otherKey.getPolysQ().size());

    for (int index = 0; index < serializerSecretKey.getPolysP().size(); ++index) {
        ASSERT_EQ(serializerSecretKey.getPolysP()[index].getCoeffs(), otherKey.getPolysP()[index].getCoeffs());
    }
    for (int index = 0; index < serializerSecretKey.getPolysQ().size(); ++index) {
        ASSERT_EQ(serializerSecretKey.getPolysQ()[index].getCoeffs(), otherKey.getPolysQ()[index].getCoeffs());
    }
}

TEST(Serializer, PublicKey) {
    string serializedString;
    serializerPublicKey.serialize(serializedString);
    PublicKey otherKey;
    otherKey.deserialize(serializedString);

    ASSERT_EQ(serializerPublicKey.getLevels(), otherKey.getLevels());

    for (int index = 0; index < otherKey.getLevels(); ++index) {
        ASSERT_EQ(serializerPublicKey.getA(index).getCoeffs(), otherKey.getA(index).getCoeffs());
        ASSERT_EQ(serializerPublicKey.getB(index).getCoeffs(), otherKey.getB(index).getCoeffs());
    }
}

TEST(Serializer, Ciphertext) {
    Encoder enc(serializerParams);
    Plaintext plain = enc.encode(vector<double>{1.6});
    Encryptor encryptor(serializerParams);
    Ciphertext cipher = encryptor.encrypt(plain, serializerPublicKey);

    string serializedString;
    cipher.serialize(serializedString);

    Ciphertext otherCipher;
    otherCipher.deserialize(serializedString);

    ASSERT_EQ(cipher.getLevel(), otherCipher.getLevel());

    for (int index = 0; index <= cipher.getLevel(); ++index) {
        ASSERT_EQ(cipher.getPolyA(index).getCoeffs(), otherCipher.getPolyA(index).getCoeffs());
        ASSERT_EQ(cipher.getPolyB(index).getCoeffs(), otherCipher.getPolyB(index).getCoeffs());
    }
}

TEST(Serializer, EvaluationKey) {
    string serializedString;
    serializerEvaluationKey.serialize(serializedString);
    EvaluationKey otherKey;
    otherKey.deserialize(serializedString);

    ASSERT_EQ(serializerEvaluationKey.aP.size(), otherKey.aP.size());
    ASSERT_EQ(serializerEvaluationKey.aQ.size(), otherKey.aQ.size());
    ASSERT_EQ(serializerEvaluationKey.bP.size(), otherKey.bP.size());
    ASSERT_EQ(serializerEvaluationKey.bQ.size(), otherKey.bQ.size());

    for (int index = 0; index < serializerEvaluationKey.aP.size(); ++index) {
        ASSERT_EQ(serializerEvaluationKey.aP[index].getCoeffs(), otherKey.aP[index].getCoeffs());
        ASSERT_EQ(serializerEvaluationKey.bP[index].getCoeffs(), otherKey.bP[index].getCoeffs());
    }
    for (int index = 0; index < serializerEvaluationKey.aQ.size(); ++index) {
        ASSERT_EQ(serializerEvaluationKey.aQ[index].getCoeffs(), otherKey.aQ[index].getCoeffs());
        ASSERT_EQ(serializerEvaluationKey.bQ[index].getCoeffs(), otherKey.bQ[index].getCoeffs());
    }
}

TEST(Serializer, MKPublicKey) {
    string serializedString;
    serializerMKPublicKey.serialize(serializedString);
    MKPublicKey otherKey;
    otherKey.deserialize(serializedString);

    ASSERT_EQ(serializerMKPublicKey.getLevelsP(), otherKey.getLevelsP());
    ASSERT_EQ(serializerMKPublicKey.getLevelsQ(), otherKey.getLevelsQ());

    for (int index = 0; index < otherKey.getLevelsQ(); ++index) {
        ASSERT_EQ(serializerMKPublicKey.getPolyAQ(index).getCoeffs(), otherKey.getPolyAQ(index).getCoeffs());
        ASSERT_EQ(serializerMKPublicKey.getPolyBQ(index).getCoeffs(), otherKey.getPolyBQ(index).getCoeffs());
    }
    for (int index = 0; index < otherKey.getLevelsP(); ++index) {
        ASSERT_EQ(serializerMKPublicKey.getPolyAP(index).getCoeffs(), otherKey.getPolyAP(index).getCoeffs());
        ASSERT_EQ(serializerMKPublicKey.getPolyBP(index).getCoeffs(), otherKey.getPolyBP(index).getCoeffs());
    }
}

TEST(Serializer, MKCiphertext) {
    Encoder enc(serializerParams);
    Plaintext plain = enc.encode(vector<double>{1.6});
    Encryptor encryptor(serializerParams);
    PublicKey retKey = serializerMKPublicKey.getPublicKey();
    Ciphertext cipher = encryptor.encrypt(plain, retKey);
    MKCiphertext mkCiphertext(cipher, 1, cipher.getLevel());

    string serializedString;
    mkCiphertext.serialize(serializedString);

    MKCiphertext otherCipher;
    otherCipher.deserialize(serializedString);

    ASSERT_EQ(mkCiphertext.getLevel(), otherCipher.getLevel());
    ASSERT_EQ(mkCiphertext.getIds(), otherCipher.getIds());

    for (int index = 0; index <= mkCiphertext.getIds().size(); ++index) {
        for (int level = 0; level <= mkCiphertext.getLevel(); ++level) {
            ASSERT_EQ(mkCiphertext.getCipherForIndex(index)[level].getCoeffs(), otherCipher.getCipherForIndex(index)[level].getCoeffs());
        }
    }
}

TEST(Serializer, MKEvaluationKey) {
    string serializedString;
    serializedMKEvalutionKey.serialize(serializedString);
    MKEvaluationKey otherKey;
    otherKey.deserialize(serializedString);

    ASSERT_EQ(serializedMKEvalutionKey.d0P.size(), otherKey.d0P.size());
    ASSERT_EQ(serializedMKEvalutionKey.d0Q.size(), otherKey.d0Q.size());
    ASSERT_EQ(serializedMKEvalutionKey.d1P.size(), otherKey.d1P.size());
    ASSERT_EQ(serializedMKEvalutionKey.d1Q.size(), otherKey.d1Q.size());
    ASSERT_EQ(serializedMKEvalutionKey.d2P.size(), otherKey.d2P.size());
    ASSERT_EQ(serializedMKEvalutionKey.d2Q.size(), otherKey.d2Q.size());

    for (int index = 0; index < serializedMKEvalutionKey.d0P.size(); ++index) {
        ASSERT_EQ(serializedMKEvalutionKey.d0P[index].getCoeffs(), otherKey.d0P[index].getCoeffs());
        ASSERT_EQ(serializedMKEvalutionKey.d1P[index].getCoeffs(), otherKey.d1P[index].getCoeffs());
        ASSERT_EQ(serializedMKEvalutionKey.d2P[index].getCoeffs(), otherKey.d2P[index].getCoeffs());
    }
    for (int index = 0; index < serializedMKEvalutionKey.d0Q.size(); ++index) {
        ASSERT_EQ(serializedMKEvalutionKey.d0Q[index].getCoeffs(), otherKey.d0Q[index].getCoeffs());
        ASSERT_EQ(serializedMKEvalutionKey.d1Q[index].getCoeffs(), otherKey.d1Q[index].getCoeffs());
        ASSERT_EQ(serializedMKEvalutionKey.d2Q[index].getCoeffs(), otherKey.d2Q[index].getCoeffs());
    }
}

TEST(Serializer, MKPartialCiphertext) {
    Encoder enc(serializerParams);
    Plaintext plain = enc.encode(vector<double>{1.6});
    Encryptor encryptor(serializerParams);
    PublicKey retKey = serializerMKPublicKey.getPublicKey();
    Ciphertext cipher = encryptor.encrypt(plain, retKey);
    MKCiphertext mkCiphertext(cipher, 1, cipher.getLevel());
    MKDecryptor decryptor(serializerParams);
    PartialCiphertext partialCiphertext = decryptor.partialDecryption(mkCiphertext, 1, serializerMKSecretKey);

    string serializedString;
    partialCiphertext.serialize(serializedString);

PartialCiphertext otherCipher;
    otherCipher.deserialize(serializedString);

    ASSERT_EQ(partialCiphertext.getPolynomial().size(), otherCipher.getPolynomial().size());

    for (int index = 0; index < partialCiphertext.getPolynomial().size(); ++index) {
        ASSERT_EQ(partialCiphertext.getPolynomial()[index].getCoeffs(), otherCipher.getPolynomial()[index].getCoeffs());
    }
}

