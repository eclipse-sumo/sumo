/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorFlow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utility>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorVehicleType.h"
#include "GNECalibratorRoute.h"
#include "GNECalibrator.h"
#include "GNECalibratorDialog.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorFlow::GNECalibratorFlow(GNECalibratorDialog* calibratorDialog) :
    GNEAttributeCarrier(SUMO_TAG_FLOW, ICON_EMPTY),
    myCalibratorParent(calibratorDialog->getEditedCalibrator()),
    myFlowID(calibratorDialog->getEditedCalibrator()->getViewNet()->getNet()->generateCalibratorFlowID()),
    myVehicleType(calibratorDialog->getEditedCalibrator()->getCalibratorVehicleTypes().front()),
    myRoute(calibratorDialog->getEditedCalibrator()->getCalibratorRoutes().front()),
    myColor(getDefaultValue<RGBColor>(SUMO_TAG_FLOW, SUMO_ATTR_COLOR)),
    myDepartLane(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_DEPARTLANE)),
    myDepartPos(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_DEPARTPOS)),
    myDepartSpeed(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_DEPARTSPEED)),
    myArrivalLane(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_ARRIVALLANE)),
    myArrivalPos(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_ARRIVALPOS)),
    myArrivalSpeed(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_ARRIVALSPEED)),
    myLine(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_LINE)),
    myPersonNumber(getDefaultValue<int>(SUMO_TAG_FLOW, SUMO_ATTR_PERSON_NUMBER)),
    myContainerNumber(getDefaultValue<int>(SUMO_TAG_FLOW, SUMO_ATTR_CONTAINER_NUMBER)),
    myReroute(getDefaultValue<bool>(SUMO_TAG_FLOW, SUMO_ATTR_CONTAINER_NUMBER)),
    myDepartPosLat(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_DEPARTPOS_LAT)),
    myArrivalPosLat(getDefaultValue<std::string>(SUMO_TAG_FLOW, SUMO_ATTR_ARRIVALPOS_LAT)),
    myBegin(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_BEGIN)),
    myEnd(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_END)),
    myVehsPerHour(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_VEHSPERHOUR)),
    myPeriod(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_PERIOD)),
    myProbability(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_PROB)),
    myNumber(getDefaultValue<int>(SUMO_TAG_FLOW, SUMO_ATTR_NUMBER)),
    myFlowType(GNE_CALIBRATORFLOW_VEHSPERHOUR) {}


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, const std::string& flowID, GNECalibratorVehicleType* vehicleType, GNECalibratorRoute* route,
                                     const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
                                     const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
                                     const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end, double vehsPerHour, double period, double probability,
                                     int number, GNECalibratorFlow::TypeOfFlow flowType) :
    GNEAttributeCarrier(SUMO_TAG_FLOW, ICON_EMPTY),
    myCalibratorParent(calibratorParent),
    myFlowID(flowID),
    myVehicleType(vehicleType),
    myRoute(route),
    myColor(color),
    myDepartLane(departLane),
    myDepartPos(departPos),
    myDepartSpeed(departSpeed),
    myArrivalLane(arrivalLane),
    myArrivalPos(arrivalPos),
    myArrivalSpeed(arrivalSpeed),
    myLine(line),
    myPersonNumber(personNumber),
    myContainerNumber(containerNumber),
    myReroute(reroute),
    myDepartPosLat(departPosLat),
    myArrivalPosLat(arrivalPosLat),
    myBegin(begin),
    myEnd(end),
    myVehsPerHour(vehsPerHour),
    myPeriod(period),
    myProbability(probability),
    myNumber(number),
    myFlowType(flowType) {
}


GNECalibratorFlow::~GNECalibratorFlow() {}


