#include "smkhe/evaluator.h"

Evaluator::Evaluator(Parameters params, EvaluationKey evk) : params(params), evk(evk),
                                                             rnsTransformer(params.getSpecialPrimes(), params.getPrimes()) {}

vector<Polynomial<uint64_t>> raisePolys(vector<Polynomial<uint64_t>> polys, RNSTransformer &transformer, NTTTransformer transformerQ,
           NTTTransformer transformerP) {
    int levels = polys.size();
    vector<Polynomial<uint64_t>> result;
    vector<vector<uint64_t>> coeffs;
    for (int i = 0; i < polys.size(); ++i) {
        transformerQ.fromNTT(polys[i], i);
        coeffs.push_back(polys[i].getCoeffs());
    }
    coeffs = transformer.modUp(coeffs);
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

vector<Polynomial<uint64_t>> downPolys(vector<Polynomial<uint64_t>> polys, RNSTransformer &transformer, NTTTransformer transformerQ,
          NTTTransformer transformerP, int primesP) {
    vector<vector<uint64_t>> coeffs;
    for (int i = 0; i < polys.size(); ++i) {
        if (i < primesP) {
            transformerP.fromNTT(polys[i], i);
        } else {
            transformerQ.fromNTT(polys[i], i - primesP);
        }
        coeffs.push_back(polys[i].getCoeffs());
    }
    coeffs = transformer.modDown(coeffs);
    vector<Polynomial<uint64_t>> result;
    for (int i = 0; i < coeffs.size(); ++i) {
        Polynomial<uint64_t> newPoly(coeffs[i].size());
        newPoly.setCoeffs(coeffs[i]);
        transformerQ.toNTT(newPoly, i);
        result.push_back(newPoly);
    }

    return result;
}

void Evaluator::rescaleInPlace(Ciphertext &cipher) {
    int newLevel = cipher.getLevel() - 1;
    params.getTransformerQ().fromNTT(cipher.getPolyA(cipher.getLevel()), cipher.getLevel());
    params.getTransformerQ().fromNTT(cipher.getPolyB(cipher.getLevel()), cipher.getLevel());

    for (int i = 0; i <= newLevel; ++i) {
        uint64_t qinv = fastExp(params.getPrimes()[cipher.getLevel()], params.getPrimes()[i] - 2,
                                params.getPrimes()[i]);
        Polynomial<uint64_t> polyASub = cipher.getPolyA(cipher.getLevel());
        Polynomial<uint64_t> polyBSub = cipher.getPolyB(cipher.getLevel());
        params.getTransformerQ().toNTT(polyASub, i);
        params.getTransformerQ().toNTT(polyBSub, i);
        cipher.getPolyA(i).sub(polyASub, params.getPrimes()[i]);
        cipher.getPolyB(i).sub(polyBSub, params.getPrimes()[i]);
        cipher.getPolyA(i).multiply(qinv, params.getPrimes()[i]);
        cipher.getPolyB(i).multiply(qinv, params.getPrimes()[i]);
    }

    params.getTransformerQ().toNTT(cipher.getPolyA(cipher.getLevel()), cipher.getLevel());
    params.getTransformerQ().toNTT(cipher.getPolyB(cipher.getLevel()), cipher.getLevel());
    cipher.decreaseLevel(1);
}

void Evaluator::multiplyRelinInPlace(Ciphertext &cipherA, Ciphertext &cipherB) {
    int minLevel = min(cipherA.getLevel(), cipherB.getLevel());
    if (minLevel == 0) {
        throw ("There are no levels remaining for relinearization");
    }

    cipherA.decreaseLevel(cipherA.getLevel() - minLevel);
    cipherB.decreaseLevel(cipherB.getLevel() - minLevel);

    vector<Polynomial<uint64_t>> axbx1 = cipherA.getPolysB();
    for (int i = 0; i < axbx1.size(); ++i) {
        axbx1[i].add(cipherA.getPolyA(i), params.getPrimes()[i]);
    }
    vector<Polynomial<uint64_t>> axbx2 = cipherB.getPolysB();
    for (int i = 0; i < axbx2.size(); ++i) {
        axbx2[i].add(cipherB.getPolyA(i), params.getPrimes()[i]);
    }
    for (int i = 0; i < axbx1.size(); ++i) {
        axbx1[i].multiply(axbx2[i], params.getPrimes()[i]);
    }
    vector<Polynomial<uint64_t>> axax = cipherA.getPolysB();
    for (int i = 0; i < axax.size(); ++i) {
        axax[i].multiply(cipherB.getPolyB(i), params.getPrimes()[i]);
    }
    vector<Polynomial<uint64_t>> bxbx = cipherA.getPolysA();
    for (int i = 0; i < bxbx.size(); ++i) {
        bxbx[i].multiply(cipherB.getPolyA(i), params.getPrimes()[i]);
    }
    vector<Polynomial<uint64_t>> axraised = raisePolys(axax, rnsTransformer, params.getTransformerQ(),
                                                       params.getTransformerP());
    vector<Polynomial<uint64_t>> axmult;
    for (int i = 0; i < axraised.size(); ++i) {
        axmult.push_back(axraised[i]);
        if (i < params.getSpecialPrimesNumber()) {
            axmult[i].multiply(evk.aP[i], params.getSpecialPrimes()[i]);
        } else {
            axmult[i].multiply(evk.aQ[i - params.getSpecialPrimesNumber()],
                               params.getPrimes()[i - params.getSpecialPrimesNumber()]);
        }
    }
    vector<Polynomial<uint64_t>> bxmult;
    for (int i = 0; i < axraised.size(); ++i) {
        bxmult.push_back(axraised[i]);
        if (i < params.getSpecialPrimesNumber()) {
            bxmult[i].multiply(evk.bP[i], params.getSpecialPrimes()[i]);
        } else {
            bxmult[i].multiply(evk.bQ[i - params.getSpecialPrimesNumber()],
                               params.getPrimes()[i - params.getSpecialPrimesNumber()]);
        }
    }
    axmult = downPolys(axmult, rnsTransformer, params.getTransformerQ(), params.getTransformerP(),
                       params.getSpecialPrimesNumber());
    bxmult = downPolys(bxmult, rnsTransformer, params.getTransformerQ(), params.getTransformerP(),
                       params.getSpecialPrimesNumber());
    for (int i = 0; i < axmult.size(); ++i) {
        axmult[i].add(axbx1[i], params.getPrimes()[i]);
        axmult[i].sub(bxbx[i], params.getPrimes()[i]);
        axmult[i].sub(axax[i], params.getPrimes()[i]);
        bxmult[i].add(bxbx[i], params.getPrimes()[i]);
        cipherA.getPolyA(i) = bxmult[i];
        cipherA.getPolyB(i) = axmult[i];
    }
    rescaleInPlace(cipherA);
}

void Evaluator::multiplyPlainInPlace(Ciphertext &cipher, Plaintext &plaintext) {
    int minLevel = min(cipher.getLevel(), plaintext.getLevel());
    if (minLevel == 0) {
        throw ("There are no levels remaining for relinearization");
    }

    cipher.decreaseLevel(cipher.getLevel() - minLevel);
    plaintext.decreaseLevel(plaintext.getLevel() - minLevel);

    for (int currentLevel = 0; currentLevel <= minLevel; ++currentLevel) {
        params.getTransformerQ().toNTT(plaintext.getPolynomial(currentLevel), currentLevel);
        cipher.getPolyA(currentLevel).multiply(plaintext.getPolynomial(currentLevel), params.getPrimes()[currentLevel]);
        cipher.getPolyB(currentLevel).multiply(plaintext.getPolynomial(currentLevel), params.getPrimes()[currentLevel]);
        params.getTransformerQ().fromNTT(plaintext.getPolynomial(currentLevel), currentLevel);
    }
    rescaleInPlace(cipher);
}

void Evaluator::addPlainInPlace(Ciphertext &cipher, Plaintext &plaintext) {
    int minLevel = min(cipher.getLevel(), plaintext.getLevel());

    cipher.decreaseLevel(cipher.getLevel() - minLevel);
    plaintext.decreaseLevel(plaintext.getLevel() - minLevel);

    for (int i = 0; i <= minLevel; ++i) {
        params.getTransformerQ().toNTT(plaintext.getPolynomial(i), i);
        cipher.getPolyA(i).add(plaintext.getPolynomial(i), params.getPrimes()[i]);
        params.getTransformerQ().fromNTT(plaintext.getPolynomial(i), i);
    }
}

void Evaluator::addCipherInPlace(Ciphertext &cipherA, Ciphertext &cipherB) {
    int minLevel = min(cipherA.getLevel(), cipherB.getLevel());

    cipherA.decreaseLevel(cipherA.getLevel() - minLevel);
    cipherB.decreaseLevel(cipherB.getLevel() - minLevel);

    for (int i = 0; i <= minLevel; ++i) {
        cipherA.getPolyA(i).add(cipherB.getPolyA(i), params.getPrimes()[i]);
        cipherA.getPolyB(i).add(cipherB.getPolyB(i), params.getPrimes()[i]);
    }
}

void Evaluator::negateInPlace(Ciphertext &cipher) {
    for (int level = 0; level <= cipher.getLevel(); ++level) {
        for (auto &coeff: cipher.getPolyA(level).getCoeffs()) {
            coeff = params.getPrimes()[level] - coeff;
        }
        for (auto &coeff: cipher.getPolyB(level).getCoeffs()) {
            coeff = params.getPrimes()[level] - coeff;
        }
    }
}
