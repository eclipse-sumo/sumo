#ifndef GUIDetectorWrapper_h
#define GUIDetectorWrapper_h

#include <utils/geom/HaveBoundery.h>
#include <gui/GUIGlObject.h>

class GUIDetectorWrapper
    : public HaveBoundery, public GUIGlObject {
public:
    GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
        std::string id);
    ~GUIDetectorWrapper();
    virtual Boundery getBoundery() const = 0;
    virtual void drawGL(double scale) const = 0;
};

#endif
