#ifndef GUIDetectorWrapper_h
#define GUIDetectorWrapper_h

#include <utils/geom/HaveBoundery.h>

class GUIDetectorWrapper
    : public HaveBoundery {
public:
    GUIDetectorWrapper() { }
    ~GUIDetectorWrapper() { }
    Boundery getBoundery() const;
};

#endif
