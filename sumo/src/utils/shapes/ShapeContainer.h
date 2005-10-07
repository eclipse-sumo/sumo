#ifndef ShapeContainer_h
#define ShapeContainer_h

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/helpers/NamedObjectCont.h>
#include <utils/shapes/Polygon2D.h>
#include "PointOfInterest.h"

class ShapeContainer {
public:
    ShapeContainer() { }
    virtual ~ShapeContainer()
    {
    }

    bool add(Polygon2D *p) {
        return myPolygons.add(p->getName(), p);
    }

    bool add(PointOfInterest *p) {
        return myPOIs.add(p->getID(), p);
    }

    const NamedObjectCont<Polygon2D*> &getPolygonCont() const { return myPolygons; }
    const NamedObjectCont<PointOfInterest*> &getPOICont() const { return myPOIs; }

private:
    NamedObjectCont<Polygon2D*> myPolygons;
    NamedObjectCont<PointOfInterest*> myPOIs;


};

#endif
