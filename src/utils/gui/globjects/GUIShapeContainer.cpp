/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// method definitions
// ===========================================================================
GUIShapeContainer::GUIShapeContainer(SUMORTree& vis)
    : myVis(vis) {}


GUIShapeContainer::~GUIShapeContainer() {}


bool
GUIShapeContainer::addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
                          const std::string& lane, double posOverLane, double posLat, double layer, double angle,
                          const std::string& imgFile, double width, double height, bool /* ignorePruning */) {
    GUIPointOfInterest* p = new GUIPointOfInterest(id, type, color, pos, geo, lane, posOverLane, posLat, layer, angle, imgFile, width, height);
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
                              const RGBColor& color, double layer,
                              double angle, const std::string& imgFile,
                              const PositionVector& shape, bool geo, bool fill, bool /* ignorePruning */) {
    GUIPolygon* p = new GUIPolygon(id, type, color, shape, geo, fill, layer, angle, imgFile);
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
    for (const auto& poi : getPOIs()) {
        ret.push_back(static_cast<GUIPointOfInterest*>(poi.second)->getGlID());
    }
    return ret;
}


std::vector<GUIGlID>
GUIShapeContainer::getPolygonIDs() const {
    AbstractMutex::ScopedLocker locker(myLock);
    std::vector<GUIGlID> ret;
    for (const auto& poly : getPolygons()) {
        ret.push_back(static_cast<GUIPolygon*>(poly.second)->getGlID());
    }
    return ret;
}

/****************************************************************************/

