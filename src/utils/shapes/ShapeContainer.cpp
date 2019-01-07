/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ShapeContainer.cpp
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Storage for geometrical objects, sorted by the layers they are in
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <string>
#include <cmath>
#include <utils/common/NamedObjectCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/StdDefs.h>
#include "ShapeContainer.h"


// ===========================================================================
// method definitions
// ===========================================================================
ShapeContainer::ShapeContainer() {}

ShapeContainer::~ShapeContainer() {}

bool
ShapeContainer::addPolygon(const std::string& id, const std::string& type,
                           const RGBColor& color, double layer,
                           double angle, const std::string& imgFile, bool relativePath,
                           const PositionVector& shape, bool geo, bool fill, double lineWidth, bool ignorePruning) {
    return add(new SUMOPolygon(id, type, color, shape, geo, fill, lineWidth, layer, angle, imgFile, relativePath), ignorePruning);
}


bool
ShapeContainer::addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
                       const std::string& lane, double posOverLane, double posLat, double layer, double angle,
                       const std::string& imgFile, bool relativePath, double width, double height, bool ignorePruning) {
    return add(new PointOfInterest(id, type, color, pos, geo, lane, posOverLane, posLat, layer, angle, imgFile, relativePath, width, height), ignorePruning);
}


bool
ShapeContainer::removePolygon(const std::string& id) {
    return myPolygons.remove(id);
}


bool
ShapeContainer::removePOI(const std::string& id) {
    return myPOIs.remove(id);
}


void
ShapeContainer::movePOI(const std::string& id, const Position& pos) {
    PointOfInterest* p = myPOIs.get(id);
    if (p != nullptr) {
        static_cast<Position*>(p)->set(pos);
    }
}


void
ShapeContainer::reshapePolygon(const std::string& id, const PositionVector& shape) {
    SUMOPolygon* p = myPolygons.get(id);
    if (p != nullptr) {
        p->setShape(shape);
    }
}


bool
ShapeContainer::add(SUMOPolygon* poly, bool /* ignorePruning */) {
    if (!myPolygons.add(poly->getID(), poly)) {
        delete poly;
        return false;
    }
    return true;
}


bool
ShapeContainer::add(PointOfInterest* poi, bool /* ignorePruning */) {
    if (!myPOIs.add(poi->getID(), poi)) {
        delete poi;
        return false;
    }
    return true;
}


/****************************************************************************/

