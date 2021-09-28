/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    CommonXMLStructure.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2021
///
// Structure for common XML Parsing
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "CommonXMLStructure.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// CommonXMLStructure::SumoBaseObject - methods
// ---------------------------------------------------------------------------

CommonXMLStructure::SumoBaseObject::SumoBaseObject(SumoBaseObject* parent) :
    mySumoBaseObjectParent(parent),
    myTag(SUMO_TAG_NOTHING),
    myVClass(SVC_IGNORING),
    myVehicleTypeParameter(""),
    myDefinedVehicleTypeParameter(false),
    myDefinedVehicleParameter(false),
    myDefinedStopParameter(false) {
    // add this SumoBaseObject into parent children
    if (mySumoBaseObjectParent) {
        mySumoBaseObjectParent->addSumoBaseObjectChild(this);
    }
}


CommonXMLStructure::SumoBaseObject::~SumoBaseObject() {
    // remove this SumoBaseObject from parent children
    if (mySumoBaseObjectParent) {
        mySumoBaseObjectParent->removeSumoBaseObjectChild(this);
    }
    // delete all SumoBaseObjectChildrens
    while (mySumoBaseObjectChildren.size() > 0) {
        delete mySumoBaseObjectChildren.back();
    }
}


void
CommonXMLStructure::SumoBaseObject::clear() {
    // reset tag
    myTag = SUMO_TAG_NOTHING;
    // reset vClass
    myVClass = SVC_IGNORING;
    // clear containers
    myStringAttributes.clear();
    myIntAttributes.clear();
    myDoubleAttributes.clear();
    myBoolAttributes.clear();
    myPositionAttributes.clear();
    myTimeAttributes.clear();
    myColorAttributes.clear();
    myStringListAttributes.clear();
    myPositionVectorAttributes.clear();
    myParameters.clear();
    mySumoBaseObjectChildren.clear();
    // reset flags
    myDefinedVehicleTypeParameter = false;
    myDefinedVehicleParameter = false;
    myDefinedStopParameter = false;
    // delete all SumoBaseObjectChildrens
    while (mySumoBaseObjectChildren.size() > 0) {
        delete mySumoBaseObjectChildren.back();
    }
}


void
CommonXMLStructure::SumoBaseObject::setTag(const SumoXMLTag tag) {
    myTag = tag;
}


SumoXMLTag
CommonXMLStructure::SumoBaseObject::getTag() const {
    return myTag;
}


CommonXMLStructure::SumoBaseObject*
CommonXMLStructure::SumoBaseObject::getParentSumoBaseObject() const {
    return mySumoBaseObjectParent;
}


