/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <utils/common/StringTokenizer.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "CommonXMLStructure.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// CommonXMLStructure::PlanParameters - methods
// ---------------------------------------------------------------------------

CommonXMLStructure::PlanParameters::PlanParameters() {}


CommonXMLStructure::PlanParameters::PlanParameters(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
        const SUMOSAXAttributes& attrs, bool& parsedOk) {
    // get plan parent ID
    const auto planParentID = sumoBaseObject->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID).c_str();
    // edges
    fromEdge = attrs.getOpt<std::string>(SUMO_ATTR_FROM, planParentID, parsedOk, "");
    toEdge = attrs.getOpt<std::string>(SUMO_ATTR_TO, planParentID, parsedOk, "");
    if (toEdge.empty()) {
        toEdge = attrs.getOpt<std::string>(SUMO_ATTR_EDGE, planParentID, parsedOk, "");
    }
    consecutiveEdges = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_EDGES, planParentID, parsedOk);
    // junctions
    fromJunction = attrs.getOpt<std::string>(SUMO_ATTR_FROM_JUNCTION, planParentID, parsedOk, "");
    toJunction = attrs.getOpt<std::string>(SUMO_ATTR_TO_JUNCTION, planParentID, parsedOk, "");
    // TAZs
    fromTAZ = attrs.getOpt<std::string>(SUMO_ATTR_FROM_TAZ, planParentID, parsedOk, "");
    toTAZ = attrs.getOpt<std::string>(SUMO_ATTR_TO_TAZ, planParentID, parsedOk, "");
    // bus stops
    fromBusStop = attrs.getOpt<std::string>(GNE_ATTR_FROM_BUSSTOP, planParentID, parsedOk, "");
    toBusStop = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, planParentID, parsedOk, "");
    // train stops
    fromTrainStop = attrs.getOpt<std::string>(GNE_ATTR_FROM_TRAINSTOP, planParentID, parsedOk, "");
    toTrainStop = attrs.getOpt<std::string>(SUMO_ATTR_TRAIN_STOP, planParentID, parsedOk, "");
    // container stops
    fromContainerStop = attrs.getOpt<std::string>(GNE_ATTR_FROM_CONTAINERSTOP, planParentID, parsedOk, "");
    toContainerStop = attrs.getOpt<std::string>(SUMO_ATTR_CONTAINER_STOP, planParentID, parsedOk, "");
    // charging stations
    fromChargingStation = attrs.getOpt<std::string>(GNE_ATTR_FROM_CHARGINGSTATION, planParentID, parsedOk, "");
    toChargingStation = attrs.getOpt<std::string>(SUMO_ATTR_CHARGING_STATION, planParentID, parsedOk, "");
    // parking areas
    fromParkingArea = attrs.getOpt<std::string>(GNE_ATTR_FROM_PARKINGAREA, planParentID, parsedOk, "");
    toParkingArea = attrs.getOpt<std::string>(SUMO_ATTR_PARKING_AREA, planParentID, parsedOk, "");
    // routes
    fromRoute = attrs.getOpt<std::string>(GNE_ATTR_FROM_ROUTE, planParentID, parsedOk, "");
    toRoute = attrs.getOpt<std::string>(SUMO_ATTR_ROUTE, planParentID, parsedOk, "");
    // update from attributes
    updateFromAttributes(sumoBaseObject);
}


void
CommonXMLStructure::PlanParameters::clear() {
    fromJunction.clear();
    toJunction.clear();
    fromEdge.clear();
    toEdge.clear();
    fromTAZ.clear();
    toTAZ.clear();
    fromBusStop.clear();
    toBusStop.clear();
    fromTrainStop.clear();
    toTrainStop.clear();
    fromContainerStop.clear();
    toContainerStop.clear();
    fromChargingStation.clear();
    toChargingStation.clear();
    fromParkingArea.clear();
    toParkingArea.clear();
    consecutiveEdges.clear();
    fromRoute.clear();
    toRoute.clear();
}


