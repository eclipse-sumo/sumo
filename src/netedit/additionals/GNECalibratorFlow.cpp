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
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>

#include "GNECalibratorFlow.h"
#include "GNECalibratorVehicleType.h"
#include "GNECalibratorRoute.h"
#include "GNECalibrator.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNECalibratorFlow::GNECalibratorFlow(GNECalibratorDialog* calibratorDialog, GNENet* net) :
    GNEAttributeCarrier(SUMO_TAG_FLOW, ICON_EMPTY),
    myCalibratorParent(calibratorDialog->getEditedCalibrator()),
    myVehicleType(net->retrieveCalibratorVehicleType(DEFAULT_VTYPE_ID)),
    myRoute(calibratorDialog->getEditedCalibrator()->getCalibratorRoutes().front()),
    myVehsPerHour(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_VEHSPERHOUR)),
    mySpeed(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_SPEED)),
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
    myEnd(getDefaultValue<double>(SUMO_TAG_FLOW, SUMO_ATTR_END)) {}


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, GNECalibratorVehicleType* vehicleType, GNECalibratorRoute* route, double vehsPerHour, double speed,
                                     const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
                                     const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
                                     const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end) :
    GNEAttributeCarrier(SUMO_TAG_FLOW, ICON_EMPTY),
    myCalibratorParent(calibratorParent),
    myVehicleType(vehicleType),
    myRoute(route),
    myVehsPerHour(vehsPerHour),
    mySpeed(speed),
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
    myEnd(end) {
}


GNECalibratorFlow::~GNECalibratorFlow() {
    if(myCalibratorParent->calibratorFlowExist(this, false)) {
        myCalibratorParent->removeCalibratorFlow(this);
    }
}


void
GNECalibratorFlow::writeFlow(OutputDevice& device) {
    // Open flow tag
    device.openTag(getTag());
    // Write begin
    writeAttribute(device, SUMO_ATTR_BEGIN);
    // Write end
    writeAttribute(device, SUMO_ATTR_END);
    // Write type
    writeAttribute(device, SUMO_ATTR_TYPE);
    // Write route
    writeAttribute(device, SUMO_ATTR_ROUTE);
    // write vehs per hour only if is different of -1
    if(myVehsPerHour > 0) {
        writeAttribute(device, SUMO_ATTR_VEHSPERHOUR);
    }
    // write speed only if is different of -1
    if(mySpeed > 0) {
        writeAttribute(device, SUMO_ATTR_SPEED);
    }
    // Write color
    writeAttribute(device, SUMO_ATTR_COLOR);
    // Write depart lane
    writeAttribute(device, SUMO_ATTR_DEPARTLANE);
    // Write depart pos
    writeAttribute(device, SUMO_ATTR_DEPARTPOS);
    // Write depart speed
    writeAttribute(device, SUMO_ATTR_DEPARTSPEED);
    // Write arrival lane
    writeAttribute(device, SUMO_ATTR_ARRIVALLANE);
    // Write arrival pos
    writeAttribute(device, SUMO_ATTR_ARRIVALPOS);
    // Write arrival speed
    writeAttribute(device, SUMO_ATTR_ARRIVALSPEED);
    // Write line
    writeAttribute(device, SUMO_ATTR_LINE);
    // Write person number
    writeAttribute(device, SUMO_ATTR_PERSON_NUMBER);
    // Write container number
    writeAttribute(device, SUMO_ATTR_CONTAINER_NUMBER);
    // Write reroute
    writeAttribute(device, SUMO_ATTR_REROUTE);
    // Write departPosLat
    writeAttribute(device, SUMO_ATTR_DEPARTPOS_LAT);
    // Write arrivalPosLat
    writeAttribute(device, SUMO_ATTR_ARRIVALPOS_LAT);
    // Close flow tag
    device.closeTag();
}


GNECalibrator*
GNECalibratorFlow::getCalibratorParent() const {
    return myCalibratorParent;
}


void 
GNECalibratorFlow::selectAttributeCarrier(bool) {
    // this AC cannot be selected
}


void 
GNECalibratorFlow::unselectAttributeCarrier(bool) {
    // this AC cannot be unselected
}


bool 
GNECalibratorFlow::isAttributeCarrierSelected() const {
    // this AC doesn't own a select flag
    return false;
}


std::string
GNECalibratorFlow::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myCalibratorParent->getID() + "_CalibratorFlow_" + toString(myCalibratorParent->getCalibratorFlowIndex(this));
        case SUMO_ATTR_TYPE:
            return myVehicleType->getID();
        case SUMO_ATTR_ROUTE:
            return myRoute->getID();
        case SUMO_ATTR_VEHSPERHOUR:
            return toString(myVehsPerHour);
        case SUMO_ATTR_SPEED:
            return toString(mySpeed);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        case SUMO_ATTR_END:
            return toString(myEnd);
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
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_ROUTE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_VEHSPERHOUR:
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
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
        case SUMO_ATTR_TYPE:
            return isValidID(value) && (value == DEFAULT_VTYPE_ID || (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorVehicleType(value, false) != nullptr));
        case SUMO_ATTR_ROUTE:
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorRoute(value, false) != nullptr);
        case SUMO_ATTR_VEHSPERHOUR:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_SPEED:
            return canParse<double>(value) && (parse<double>(value) >= -1);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_END:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_DEPARTLANE:
            if ((value == "random") || (value == "free") || (value == "allowed") || (value == "best") || (value == "first")) {
                return true;
            } else {
                return (myCalibratorParent->getViewNet()->getNet()->retrieveLane(value, false) != nullptr);
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
                return (myCalibratorParent->getViewNet()->getNet()->retrieveLane(value, false) != nullptr);
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
        case SUMO_ATTR_TYPE:
            myVehicleType = myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorVehicleType(value);
            break;
        case SUMO_ATTR_ROUTE:
            myRoute = myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorRoute(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            myVehsPerHour = parse<double>(value);
            break;
        case SUMO_ATTR_SPEED:
            mySpeed = parse<double>(value);
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
