/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    POI.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <microsim/MSNet.h>
#include "POI.h"
#include "Helper.h"




// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
POI::getIDList() {
    std::vector<std::string> ids;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.getPOIs().insertIDs(ids);
    return ids;
}

int
POI::getIDCount() {
    return (int)getIDList().size();
}

std::string
POI::getType(const std::string& poiID) {
    return getPoI(poiID)->getType();
}

TraCIColor
POI::getColor(const std::string& poiID) {
    PointOfInterest* sumoPoi = getPoI(poiID);
    RGBColor col = sumoPoi->getColor();
    return Helper::makeTraCIColor(col);
}

TraCIPosition
POI::getPosition(const std::string& poiID) {
    TraCIPosition pos;
    PointOfInterest* p = getPoI(poiID);
    pos.x = p->x();
    pos.y = p->y();
    pos.z = p->z();
    return pos;
}

std::string
POI::getParameter(const std::string& poiID, const std::string& param) {
    PointOfInterest* p = getPoI(poiID);
    return p->getParameter(param, "");
}

void
POI::setType(const std::string& poiID, const std::string& type) {
    PointOfInterest* p = getPoI(poiID);
    p->setType(type);
}

void
POI::setPosition(const std::string& poiID, const TraCIPosition& pos) {
    PointOfInterest* p = getPoI(poiID);
    p->set(Helper::makePosition(pos));
}

void
POI::setColor(const std::string& poiID, const TraCIColor& c) {
    PointOfInterest* p = getPoI(poiID);
    p->setColor(Helper::makeRGBColor(c));
}

bool
POI::add(const std::string& poiID, const TraCIPosition& pos, const TraCIColor& c, const std::string& type, int layer) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    return shapeCont.addPOI(poiID, type, Helper::makeRGBColor(c), Helper::makePosition(pos), false, "", 0, 0, (double)layer,
                            Shape::DEFAULT_ANGLE,
                            Shape::DEFAULT_IMG_FILE,
                            Shape::DEFAULT_IMG_WIDTH,
                            Shape::DEFAULT_IMG_HEIGHT);
}

bool
POI::remove(const std::string& poiID, int /* layer */) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    return shapeCont.removePOI(poiID);
}

void
POI::setParameter(const std::string& poiID, const std::string& param, const std::string& value) {
    PointOfInterest* p = getPoI(poiID);
    p->setParameter(param, value);
}


PointOfInterest*
POI::getPoI(const std::string& id) {
    PointOfInterest* sumoPoi = MSNet::getInstance()->getShapeContainer().getPOIs().get(id);
    if (sumoPoi == 0) {
        throw TraCIException("POI '" + id + "' is not known");
    }
    return sumoPoi;
}


NamedRTree*
POI::getTree() {
    NamedRTree* t = new NamedRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    for (const auto& i : shapeCont.getPOIs()) {
        const float cmin[2] = {(float)i.second->x(), (float)i.second->y()};
        const float cmax[2] = {(float)i.second->x(), (float)i.second->y()};
        t->Insert(cmin, cmax, i.second);
    }
    return t;
}


}


/****************************************************************************/
