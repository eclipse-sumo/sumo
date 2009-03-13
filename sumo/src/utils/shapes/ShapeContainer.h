/****************************************************************************/
/// @file    ShapeContainer.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ShapeContainer_h
#define ShapeContainer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/NamedObjectCont.h>
#include <utils/shapes/Polygon2D.h>
#include "PointOfInterest.h"

class ShapeContainer {
public:
    ShapeContainer();
    virtual ~ShapeContainer();

    bool add(int layer, Polygon2D *p);

    bool add(int layer, PointOfInterest *p);

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

/****************************************************************************/

