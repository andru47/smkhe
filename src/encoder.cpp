#include "smkhe/encoder.h"
#include <vector>
#include <random>

void scaleUp(vector<complex<double>> &vector, double scale) {
    for (auto &elem: vector) {
        elem *= scale;
    }
}

void scaleDown(vector<complex<double>> &vector, double scale) {
    for (auto &elem: vector) {
        elem /= scale;
    }
}

void bitReverse(vector<complex<double>> &vec) {
    int n = vec.size();
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t ci = i;
        uint32_t bitReversedI = 0;
        uint32_t currentPos = 0;
        while (ci) {
            bitReversedI |= (currentPos << (ci & 1));
            ci >>= 1;
        }
        if (i < bitReversedI) {
            swap(vec[i], vec[bitReversedI]);
        }
    }
}

void fftDecode(vector<complex<double>> &valuesToDecode, vector<int> &indexHash, vector<complex<double>> &rootAtPower) {

    bitReverse(valuesToDecode);
    int N = valuesToDecode.size();

    for (int currentMatrixLen = 2; currentMatrixLen <= N; currentMatrixLen <<= 1) {
        for (int currentMatrixStartCol = 0; currentMatrixStartCol < N; currentMatrixStartCol += currentMatrixLen) {
            int newM = currentMatrixLen * 4;
            for (int currentCol = currentMatrixStartCol;
                 currentCol < currentMatrixStartCol + currentMatrixLen / 2; ++currentCol) {
                int currentIndexHash = (indexHash[currentCol - currentMatrixStartCol] % newM) * (N / currentMatrixLen);
                complex<double> element1 = valuesToDecode[currentCol];
                complex<double> element2 = valuesToDecode[currentCol + currentMatrixLen / 2];
                element2 *= rootAtPower[currentIndexHash];

                valuesToDecode[currentCol] = element1 + element2;
                valuesToDecode[currentCol + currentMatrixLen / 2] = element1 - element2;
            }
        }
    }
}


void inverseFFTEncode(vector<complex<double>> &valuesToEncode, vector<int> &indexHash,
                      vector<complex<double>> &rootAtPower) {
    int N = valuesToEncode.size();

    for (int currentMatrixLen = N; currentMatrixLen >= 1; currentMatrixLen >>= 1) {
        for (int currentMatrixStartCol = 0; currentMatrixStartCol < N; currentMatrixStartCol += currentMatrixLen) {
            int newM = currentMatrixLen * 4;
            for (int currentCol = currentMatrixStartCol;
                 currentCol < currentMatrixStartCol + currentMatrixLen / 2; ++currentCol) {
                int currentIndexHash = (indexHash[currentCol - currentMatrixStartCol] % newM) * (N / currentMatrixLen);
                complex<double> element1 =
                        valuesToEncode[currentCol] + valuesToEncode[currentCol + currentMatrixLen / 2];
                complex<double> element2 =
                        valuesToEncode[currentCol] - valuesToEncode[currentCol + currentMatrixLen / 2];
                element2 *= conj(rootAtPower[currentIndexHash]);

                valuesToEncode[currentCol] = element1;
                valuesToEncode[currentCol + currentMatrixLen / 2] = element2;
            }
        }
    }

    bitReverse(valuesToEncode);
}

vector<complex<double>>
tauInverse(vector<complex<double>> &givenValues, vector<int> &indexHash, vector<complex<double>> &rootAtPower) {
    vector<complex<double>> res(givenValues.size() * 2);

    vector<complex<double>> fftVec(givenValues.size());
    for (int i = 0; i < givenValues.size(); ++i) {
        fftVec[i] = givenValues[i];
    }

    inverseFFTEncode(fftVec, indexHash, rootAtPower);

    for (int i = 0; i < givenValues.size(); ++i) {
        fftVec[i] /= givenValues.size();
        res[i] = complex<double>(fftVec[i].real(), 0);
        res[i + givenValues.size()] = complex<double>(fftVec[i].imag(), 0);
    }
    return res;
}

vector<complex<double>>
tau(vector<complex<double>> &givenPolynomial, vector<int> &indexHash, vector<complex<double>> &rootAtPower) {
    static complex<double> i(0, 1);
    vector<complex<double>> mCombined(givenPolynomial.size() / 2);

    for (int index = 0; index < givenPolynomial.size() / 2; ++index) {
        mCombined[index] = givenPolynomial[index] + i * givenPolynomial[index + givenPolynomial.size() / 2];
    }

    fftDecode(mCombined, indexHash, rootAtPower);
    return vector<complex<double>>(mCombined.begin(), mCombined.end());
}

Encoder::Encoder(Parameters &givenParameters) : parameters(givenParameters) {
    int fivePow = 1;
    indexHash.resize(givenParameters.getRingDegree());
    for (int i = 0; i < givenParameters.getRingDegree(); ++i) {
        indexHash[i] = fivePow;
        fivePow = (fivePow * 5) % (2 * givenParameters.getRingDegree());
    }

    rootAtPower.resize(2 * givenParameters.getRingDegree() + 1);

    for (int i = 0; i < givenParameters.getRingDegree() * 2; ++i) {
        double currentAngle = (1.0 * i) * M_PI / (1.0 * givenParameters.getRingDegree());
        rootAtPower[i] = complex<double>(cos(currentAngle), sin(currentAngle));
    }
    rootAtPower[givenParameters.getRingDegree() * 2] = rootAtPower[0];
}

Parameters Encoder::getParameters() {
    return this->parameters;
}

void Encoder::setParameters(Parameters &givenParameters) {
    this->parameters = givenParameters;
}

void randomRound(vector<complex<double>> &realCoeffs) {
    static random_device rd;
    static mt19937 generator(rd());
    for (int index = 0; index < realCoeffs.size(); ++index) {
        double weight = realCoeffs[index].real() - floor(realCoeffs[index].real());
        auto distribution = binomial_distribution(1, weight);
        long long integerCoefficient = floor(realCoeffs[index].real()) + distribution(generator);
        realCoeffs[index].real(integerCoefficient);
        realCoeffs[index].imag(0);
    }
}

Plaintext Encoder::encode(vector<complex<double>> toEncode) {
    if (toEncode.size() > parameters.getRingDegree() / 2) {
        throw ("There are more values to encode than slots: can only encode parameters.getRingDegree() / 2");
    }

    scaleUp(toEncode, parameters.getScale());
    toEncode.resize(parameters.getRingDegree() / 2, complex<double>(0, 0));

    auto realCoeffs = tauInverse(toEncode, indexHash, rootAtPower);
    randomRound(realCoeffs);

    return Plaintext(realCoeffs, parameters);
}

Plaintext Encoder::encode(vector<double> toEncode) {
    vector<complex<double>> complexVec(toEncode.begin(), toEncode.end());

    return encode(complexVec);
}

vector<complex<double>> Encoder::decode(Plaintext &givenPlaintext) {
    vector<long long> coeffs = givenPlaintext.getPolynomial().getCoeffs();
    vector<complex<double>> givenVector(coeffs.size());

    for (int index = 0; index < coeffs.size(); ++index) {
        givenVector[index] = complex<double>(coeffs[index]);
    }

    vector<complex<double>> result = tau(givenVector, indexHash, rootAtPower);
    scaleDown(result, parameters.getScale());
    return result;
}
