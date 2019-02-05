/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicle.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Representation of vehicles in NETEDIT
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/options/OptionsCont.h>

#include "GNEVehicle.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const std::string &vehicleID, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleID, viewNet, GLO_VEHICLE, SUMO_TAG_VEHICLE),
    SUMOVehicleParameter(),
    myVehicleType(vehicleType),
    myRoute(route) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleID;
}


GNEVehicle::GNEVehicle(GNEViewNet* viewNet, const SUMOVehicleParameter &vehicleParameter, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEDemandElement(vehicleParameter.id, viewNet, GLO_VEHICLE, SUMO_TAG_VEHICLE),
    SUMOVehicleParameter(vehicleParameter),
    myVehicleType(vehicleType),
    myRoute(route) {
    // SUMOVehicleParameter ID has to be set manually
    id = vehicleParameter.id;
}


GNEVehicle::~GNEVehicle() {}


void 
GNEVehicle::writeDemandElement(OutputDevice& device) const {
    write(device, OptionsCont::getOptions(), SUMO_TAG_VEHICLE);
    // write manually type and route
    device.writeAttr(SUMO_ATTR_TYPE, myVehicleType->getID());
    device.writeAttr(SUMO_ATTR_ROUTE, myRoute->getID());
    // close vehicle tag
    device.closeTag();
}


void
GNEVehicle::moveGeometry(const Position&) {
    // This demand element cannot be moved
}


void
GNEVehicle::commitGeometryMoving(GNEUndoList*) {
    // This demand element cannot be moved
}


void
GNEVehicle::updateGeometry(bool /*updateGrid*/) {
    // Currently this demand element doesn't own a Geometry
}


Position
GNEVehicle::getPositionInView() const {
    return myFirstDemandElementParent->getPositionInView();
}


std::string
GNEVehicle::getParentName() const {
    return myFirstDemandElementParent->getID();
}


void
GNEVehicle::drawGL(const GUIVisualizationSettings& /* s */) const {
    // Currently This demand element isn't drawn
}


void
GNEVehicle::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEVehicle::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_VEHICLE);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEVehicle::getAttribute(SumoXMLAttr key) const {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        case SUMO_ATTR_TYPE:
            return myVehicleType->getID();
        case SUMO_ATTR_ROUTE:
            return myRoute->getID();
        case SUMO_ATTR_COLOR:
            return toString(color);
        case SUMO_ATTR_DEPARTLANE:
            return getDepartLane();
        case SUMO_ATTR_DEPARTPOS:
            return getDepartPos();
        case SUMO_ATTR_DEPARTSPEED:
            return getDepartSpeed();
        case SUMO_ATTR_ARRIVALLANE:
            return getArrivalLane();
        case SUMO_ATTR_ARRIVALPOS:
            return getArrivalPos();
        case SUMO_ATTR_ARRIVALSPEED:
            return getArrivalSpeed();
        case SUMO_ATTR_LINE:
            return line;
        case SUMO_ATTR_PERSON_NUMBER:
            return toString(personNumber);
        case SUMO_ATTR_CONTAINER_NUMBER:
            return toString(containerNumber);
        case SUMO_ATTR_REROUTE:
            return toString("false"); // check
        case SUMO_ATTR_VIA:
            return toString(""); // check
        case SUMO_ATTR_DEPARTPOS_LAT:
            return getDepartPosLat();
        case SUMO_ATTR_ARRIVALPOS_LAT:
            return getArrivalPosLat();
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            return toString(depart);
        //
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_ROUTE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_DEPARTLANE:
        case SUMO_ATTR_DEPARTPOS:
        case SUMO_ATTR_DEPARTSPEED:
        case SUMO_ATTR_ARRIVALLANE:
        case SUMO_ATTR_ARRIVALPOS:
        case SUMO_ATTR_ARRIVALSPEED:
        case SUMO_ATTR_LINE:
        case SUMO_ATTR_PERSON_NUMBER:
        case SUMO_ATTR_CONTAINER_NUMBER:
        case SUMO_ATTR_REROUTE:
        case SUMO_ATTR_VIA:
        case SUMO_ATTR_DEPARTPOS_LAT:
        case SUMO_ATTR_ARRIVALPOS_LAT:
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
        //
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVehicle::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
        case SUMO_ATTR_TYPE:
            return SUMOXMLDefinitions::isValidTypeID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value, false) != nullptr);
        case SUMO_ATTR_ROUTE:
            return SUMOXMLDefinitions::isValidVehicleID(value) && (myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value, false) != nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_DEPARTLANE: {
            int dummyDepartLane;
            DepartLaneDefinition dummyDepartLaneProcedure;
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartLane, dummyDepartLaneProcedure, error); 
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTPOS: {
            double dummyDepartPos;
            DepartPosDefinition dummyDepartPosProcedure;
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPos, dummyDepartPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_DEPARTSPEED: {
            double dummyDepartSpeed;
            DepartSpeedDefinition dummyDepartSpeedProcedure;
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartSpeed, dummyDepartSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALLANE: {
            int dummyArrivalLane;
            ArrivalLaneDefinition dummyArrivalLaneProcedure;
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalLane, dummyArrivalLaneProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS: {
            double dummyArrivalPos;
            ArrivalPosDefinition dummyArrivalPosProcedure;
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPos, dummyArrivalPosProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALSPEED: {
            double dummyArrivalSpeed;
            ArrivalSpeedDefinition dummyArrivalSpeedProcedure;
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalSpeed, dummyArrivalSpeedProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_PERSON_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_CONTAINER_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_REROUTE:
            return true;    // check
        case SUMO_ATTR_VIA:
            return true;    // check
        case SUMO_ATTR_DEPARTPOS_LAT: {
            double dummyDepartPosLat;
            DepartPosLatDefinition dummyDepartPosLatProcedure;
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyDepartPosLat, dummyDepartPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        case SUMO_ATTR_ARRIVALPOS_LAT: {
            double dummyArrivalPosLat;
            ArrivalPosLatDefinition dummyArrivalPosLatProcedure;
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, dummyArrivalPosLat, dummyArrivalPosLatProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        // Specific of vehicles
        case SUMO_ATTR_DEPART: {
            SUMOTime dummyDepart;
            DepartDefinition dummyDepartProcedure;
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, dummyDepart, dummyDepartProcedure, error);
            // if error is empty, given value is valid
            return error.empty();
        }
        //
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEVehicle::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicle::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicle::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        case SUMO_ATTR_TYPE:
            myVehicleType = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_VTYPE, value);
            break;
        case SUMO_ATTR_ROUTE:
            myRoute = myViewNet->getNet()->retrieveDemandElement(SUMO_TAG_ROUTE, value);
            break;
        case SUMO_ATTR_COLOR:
            color = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_DEPARTLANE:
            parseDepartLane(value, toString(SUMO_TAG_VEHICLE), id, departLane, departLaneProcedure, error); 
            break;       
        case SUMO_ATTR_DEPARTPOS:
            parseDepartPos(value, toString(SUMO_TAG_VEHICLE), id, departPos, departPosProcedure, error);
            break;
        case SUMO_ATTR_DEPARTSPEED:
            parseDepartSpeed(value, toString(SUMO_TAG_VEHICLE), id, departSpeed, departSpeedProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALLANE:
            parseArrivalLane(value, toString(SUMO_TAG_VEHICLE), id, arrivalLane, arrivalLaneProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            parseArrivalPos(value, toString(SUMO_TAG_VEHICLE), id, arrivalPos, arrivalPosProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            parseArrivalSpeed(value, toString(SUMO_TAG_VEHICLE), id, arrivalSpeed, arrivalSpeedProcedure, error);
            break;
        case SUMO_ATTR_LINE:
            line = value;
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            personNumber = parse<int>(value);
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            containerNumber = parse<int>(value);
            break;
        case SUMO_ATTR_REROUTE:
            // check
            break;
        case SUMO_ATTR_VIA:
            // check
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            parseDepartPosLat(value, toString(SUMO_TAG_VEHICLE), id, departPosLat, departPosLatProcedure, error);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            parseArrivalPosLat(value, toString(SUMO_TAG_VEHICLE), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;        
        // Specific of vehicles
        case SUMO_ATTR_DEPART:
            parseDepart(value, toString(SUMO_TAG_VEHICLE), id, depart, departProcedure, error);
            break;     
        //
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
