#ifndef Distribution_MeanDev_h
#define Distribution_MeanDev_h

#include "Distribution.h"

class Distribution_MeanDev :
        public Distribution {
public:
    Distribution_MeanDev(const std::string &id, double mean,
        double deviation);
    virtual ~Distribution_MeanDev();
private:
    double myMeanValue;
    double myDeviation;
};

#endif
