#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/keygen.h"
#include "smkhe/ciphertext.h"
#include "smkhe/encryptor.h"

#define PRIMES {1152921504606748673, 576460752308273153, 576460752302473217}
#define SPECIAL_PRIMES {0x7fffffffe0001, 0x80000001c0001, 0x80000002c0001, 0x7ffffffd20001}

using namespace std;

Parameters serializerParams(1ULL << 59, 16384, PRIMES, SPECIAL_PRIMES);
Keygen serializerKeygen(serializerParams);
SecretKey serializerSecretKey = serializerKeygen.generateSecretKey();
PublicKey serializerPublicKey = serializerKeygen.generatePublicKey();
EvaluationKey serializerEvaluationKey = serializerKeygen.generateEvaluationKey();

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
