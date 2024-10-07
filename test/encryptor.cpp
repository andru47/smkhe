#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/encryptor.h"
#include "smkhe/keygen.h"
#include "smkhe/mk_keygen.h"
#include <random>

#define SEED 9993211
#define BIG_PRIME 1152921504606748673
using namespace std;
using namespace smkhe;

Parameters parameters(pow(2.0, 40), 16384, {BIG_PRIME}, {BIG_PRIME});

// Provide a seed to MKKeygen constructor
MKKeygen keygen(parameters, SEED); 
SecretKey secretKey = keygen.generateSecretKey();
MKPublicKey mkPublicKey = keygen.generatePublicKey();
PublicKey publicKey = mkPublicKey.getPublicKey();  // Convert MKPublicKey to PublicKey

Encryptor encryptor(parameters);
Encoder enc(parameters);

vector<double> generateDoublesEncryptor(int number) {
    vector<double> result(number);
    static default_random_engine generator(SEED);
    static uniform_real_distribution<double> distribution(0.0, 10000.0);

    for (int i = 0; i < number; ++i) {
        result[i] = distribution(generator);
    }

    return result;
}

TEST(Encryptor, EncryptDecryptIsIdentity) {
    vector<double> numbers = generateDoublesEncryptor(8192);

    Plaintext returnedPlaintext = enc.encode(numbers);
    Ciphertext ciphertext = encryptor.encrypt(returnedPlaintext, publicKey);
    Plaintext newPlaintext = encryptor.decrypt(ciphertext, secretKey);
    vector<complex<double>> newNumbers = enc.decode(newPlaintext);

    for (int index = 0; index < 8192; ++index) {
        ASSERT_NEAR(numbers[index], newNumbers[index].real(), 1e-6);
    }
}

TEST(Encryptor, EncryptSumDecrypt) {
    vector<double> numbers1 = generateDoublesEncryptor(8192);
    vector<double> numbers2 = generateDoublesEncryptor(8192);
    Plaintext returnedPlaintext1 = enc.encode(numbers1);
    Plaintext returnedPlaintext2 = enc.encode(numbers2);

    Ciphertext ciphertext1 = encryptor.encrypt(returnedPlaintext1, publicKey);
    Ciphertext ciphertext2 = encryptor.encrypt(returnedPlaintext2, publicKey);
    ciphertext1.getPolyA(0).add(ciphertext2.getPolyA(0), BIG_PRIME);
    ciphertext1.getPolyB(0).add(ciphertext2.getPolyB(0), BIG_PRIME);

    Plaintext newPlaintext = encryptor.decrypt(ciphertext1, secretKey);
    vector<complex<double>> newNumbers = enc.decode(newPlaintext);

    for (int index = 0; index < 8192; ++index) {
        ASSERT_NEAR(numbers1[index] + numbers2[index], newNumbers[index].real(), 1e-6);
    }
}

