#ifndef ShapeContainer_h
#define ShapeContainer_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

    bool add(int layer, Polygon2D *p);

    bool add(int layer, PointOfInterest *p);

	bool save(const std::string &file);

    NamedObjectCont<Polygon2D*> &getPolygonCont(int layer) const;
    NamedObjectCont<PointOfInterest*> &getPOICont(int layer) const;

    int getMinLayer() const;
    int getMaxLayer() const;


private:
    mutable std::map<int, NamedObjectCont<Polygon2D*> > myPolygonLayers;
    mutable std::map<int, NamedObjectCont<PointOfInterest*> > myPOILayers;
    int myCurrentLayer;
    mutable int myMinLayer, myMaxLayer;

};

#endif
