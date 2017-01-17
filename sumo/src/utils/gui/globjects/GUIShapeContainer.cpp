/****************************************************************************/
/// @file    GUIShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    08.10.2009
/// @version $Id$
///
// Storage for geometrical objects extended by mutexes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include "GUIShapeContainer.h"
#include <foreign/rtree/SUMORTree.h>
#include <utils/gui/globjects/GUIPolygon.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/Polygon.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIShapeContainer::GUIShapeContainer(SUMORTree& vis)
    : myVis(vis) {}


GUIShapeContainer::~GUIShapeContainer() {}


bool
GUIShapeContainer::addPOI(const std::string& id, const std::string& type,
                          const RGBColor& color, SUMOReal layer, SUMOReal angle, const std::string& imgFile,
                          const Position& pos, SUMOReal width, SUMOReal height, bool /* ignorePruning */) {
    GUIPointOfInterest* p = new GUIPointOfInterest(id, type, color, pos, layer, angle, imgFile, width, height);
    AbstractMutex::ScopedLocker locker(myLock);
    if (!myPOIs.add(id, p)) {
        delete p;
        return false;
    } else {
        myVis.addAdditionalGLObject(p);
        return true;
    }
}


bool
GUIShapeContainer::addPolygon(const std::string& id, const std::string& type,
                              const RGBColor& color, SUMOReal layer,
                              SUMOReal angle, const std::string& imgFile,
                              const PositionVector& shape, bool fill, bool /* ignorePruning */) {
    GUIPolygon* p = new GUIPolygon(id, type, color, shape, fill, layer, angle, imgFile);
    AbstractMutex::ScopedLocker locker(myLock);
    if (!myPolygons.add(id, p)) {
        delete p;
        return false;
    } else {
        myVis.addAdditionalGLObject(p);
        return true;
    }
}


bool
GUIShapeContainer::removePolygon(const std::string& id) {
    AbstractMutex::ScopedLocker locker(myLock);
    GUIPolygon* p = dynamic_cast<GUIPolygon*>(myPolygons.get(id));
    if (p == 0) {
        return false;
    }
    myVis.removeAdditionalGLObject(p);
    return myPolygons.remove(id);
}


bool
GUIShapeContainer::removePOI(const std::string& id) {
    AbstractMutex::ScopedLocker locker(myLock);
    GUIPointOfInterest* p = dynamic_cast<GUIPointOfInterest*>(myPOIs.get(id));
    if (p == 0) {
        return false;
    }
    myVis.removeAdditionalGLObject(p);
    return myPOIs.remove(id);
}


void
GUIShapeContainer::movePOI(const std::string& id, const Position& pos) {
    AbstractMutex::ScopedLocker locker(myLock);
    GUIPointOfInterest* p = dynamic_cast<GUIPointOfInterest*>(myPOIs.get(id));
    if (p != 0) {
        myVis.removeAdditionalGLObject(p);
        static_cast<Position*>(p)->set(pos);
        myVis.addAdditionalGLObject(p);
    }
}


void
GUIShapeContainer::reshapePolygon(const std::string& id, const PositionVector& shape) {
    AbstractMutex::ScopedLocker locker(myLock);
    GUIPolygon* p = dynamic_cast<GUIPolygon*>(myPolygons.get(id));
    if (p != 0) {
        myVis.removeAdditionalGLObject(p);
        p->setShape(shape);
        myVis.addAdditionalGLObject(p);
    }
}



std::vector<GUIGlID>
GUIShapeContainer::getPOIIds() const {
    AbstractMutex::ScopedLocker locker(myLock);
    std::vector<GUIGlID> ret;
    const std::map<std::string, PointOfInterest*>& pois = getPOIs().getMyMap();
    for (std::map<std::string, PointOfInterest*>::const_iterator it = pois.begin(); it != pois.end(); ++it) {
        ret.push_back(static_cast<GUIPointOfInterest*>(it->second)->getGlID());
    }
    return ret;
}


std::vector<GUIGlID>
GUIShapeContainer::getPolygonIDs() const {
    AbstractMutex::ScopedLocker locker(myLock);
    std::vector<GUIGlID> ret;
    const std::map<std::string, SUMO::Polygon*>& polygons = getPolygons().getMyMap();
    for (std::map<std::string, SUMO::Polygon*>::const_iterator it = polygons.begin(); it != polygons.end(); ++it) {
        ret.push_back(static_cast<GUIPolygon*>(it->second)->getGlID());
    }
    return ret;
}

/****************************************************************************/

