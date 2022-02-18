#include "smkhe/math/rns_converter.h"

namespace smkhe {
    RNSConverter::RNSConverter(Parameters &params) : params(params), qPerLevel(params.getModulusLevels()),
                                                     crtTransformation(params.getModulusLevels(),
                                                                       vector<BigInt>(params.getModulusLevels())) {
        qPerLevel[0] = params.getModulus(0);
        for (int level = 1; level < params.getModulusLevels(); ++level) {
            qPerLevel[level] = qPerLevel[level - 1] * params.getModulus(level);
        }
        for (int primaryLevel = 0; primaryLevel < params.getModulusLevels(); ++primaryLevel) {
            for (int currentLevel = 0; currentLevel <= primaryLevel; ++currentLevel) {
                BigInt qHat = qPerLevel[primaryLevel] / params.getModulus(currentLevel);
                uint64_t qHatModulo = fastExp((qHat % params.getModulus(currentLevel)).to_long_long(),
                                              params.getModulus(currentLevel) - 2, params.getModulus(currentLevel));
                crtTransformation[primaryLevel][currentLevel] = qHatModulo;
                crtTransformation[primaryLevel][currentLevel] *= qHat;
            }
        }
    }

    void RNSConverter::scaleDownFromCRT(vector<vector<uint64_t>> &coeffsInCRT, vector<complex<double>> &destination) {
        int levels = coeffsInCRT.size();
        BigInt qHalf = qPerLevel[levels - 1] / 2;
        for (int i = 0; i < destination.size(); ++i) {
            BigInt bigIntCoeff = 0;
            for (int level = 0; level < levels; ++level) {
                bigIntCoeff += crtTransformation[levels - 1][level] * coeffsInCRT[level][i];
            }
            bigIntCoeff %= qPerLevel[levels - 1];
            bool sign = false;
            if (bigIntCoeff >= qHalf) {
                sign = true;
                bigIntCoeff = qPerLevel[levels - 1] - bigIntCoeff;
            }

            uint64_t quotient = (bigIntCoeff / params.getScale()).to_long_long();
            uint64_t remainder = (bigIntCoeff % params.getScale()).to_long_long();
            double realPart = (remainder * 1.0) / (1.0 * params.getScale());
            destination[i] = sign ? -(quotient + realPart) : (quotient + realPart);
        }
    }
}
