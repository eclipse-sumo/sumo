#include <vector>
#include "IntVector.h"

void
IntVectorHelper::removeDouble(IntVector &v)
{
    size_t pos = 0;
    IntVector::iterator i=v.begin();
    while(i!=v.end()) {
        int no1 = *i;
        bool changed = false;
        for(IntVector::iterator j=i+1; j!=v.end()&&!changed; j++) {
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



