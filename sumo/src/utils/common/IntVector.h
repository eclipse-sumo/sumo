#ifndef IntVector_h
#define IntVector_h

#include <vector>

typedef std::vector<int> IntVector;

class IntVectorHelper {
public:
    static void removeDouble(IntVector &v);

    /// Returns the information whether at least one element is within both vectors
    static bool subSetExists(const IntVector &v1, const IntVector &v2);

};


#endif


