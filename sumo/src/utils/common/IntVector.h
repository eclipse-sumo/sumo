#ifndef IntVector_h
#define IntVector_h

#include <vector>

typedef std::vector<int> IntVector;

class IntVectorHelper {
public:
    static void removeDouble(IntVector &v);
};


#endif