void
GNECalibratorFlow::writeFlow(OutputDevice& device) {
    // Open flow tag
    device.openTag(getTag());
    // Write begin
    device.writeAttr(SUMO_ATTR_BEGIN, myBegin);
    // Write end
    device.writeAttr(SUMO_ATTR_END, myEnd);
    // Write type
    device.writeAttr(SUMO_ATTR_TYPE, myVehicleType);
    // Write route
    device.writeAttr(SUMO_ATTR_ROUTE, myRoute);
    // Write color
    device.writeAttr(SUMO_ATTR_COLOR, myColor);
    // Write depart lane
    device.writeAttr(SUMO_ATTR_DEPARTLANE, myDepartLane);
    // Write depart pos
    device.writeAttr(SUMO_ATTR_DEPARTPOS, myDepartPos);
    // Write depart speed
    device.writeAttr(SUMO_ATTR_DEPARTSPEED, myDepartSpeed);
    // Write arrival lane
    device.writeAttr(SUMO_ATTR_ARRIVALLANE, myArrivalLane);
    // Write arrival pos
    device.writeAttr(SUMO_ATTR_ARRIVALPOS, myArrivalPos);
    // Write arrival speed
    device.writeAttr(SUMO_ATTR_ARRIVALSPEED, myArrivalSpeed);
    // Write line
    device.writeAttr(SUMO_ATTR_LINE, myLine);
    // Write person number
    device.writeAttr(SUMO_ATTR_PERSON_NUMBER, myPersonNumber);
    // Write container number
    device.writeAttr(SUMO_ATTR_CONTAINER_NUMBER, myContainerNumber);
    // Write reroute
    device.writeAttr(SUMO_ATTR_REROUTE, myReroute);
    // Write departPosLat
    device.writeAttr(SUMO_ATTR_DEPARTPOS_LAT, myDepartPosLat);
    // Write arrivalPosLat
    device.writeAttr(SUMO_ATTR_ARRIVALPOS_LAT, myArrivalPosLat);
    // Write number
    device.writeAttr(SUMO_ATTR_NUMBER, myNumber);
    // Write type of flow
    if (myFlowType == GNECalibratorFlow::GNE_CALIBRATORFLOW_PERIOD) {
        // write period
        device.writeAttr(SUMO_ATTR_PERIOD, myPeriod);
    } else if (myFlowType == GNECalibratorFlow::GNE_CALIBRATORFLOW_VEHSPERHOUR) {
        // write vehs per hour
        device.writeAttr(SUMO_ATTR_VEHSPERHOUR, myVehsPerHour);
    } else if (myFlowType == GNECalibratorFlow::GNE_CALIBRATORFLOW_PROBABILITY) {
        // write probability
        device.writeAttr(SUMO_ATTR_PROB, myProbability);
    }
    // Close flow tag
    device.closeTag();
}


GNECalibrator*
GNECalibratorFlow::getCalibratorParent() const {
    return myCalibratorParent;
}


GNECalibratorFlow::TypeOfFlow
GNECalibratorFlow::getFlowType() const {
    return myFlowType;
}


void
GNECalibratorFlow::setFlowType(GNECalibratorFlow::TypeOfFlow type) {
    myFlowType = type;
}


