#include "smkhe/mk_evaluator.h"
#include <set>


namespace smkhe {
    MKEvaluator::MKEvaluator(Parameters &params, unordered_map<uint64_t, MKPublicKey> &publicKeys,
                             unordered_map<uint64_t, MKEvaluationKey> &evaluationKeys) : params(params),
                                                                                         rnsTransformer(params.getSpecialPrimes(), params.getPrimes()),
                                                                                         publicKeys(publicKeys),
                                                                                         evaluationKeys(evaluationKeys) {}

    pair<MKCiphertext, MKCiphertext> getTransformedCiphers(MKCiphertext &cipherA, MKCiphertext &cipherB) {
        set<uint64_t> idSet;
        vector<uint64_t> idsA = cipherA.getIds(), idsB = cipherB.getIds();
        int level = min(cipherA.getLevel(), cipherB.getLevel());
        cipherA.decreaseLevel(cipherA.getLevel() - level);
        cipherB.decreaseLevel(cipherB.getLevel() - level);

        idSet.insert(idsA.begin(), idsA.end());
        idSet.insert(idsB.begin(), idsB.end());

        vector<uint64_t> allIds(idSet.begin(), idSet.end());

        vector<vector<Polynomial<uint64_t>>> polynomialsInIdOrderA(idSet.size() + 1),
                                             polynomialsInIdOrderB(idSet.size() + 1);

        polynomialsInIdOrderA[0] = cipherA.getCipherForIndex(0);
        polynomialsInIdOrderB[0] = cipherB.getCipherForIndex(0);

        int indexWrite = 1;
        int indexA = 1;
        int indexB = 1;
        for (auto &id: idSet) {
            if (find(idsA.begin(), idsA.end(), id) != idsA.end()) {
                polynomialsInIdOrderA[indexWrite] = cipherA.getCipherForIndex(indexA);
                ++indexA;
            } else {
                polynomialsInIdOrderA[indexWrite] = {};
            }
            if (find(idsB.begin(), idsB.end(), id) != idsB.end()) {
                polynomialsInIdOrderB[indexWrite] = cipherB.getCipherForIndex(indexB);
                ++indexB;
            } else {
                polynomialsInIdOrderB[indexWrite] = {};
            }
            ++indexWrite;
        }

        return {MKCiphertext(polynomialsInIdOrderA, allIds, level), MKCiphertext(polynomialsInIdOrderB, allIds, level)};
    }

    MKCiphertext MKEvaluator::add(MKCiphertext &cipherA, MKCiphertext &cipherB) {
        pair<MKCiphertext, MKCiphertext> transformedCiphers = getTransformedCiphers(cipherA, cipherB);
        vector<vector<Polynomial<uint64_t>>> addedPolysPerId(transformedCiphers.first.getIds().size() + 1);
        for (int id = 0; id <= transformedCiphers.first.getIds().size(); ++id) {
            int maxLevel = max(transformedCiphers.first.getCipherForIndex(id).size(),
                               transformedCiphers.second.getCipherForIndex(id).size());
            addedPolysPerId[id].resize(maxLevel);
            for (int level = 0; level < maxLevel; ++level) {
                if (level < transformedCiphers.first.getCipherForIndex(id).size() &&
                    level < transformedCiphers.second.getCipherForIndex(id).size()) {
                    addedPolysPerId[id][level] = transformedCiphers.first.getCipherForIndex(id)[level];
                    addedPolysPerId[id][level].add(transformedCiphers.second.getCipherForIndex(id)[level],
                                                   params.getPrimes()[level]);
                } else if (level < transformedCiphers.first.getCipherForIndex(id).size()) {
                    addedPolysPerId[id][level] = transformedCiphers.first.getCipherForIndex(id)[level];
                } else {
                    addedPolysPerId[id][level] = transformedCiphers.second.getCipherForIndex(id)[level];
                }
            }
        }

        return MKCiphertext(addedPolysPerId, transformedCiphers.first.getIds(), transformedCiphers.first.getLevel());
    }

    void addToVectorOrCopy(vector<Polynomial<uint64_t>> &destination, vector<Polynomial<uint64_t>> &source,
                           Parameters &params) {
        if (destination.size() == 0) {
            destination = source;
        } else {
            for (int level = 0; level < destination.size(); ++level) {
                if (level < params.getSpecialPrimesNumber()) {
                    destination[level].add(source[level], params.getSpecialPrimes()[level]);
                } else {
                    destination[level].add(source[level], params.getPrimes()[level - params.getSpecialPrimesNumber()]);
                }
            }
        }
    }

