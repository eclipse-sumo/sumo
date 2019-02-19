/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <microsim/MSNet.h>
#include <libsumo/TraCIConstants.h>
#include "POI.h"
#include "Helper.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults POI::mySubscriptionResults;
ContextSubscriptionResults POI::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
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
    return getPoI(poiID)->getShapeType();
}


TraCIColor
POI::getColor(const std::string& poiID) {
    return Helper::makeTraCIColor(getPoI(poiID)->getShapeColor());
}


TraCIPosition
POI::getPosition(const std::string& poiID, const bool includeZ) {
    return Helper::makeTraCIPosition(*getPoI(poiID), includeZ);
}


double
POI::getWidth(const std::string& poiID) {
	return getPoI(poiID)->getWidth();
}


double
POI::getHeight(const std::string& poiID) {
	return getPoI(poiID)->getHeight();
}


double
POI::getAngle(const std::string& poiID) {
	return getPoI(poiID)->getShapeNaviDegree();
}


std::string 
POI::getImageFile(const std::string& poiID) {
    return getPoI(poiID)->getShapeImgFile();
}


std::string
POI::getParameter(const std::string& poiID, const std::string& key) {
    return getPoI(poiID)->getParameter(key, "");
}


void
POI::setType(const std::string& poiID, const std::string& type) {
    getPoI(poiID)->setShapeType(type);
}


void
POI::setPosition(const std::string& poiID, double x, double y) {
    // try to retrieve so that the correct error is generated for unknown poiIDs
    getPoI(poiID);
    MSNet::getInstance()->getShapeContainer().movePOI(poiID, Position(x, y));
}


void
POI::setColor(const std::string& poiID, const TraCIColor& c) {
    getPoI(poiID)->setShapeColor(Helper::makeRGBColor(c));
}


void
POI::setWidth(const std::string& poiID, double width) {
	getPoI(poiID)->setWidth(width);
}


void
POI::setHeight(const std::string& poiID, double height) {
	getPoI(poiID)->setHeight(height);
}


void
POI::setAngle(const std::string& poiID, double angle) {
	getPoI(poiID)->setShapeNaviDegree(angle);
}


void 
POI::setImageFile(const std::string& poiID, const std::string& imageFile) {
    getPoI(poiID)->setShapeImgFile(imageFile);
}


bool
POI::add(const std::string& poiID, double x, double y, const TraCIColor& color, const std::string& poiType, int layer, const std::string& imgFile, double width, double height, double angle) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    return shapeCont.addPOI(poiID, poiType, Helper::makeRGBColor(color), Position(x, y), false, "", 0, 0, (double)layer,
                            angle,
                            imgFile,
                            Shape::DEFAULT_RELATIVEPATH,
                            width,
                            height);
}


bool
POI::remove(const std::string& poiID, int /* layer */) {
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    return shapeCont.removePOI(poiID);
}


void
POI::setParameter(const std::string& poiID, const std::string& key, const std::string& value) {
    PointOfInterest* p = getPoI(poiID);
    p->setParameter(key, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(POI, POI)


PointOfInterest*
POI::getPoI(const std::string& id) {
    PointOfInterest* sumoPoi = MSNet::getInstance()->getShapeContainer().getPOIs().get(id);
    if (sumoPoi == nullptr) {
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


void
POI::storeShape(const std::string& id, PositionVector& shape) {
    shape.push_back(*getPoI(id));
}


std::shared_ptr<VariableWrapper>
POI::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
POI::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_TYPE:
            return wrapper->wrapString(objID, variable, getType(objID));
        case VAR_COLOR:
            return wrapper->wrapColor(objID, variable, getColor(objID));
        case VAR_POSITION:
            return wrapper->wrapPosition(objID, variable, getPosition(objID));
        case VAR_POSITION3D:
            return wrapper->wrapPosition(objID, variable, getPosition(objID, true));
		case VAR_WIDTH:
			return wrapper->wrapDouble(objID, variable, getWidth(objID));
		case VAR_HEIGHT:
			return wrapper->wrapDouble(objID, variable, getHeight(objID));
		case VAR_ANGLE:
			return wrapper->wrapDouble(objID, variable, getAngle(objID));
        case VAR_IMAGEFILE:
            return wrapper->wrapString(objID, variable, getImageFile(objID));
        default:
            return false;
    }
}


}


/****************************************************************************/
