#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/keygen.h"
#include "smkhe/evaluation_key.h"
#include "smkhe/encryptor.h"
#include "smkhe/evaluator.h"
#include <random>

#define SEED 1231121
#define PRIMES {1152921504606748673, 576460752308273153, 576460752302473217}
#define SPECIAL_PRIMES {0x7fffffffe0001, 0x80000001c0001, 0x80000002c0001, 0x7ffffffd20001}

using namespace std;
using namespace smkhe;

Parameters parametersEval((1ULL << 59), 16384, PRIMES, SPECIAL_PRIMES);
Encoder encEval(parametersEval);
Encryptor encryptorEval(parametersEval);
Keygen keygenEval(parametersEval);
SecretKey secretKeyEval = keygenEval.generateSecretKey();
PublicKey publicKeyEval = keygenEval.generatePublicKey();
EvaluationKey evk = keygenEval.generateEvaluationKey();
Evaluator eval(parametersEval, evk);

vector<double> generateDoublesEval(int number) {
    vector<double> result(number);
    static default_random_engine generator(SEED);
    static uniform_real_distribution<double> distribution(0.0, 1.5);

    for (int i = 0; i < number; ++i) {
        result[i] = distribution(generator);
    }

    return result;
}

TEST(Evaluator, AddPlainSameLevel) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintex = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintex, publicKeyEval);
    eval.addPlainInPlace(ciphertext, returnedPlaintex);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * 2, computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, MultiplyPlainSameLevel) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.multiplyPlainInPlace(ciphertext, returnedPlaintext);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * numbers[index], computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, MultiplyCipherSameLevel) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    Ciphertext ciphertext2 = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.multiplyRelinInPlace(ciphertext, ciphertext2);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * numbers[index], computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, MultiplyCipherAddPlain) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    Ciphertext ciphertext2 = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.multiplyRelinInPlace(ciphertext, ciphertext2);
    eval.addPlainInPlace(ciphertext, returnedPlaintext);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * (numbers[index] + 1), computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, AddCipherSameLevel) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    Ciphertext ciphertext2 = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.addCipherInPlace(ciphertext, ciphertext2);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * 2, computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, MultiplyPlainAddCipher) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    Ciphertext ciphertext2 = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.multiplyPlainInPlace(ciphertext, returnedPlaintext);
    eval.addCipherInPlace(ciphertext, ciphertext2);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(numbers[index] * (numbers[index] + 1), computedNumbers[index].real(), 1e-8);
    }
}

TEST(Evaluator, Negate) {
    vector<double> numbers = generateDoublesEval(4096);
    Plaintext returnedPlaintext = encEval.encode(numbers);
    Ciphertext ciphertext = encryptorEval.encrypt(returnedPlaintext, publicKeyEval);
    eval.negateInPlace(ciphertext);
    Plaintext computedPlaintext = encryptorEval.decrypt(ciphertext, secretKeyEval);
    vector<complex<double>> computedNumbers = encEval.decode(computedPlaintext);

    for (int index = 0; index < 4096; ++index) {
        ASSERT_NEAR(-numbers[index], computedNumbers[index].real(), 1e-8);
    }
}