bool
CommonXMLStructure::PlanParameters::isSingleEdgePlan() const {
    if (fromEdge.empty()) {
        return false;
    } else {
        return getNumberOfDefinedParameters() == 1;
    }
}


int
CommonXMLStructure::PlanParameters::getNumberOfDefinedParameters() const {
    return (int)consecutiveEdges.size() +
           (fromJunction.empty() ? 0 : 1) +
           (toJunction.empty() ? 0 : 1) +
           (fromEdge.empty() ? 0 : 1) +
           (toEdge.empty() ? 0 : 1) +
           (fromTAZ.empty() ? 0 : 1) +
           (toTAZ.empty() ? 0 : 1) +
           (fromBusStop.empty() ? 0 : 1) +
           (toBusStop.empty() ? 0 : 1) +
           (fromTrainStop.empty() ? 0 : 1) +
           (toTrainStop.empty() ? 0 : 1) +
           (fromContainerStop.empty() ? 0 : 1) +
           (toContainerStop.empty() ? 0 : 1) +
           (fromChargingStation.empty() ? 0 : 1) +
           (toChargingStation.empty() ? 0 : 1) +
           (fromParkingArea.empty() ? 0 : 1) +
           (toParkingArea.empty() ? 0 : 1) +
           (fromRoute.empty() ? 0 : 1) +
           (toRoute.empty() ? 0 : 1);
}


const CommonXMLStructure::SumoBaseObject*
CommonXMLStructure::PlanParameters::getPreviousPlanObj(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const {
    // first check if object exist
    if (sumoBaseObject == nullptr) {
        return nullptr;
    }
    // check if object has parent
    const CommonXMLStructure::SumoBaseObject* parentObject = sumoBaseObject->getParentSumoBaseObject();
    if (parentObject == nullptr) {
        return nullptr;
    }
    // check number of children
    if (parentObject->getSumoBaseObjectChildren().size() < 2) {
        return nullptr;
    }
    // search position of the given plan obj in the parent children
    const auto objIterator = std::find(parentObject->getSumoBaseObjectChildren().begin(), parentObject->getSumoBaseObjectChildren().end(), sumoBaseObject);
    // if obj is the first plan of person/container parent, then return null. If not, return previous object
    if (objIterator == parentObject->getSumoBaseObjectChildren().begin()) {
        return nullptr;
    } else {
        return *(objIterator - 1);
    }
}


void
CommonXMLStructure::PlanParameters::updateFromAttributes(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) {
    // check if previous plan object was defined but not the from
    const auto previousPlanObj = getPreviousPlanObj(sumoBaseObject);
    if (previousPlanObj) {
        if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO)) {
            // edge (to)
            resetPreviousFromAttributes(previousPlanObj, "edge", previousPlanObj->getStringAttribute(SUMO_ATTR_TO));
            fromEdge = previousPlanObj->getStringAttribute(SUMO_ATTR_TO);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_EDGE)) {
            // edge
            resetPreviousFromAttributes(previousPlanObj, "edge", previousPlanObj->getStringAttribute(SUMO_ATTR_EDGE));
            fromEdge = previousPlanObj->getStringAttribute(SUMO_ATTR_EDGE);
        } else if (previousPlanObj->hasStringListAttribute(SUMO_ATTR_EDGES)) {
            // consecutive edge
            const std::string consecutiveEdge = previousPlanObj->getStringListAttribute(SUMO_ATTR_EDGES).back();
            resetPreviousFromAttributes(previousPlanObj, "consecutive edge", consecutiveEdge);
            fromEdge = consecutiveEdge;
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_ROUTE)) {
            // route
            resetPreviousFromAttributes(previousPlanObj, "route edge", previousPlanObj->getStringAttribute(SUMO_ATTR_ID));
            fromRoute = previousPlanObj->getStringAttribute(SUMO_ATTR_ROUTE).back();
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_JUNCTION)) {
            // junction
            resetPreviousFromAttributes(previousPlanObj, "junction", previousPlanObj->getStringAttribute(SUMO_ATTR_TO_JUNCTION));
            fromJunction = previousPlanObj->getStringAttribute(SUMO_ATTR_TO_JUNCTION);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TO_TAZ)) {
            // TAZ
            resetPreviousFromAttributes(previousPlanObj, "TAZ", previousPlanObj->getStringAttribute(SUMO_ATTR_TO_TAZ));
            fromTAZ = previousPlanObj->getStringAttribute(SUMO_ATTR_TO_TAZ);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_BUS_STOP)) {
            // busStop
            resetPreviousFromAttributes(previousPlanObj, "bus stop", previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP));
            fromBusStop = previousPlanObj->getStringAttribute(SUMO_ATTR_BUS_STOP);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_TRAIN_STOP)) {
            // trainStop
            resetPreviousFromAttributes(previousPlanObj, "train stop", previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP));
            fromTrainStop = previousPlanObj->getStringAttribute(SUMO_ATTR_TRAIN_STOP);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CONTAINER_STOP)) {
            // containerStop
            resetPreviousFromAttributes(previousPlanObj, "container stop", previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP));
            fromContainerStop = previousPlanObj->getStringAttribute(SUMO_ATTR_CONTAINER_STOP);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_CHARGING_STATION)) {
            // chargingStation
            resetPreviousFromAttributes(previousPlanObj, "charging station", previousPlanObj->getStringAttribute(SUMO_ATTR_CHARGING_STATION));
            fromChargingStation = previousPlanObj->getStringAttribute(SUMO_ATTR_CHARGING_STATION);
        } else if (previousPlanObj->hasStringAttribute(SUMO_ATTR_PARKING_AREA)) {
            // parkingArea
            resetPreviousFromAttributes(previousPlanObj, "parking area", previousPlanObj->getStringAttribute(SUMO_ATTR_PARKING_AREA));
            fromParkingArea = previousPlanObj->getStringAttribute(SUMO_ATTR_PARKING_AREA);
        }
    }
}


