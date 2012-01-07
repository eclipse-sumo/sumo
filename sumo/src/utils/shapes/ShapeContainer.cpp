/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StdDefs.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer()
    : myMinLayer(100), myMaxLayer(-100) {}


ShapeContainer::~ShapeContainer() {}


bool
ShapeContainer::addPoI(const std::string& name, int layer, const std::string& type, const RGBColor& c,
                       const Position& pos) {
    PointOfInterest* p = new PointOfInterest(name, type, pos, c);
    if (!add(layer, p)) {
        delete p;
        return false;
    }
    return true;
}


bool
ShapeContainer::addPolygon(const std::string& name, int layer, const std::string& type, const RGBColor& c,
                           bool filled, const PositionVector& shape) {
    Polygon* p = new Polygon(name, type, c, shape, filled);
    if (!add(layer, p)) {
        delete p;
        return false;
    }
    return true;
}



bool
ShapeContainer::removePolygon(int layer, const std::string& id) {
    if (myPolygonLayers.find(layer) == myPolygonLayers.end()) {
        return false;
    }
    return myPolygonLayers.find(layer)->second.remove(id);
}


bool
ShapeContainer::removePoI(int layer, const std::string& id) {
    if (myPOILayers.find(layer) == myPOILayers.end()) {
        return false;
    }
    return myPOILayers.find(layer)->second.remove(id);
}



void
ShapeContainer::movePoI(int layer, const std::string& id, const Position& pos) {
    if (myPOILayers.find(layer) != myPOILayers.end()) {
        PointOfInterest* p = myPOILayers.find(layer)->second.get(id);
        if (p != 0) {
            static_cast<Position*>(p)->set(pos);
        }
    }
}


void
ShapeContainer::reshapePolygon(int layer, const std::string& id, const PositionVector& shape) {
    if (myPolygonLayers.find(layer) != myPolygonLayers.end()) {
        Polygon* p = myPolygonLayers.find(layer)->second.get(id);
        if (p != 0) {
            p->setShape(shape);
        }
    }
}



const NamedObjectCont<Polygon*> &
ShapeContainer::getPolygonCont(int layer) const {
    if (myPolygonLayers.find(layer) == myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer];
}


const NamedObjectCont<PointOfInterest*> &
ShapeContainer::getPOICont(int layer) const {
    if (myPOILayers.find(layer) == myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer];
}



bool
ShapeContainer::add(int layer, Polygon* p) {
    if (myPolygonLayers.find(layer) == myPolygonLayers.end()) {
        myPolygonLayers[layer] = NamedObjectCont<Polygon*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPolygonLayers[layer].add(p->getID(), p);
}


bool
ShapeContainer::add(int layer, PointOfInterest* p) {
    if (myPOILayers.find(layer) == myPOILayers.end()) {
        myPOILayers[layer] = NamedObjectCont<PointOfInterest*>();
        myMinLayer = MIN2(layer, myMinLayer);
        myMaxLayer = MAX2(layer, myMaxLayer);
    }
    return myPOILayers[layer].add(p->getID(), p);
}



/****************************************************************************/

