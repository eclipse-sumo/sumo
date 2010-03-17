/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer() throw()
        : myMinLayer(100), myMaxLayer(-100) {}


ShapeContainer::~ShapeContainer() throw() {}


bool
ShapeContainer::add(int layer, Polygon2D *p) throw() {
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer].add(p->getID(), p);
}


bool
ShapeContainer::add(int layer, PointOfInterest *p) throw() {
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer].add(p->getID(), p);
}


bool
ShapeContainer::removePolygon(int layer, const std::string &id) throw() {
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        return false;
    }
    return myPolygonLayers.find(layer)->second.remove(id);
}


bool
ShapeContainer::removePOI(int layer, const std::string &id) throw() {
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        return false;
    }
    return myPOILayers.find(layer)->second.remove(id);
}


const NamedObjectCont<Polygon2D*> &
ShapeContainer::getPolygonCont(int layer) const throw() {
    if (myPolygonLayers.find(layer)==myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon2D*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer];
}


const NamedObjectCont<PointOfInterest*> &
ShapeContainer::getPOICont(int layer) const throw() {
    if (myPOILayers.find(layer)==myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer];
}



/****************************************************************************/

