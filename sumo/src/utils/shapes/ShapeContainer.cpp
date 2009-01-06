/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A view on the simulation; this view is a microscopic one
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <string>
#include <cmath>
#include <utils/common/NamedObjectCont.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StdDefs.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer()
        : myCurrentLayer(-10), myMinLayer(100), myMaxLayer(-100)
{}


ShapeContainer::~ShapeContainer()
{}


bool
ShapeContainer::add(int layer, Polygon2D *p)
{
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer].add(p->getName(), p);
}


bool
ShapeContainer::add(int layer, PointOfInterest *p)
{
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer].add(p->getID(), p);
}


NamedObjectCont<Polygon2D*> &
ShapeContainer::getPolygonCont(int layer) const
{
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer];
}


NamedObjectCont<PointOfInterest*> &
ShapeContainer::getPOICont(int layer) const
{
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer];
}


int
ShapeContainer::getMinLayer() const
{
    return myMinLayer;
}


int
ShapeContainer::getMaxLayer() const
{
    return myMaxLayer;
}



/****************************************************************************/