std::string
GNECalibratorFlow::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myFlowID;
        case SUMO_ATTR_TYPE:
            return myVehicleType->getID();
        case SUMO_ATTR_ROUTE:
            return myRoute->getID();
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        case SUMO_ATTR_END:
            return toString(myEnd);
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(myVehsPerHour);
        case SUMO_ATTR_PERIOD:
            return toString(myPeriod);
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_NUMBER:
            return toString(myNumber);
        case SUMO_ATTR_DEPARTLANE:
            return myDepartLane;
        case SUMO_ATTR_DEPARTPOS:
            return myDepartPos;
        case SUMO_ATTR_DEPARTSPEED:
            return myDepartSpeed;
        case SUMO_ATTR_ARRIVALLANE:
            return myArrivalLane;
        case SUMO_ATTR_ARRIVALPOS:
            return myArrivalPos;
        case SUMO_ATTR_ARRIVALSPEED:
            return myArrivalSpeed;
        case SUMO_ATTR_LINE:
            return myLine;
        case SUMO_ATTR_PERSON_NUMBER:
            return toString(myPersonNumber);
        case SUMO_ATTR_CONTAINER_NUMBER:
            return toString(myContainerNumber);
        case SUMO_ATTR_REROUTE:
            return toString(myReroute);
        case SUMO_ATTR_DEPARTPOS_LAT:
            return myDepartPosLat;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            return myArrivalPosLat;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibratorFlow::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_ROUTE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_NUMBER:
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
        case SUMO_ATTR_DEPARTPOS_LAT:
        case SUMO_ATTR_ARRIVALPOS_LAT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECalibratorFlow::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorFlow(value, false) == NULL);
        case SUMO_ATTR_TYPE:
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorVehicleType(value, false) != NULL);
        case SUMO_ATTR_ROUTE:
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorRoute(value, false) != NULL);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_END:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_VEHSPERHOUR:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_PERIOD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_PROB:
            return canParse<double>(value)  && (parse<double>(value) >= 0) && (parse<double>(value) <= 1);
        case SUMO_ATTR_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_DEPARTLANE:
            if ((value == "random") || (value == "free") || (value == "allowed") || (value == "best") || (value == "first")) {
                return true;
            } else {
                return (myCalibratorParent->getViewNet()->getNet()->retrieveLane(value, false) != NULL);
            }
        case SUMO_ATTR_DEPARTPOS:
            if ((value == "random") || (value == "free") || (value == "random_free") || (value == "base") || (value == "last")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_DEPARTSPEED:
            if ((value == "random") || (value == "max")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_ARRIVALLANE:
            if (value == "current") {
                return true;
            } else {
                return (myCalibratorParent->getViewNet()->getNet()->retrieveLane(value, false) != NULL);
            }
        case SUMO_ATTR_ARRIVALPOS:
            if ((value == "random") || (value == "max")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_ARRIVALSPEED:
            if (value == "current") {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_LINE:
            return true;
        case SUMO_ATTR_PERSON_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_CONTAINER_NUMBER:
            return canParse<int>(value) && parse<int>(value) >= 0;
        case SUMO_ATTR_REROUTE:
            return canParse<bool>(value);
        case SUMO_ATTR_DEPARTPOS_LAT:
            if ((value == "left") || (value == "right") || (value == "center") || (value == "compact") || (value == "nice") || (value == "arbitrary")) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if ((value == "") || (value == "left") || (value == "right") || (value == "center")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibratorFlow::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myFlowID;
            myFlowID = value;
            myCalibratorParent->getViewNet()->getNet()->changeCalibratorFlowID(this, oldID);
            break;
        }
        case SUMO_ATTR_TYPE:
            myVehicleType = myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorVehicleType(value);
            break;
        case SUMO_ATTR_ROUTE:
            myRoute = myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorRoute(value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<double>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<double>(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            myVehsPerHour = parse<double>(value);
            break;
        case SUMO_ATTR_PERIOD:
            myPeriod = parse<double>(value);
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<double>(value);
            break;
        case SUMO_ATTR_NUMBER:
            myNumber = parse<int>(value);
            break;
        case SUMO_ATTR_DEPARTLANE:
            myDepartLane = value;
            break;
        case SUMO_ATTR_DEPARTPOS:
            myDepartPos = value;
            break;
        case SUMO_ATTR_DEPARTSPEED:
            myDepartSpeed = value;
            break;
        case SUMO_ATTR_ARRIVALLANE:
            myArrivalLane = value;
            break;
        case SUMO_ATTR_ARRIVALPOS:
            myArrivalPos = value;
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            myArrivalSpeed = value;
            break;
        case SUMO_ATTR_LINE:
            myLine = value;
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            myPersonNumber = parse<int>(value);
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            myContainerNumber = parse<int>(value);
            break;
        case SUMO_ATTR_REROUTE:
            myReroute = parse<bool>(value);
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            myDepartPosLat = value;
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            myArrivalPosLat = value;
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
