#ifndef GUIEmitterWrapper_h
#define GUIEmitterWrapper_h

#include <utils/geom/HaveBoundery.h>

class GUIEmitterWrapper
    : public HaveBoundery {
public:
    GUIEmitterWrapper() { }
    ~GUIEmitterWrapper() { }
    Boundery getBoundery() const;
};

#endif
