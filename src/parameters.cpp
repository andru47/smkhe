#include "smkhe/parameters.h"

Parameters::Parameters(double scale, int ringDegree) : scale(scale), ringDegree(ringDegree) {}

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