    MKCiphertext MKEvaluator::multiplyAndRelinearize(MKCiphertext &cipherA, MKCiphertext &cipherB) {
        pair<MKCiphertext, MKCiphertext> transformedCiphers = getTransformedCiphers(cipherA, cipherB);
        int howManyIds = transformedCiphers.first.getIds().size();
        vector<vector<Polynomial<uint64_t>>> cDoublePrime(howManyIds + 1);

        for (int i = 1; i <= howManyIds; ++i) {
            for (int j = 1; j <= howManyIds; ++j) {
                if (!transformedCiphers.first.getCipherForIndex(i).size() ||
                    !transformedCiphers.second.getCipherForIndex(j).size()) {
                    continue;
                }
                vector<Polynomial<uint64_t>> cij = transformedCiphers.first.getCipherForIndex(i);
                for (int level = 0; level < cij.size(); ++level) {
                    cij[level].multiply(transformedCiphers.second.getCipherForIndex(j)[level],
                                        params.getModulus(level));
                }

                vector<Polynomial<uint64_t>> cijRaised = rnsTransformer.raisePolys(cij, params.getTransformerQ(),
                                                                                   params.getTransformerP());
                vector<Polynomial<uint64_t>> cijRaisedMultBj = cijRaised;
                for (int level = 0; level < cijRaisedMultBj.size(); ++level) {
                    if (level < params.getSpecialPrimesNumber()) {
                        cijRaisedMultBj[level].multiply(
                                publicKeys[transformedCiphers.second.getIds()[j - 1]].getPolyAP(level),
                                params.getSpecialPrimes()[level]);
                    } else {
                        cijRaisedMultBj[level].multiply(publicKeys[transformedCiphers.second.getIds()[j - 1]].getPolyAQ(
                                level - params.getSpecialPrimesNumber()),
                                                        params.getPrimes()[level - params.getSpecialPrimesNumber()]);
                    }
                }

                vector<Polynomial<uint64_t>> cijPrime = rnsTransformer.downPolys(cijRaisedMultBj,
                                                                                 params.getTransformerQ(),
                                                                                 params.getTransformerP(),
                                                                                 params.getSpecialPrimesNumber());
                vector<Polynomial<uint64_t>> cijPrimeRaised = rnsTransformer.raisePolys(cijPrime,
                                                                                        params.getTransformerQ(),
                                                                                        params.getTransformerP());
                cijRaisedMultBj = cijPrimeRaised;
                cijPrime = cijPrimeRaised;

                for (int level = 0; level < cijPrimeRaised.size(); ++level) {
                    if (level < params.getSpecialPrimesNumber()) {
                        cijPrimeRaised[level].multiply(
                                evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d0P[level],
                                params.getSpecialPrimes()[level]);
                        cijRaisedMultBj[level].multiply(
                                evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d1P[level],
                                params.getSpecialPrimes()[level]);
                        cijRaised[level].multiply(evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d2P[level],
                                                  params.getSpecialPrimes()[level]);
                    } else {
                        cijPrimeRaised[level].multiply(
                                evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d0Q[level -
                                                                                             params.getSpecialPrimesNumber()],
                                params.getPrimes()[level - params.getSpecialPrimesNumber()]);
                        cijRaisedMultBj[level].multiply(
                                evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d1Q[level -
                                                                                             params.getSpecialPrimesNumber()],
                                params.getPrimes()[level - params.getSpecialPrimesNumber()]);
                        cijRaised[level].multiply(evaluationKeys[transformedCiphers.first.getIds()[i - 1]].d2Q[level -
                                                                                                               params.getSpecialPrimesNumber()],
                                                  params.getPrimes()[level - params.getSpecialPrimesNumber()]);
                    }
                }

                addToVectorOrCopy(cDoublePrime[0], cijPrimeRaised, params);
                addToVectorOrCopy(cDoublePrime[i], cijRaisedMultBj, params);
                addToVectorOrCopy(cDoublePrime[j], cijRaised, params);
            }
        }

        for (int i = 0; i <= howManyIds; ++i) {
            cDoublePrime[i] = rnsTransformer.downPolys(cDoublePrime[i], params.getTransformerQ(),
                                                       params.getTransformerP(), params.getSpecialPrimesNumber());
        }
        vector<Polynomial<uint64_t>> c00 = transformedCiphers.first.getCipherForIndex(0);
        for (int level = 0; level < c00.size(); ++level) {
            c00[level].multiply(transformedCiphers.second.getCipherForIndex(0)[level], params.getPrimes()[level]);
            cDoublePrime[0][level].add(c00[level], params.getPrimes()[level]);
        }
        for (int i = 1; i <= howManyIds; ++i) {
            vector<Polynomial<uint64_t>> c0i = transformedCiphers.first.getCipherForIndex(0);
            vector<Polynomial<uint64_t>> ci0 = transformedCiphers.first.getCipherForIndex(i);
            if (ci0.size()) {
                for (int level = 0; level < ci0.size(); ++level) {
                    ci0[level].multiply(transformedCiphers.second.getCipherForIndex(0)[level],
                                        params.getPrimes()[level]);
                    cDoublePrime[i][level].add(ci0[level], params.getPrimes()[level]);
                }
            }

            if (transformedCiphers.second.getCipherForIndex(i).size()) {
                for (int level = 0; level < c0i.size(); ++level) {
                    c0i[level].multiply(transformedCiphers.second.getCipherForIndex(i)[level],
                                        params.getPrimes()[level]);
                    cDoublePrime[i][level].add(c0i[level], params.getPrimes()[level]);
                }
            }
        }

        return MKCiphertext(cDoublePrime, transformedCiphers.first.getIds(), transformedCiphers.first.getLevel());
    }