void
CommonXMLStructure::PlanParameters::resetPreviousFromAttributes(const CommonXMLStructure::SumoBaseObject* previousPlanObj,
        const std::string& newType, const std::string& newId) const {
    if (!fromEdge.empty()) {
        writeIgnoringMessage(previousPlanObj, "edge", fromEdge, newType, newId);
    }
    if (!fromJunction.empty()) {
        writeIgnoringMessage(previousPlanObj, "junction", fromJunction, newType, newId);
    }
    if (!fromTAZ.empty()) {
        writeIgnoringMessage(previousPlanObj, "TAZ", fromTAZ, newType, newId);
    }
    if (!fromBusStop.empty()) {
        writeIgnoringMessage(previousPlanObj, "bus stop", fromBusStop, newType, newId);
    }
    if (!fromTrainStop.empty()) {
        writeIgnoringMessage(previousPlanObj, "train stop", fromTrainStop, newType, newId);
    }
    if (!fromContainerStop.empty()) {
        writeIgnoringMessage(previousPlanObj, "container stop", fromContainerStop, newType, newId);
    }
    if (!fromChargingStation.empty()) {
        writeIgnoringMessage(previousPlanObj, "charging station", fromChargingStation, newType, newId);
    }
    if (!fromParkingArea.empty()) {
        writeIgnoringMessage(previousPlanObj, "parking area", fromParkingArea, newType, newId);
    }
}


