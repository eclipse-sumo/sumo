#ifndef GUIEmitterWrapper_h
#define GUIEmitterWrapper_h

#include <utils/geom/HaveBoundery.h>
#include <utils/geom/Position2D.h>

class GUIEmitterWrapper
    : public HaveBoundery {
public:
    GUIEmitterWrapper();
    ~GUIEmitterWrapper();
    Boundery getBoundery() const;
    virtual Position2D getPosition() const = 0;
};

#endif
