#include <vector>
#include <algorithm>
#include "IntVector.h"

using namespace std;

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


bool
IntVectorHelper::subSetExists(const IntVector &v1, const IntVector &v2)
{
    for(IntVector::const_iterator i=v1.begin(); i!=v1.end(); i++) {
        int val1 = (*i);
		if(find(v2.begin(), v2.end(), val1)!=v2.end()) {
			return true;
		}
    }
    return false;
}