    void MKEvaluator::rescaleInPlace(MKCiphertext &cipher) {
        if (cipher.getLevel() < 1) {
            throw ("There are no levels remaining for rescaling.");
        }
        for (int index = 0; index <= cipher.getIds().size(); ++index) {
            params.getTransformerQ().fromNTT(cipher.getCipherForIndex(index).back(), cipher.getLevel());

            for (int i = 0; i <= cipher.getLevel() - 1; ++i) {
                uint64_t qinv = fastExp(params.getPrimes()[cipher.getLevel()], params.getPrimes()[i] - 2,
                                        params.getPrimes()[i]);
                Polynomial<uint64_t> polyASub = cipher.getCipherForIndex(index).back();
                params.getTransformerQ().toNTT(polyASub, i);
                cipher.getCipherForIndex(index)[i].sub(polyASub, params.getPrimes()[i]);
                cipher.getCipherForIndex(index)[i].multiply(qinv, params.getPrimes()[i]);
            }
        }
        cipher.decreaseLevel(1);
    }

    void MKEvaluator::addPlainInPlace(MKCiphertext &cipher, Plaintext &plain) {
        int minLevel = min(cipher.getLevel(), plain.getLevel());

        cipher.decreaseLevel(cipher.getLevel() - minLevel);
        plain.decreaseLevel(plain.getLevel() - minLevel);

        for (int level = 0; level <= cipher.getLevel(); ++level) {
            params.getTransformerQ().toNTT(plain.getPolynomial(level), level);
            cipher.getCipherForIndex(0)[level].add(plain.getPolynomial(level), params.getPrimes()[level]);
            params.getTransformerQ().fromNTT(plain.getPolynomial(level), level);
        }
    }

    void MKEvaluator::multiplyPlainInPlace(MKCiphertext &cipher, Plaintext &plain) {
        int minLevel = min(cipher.getLevel(), plain.getLevel());
        if (minLevel == 0) {
            throw ("There are no levels remaining for relinearization");
        }

        cipher.decreaseLevel(cipher.getLevel() - minLevel);
        plain.decreaseLevel(plain.getLevel() - minLevel);
        for (int level = 0; level <= cipher.getLevel(); ++level) {
            params.getTransformerQ().toNTT(plain.getPolynomial(level), level);
            for (int index = 0; index <= cipher.getIds().size(); ++index) {
                cipher.getCipherForIndex(index)[level].multiply(plain.getPolynomial(level), params.getPrimes()[level]);
            }
            params.getTransformerQ().fromNTT(plain.getPolynomial(level), level);
        }
    }

    void MKEvaluator::negateInPlace(MKCiphertext &cipher) {
        for (int level = 0; level <= cipher.getLevel(); ++level) {
            for (int index = 0; index <= cipher.getIds().size(); ++index) {
                cipher.getCipherForIndex(index)[level].negate(params.getPrimes()[level]);
            }
        }
    }
}
