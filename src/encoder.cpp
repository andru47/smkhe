#include "smkhe/encoder.h"
#include <vector>
#include <random>

void scaleUp(vector<dcomplex> &vector, double scale) {
    for (auto &elem: vector) {
        elem *= scale;
    }
}

void scaleDown(vector<dcomplex> &vector, double scale) {
    for (auto &elem: vector) {
        elem /= scale;
    }
}

void bitReverse(VectorXcd &vec) {
    int n = vec.rows();
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t ci = i;
        uint32_t bitReversedI = 0;
        uint32_t currentPos = 0;
        while (ci) {
            bitReversedI |= (currentPos << (ci & 1));
            ci >>= 1;
        }
        if (i < bitReversedI) {
            swap(vec(i, 0), vec(bitReversedI, 0));
        }
    }
}

void fftDecode(VectorXcd &valuesToDecode, vector<int> &indexHash, vector<dcomplex> &rootAtPower) {

    bitReverse(valuesToDecode);
    int N = valuesToDecode.rows();

    for (int currentMatrixLen = 2; currentMatrixLen <= N; currentMatrixLen <<= 1) {
        for (int currentMatrixStartCol = 0; currentMatrixStartCol < N; currentMatrixStartCol += currentMatrixLen) {
            int newM = currentMatrixLen * 4;
            for (int currentCol = currentMatrixStartCol;
                 currentCol < currentMatrixStartCol + currentMatrixLen / 2; ++currentCol) {
                int currentIndexHash = (indexHash[currentCol - currentMatrixStartCol] % newM) * (N / currentMatrixLen);
                dcomplex element1 = valuesToDecode(currentCol, 0);
                dcomplex element2 = valuesToDecode(currentCol + currentMatrixLen / 2, 0);
                element2 *= rootAtPower[currentIndexHash];

                valuesToDecode(currentCol, 0) = element1 + element2;
                valuesToDecode(currentCol + currentMatrixLen / 2, 0) = element1 - element2;
            }
        }
    }
}


void inverseFFTEncode(VectorXcd &valuesToEncode, vector<int> &indexHash, vector<dcomplex> &rootAtPower) {
    int N = valuesToEncode.rows();

    for (int currentMatrixLen = N; currentMatrixLen >= 1; currentMatrixLen >>= 1) {
        for (int currentMatrixStartCol = 0; currentMatrixStartCol < N; currentMatrixStartCol += currentMatrixLen) {
            int newM = currentMatrixLen * 4;
            for (int currentCol = currentMatrixStartCol;
                 currentCol < currentMatrixStartCol + currentMatrixLen / 2; ++currentCol) {
                int currentIndexHash = (indexHash[currentCol - currentMatrixStartCol] % newM) * (N / currentMatrixLen);
                dcomplex element1 =
                        valuesToEncode(currentCol, 0) + valuesToEncode(currentCol + currentMatrixLen / 2, 0);
                dcomplex element2 =
                        valuesToEncode(currentCol, 0) - valuesToEncode(currentCol + currentMatrixLen / 2, 0);
                element2 *= conj(rootAtPower[currentIndexHash]);

                valuesToEncode(currentCol, 0) = element1;
                valuesToEncode(currentCol + currentMatrixLen / 2, 0) = element2;
            }
        }
    }

    bitReverse(valuesToEncode);
}

VectorXcd tauInverse(vector<dcomplex> &givenValues, vector<int> &indexHash, vector<dcomplex> &rootAtPower) {
    VectorXcd res(givenValues.size() * 2);

    VectorXcd fftVec(givenValues.size());
    for (int i = 0; i < givenValues.size(); ++i) {
        fftVec(i, 0) = givenValues[i];
    }

    inverseFFTEncode(fftVec, indexHash, rootAtPower);

    for (int i = 0; i < givenValues.size(); ++i) {
        fftVec(i, 0) /= givenValues.size();
        res(i, 0) = dcomplex(fftVec(i, 0).real(), 0);
        res(i + givenValues.size(), 0) = dcomplex(fftVec(i, 0).imag(), 0);
    }
    return res;
}

vector<dcomplex> tau(VectorXcd &givenPolynomial, vector<int> &indexHash, vector<dcomplex> &rootAtPower) {
    static dcomplex i(0, 1);

    VectorXcd m0 = givenPolynomial.topRows(givenPolynomial.rows() / 2);
    VectorXcd m1 = givenPolynomial.bottomRows(givenPolynomial.rows() / 2);

    VectorXcd mCombined = m0 + i * m1;
    vector<dcomplex> initialVec(mCombined.begin(), mCombined.end());
    vector<dcomplex> res(initialVec.size());

    fftDecode(mCombined, indexHash, rootAtPower);
    return vector<dcomplex>(mCombined.begin(), mCombined.end());
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
        rootAtPower[i] = dcomplex(cos(currentAngle), sin(currentAngle));
    }
    rootAtPower[givenParameters.getRingDegree() * 2] = rootAtPower[0];
}

Parameters Encoder::getParameters() {
    return this->parameters;
}

void Encoder::setParameters(Parameters &givenParameters) {
    this->parameters = givenParameters;
}

void randomRound(VectorXcd &realCoeffs) {
    static random_device rd;
    static mt19937 generator(rd());
    for (int index = 0; index < realCoeffs.rows(); ++index) {
        double weight = realCoeffs(index, 0).real() - floor(realCoeffs(index, 0).real());
        auto distribution = binomial_distribution(1, weight);
        long long integerCoefficient = floor(realCoeffs(index, 0).real()) + distribution(generator);
        realCoeffs(index, 0).real(integerCoefficient);
        realCoeffs(index, 0).imag(0);
    }
}

Plaintext Encoder::encode(vector<dcomplex> toEncode) {
    if (toEncode.size() > parameters.getRingDegree() / 2) {
        throw ("There are more values to encode than slots: can only encode parameters.getRingDegree() / 2");
    }

    scaleUp(toEncode, parameters.getScale());
    toEncode.resize(parameters.getRingDegree() / 2, dcomplex(0, 0));

    auto realCoeffs = tauInverse(toEncode, indexHash, rootAtPower);
    randomRound(realCoeffs);

    return Plaintext(realCoeffs, parameters);
}

Plaintext Encoder::encode(vector<double> toEncode) {
    vector<dcomplex> complexVec(toEncode.begin(), toEncode.end());

    return encode(complexVec);
}

vector<dcomplex> Encoder::decode(Plaintext &givenPlaintext) {
    VectorXcd givenVector = givenPlaintext.getVectorPolynomial();
    vector<dcomplex> result = tau(givenVector, indexHash, rootAtPower);
    scaleDown(result, parameters.getScale());
    return result;
}
