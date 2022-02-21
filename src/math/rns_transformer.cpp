#include "smkhe/math/rns_transformer.h"
#include "iostream"

namespace smkhe {
    RNSTransformer::RNSTransformer(vector<uint64_t> pPrimes, vector<uint64_t> qPrimes) : numberPrimesP(pPrimes.size()),
                                                                                         numberPrimesQ(qPrimes.size()),
                                                                                         primesP(pPrimes),
                                                                                         primesQ(qPrimes),
                                                                                         pHatModQ(qPrimes.size(),
                                                                                                  vector<uint64_t>(
                                                                                                          pPrimes.size())),
                                                                                         qHatInverseModQ(qPrimes.size(),
                                                                                                         vector<uint64_t>(
                                                                                                                 qPrimes.size())),
                                                                                         pHatInverseModP(
                                                                                                 pPrimes.size()),
                                                                                         pInverseModQ(qPrimes.size()),
                                                                                         qHatModP(pPrimes.size(),
                                                                                                  vector<vector<uint64_t>>(
                                                                                                          qPrimes.size(),
                                                                                                          vector<uint64_t>(
                                                                                                                  qPrimes.size()))) {
        for (int i = 0; i < qPrimes.size(); ++i) {
            uint64_t allPProd = 1;
            for (auto pPrime: pPrimes) {
                allPProd = modMultiply(allPProd, pPrime, qPrimes[i]);
            }
            pInverseModQ[i] = fastExp(allPProd, qPrimes[i] - 2, qPrimes[i]);
            for (int j = 0; j < pPrimes.size(); ++j) {
                uint64_t currPInverse = fastExp(pPrimes[j], qPrimes[i] - 2, qPrimes[i]);
                pHatModQ[i][j] = modMultiply(currPInverse, allPProd, qPrimes[i]);
            }
        }
        for (int i = 0; i < pPrimes.size(); ++i) {
            uint64_t allPProd = 1;
            for (int j = 0; j < pPrimes.size(); ++j) {
                if (i == j) {
                    continue;
                }
                allPProd = modMultiply(allPProd, pPrimes[j], pPrimes[i]);
            }
            pHatInverseModP[i] = fastExp(allPProd, pPrimes[i] - 2, pPrimes[i]);
        }
        for (int i = 0; i < pPrimes.size(); ++i) {
            for (int l = 0; l < qPrimes.size(); ++l) {
                uint64_t allPProd = 1;
                for (int j = 0; j <= l; ++j) {
                    allPProd = modMultiply(allPProd, qPrimes[j], pPrimes[i]);
                }
                for (int j = 0; j <= l; ++j) {
                    uint64_t currQInverse = fastExp(qPrimes[j], pPrimes[i] - 2, pPrimes[i]);
                    qHatModP[i][l][j] = modMultiply(currQInverse, allPProd, pPrimes[i]);
                }
            }
        }
        for (int l = 0; l < qPrimes.size(); ++l) {
            for (int j = 0; j <= l; ++j) {
                qHatInverseModQ[l][j] = 1;
                for (int k = 0; k <= l; ++k) {
                    if (k == j) {
                        continue;
                    }
                    qHatInverseModQ[l][j] = modMultiply(qHatInverseModQ[l][j], qPrimes[k], qPrimes[j]);
                }
                qHatInverseModQ[l][j] = fastExp(qHatInverseModQ[l][j], qPrimes[j] - 2, qPrimes[j]);
            }
        }
    }