void
CommonXMLStructure::PlanParameters::writeIgnoringMessage(const CommonXMLStructure::SumoBaseObject* previousPlanObj,
        const std::string& oldType, const std::string& oldId, const std::string& newType, const std::string& newId) const {
    WRITE_WARNING(TL("Ignoring from % '%' used in % '%' and using instead the previous end element % '%'"),
                  oldType, oldId,
                  toString(previousPlanObj->getParentSumoBaseObject()->getTag()),
                  previousPlanObj->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID),
                  newType, newId);
}

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
    myDoubleListAttributes.clear();
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
    for (const auto& attr : myStringAttributes) {
        result[toString(attr.first)] = attr.second;
    }
    for (const auto& attr : myIntAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myDoubleAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myBoolAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myPositionAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myTimeAttributes) {
        result[toString(attr.first)] = time2string(attr.second);
    }
    for (const auto& attr : myColorAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myStringListAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myDoubleListAttributes) {
        result[toString(attr.first)] = toString(attr.second);
    }
    for (const auto& attr : myPositionVectorAttributes) {
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


SUMOTime
CommonXMLStructure::SumoBaseObject::getPeriodAttribute() const {
    SumoXMLAttr attr = SUMO_ATTR_PERIOD;
    if (hasTimeAttribute(attr)) {
        return myTimeAttributes.at(attr);
    } else {
        // try 'freq' as alias for 'period'
        attr = SUMO_ATTR_FREQUENCY;
        if (hasTimeAttribute(attr)) {
            return myTimeAttributes.at(attr);
        }
        handleAttributeError(SUMO_ATTR_PERIOD, "time");
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


const std::vector<double>&
CommonXMLStructure::SumoBaseObject::getDoubleListAttribute(const SumoXMLAttr attr) const {
    if (hasDoubleListAttribute(attr)) {
        return myDoubleListAttributes.at(attr);
    } else {
        handleAttributeError(attr, "double list");
        throw ProcessError();
    }
}


const PositionVector&
CommonXMLStructure::SumoBaseObject::getPositionVectorAttribute(const SumoXMLAttr attr) const {
    if (hasPositionVectorAttribute(attr)) {
        return myPositionVectorAttributes.at(attr);
    } else {
        handleAttributeError(attr, "position vector");
        throw ProcessError();
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
        throw ProcessError(TL("Undefined vehicleType parameter"));
    }
}


const SUMOVehicleParameter&
CommonXMLStructure::SumoBaseObject::getVehicleParameter() const {
    if (myDefinedVehicleParameter) {
        return myVehicleParameter;
    } else {
        throw ProcessError(TL("Undefined vehicle parameter"));
    }
}


const SUMOVehicleParameter::Stop&
CommonXMLStructure::SumoBaseObject::getStopParameter() const {
    if (myDefinedStopParameter) {
        return myStopParameter;
    } else {
        throw ProcessError(TL("Undefined stop parameter"));
    }

}


const std::map<std::string, std::string>&
CommonXMLStructure::SumoBaseObject::getParameters() const {
    return myParameters;
}


const CommonXMLStructure::PlanParameters&
CommonXMLStructure::SumoBaseObject::getPlanParameters() const {
    return myPlanParameters;
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
CommonXMLStructure::SumoBaseObject::hasDoubleListAttribute(const SumoXMLAttr attr) const {
    return myDoubleListAttributes.count(attr) > 0;
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
CommonXMLStructure::SumoBaseObject::addDoubleListAttribute(const SumoXMLAttr attr, const std::vector<double>& value) {
    myDoubleListAttributes[attr] = value;
}


void
CommonXMLStructure::SumoBaseObject::addPositionVectorAttribute(const SumoXMLAttr attr, const PositionVector& value) {
    myPositionVectorAttributes[attr] = value;
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
CommonXMLStructure::SumoBaseObject::setStopParameter(const SUMOVehicleParameter::Stop& stopParameter) {
    myStopParameter = stopParameter;
    myDefinedStopParameter = true;
    // set attribute edge
    if (!myStopParameter.edge.empty()) {
        addStringAttribute(SUMO_ATTR_EDGE, myStopParameter.edge);
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
CommonXMLStructure::SumoBaseObject::setPlanParameters(const CommonXMLStructure::PlanParameters& planParameters) {
    myPlanParameters = planParameters;
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
CommonXMLStructure::SumoBaseObject::handleAttributeError(const SumoXMLAttr attr, const std::string& type) const {
    WRITE_ERRORF(TL("Trying to get undefined % attribute '%' in SUMOBaseObject '%'"), type, toString(attr), toString(myTag));
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
