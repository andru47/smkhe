#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include "smkhe/math/ntt_transformer.h"
#include <random>

#define SEED 1231121
#define NUMBER_COEFFS 16384
#define MULT_NUMBER_COEFFS 2048

using namespace std;

vector<uint64_t> predefinedBigPrimes = {1152921504606748673, 576460752308273153, 576460752302473217,
                                        576460752304439297};
Parameters params(double(pow(2.0, 40)), NUMBER_COEFFS, predefinedBigPrimes);
Parameters paramsMult(double(pow(2.0, 40)), MULT_NUMBER_COEFFS, predefinedBigPrimes);

NTTTransformer transformer(params);
NTTTransformer transformerMult(paramsMult);

uint64_t modMultiply(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;

    while (b) {
        if (b % 2) {
            result = (result + a) % mod;
            b -= 1;
        }
        a = (a + a) % mod;
        b >>= 1;
    }

    return result;
}

uint64_t modAdd(uint64_t a, uint64_t b, uint64_t mod) {
    return (a + b) % mod;
}

uint64_t fastExp(uint64_t base, uint64_t exponent, uint64_t mod) {
    if (!exponent) {
        return 1;
    }
    if (exponent == 1) {
        return base;
    }
    if (exponent % 2) {
        return modMultiply(base, fastExp(modMultiply(base, base, mod), exponent / 2, mod), mod);
    }

    return fastExp(modMultiply(base, base, mod), exponent / 2, mod);
}

vector<uint64_t> generateUints(int number) {
    vector<uint64_t> result(number);
    static default_random_engine generator(SEED);
    static uniform_int_distribution<uint64_t> distribution(10000000, 100000000000);

    for (int i = 0; i < number; ++i) {
        result[i] = distribution(generator);
    }

    return result;
}

uint64_t sub(uint64_t a, uint64_t b, uint64_t mod) {
    if (a >= b) {
        return (a - b) % mod;
    }
    return (a + mod - b) % mod;
}

/// Divides poly by (X^N + 1)
void dividePoly(vector<uint64_t> &poly, uint64_t mod) {
    while (poly.size() >= MULT_NUMBER_COEFFS + 1) {
        vector<uint64_t> newCoeffs(poly);
        uint64_t power = poly.size() - (MULT_NUMBER_COEFFS + 1);
        newCoeffs[power] = sub(newCoeffs[power], newCoeffs.back(), mod);
        newCoeffs.back() = 0;
        int index = poly.size() - 1;
        while (index >= 0 && newCoeffs[index] == 0) {
            --index;
        }
        newCoeffs.resize(index + 1);
        poly = newCoeffs;
    }
}

TEST(NTTransformer, TO_NTT_FROM_NTT_IS_IDENTITY) {
    vector<uint64_t> coeffs = generateUints(NUMBER_COEFFS);
    Polynomial<uint64_t> poly(NUMBER_COEFFS, coeffs);
    for (int level = 0; level < params.getModulusLevels(); ++level) {
        transformer.toNTT(poly, level);
        transformer.fromNTT(poly, level);
        for (int index = 0; index < NUMBER_COEFFS; ++index) {
            ASSERT_EQ(poly.getCoeffs()[index], coeffs[index]);
        }
    }
}

TEST(NTTransformer, NTT_ADDITION) {
    vector<uint64_t> coeffs1 = generateUints(NUMBER_COEFFS);
    vector<uint64_t> coeffs2 = generateUints(NUMBER_COEFFS);
    Polynomial<uint64_t> poly1(NUMBER_COEFFS, coeffs1);
    Polynomial<uint64_t> poly2(NUMBER_COEFFS, coeffs2);

    for (int level = 0; level < params.getModulusLevels(); ++level) {
        poly1.setCoeffs(coeffs1);
        transformer.toNTT(poly1, level);
        transformer.toNTT(poly2, level);
        for (int index = 0; index < NUMBER_COEFFS; ++index) {
            poly1.getCoeffs()[index] =
                    (poly1.getCoeffs()[index] + poly2.getCoeffs()[index]) % transformer.getPrime(level);
        }
        transformer.fromNTT(poly1, level);
        transformer.fromNTT(poly2, level);
        for (int index = 0; index < NUMBER_COEFFS; ++index) {
            ASSERT_EQ(poly1.getCoeffs()[index], coeffs1[index] + coeffs2[index]);
        }
    }
}

TEST(NTTransformer, NTT_MULTIPLICATION) {
    vector<uint64_t> coeffs1 = generateUints(MULT_NUMBER_COEFFS);
    vector<uint64_t> coeffs2 = generateUints(MULT_NUMBER_COEFFS);
    Polynomial<uint64_t> poly1(MULT_NUMBER_COEFFS, coeffs1);
    Polynomial<uint64_t> poly2(MULT_NUMBER_COEFFS, coeffs2);

    for (int level = 0; level < paramsMult.getModulusLevels(); ++level) {
        vector<uint64_t> normalMult(MULT_NUMBER_COEFFS * 2 - 1, 0);
        for (int i = 0; i < MULT_NUMBER_COEFFS; ++i) {
            for (int j = 0; j < MULT_NUMBER_COEFFS; ++j) {
                normalMult[i + j] = modAdd(normalMult[i + j],
                                           modMultiply(coeffs1[i], coeffs2[j], transformerMult.getPrime(level)),
                                           transformerMult.getPrime(level));
            }
        }

        transformerMult.toNTT(poly1, level);
        transformerMult.toNTT(poly2, level);

        for (int index = 0; index < MULT_NUMBER_COEFFS; ++index) {
            poly1.getCoeffs()[index] = modMultiply(poly1.getCoeffs()[index], poly2.getCoeffs()[index],
                                                   transformerMult.getPrime(level));
        }

        transformerMult.fromNTT(poly1, level);
        transformerMult.fromNTT(poly2, level);

        dividePoly(normalMult, transformerMult.getPrime(level));
        for (int index = 0; index < MULT_NUMBER_COEFFS; ++index) {
            ASSERT_EQ(poly1.getCoeffs()[index], normalMult[index]);
        }
        poly1.setCoeffs(coeffs1);
    }
}