    vector<vector<uint64_t>> RNSTransformer::modUp(vector<vector<uint64_t>> &vectorInQBasis) {
        if (vectorInQBasis.size() > numberPrimesQ) {
            throw ("Q basis from vector does not match Q basis from RNSTransformer");
        }

        vector<vector<uint64_t>> result(numberPrimesP);

        for (int i = 0; i < numberPrimesP; ++i) {
            for (int k = 0; k < vectorInQBasis[0].size(); ++k) {
                uint64_t conversionResult = 0;
                for (int j = 0; j < vectorInQBasis.size(); ++j) {
                    uint64_t currentResult = vectorInQBasis[j][k];
                    currentResult = modMultiply(currentResult, qHatInverseModQ[vectorInQBasis.size() - 1][j],
                                                primesQ[j]);
                    currentResult = modMultiply(currentResult, qHatModP[i][vectorInQBasis.size() - 1][j], primesP[i]);
                    conversionResult = modAdd(conversionResult, currentResult, primesP[i]);
                }
                result[i].push_back(conversionResult);
            }
        }
        result.insert(result.end(), vectorInQBasis.begin(), vectorInQBasis.end());

        return result;
    }

    vector<vector<uint64_t>> RNSTransformer::modDown(vector<vector<uint64_t>> &vectorInPQBasis) {
        int levels = vectorInPQBasis.size() - numberPrimesP;
        vector<vector<uint64_t>> result(levels);

        for (int i = 0; i < levels; ++i) {
            for (int k = 0; k < vectorInPQBasis[0].size(); ++k) {
                uint64_t conversionResult = 0;
                for (int j = 0; j < numberPrimesP; ++j) {
                    uint64_t currentResult = vectorInPQBasis[j][k];
                    currentResult = modMultiply(currentResult, pHatInverseModP[j], primesP[j]);
                    currentResult = modMultiply(currentResult, pHatModQ[i][j], primesQ[i]);
                    conversionResult = modAdd(conversionResult, currentResult, primesQ[i]);
                }
                result[i].push_back(conversionResult);
            }
        }

        for (int i = 0; i < levels; ++i) {
            for (int k = 0; k < result[i].size(); ++k) {
                result[i][k] = modAdd(primesQ[i] - result[i][k], vectorInPQBasis[i + numberPrimesP][k], primesQ[i]);
                result[i][k] = modMultiply(pInverseModQ[i], result[i][k], primesQ[i]);
            }
        }

        return result;
    }

    vector<Polynomial<uint64_t>>
    RNSTransformer::raisePolys(vector<Polynomial<uint64_t>> polys, NTTTransformer transformerQ,
                               NTTTransformer transformerP) {
        int levels = polys.size();
        vector<Polynomial<uint64_t>> result;
        vector<vector<uint64_t>> coeffs;
        for (int i = 0; i < polys.size(); ++i) {
            transformerQ.fromNTT(polys[i], i);
            coeffs.push_back(polys[i].getCoeffs());
        }
        coeffs = modUp(coeffs);
        int levelsP = coeffs.size() - levels;
        for (int i = 0; i < coeffs.size(); ++i) {
            Polynomial<uint64_t> newPoly(coeffs[i].size());
            newPoly.setCoeffs(coeffs[i]);
            if (i < levelsP) {
                transformerP.toNTT(newPoly, i);
            } else {
                transformerQ.toNTT(newPoly, i - levelsP);
            }
            result.push_back(newPoly);
        }
        return result;
    }

    vector<Polynomial<uint64_t>>
    RNSTransformer::downPolys(vector<Polynomial<uint64_t>> polys, NTTTransformer transformerQ,
                              NTTTransformer transformerP, int numberPrimesP) {
        vector<vector<uint64_t>> coeffs;
        for (int i = 0; i < polys.size(); ++i) {
            if (i < numberPrimesP) {
                transformerP.fromNTT(polys[i], i);
            } else {
                transformerQ.fromNTT(polys[i], i - numberPrimesP);
            }
            coeffs.push_back(polys[i].getCoeffs());
        }
        coeffs = modDown(coeffs);
        vector<Polynomial<uint64_t>> result;
        for (int i = 0; i < coeffs.size(); ++i) {
            Polynomial<uint64_t> newPoly(coeffs[i].size());
            newPoly.setCoeffs(coeffs[i]);
            transformerQ.toNTT(newPoly, i);
            result.push_back(newPoly);
        }

        return result;
    }
}
