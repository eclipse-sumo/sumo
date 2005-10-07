#ifndef MMNet_h
#define MMNet_h

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/StdDefs.h>
#include "MMLayer.h"

class MMNet {
public:
    MMNet() { }

    ~MMNet() { }

    void addLayer(MMLayer *l) {
        myLayers.push_back(l);
    }

    /// performs a single simulation step
    void simulationStep( SUMOTime start, SUMOTime step) {
        for(std::vector<MMLayer*>::iterator i=myLayers.begin(); i!=myLayers.end(); ++i) {
            (*i)->simulationStep(start, step);
        }
    }

protected:
    std::vector<MMLayer*> myLayers;

};

#endif
