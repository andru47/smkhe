#include "smkhe/parameters.h"

Parameters::Parameters(double scale, int ringDegree, vector<uint64_t> primes) : scale(scale), ringDegree(ringDegree),
                                                                                primes(primes), transformer(ringDegree, primes) {}

double Parameters::getScale() {
    return this->scale;
}

void Parameters::setScale(double scale) {
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
    return primes.size();
}

uint64_t Parameters::getModulus(int level) {
    return primes[level];
}

vector<uint64_t>& Parameters::getPrimes() {
    return primes;
}

NTTTransformer& Parameters::getTransformer() {
    return transformer;
}
