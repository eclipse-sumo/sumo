/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
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
#include <libsumo/TraCIConstants.h>
#include <utils/shapes/SUMOPolygon.h>
#include <utils/shapes/ShapeContainer.h>

#include "Polygon.h"
#include "Helper.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Polygon::mySubscriptionResults;
ContextSubscriptionResults Polygon::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Polygon::getIDList() {
    std::vector<std::string> ids;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.getPolygons().insertIDs(ids);
    return ids;
}


int
Polygon::getIDCount() {
    return (int)getIDList().size();
}


std::string
Polygon::getType(const std::string& polygonID) {
    return getPolygon(polygonID)->getShapeType();
}


TraCIPositionVector
Polygon::getShape(const std::string& polygonID) {
    SUMOPolygon* p = getPolygon(polygonID);
    return Helper::makeTraCIPositionVector(p->getShape());
}


bool
Polygon::getFilled(const std::string& polygonID) {
    return getPolygon(polygonID)->getFill();
}

double
Polygon::getLineWidth(const std::string& polygonID) {
    return getPolygon(polygonID)->getLineWidth();
}

TraCIColor
Polygon::getColor(const std::string& polygonID) {
    SUMOPolygon* p = getPolygon(polygonID);
    return Helper::makeTraCIColor(p->getShapeColor());
}


std::string
Polygon::getParameter(const std::string& polygonID, const std::string& key) {
    return getPolygon(polygonID)->getParameter(key, "");
}


void
Polygon::setType(const std::string& polygonID, const std::string& setType) {
    SUMOPolygon* p = getPolygon(polygonID);
    p->setShapeType(setType);
}


void
Polygon::setShape(const std::string& polygonID, const TraCIPositionVector& shape) {
    PositionVector positionVector = Helper::makePositionVector(shape);
    getPolygon(polygonID); // just to check whether it exists
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.reshapePolygon(polygonID, positionVector);
}


void
Polygon::setColor(const std::string& polygonID, const TraCIColor& c) {
    getPolygon(polygonID)->setShapeColor(Helper::makeRGBColor(c));
}


void
Polygon::add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& color, bool fill, double lineWidth, const std::string& polygonType, int layer) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    PositionVector pShape = Helper::makePositionVector(shape);
    RGBColor col = Helper::makeRGBColor(color);
    if (!shapeCont.addPolygon(polygonID, polygonType, col, (double)layer, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, Shape::DEFAULT_RELATIVEPATH, pShape, false, fill, lineWidth)) {
        throw TraCIException("Could not add polygon '" + polygonID + "'");
    }
}


void
Polygon::remove(const std::string& polygonID, int /* layer */) {
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

void
Polygon::setLineWidth(std::string polygonID, double lineWidth) {
    SUMOPolygon* p = getPolygon(polygonID);
    p->setLineWidth(lineWidth);
}


SUMOPolygon*
Polygon::getPolygon(const std::string& id) {
    SUMOPolygon* p = MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
    if (p == nullptr) {
        throw TraCIException("Polygon '" + id + "' is not known");
    }
    return p;
}


void
Polygon::setParameter(const std::string& polygonID, const std::string& key, const std::string& value) {
    SUMOPolygon* p = getPolygon(polygonID);
    p->setParameter(key, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Polygon, POLYGON)


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


void
Polygon::storeShape(const std::string& id, PositionVector& shape) {
    shape = getPolygon(id)->getShape();
}


std::shared_ptr<VariableWrapper>
Polygon::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Polygon::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_TYPE:
            return wrapper->wrapString(objID, variable, getType(objID));
        case VAR_COLOR:
            return wrapper->wrapColor(objID, variable, getColor(objID));
        case VAR_FILL:
            return wrapper->wrapInt(objID, variable, getFilled(objID));
        case VAR_WIDTH:
            return wrapper->wrapDouble(objID, variable, getLineWidth(objID));
        default:
            return false;
    }
}


}


/****************************************************************************/
