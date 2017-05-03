/****************************************************************************/
/// @file    TraCI_Polygon.cpp
/// @author  Gregor L\"ammel
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017 - 2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/



#include <microsim/MSNet.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>

#include "TraCI_Polygon.h"
#include "TraCI.h"

std::vector<std::string> TraCI_Polygon::getIDList() {
    std::vector<std::string> ids;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.getPolygons().insertIDs(ids);
    return ids;
}
std::string TraCI_Polygon::getType(const std::string& polygonID) {
    return getPolygon(polygonID)->getType();
}
TraCIPositionVector TraCI_Polygon::getShape(const std::string& polygonID) {
    SUMO::Polygon* p = getPolygon(polygonID);
    return TraCI::makeTraCIPositionVector(p->getShape());
}
bool TraCI_Polygon::getFilled(const std::string& polygonID) {
    return getPolygon(polygonID)->getFill();
}
TraCIColor TraCI_Polygon::getColor(const std::string& polygonID) {
    SUMO::Polygon* p = getPolygon(polygonID);
    return TraCI::makeTraCIColor(p->getColor());
}
std::string TraCI_Polygon::getParameter(const std::string& polygonID, const std::string& paramName) {
    return getPolygon(polygonID)->getParameter(paramName, "");
}
void TraCI_Polygon::setType(const std::string& polygonID, const std::string& setType) {
    SUMO::Polygon* p = getPolygon(polygonID);
    p->setType(setType);
}
void TraCI_Polygon::setShape(const std::string& polygonID, const TraCIPositionVector& shape) {
    PositionVector positionVector = TraCI::makePositionVector(shape);
    getPolygon(polygonID); // just to check whether it exists
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.reshapePolygon(polygonID, positionVector);
}
void TraCI_Polygon::setColor(const std::string& polygonID, const TraCIColor& c) {
    getPolygon(polygonID)->setColor(TraCI::makeRGBColor(c));
}
void
TraCI_Polygon::add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    PositionVector pShape = TraCI::makePositionVector(shape);
    RGBColor col = TraCI::makeRGBColor(c);
    if (!shapeCont.addPolygon(polygonID, type, col, (double)layer, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, pShape, fill)) {
        throw TraCIException("Could not add polygon '" + polygonID + "'");
    }
}
void TraCI_Polygon::remove(const std::string& polygonID, int /* layer */) {
    // !!! layer not used yet (shouldn't the id be enough?)
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    if (!shapeCont.removePolygon(polygonID)) {
        throw TraCIException("Could not remove polygon '" + polygonID + "'");
    }
}

void TraCI_Polygon::setFilled(std::string polygonID, bool filled) {
    SUMO::Polygon* p = getPolygon(polygonID);
    p->setFill(filled);
}

SUMO::Polygon* TraCI_Polygon::getPolygon(const std::string& id) {
    SUMO::Polygon* p = MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
    if (p == 0) {
        throw TraCIException("Polygon '" + id +  "' is not known");
    }
    return p;
}
void TraCI_Polygon::setParameter(std::string& id, std::string& name, std::string& value) {
    SUMO::Polygon* p = getPolygon(id);
    p->addParameter(name, value);
}