std::map<std::string, std::string>
CommonXMLStructure::SumoBaseObject::getAllAttributes() const {
    std::map<std::string, std::string> result;
    for (const auto &attr : myStringAttributes) {
        result[toString(attr.first)] = attr.second;
    }
    for (const auto &attr : myIntAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myDoubleAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myBoolAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myPositionAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myTimeAttributes) {
        result[toString(attr.first)] = time2string(attr.second);
    }
    for (const auto &attr : myColorAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myStringListAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto &attr : myPositionVectorAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    return result;
}


const std::string&
CommonXMLStructure::SumoBaseObject::getStringAttribute(const SumoXMLAttr attr) const {
    if (hasStringAttribute(attr)) {
        return myStringAttributes.at(attr);
    } else {
        handleAttributeError(attr, "string");
        throw ProcessError();
    }
}


int
CommonXMLStructure::SumoBaseObject::getIntAttribute(const SumoXMLAttr attr) const {
    if (hasIntAttribute(attr)) {
        return myIntAttributes.at(attr);
    } else {
        handleAttributeError(attr, "int");
        throw ProcessError();
    }
}


double
CommonXMLStructure::SumoBaseObject::getDoubleAttribute(const SumoXMLAttr attr) const {
    if (hasDoubleAttribute(attr)) {
        return myDoubleAttributes.at(attr);
    } else {
        handleAttributeError(attr, "double");
        throw ProcessError();
    }
}


bool
CommonXMLStructure::SumoBaseObject::getBoolAttribute(const SumoXMLAttr attr) const {
    if (hasBoolAttribute(attr)) {
        return myBoolAttributes.at(attr);
    } else {
        handleAttributeError(attr, "bool");
        throw ProcessError();
    }
}


const Position&
CommonXMLStructure::SumoBaseObject::getPositionAttribute(const SumoXMLAttr attr) const {
    if (hasPositionAttribute(attr)) {
        return myPositionAttributes.at(attr);
    } else {
        handleAttributeError(attr, "position");
        throw ProcessError();
    }
}


SUMOTime
CommonXMLStructure::SumoBaseObject::getTimeAttribute(const SumoXMLAttr attr) const {
    if (hasTimeAttribute(attr)) {
        return myTimeAttributes.at(attr);
    } else {
        handleAttributeError(attr, "time");
        throw ProcessError();
    }
}


const RGBColor&
CommonXMLStructure::SumoBaseObject::getColorAttribute(const SumoXMLAttr attr) const {
    if (hasColorAttribute(attr)) {
        return myColorAttributes.at(attr);
    } else {
        handleAttributeError(attr, "color");
        throw ProcessError();
    }
}


const std::vector<std::string>&
CommonXMLStructure::SumoBaseObject::getStringListAttribute(const SumoXMLAttr attr) const {
    if (hasStringListAttribute(attr)) {
        return myStringListAttributes.at(attr);
    } else {
        handleAttributeError(attr, "string list");
        throw ProcessError();
    }
}


const PositionVector&
CommonXMLStructure::SumoBaseObject::getPositionVectorAttribute(const SumoXMLAttr attr) const {
    if (hasPositionVectorAttribute(attr)) {
        return myPositionVectorAttributes.at(attr);
    } else {
        handleAttributeError(attr, "position vector");
        throw ProcessError();;
    }
}


SUMOVehicleClass 
CommonXMLStructure::SumoBaseObject::getVClass() const {
    return myVClass;
}


const SUMOVTypeParameter&
CommonXMLStructure::SumoBaseObject::getVehicleTypeParameter() const {
    if (myDefinedVehicleTypeParameter) {
        return myVehicleTypeParameter;
    } else {
        throw ProcessError("Undefined vehicleType parameter");
    }
}


const SUMOVehicleParameter&
CommonXMLStructure::SumoBaseObject::getVehicleParameter() const {
    if (myDefinedVehicleParameter) {
        return myVehicleParameter;
    } else {
        throw ProcessError("Undefined vehicle parameter");
    }
}


const SUMOVehicleParameter::Stop&
CommonXMLStructure::SumoBaseObject::getStopParameter() const {
    if (myDefinedStopParameter) {
        return myStopParameter;
    } else {
        throw ProcessError("Undefined stop parameter");
    }

}


const std::map<std::string, std::string>&
CommonXMLStructure::SumoBaseObject::getParameters() const {
    return myParameters;
}


const std::vector<CommonXMLStructure::SumoBaseObject*>&
CommonXMLStructure::SumoBaseObject::getSumoBaseObjectChildren() const {
    return mySumoBaseObjectChildren;
}


bool
CommonXMLStructure::SumoBaseObject::hasStringAttribute(const SumoXMLAttr attr) const {
    return myStringAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasIntAttribute(const SumoXMLAttr attr) const {
    return myIntAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasDoubleAttribute(const SumoXMLAttr attr) const {
    return myDoubleAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasBoolAttribute(const SumoXMLAttr attr) const {
    return myBoolAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasPositionAttribute(const SumoXMLAttr attr) const {
    return myPositionAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasTimeAttribute(const SumoXMLAttr attr) const {
    return myTimeAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasColorAttribute(const SumoXMLAttr attr) const {
    return myColorAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasStringListAttribute(const SumoXMLAttr attr) const {
    return myStringListAttributes.count(attr) > 0;
}


bool
CommonXMLStructure::SumoBaseObject::hasPositionVectorAttribute(const SumoXMLAttr attr) const {
    return myPositionVectorAttributes.count(attr) > 0;
}


void
CommonXMLStructure::SumoBaseObject::addStringAttribute(const SumoXMLAttr attr, const std::string& value) {
    myStringAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addIntAttribute(const SumoXMLAttr attr, const int value) {
    myIntAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addDoubleAttribute(const SumoXMLAttr attr, const double value) {
    myDoubleAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addBoolAttribute(const SumoXMLAttr attr, const bool value) {
    myBoolAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addPositionAttribute(const SumoXMLAttr attr, const Position& value) {
    myPositionAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addTimeAttribute(const SumoXMLAttr attr, const SUMOTime value) {
    myTimeAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addColorAttribute(const SumoXMLAttr attr, const RGBColor& value) {
    myColorAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addStringListAttribute(const SumoXMLAttr attr, const std::vector<std::string>& value) {
    myStringListAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addPositionVectorAttribute(const SumoXMLAttr attr, const PositionVector& value) {
    myPositionVectorAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::setVClass(SUMOVehicleClass vClass) {
    myVClass = vClass;
}


void 
CommonXMLStructure::SumoBaseObject::setVehicleTypeParameter(const SUMOVTypeParameter* vehicleTypeParameter) {
    myVehicleTypeParameter = *vehicleTypeParameter;
    myDefinedVehicleTypeParameter = true;
    // set attribute id
    addStringAttribute(SUMO_ATTR_ID, myVehicleTypeParameter.id);
}


void
CommonXMLStructure::SumoBaseObject::setVehicleParameter(const SUMOVehicleParameter* vehicleParameter) {
    myVehicleParameter = *vehicleParameter;
    myDefinedVehicleParameter = true;
    // set attribute id
    if (!myVehicleParameter.id.empty()) {
        addStringAttribute(SUMO_ATTR_ID, myVehicleParameter.id);
    }
    // set attribute route
    if (!vehicleParameter->routeid.empty()) {
        addStringAttribute(SUMO_ATTR_ROUTE, myVehicleParameter.routeid);
    }
}


void
CommonXMLStructure::SumoBaseObject::setStopParameter(const SUMOVehicleParameter::Stop &stopParameter) {
    myStopParameter = stopParameter;
    myDefinedStopParameter = true;
    // set attribute edge
    if (!myStopParameter.edge.empty()) {
        addStringAttribute(SUMO_ATTR_ID, myStopParameter.edge);
    }
    // set attribute lane
    if (!myStopParameter.lane.empty()) {
        addStringAttribute(SUMO_ATTR_LANE, myStopParameter.lane);
    }
    // set attribute busStop
    if (!myStopParameter.busstop.empty()) {
        addStringAttribute(SUMO_ATTR_BUS_STOP, myStopParameter.busstop);
    }
    // set attribute containerstop
    if (!myStopParameter.containerstop.empty()) {
        addStringAttribute(SUMO_ATTR_CONTAINER_STOP, myStopParameter.containerstop);
    }
    // set attribute parkingarea
    if (!myStopParameter.parkingarea.empty()) {
        addStringAttribute(SUMO_ATTR_PARKING_AREA, myStopParameter.parkingarea);
    }
    // set attribute chargingStation
    if (!myStopParameter.chargingStation.empty()) {
        addStringAttribute(SUMO_ATTR_CHARGING_STATION, myStopParameter.chargingStation);
    }
}


void
CommonXMLStructure::SumoBaseObject::addParameter(const std::string& key, const std::string& value) {
    // check if we have to insert in vType, vehicle or stop parameters
    if (myDefinedVehicleTypeParameter) {
        myVehicleTypeParameter.setParameter(key, value);
    } else if (myDefinedVehicleParameter) {
        myVehicleParameter.setParameter(key, value);
    } else if (myDefinedStopParameter) {
        myStopParameter.setParameter(key, value);
    } else {
        myParameters[key] = value;
    }
}


void
CommonXMLStructure::SumoBaseObject::addSumoBaseObjectChild(SumoBaseObject* sumoBaseObject) {
    // just add it into mySumoBaseObjectChildren
    mySumoBaseObjectChildren.push_back(sumoBaseObject);
}


void
CommonXMLStructure::SumoBaseObject::removeSumoBaseObjectChild(SumoBaseObject* sumoBaseObject) {
    // find sumoBaseObject
    auto it = std::find(mySumoBaseObjectChildren.begin(), mySumoBaseObjectChildren.end(), sumoBaseObject);
    // check iterator
    if (it != mySumoBaseObjectChildren.end()) {
        mySumoBaseObjectChildren.erase(it);
    }
}


void
CommonXMLStructure::SumoBaseObject::handleAttributeError(const SumoXMLAttr attr, const std::string &type) const {
    WRITE_ERROR("Trying to get undefined " + type + " attribute '" + toString(attr) + "' in SUMOBaseObject '" + toString(myTag) + "'");
}

// ---------------------------------------------------------------------------
// CommonXMLStructure - methods
// ---------------------------------------------------------------------------

CommonXMLStructure::CommonXMLStructure() :
    mySumoBaseObjectRoot(nullptr),
    myCurrentSumoBaseObject(nullptr) {

}


CommonXMLStructure::~CommonXMLStructure() {
    // delete mySumoBaseObjectRoot (this will also delete all SumoBaseObjectChildrens)
    if (mySumoBaseObjectRoot) {
        delete mySumoBaseObjectRoot;
    }
}


void
CommonXMLStructure::openSUMOBaseOBject() {
    // first check if root is empty
    if (mySumoBaseObjectRoot == nullptr) {
        // create root
        mySumoBaseObjectRoot = new SumoBaseObject(nullptr);
        // set tag
        mySumoBaseObjectRoot->setTag(SUMO_TAG_ROOTFILE);
        // update last inserted Root
        myCurrentSumoBaseObject = mySumoBaseObjectRoot;
    } else {
        // create new node
        SumoBaseObject* newSumoBaseObject = new SumoBaseObject(myCurrentSumoBaseObject);
        // update last inserted node
        myCurrentSumoBaseObject = newSumoBaseObject;
    }
}


void
CommonXMLStructure::closeSUMOBaseOBject() {
    // check that myCurrentSumoBaseObject is valid
    if (myCurrentSumoBaseObject) {
        // check if last inserted SumoBaseObject is the root
        if (myCurrentSumoBaseObject->getParentSumoBaseObject() == nullptr) {
            // reset both pointers
            myCurrentSumoBaseObject = nullptr;
            mySumoBaseObjectRoot = nullptr;
        } else {
            // update last inserted SumoBaseObject
            myCurrentSumoBaseObject = myCurrentSumoBaseObject->getParentSumoBaseObject();
        }
    }
}


CommonXMLStructure::SumoBaseObject*
CommonXMLStructure::getSumoBaseObjectRoot() const {
    return mySumoBaseObjectRoot;
}


CommonXMLStructure::SumoBaseObject*
CommonXMLStructure::getCurrentSumoBaseObject() const {
    return myCurrentSumoBaseObject;
}

/****************************************************************************/
