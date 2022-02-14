#include "smkhe/util.h"

using namespace std;

#ifndef SMKHE_POLYNOMIAL_H
#define SMKHE_POLYNOMIAL_H

template<typename T>
class Polynomial {
    int degree;
    vector<T> coeffs;
    bool transformedToNTT = false;

public:
    Polynomial(int degree, vector<T> givenCoeffs) : degree(degree), coeffs(givenCoeffs) {
        if (degree != givenCoeffs.size()) {
            throw ("Degree not equal to vector size");
        }
    }

    explicit Polynomial(int degree) : degree(degree), coeffs(degree) {}

    void setDegree(int degree) {
        this->degree = degree;
        coeffs.resize(degree);
    }

    int getDegree() {
        return degree;
    }

    void setCoeff(int pos, T value) {
        if (pos >= degree) {
            coeffs.resize(pos + 1);
        }
        coeffs[pos] = value;
    }

    bool isTransformedToNTT() {
        return transformedToNTT;
    }

    void setTransformedToNTT(bool transformed) {
        transformedToNTT = transformed;
    }

    T getCoeff(int index) {
        if (index >= degree) {
            throw ("Index out of bounds");
        }
        return coeffs[index];
    }

    vector<T> &getCoeffs() {
        return this->coeffs;
    }

    void setCoeffs(vector<T> givenCoeffs) {
        this->degree = givenCoeffs.size();
        this->coeffs = givenCoeffs;
    }

    Polynomial<T> operator+(const Polynomial<T> &other) {
        Polynomial<T> res(max(degree, other.degree));

        for (int coeffIndex = 0; coeffIndex < max(degree, other.degree); ++coeffIndex) {
            if (coeffIndex < degree && coeffIndex < other.degree) {
                res.setCoeff(coeffIndex, coeffs[coeffIndex] + other.coeffs[coeffIndex]);
            } else if (coeffIndex < degree) {
                res.setCoeff(coeffIndex, coeffs[coeffIndex]);
            } else {
                res.setCoeff(coeffIndex, other.coeffs[coeffIndex]);
            }
        }

        int coeffIndex;

        for (coeffIndex = max(degree, other.degree) - 1; coeffIndex >= 0; --coeffIndex) {
            if (res[coeffIndex] == T()) {
                continue;
            }
            break;
        }
        res.degree = coeffIndex + 1;

        return res;
    }

    Polynomial<T> operator-(const Polynomial<T> &other) {
        Polynomial<T> res(max(degree, other.degree));

        for (int coeffIndex = 0; coeffIndex < max(degree, other.degree); ++coeffIndex) {
            if (coeffIndex < degree && coeffIndex < other.degree) {
                res.setCoeff(coeffIndex, coeffs[coeffIndex] - other.coeffs[coeffIndex]);
            } else if (coeffIndex < degree) {
                res.setCoeff(coeffIndex, coeffs[coeffIndex]);
            } else {
                res.setCoeff(coeffIndex, -other.coeffs[coeffIndex]);
            }
        }

        int coeffIndex;

        for (coeffIndex = max(degree, other.degree) - 1; coeffIndex >= 0; --coeffIndex) {
            if (res[coeffIndex] == T()) {
                continue;
            }
            break;
        }
        res.degree = coeffIndex + 1;
        return res;
    }

    Polynomial &operator+=(Polynomial &other) {
        coeffs.resize(max(degree, other.degree));
        for (int index = 0; index < other.degree; ++index) {
            coeffs[index] += other.coeffs[index];
        }
        int coeffIndex;

        for (coeffIndex = max(degree, other.degree) - 1; coeffIndex >= 0; --coeffIndex) {
            if (coeffs[coeffIndex] == T()) {
                continue;
            }
            break;
        }
        degree = coeffIndex + 1;
        return *this;
    }

    void multiply(uint64_t num, uint64_t mod) {
        for (int index = 0; index < degree; ++index) {
            coeffs[index] = modMultiply(coeffs[index], num, mod);
        }
    }

    void multiply(Polynomial<T> &other, uint64_t mod) {
        if (!transformedToNTT || !other.transformedToNTT) {
            throw ("Both polynomials need to be transformed to NTT.");
        }
        if (degree != other.degree) {
            throw ("Both polynomials need to have the same degree.");
        }
        for (int index = 0; index < degree; ++index) {
            coeffs[index] = modMultiply(coeffs[index], other.coeffs[index], mod);
        }
    }

    void add(Polynomial<uint64_t> &other, uint64_t mod) {
        if (!transformedToNTT || !other.transformedToNTT) {
            throw ("Both polynomials need to be transformed to NTT.");
        }
        if (degree != other.degree) {
            throw ("Both polynomials need to have the same degree.");
        }
        for (int index = 0; index < degree; ++index) {
            coeffs[index] = modAdd(coeffs[index], other.coeffs[index], mod);
        }
    }

    void sub(Polynomial<uint64_t> &other, uint64_t mod) {
        if (!transformedToNTT || !other.transformedToNTT) {
            throw ("Both polynomials need to be transformed to NTT.");
        }
        if (degree != other.degree) {
            throw ("Both polynomials need to have the same degree.");
        }
        for (int index = 0; index < degree; ++index) {
            if (other.coeffs[index] <= coeffs[index]) {
                coeffs[index] = coeffs[index] - other.coeffs[index];
            } else {
                coeffs[index] = coeffs[index] + mod - other.coeffs[index];
            }
            coeffs[index] %= mod;
        }
    }
};

#endif //SMKHE_POLYNOMIAL_H
