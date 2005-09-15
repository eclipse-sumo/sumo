#ifndef MMLayer_h
#define MMLayer_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <utils/common/StdDefs.h>

class MMLayer {
public:
    MMLayer() { }
    ~MMLayer() { }
    virtual void simulationStep( SUMOTime start, SUMOTime step) = 0;

};

#endif
