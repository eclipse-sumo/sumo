/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Polygon.cpp
/// @author  Gregor L\"ammel
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <microsim/MSNet.h>
#include <utils/shapes/SUMOPolygon.h>
#include <utils/shapes/ShapeContainer.h>

#include "Polygon.h"
#include "Helper.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string> Polygon::getIDList() {
    std::vector<std::string> ids;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.getPolygons().insertIDs(ids);
    return ids;
}


std::string Polygon::getType(const std::string& polygonID) {
    return getPolygon(polygonID)->getType();
}


TraCIPositionVector Polygon::getShape(const std::string& polygonID) {
    SUMOPolygon* p = getPolygon(polygonID);
    return Helper::makeTraCIPositionVector(p->getShape());
}


bool Polygon::getFilled(const std::string& polygonID) {
    return getPolygon(polygonID)->getFill();
}


TraCIColor Polygon::getColor(const std::string& polygonID) {
    SUMOPolygon* p = getPolygon(polygonID);
    return Helper::makeTraCIColor(p->getColor());
}


std::string Polygon::getParameter(const std::string& polygonID, const std::string& paramName) {
    return getPolygon(polygonID)->getParameter(paramName, "");
}


void Polygon::setType(const std::string& polygonID, const std::string& setType) {
    SUMOPolygon* p = getPolygon(polygonID);
    p->setType(setType);
}


void Polygon::setShape(const std::string& polygonID, const TraCIPositionVector& shape) {
    PositionVector positionVector = Helper::makePositionVector(shape);
    getPolygon(polygonID); // just to check whether it exists
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.reshapePolygon(polygonID, positionVector);
}


void Polygon::setColor(const std::string& polygonID, const TraCIColor& c) {
    getPolygon(polygonID)->setColor(Helper::makeRGBColor(c));
}


void
Polygon::add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    PositionVector pShape = Helper::makePositionVector(shape);
    RGBColor col = Helper::makeRGBColor(c);
    if (!shapeCont.addPolygon(polygonID, type, col, (double)layer, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, pShape, false, fill)) {
        throw TraCIException("Could not add polygon '" + polygonID + "'");
    }
}


void Polygon::remove(const std::string& polygonID, int /* layer */) {
    // !!! layer not used yet (shouldn't the id be enough?)
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (!shapeCont.removePolygon(polygonID)) {
        throw TraCIException("Could not remove polygon '" + polygonID + "'");
    }
}


void
Polygon::setFilled(std::string polygonID, bool filled) {
    SUMOPolygon* p = getPolygon(polygonID);
    p->setFill(filled);
}


SUMOPolygon*
Polygon::getPolygon(const std::string& id) {
    SUMOPolygon* p = MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
    if (p == 0) {
        throw TraCIException("Polygon '" + id + "' is not known");
    }
    return p;
}


void
Polygon::setParameter(std::string& id, std::string& name, std::string& value) {
    SUMOPolygon* p = getPolygon(id);
    p->setParameter(name, value);
}

NamedRTree*
Polygon::getTree() {
    NamedRTree* t = new NamedRTree();
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    for (const auto& i : shapeCont.getPolygons()) {
        Boundary b = i.second->getShape().getBoxBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        t->Insert(cmin, cmax, i.second);
    }
    return t;
}

}


/****************************************************************************/
