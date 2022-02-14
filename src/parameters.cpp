#include "smkhe/parameters.h"

Parameters::Parameters(uint64_t scale, int ringDegree, vector<uint64_t> qPrimes, vector<uint64_t> pPrimes) : scale(scale), ringDegree(ringDegree),
                                                                                                             qPrimes(qPrimes), pPrimes(pPrimes),
                                                                                                             transformerQ(ringDegree, qPrimes),
                                                                                                             transformerP(ringDegree, pPrimes) {}

uint64_t Parameters::getScale() {
    return this->scale;
}

void Parameters::setScale(uint64_t scale) {
    this->scale = scale;
}

int Parameters::getRingDegree() {
    return this->ringDegree;
}

void Parameters::setRingDegree(int ringDegree) {
    this->ringDegree = ringDegree;
}

bool Parameters::operator==(const Parameters other) {
    return this->ringDegree == other.ringDegree && this->scale == other.scale;
}

int Parameters::getModulusLevels() {
    return qPrimes.size();
}

uint64_t Parameters::getModulus(int level) {
    return qPrimes[level];
}

vector<uint64_t> &Parameters::getPrimes() {
    return qPrimes;
}

NTTTransformer &Parameters::getTransformerQ() {
    return transformerQ;
}

NTTTransformer &Parameters::getTransformerP() {
    return transformerP;
}

vector<uint64_t> &Parameters::getSpecialPrimes() {
    return pPrimes;
}

int Parameters::getSpecialPrimesNumber() {
    return pPrimes.size();
}
