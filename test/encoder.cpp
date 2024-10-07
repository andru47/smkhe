#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/encryptor.h"
#include "smkhe/mk_evaluator.h"
#include "smkhe/mk_keygen.h"
#include "smkhe/mk_decryptor.h"
#include <vector>
#include <random>

using namespace std;
using namespace smkhe;

// Function to generate data based on SNPs and weights, repeating them based on input size
vector<double> generateSNPWeightedPatternData(int size) {
    vector<double> SNPs = {0, 1, 2, 0, 1};  // Example SNP data
    vector<double> weights = {0.5, 1.2, 0.8, 0.3, 0.9};  // Weights associated with each SNP
    
    vector<double> data(size);
    
    // Repeat the SNP and weight pattern to fill the data based on input size
    for (int i = 0; i < size; i++) {
        data[i] = SNPs[i % SNPs.size()] * weights[i % weights.size()];
    }
    
    return data;
}

TEST(Encoder, DynamicInputSizeTest) {
    // Initialize Parameters
    Parameters parameters(pow(2.0, 50), 16384, {1152921504606748673}, {1152921504606748673});
    MKKeygen keygen(parameters, 12345);
    SecretKey secretKey = keygen.generateSecretKey();
    MKPublicKey publicKey = keygen.generatePublicKey();
    MKEvaluationKey evalKey = keygen.generateEvaluationKey(publicKey);

    // Create components
    Encryptor encryptor(parameters);
    unordered_map<uint64_t, MKPublicKey> publicKeys = {{1, publicKey}};
    unordered_map<uint64_t, MKEvaluationKey> evaluationKeys = {{1, evalKey}};
    MKEvaluator evaluator(parameters, publicKeys, evaluationKeys);
    Encoder encoder(parameters);
    MKDecryptor decryptor(parameters);

    // Generate dynamic input size data with the pattern
    int inputSize = 9000;  // Change this value to adjust the size of input data
    vector<double> data = generateSNPWeightedPatternData(inputSize);

    Plaintext encodedData = encoder.encode(data);

    // Encrypt the data
    PublicKey pk = publicKey.getPublicKey();
    Ciphertext encryptedData = encryptor.encrypt(encodedData, pk);

    // Directly decrypt without operations to verify
    MKCiphertext mkEncryptedData(encryptedData, 1, 0);
    PartialCiphertext partialDecryption = decryptor.partialDecryption(mkEncryptedData, 1, secretKey);
    vector<PartialCiphertext> partialDecryptions = {partialDecryption};
    Plaintext decryptedData = decryptor.mergeDecryptions(mkEncryptedData, partialDecryptions);

    // Decode the result
    vector<complex<double>> decodedData = encoder.decode(decryptedData);

    // Check if decrypted result matches the original
    for (int i = 0; i < inputSize; i++) {
        ASSERT_NEAR(data[i], decodedData[i].real(), 1e-6);  // Adjusting tolerance if needed
    }
}
