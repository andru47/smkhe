#include <vector>

using namespace std;
#ifndef SMKHE_PARAMETERS_H
#define SMKHE_PARAMETERS_H


class Parameters {
    double scale;
    int ringDegree;
    vector<uint64_t> primes;

public:
    Parameters(double scale, int ringDegree, vector<uint64_t> primes);

    double getScale();

    void setScale(double scale);

    int getRingDegree();

    int getModulusLevels();

    uint64_t getModulus(int level);

    void setRingDegree(int ringDegree);

    bool operator==(const Parameters);

    vector<uint64_t> getPrimes();
};

#endif //SMKHE_PARAMETERS_H
