#include "Distribution.h"
#include "Distribution_MeanDev.h"

Distribution_MeanDev::Distribution_MeanDev(const std::string &id,
                                           double mean, double deviation)
    : Distribution(id), myMeanValue(mean), myDeviation(deviation)
{
}


Distribution_MeanDev::~Distribution_MeanDev()
{
}

