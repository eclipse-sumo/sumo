
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
    if(msum==0) {
        // is an error; do nothing
        return;
    }
    double rsum = sum(dv);
    if(rsum==0) {
        set(dv, 1.0/msum/(double) dv.size());
        return;
    }
    div(dv, rsum/msum);
}


void
DoubleVectorHelper::div(DoubleVector &dv, double by)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i /= by;
    }
}

void
DoubleVectorHelper::set(DoubleVector &dv, double to)
{
    for(DoubleVector::iterator i=dv.begin(); i!=dv.end(); i++) {
        *i = to;
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

