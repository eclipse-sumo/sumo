#ifndef GUIAddWeightsStorage_h
#define GUIAddWeightsStorage_h

#include <map>
#include <vector>

struct GUIAddWeight {
    std::string edgeID;
    float abs;
    float add;
    float mult;
    size_t timeBeg;
    size_t timeEnd;
};

typedef std::vector<GUIAddWeight> GUIAddWeightsStorage;


#endif
