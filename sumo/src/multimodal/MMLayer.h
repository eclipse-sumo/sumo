#ifndef MMLayer_h
#define MMLayer_h

#include <utils/common/StdDefs.h>

class MMLayer {
public:
    MMLayer() { }
    ~MMLayer() { }
    virtual void simulationStep( SUMOTime start, SUMOTime step) = 0;

};

#endif
