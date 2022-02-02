#include "gtest/gtest.h"
#include "smkhe/encoder.h"
#include <random>

#define SEED 1231121
#define BIG_PRIME 1152921504606748673
using namespace std;

vector<double> generateDoubles(int number) {
    vector<double> result(number);
    static default_random_engine generator(SEED);
    static uniform_real_distribution<double> distribution(0.0, 10000.0);

    for (int i = 0; i < number; ++i) {
        result[i] = distribution(generator);
    }

    return result;
}

TEST(Encoder, EncodeAndDecodeWithValidParams) {
    Parameters parameters(pow(2.0, 40), 16384, {BIG_PRIME});
    Encoder enc(parameters);
    vector<double> numbers = generateDoubles(8192);
    Plaintext returnedPlaintex = enc.encode(numbers);
    vector<complex<double>> newNumbers = enc.decode(returnedPlaintex);

    for (int index = 0; index < 8192; ++index) {
        ASSERT_NEAR(numbers[index], newNumbers[index].real(), 1e-8);
    }
}
