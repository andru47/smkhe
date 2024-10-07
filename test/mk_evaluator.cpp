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

// Tests continue as in the previous provided code...

