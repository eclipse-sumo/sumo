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
    ShapeContainer();
    virtual ~ShapeContainer();

    bool add(Polygon2D *p);

    bool add(PointOfInterest *p);

	bool save(const std::string &file);

     NamedObjectCont<Polygon2D*> &getPolygonCont() const;
     NamedObjectCont<PointOfInterest*> &getPOICont() const;

private:
    mutable NamedObjectCont<Polygon2D*> myPolygons;
    mutable NamedObjectCont<PointOfInterest*> myPOIs;


};

#endif
