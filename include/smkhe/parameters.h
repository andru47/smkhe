#ifndef SMKHE_PARAMETERS_H
#define SMKHE_PARAMETERS_H


class Parameters {
    double scale;
    int ringDegree;
public:
    Parameters(double scale, int ringDegree);

    double getScale();

    void setScale(double scale);

    int getRingDegree();

    void setRingDegree(int ringDegree);

    bool operator==(const Parameters);
};


#endif //SMKHE_PARAMETERS_H
