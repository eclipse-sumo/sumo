
#include "DoubleVector.h"

double 
DoubleVectorHelper::sum(const DoubleVector &dv)
{
    double sum = 0;
    for(DoubleVector::const_iterator i=dv.begin(); i!=dv.end(); i++) {
        sum += *i;
    }
    return sum;
}


void 
DoubleVectorHelper::normalise(DoubleVector &dv, double msum)
{
    double rsum = sum(dv);
    div(dv, msum/rsum);
}


void 
DoubleVectorHelper::div(DoubleVector &dv, double by)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i /= by;
    }
}
