#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/mk_keygen.h"
#include "smkhe/encryptor.h"
#include "smkhe/mk_evaluator.h"
#include "smkhe/mk_ciphertext.h"
#include "smkhe/partial_ciphertext.h"
#include "smkhe/mk_public_key.h"
#include "smkhe/mk_decryptor.h"
#include <random>
#include <algorithm>

#define SEED 1231121
#define PRIMES {1152921504606748673, 576460752308273153, 576460752302473217}
#define SPECIAL_PRIMES {0x7fffffffe0001, 0x80000001c0001, 0x80000002c0001, 0x7ffffffd20001}
using namespace std;
using namespace smkhe;

Parameters parametersMKEval((1ULL << 59), 16384, PRIMES, SPECIAL_PRIMES);
Encoder encMKEval(parametersMKEval);
Encryptor encryptorMKEval(parametersMKEval);
MKDecryptor mkDecryptor(parametersMKEval);

MKKeygen mkKeygen1(parametersMKEval, 1123123);
MKKeygen mkKeygen2(parametersMKEval, 1123123);

SecretKey mkSecretKey1 = mkKeygen1.generateSecretKey();
SecretKey mkSecretKey2 = mkKeygen2.generateSecretKey();

MKPublicKey mkPublicKey1 = mkKeygen1.generatePublicKey();
MKPublicKey mkPublicKey2 = mkKeygen2.generatePublicKey();

PublicKey mkPublicKey1Normal = mkPublicKey1.getPublicKey();
PublicKey mkPublicKey2Normal = mkPublicKey2.getPublicKey();

MKEvaluationKey mkEvaluationKey1 = mkKeygen1.generateEvaluationKey(mkPublicKey1);
MKEvaluationKey mkEvaluationKey2 = mkKeygen2.generateEvaluationKey(mkPublicKey2);

unordered_map<uint64_t, MKPublicKey> publicKeyMap = {{1, mkPublicKey1},
                                                     {2, mkPublicKey2}};
unordered_map<uint64_t, MKEvaluationKey> evkMap = {{1, mkEvaluationKey1},
                                                   {2, mkEvaluationKey2}};

MKEvaluator mkEvaluator(parametersMKEval, publicKeyMap, evkMap);

vector<double> generateDoublesMKEval(int number) {
    vector<double> result(number);
    static default_random_engine generator(SEED);
    static uniform_real_distribution<double> distribution(0.0, 1.5);

    for (int i = 0; i < number; ++i) {
        result[i] = distribution(generator);
    }

    return result;
}

TEST(MKEvaluator, AddPlainSameLevel) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);

    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    MKCiphertext mkCiphertext(ciphertext, 1, ciphertext.getLevel());

    mkEvaluator.addPlainInPlace(mkCiphertext, returnedPlaintext);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(mkCiphertext, 1, mkSecretKey1);
    vector<PartialCiphertext> partialDecryptions = {partialCiphertext};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(mkCiphertext, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * 2, computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, MultiplyPlainSameLevel) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);

    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    MKCiphertext mkCiphertext(ciphertext, 1, ciphertext.getLevel());

    mkEvaluator.multiplyPlainInPlace(mkCiphertext, returnedPlaintext);
    mkEvaluator.rescaleInPlace(mkCiphertext);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(mkCiphertext, 1, mkSecretKey1);
    vector<PartialCiphertext> partialDecryptions = {partialCiphertext};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(mkCiphertext, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * numbers[index], computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, MultiplyCipherSameLevel) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);

    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    Ciphertext ciphertext2 = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey2Normal);

    MKCiphertext mkCiphertext1(ciphertext, 1, ciphertext.getLevel());
    MKCiphertext mkCiphertext2(ciphertext2, 2, ciphertext2.getLevel());

    MKCiphertext ret = mkEvaluator.multiplyAndRelinearize(mkCiphertext1, mkCiphertext2);
    mkEvaluator.rescaleInPlace(ret);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(ret, 1, mkSecretKey1);
    PartialCiphertext partialCiphertext2 = mkDecryptor.partialDecryption(ret, 2, mkSecretKey2);

    vector<PartialCiphertext> partialDecryptions = {partialCiphertext, partialCiphertext2};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(ret, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * numbers[index], computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, MultiplyCipherAddPlain) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);
    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    Ciphertext ciphertext2 = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey2Normal);

    MKCiphertext mkCiphertext1(ciphertext, 1, ciphertext.getLevel());
    MKCiphertext mkCiphertext2(ciphertext2, 2, ciphertext2.getLevel());

    MKCiphertext ret = mkEvaluator.multiplyAndRelinearize(mkCiphertext1, mkCiphertext2);
    mkEvaluator.rescaleInPlace(ret);
    mkEvaluator.addPlainInPlace(ret, returnedPlaintext);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(ret, 1, mkSecretKey1);
    PartialCiphertext partialCiphertext2 = mkDecryptor.partialDecryption(ret, 2, mkSecretKey2);

    vector<PartialCiphertext> partialDecryptions = {partialCiphertext, partialCiphertext2};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(ret, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * (numbers[index] + 1), computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, AddCipherSameLevel) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);
    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    Ciphertext ciphertext2 = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey2Normal);

    MKCiphertext mkCiphertext1(ciphertext, 1, ciphertext.getLevel());
    MKCiphertext mkCiphertext2(ciphertext2, 2, ciphertext2.getLevel());

    MKCiphertext ret = mkEvaluator.add(mkCiphertext1, mkCiphertext2);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(ret, 1, mkSecretKey1);
    PartialCiphertext partialCiphertext2 = mkDecryptor.partialDecryption(ret, 2, mkSecretKey2);

    vector<PartialCiphertext> partialDecryptions = {partialCiphertext, partialCiphertext2};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(ret, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * 2, computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, MultiplyPlainAddCipher) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);
    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    Ciphertext ciphertext2 = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey2Normal);

    MKCiphertext mkCiphertext1(ciphertext, 1, ciphertext.getLevel());
    MKCiphertext mkCiphertext2(ciphertext2, 2, ciphertext2.getLevel());

    mkEvaluator.multiplyPlainInPlace(mkCiphertext1, returnedPlaintext);
    mkEvaluator.rescaleInPlace(mkCiphertext1);
    MKCiphertext ret = mkEvaluator.add(mkCiphertext1, mkCiphertext2);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(ret, 1, mkSecretKey1);
    PartialCiphertext partialCiphertext2 = mkDecryptor.partialDecryption(ret, 2, mkSecretKey2);

    vector<PartialCiphertext> partialDecryptions = {partialCiphertext, partialCiphertext2};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(ret, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * (numbers[index] + 1), computedNumbers[index].real(), 1e-8);
    }
}

TEST(MKEvaluator, Negate) {
    vector<double> numbers = generateDoublesMKEval(4096);
    Plaintext returnedPlaintext = encMKEval.encode(numbers);
    Ciphertext ciphertext = encryptorMKEval.encrypt(returnedPlaintext, mkPublicKey1Normal);
    MKCiphertext mkCiphertext(ciphertext, 1, ciphertext.getLevel());
    mkEvaluator.negateInPlace(mkCiphertext);

    PartialCiphertext partialCiphertext = mkDecryptor.partialDecryption(mkCiphertext, 1, mkSecretKey1);
    vector<PartialCiphertext> partialDecryptions = {partialCiphertext};

    Plaintext computedPlaintext = mkDecryptor.mergeDecryptions(mkCiphertext, partialDecryptions);
    vector<complex<double>> computedNumbers = encMKEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(-numbers[index], computedNumbers[index].real(), 1e-8);
    }
}
