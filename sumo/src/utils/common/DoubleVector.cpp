
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



void
DoubleVectorHelper::removeDouble(DoubleVector &v)
{
    size_t pos = 0;
    DoubleVector::iterator i=v.begin();
    while(i!=v.end()) {
        int no1 = *i;
        bool changed = false;
        for(DoubleVector::iterator j=i+1; j!=v.end()&&!changed; j++) {
            int no2 = *j;
            if(no1==no2) {
                v.erase(j);
                changed = true;
            }
        }
        if(changed) {
            i = v.begin() + pos;
        } else {
            i++;
            pos++;
        }
    }
}

