#ifndef DoubleVector_h
#define DoubleVector_h

#include <vector>

typedef std::vector<double> DoubleVector;

class DoubleVectorHelper {
public:
    static double sum(const DoubleVector &dv);
    static void normalise(DoubleVector &dv, double sum=1.0);
    static void div(DoubleVector &dv, double by);
};

#endif


