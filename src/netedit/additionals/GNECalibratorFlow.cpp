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
#include <config.h>

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


GNECalibratorFlow::GNECalibratorFlow(GNECalibratorDialog* calibratorDialog) :
    GNEAdditional(calibratorDialog->getEditedCalibrator(), calibratorDialog->getEditedCalibrator()->getViewNet(), GLO_CALIBRATOR, SUMO_TAG_FLOW, false, false),
    myVehicleType(calibratorDialog->getEditedCalibrator()->getViewNet()->getNet()->retrieveCalibratorVehicleType(DEFAULT_VTYPE_ID)),
    myRoute(nullptr),
    myVehsPerHour(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_VEHSPERHOUR)),
    mySpeed(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_SPEED)),
    myColor(parse<RGBColor>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_COLOR))),
    myDepartLane(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_DEPARTLANE)),
    myDepartPos(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_DEPARTPOS)),
    myDepartSpeed(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_DEPARTSPEED)),
    myArrivalLane(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_ARRIVALLANE)),
    myArrivalPos(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_ARRIVALPOS)),
    myArrivalSpeed(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_ARRIVALSPEED)),
    myLine(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_LINE)),
    myPersonNumber(parse<int>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_PERSON_NUMBER))),
    myContainerNumber(parse<int>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_CONTAINER_NUMBER))),
    myReroute(parse<bool>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_CONTAINER_NUMBER))),
    myDepartPosLat(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_DEPARTPOS_LAT)),
    myArrivalPosLat(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_ARRIVALPOS_LAT)),
    myBegin(parse<double>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_BEGIN))),
    myEnd(parse<double>(getTagProperties(SUMO_TAG_FLOW).getDefaultValue(SUMO_ATTR_END))) {
    // set route with the first route founded in additional parent
    for(auto i : myAdditionalParent->getAdditionalChilds()) {
        if(!myRoute && (i->getTag() == SUMO_TAG_ROUTE)) {
            myRoute = i;
        }
    }
}


GNECalibratorFlow::GNECalibratorFlow(GNECalibrator* calibratorParent, GNECalibratorVehicleType* vehicleType, GNECalibratorRoute* route, const std::string &vehsPerHour, const std::string &speed,
                                     const RGBColor& color, const std::string& departLane, const std::string& departPos, const std::string& departSpeed, const std::string& arrivalLane,
                                     const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, int personNumber, int containerNumber, bool reroute,
                                     const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end) :
    GNEAdditional(calibratorParent, calibratorParent->getViewNet(), GLO_CALIBRATOR, SUMO_TAG_FLOW, false, false),
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


GNECalibratorFlow::~GNECalibratorFlow() {}


void 
GNECalibratorFlow::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void 
GNECalibratorFlow::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void 
GNECalibratorFlow::updateGeometry() {
    // Currently this additional doesn't own a Geometry
}


Position 
GNECalibratorFlow::getPositionInView() const {
    return Position();
}


std::string 
GNECalibratorFlow::getParentName() const {
    return myAdditionalParent->getID();
}


void 
GNECalibratorFlow::drawGL(const GUIVisualizationSettings& s) const {
    // Currently This additional isn't drawn
}


std::string
GNECalibratorFlow::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
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
        case SUMO_ATTR_ID:
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
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_TYPE:
            return isValidID(value) && (myViewNet->getNet()->retrieveCalibratorVehicleType(value, false) != nullptr);
        case SUMO_ATTR_ROUTE:
            return isValidID(value) && (myViewNet->getNet()->retrieveCalibratorRoute(value, false) != nullptr);
        case SUMO_ATTR_VEHSPERHOUR:
            if(value.empty()) {
                // speed and vehsPerHour cannot be empty at the same time
                if(mySpeed.empty()) {
                    return false;
                } else {
                    return true;
                }
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }
        case SUMO_ATTR_SPEED:
            if(value.empty()) {
                // speed and vehsPerHour cannot be empty at the same time
                if(myVehsPerHour.empty()) {
                    return false;
                } else {
                    return true;
                }
            } else if (canParse<double>(value)) {
                return (parse<double>(value) >= 0);
            } else {
                return false;
            }        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_END:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_DEPARTLANE:
            if ((value == "random") || (value == "free") || (value == "allowed") || (value == "best") || (value == "first")) {
                return true;
            } else {
                return (myViewNet->getNet()->retrieveLane(value, false) != nullptr);
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
                return (myViewNet->getNet()->retrieveLane(value, false) != nullptr);
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
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_TYPE:
            myVehicleType = myViewNet->getNet()->retrieveCalibratorVehicleType(value);
            break;
        case SUMO_ATTR_ROUTE:
            myRoute = myViewNet->getNet()->retrieveCalibratorRoute(value);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            myVehsPerHour = value;
            break;
        case SUMO_ATTR_SPEED:
            mySpeed = value;
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
