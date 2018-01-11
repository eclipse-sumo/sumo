/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorVehicleType.cpp
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

#include "GNECalibratorVehicleType.h"
#include "GNECalibrator.h"
#include "GNECalibratorDialog.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEChange_Attribute.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibratorDialog* calibratorDialog) :
    GNEAttributeCarrier(SUMO_TAG_VTYPE, ICON_EMPTY),
    myCalibratorParent(calibratorDialog->getEditedCalibrator()),
    myVehicleTypeID(calibratorDialog->getEditedCalibrator()->getViewNet()->getNet()->generateCalibratorVehicleTypeID()),
    myAccel(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_ACCEL)),
    myDecel(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_DECEL)),
    mySigma(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_SIGMA)),
    myTau(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_TAU)),
    myLength(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_LENGTH)),
    myMinGap(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_MINGAP)),
    myMaxSpeed(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_MAXSPEED)),
    mySpeedFactor(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_SPEEDFACTOR)),
    mySpeedDev(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_SPEEDDEV)),
    myColor(getDefaultValue<RGBColor>(SUMO_TAG_VTYPE, SUMO_ATTR_COLOR)),
    myVClass(getDefaultValue<SUMOVehicleClass>(SUMO_TAG_VTYPE, SUMO_ATTR_VCLASS)),
    myEmissionClass(getDefaultValue<std::string>(SUMO_TAG_VTYPE, SUMO_ATTR_EMISSIONCLASS)),
    myShape(getDefaultValue<SUMOVehicleShape>(SUMO_TAG_VTYPE, SUMO_ATTR_GUISHAPE)),
    myWidth(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_WIDTH)),
    myFilename(getDefaultValue<std::string>(SUMO_TAG_VTYPE, SUMO_ATTR_IMGFILE)),
    myImpatience(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_IMPATIENCE)),
    myLaneChangeModel(getDefaultValue<std::string>(SUMO_TAG_VTYPE, SUMO_ATTR_LANE_CHANGE_MODEL)),
    myCarFollowModel(getDefaultValue<std::string>(SUMO_TAG_VTYPE, SUMO_ATTR_CAR_FOLLOW_MODEL)),
    myPersonCapacity(getDefaultValue<int>(SUMO_TAG_VTYPE, SUMO_ATTR_PERSON_CAPACITY)),
    myContainerCapacity(getDefaultValue<int>(SUMO_TAG_VTYPE, SUMO_ATTR_CONTAINER_CAPACITY)),
    myBoardingDuration(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_BOARDING_DURATION)),
    myLoadingDuration(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_LOADING_DURATION)),
    myLatAlignment(getDefaultValue<std::string>(SUMO_TAG_VTYPE, SUMO_ATTR_LATALIGNMENT)),
    myMinGapLat(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_MINGAP_LAT)),
    myMaxSpeedLat(getDefaultValue<double>(SUMO_TAG_VTYPE, SUMO_ATTR_MAXSPEED_LAT)) {
}


GNECalibratorVehicleType::GNECalibratorVehicleType(GNECalibrator* calibratorParent, std::string vehicleTypeID,
        double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
        double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
        SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
        const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
        double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat) :
    GNEAttributeCarrier(SUMO_TAG_VTYPE, ICON_EMPTY),
    myCalibratorParent(calibratorParent),
    myVehicleTypeID(vehicleTypeID),
    myAccel(accel),
    myDecel(decel),
    mySigma(sigma),
    myTau(tau),
    myLength(length),
    myMinGap(minGap),
    myMaxSpeed(maxSpeed),
    mySpeedFactor(speedFactor),
    mySpeedDev(speedDev),
    myColor(color),
    myVClass(vClass),
    myEmissionClass(emissionClass),
    myShape(shape),
    myWidth(width),
    myFilename(filename),
    myImpatience(impatience),
    myLaneChangeModel(laneChangeModel),
    myCarFollowModel(carFollowModel),
    myPersonCapacity(personCapacity),
    myContainerCapacity(containerCapacity),
    myBoardingDuration(boardingDuration),
    myLoadingDuration(loadingDuration),
    myLatAlignment(latAlignment),
    myMinGapLat(minGapLat),
    myMaxSpeedLat(maxSpeedLat) {
}


GNECalibratorVehicleType::~GNECalibratorVehicleType() {}


void
GNECalibratorVehicleType::writeVehicleType(OutputDevice& device) {
    // Open vehicle type tag
    device.openTag(getTag());
    // write id
    device.writeAttr(SUMO_ATTR_ID, myVehicleTypeID);
    //write accel
    device.writeAttr(SUMO_ATTR_ACCEL, myAccel);
    // write decel
    device.writeAttr(SUMO_ATTR_DECEL, myDecel);
    // write sigma
    device.writeAttr(SUMO_ATTR_SIGMA, mySigma);
    // write tau
    device.writeAttr(SUMO_ATTR_TAU, myTau);
    // write lenght
    device.writeAttr(SUMO_ATTR_LENGTH, myLength);
    // write min gap
    device.writeAttr(SUMO_ATTR_MINGAP, myMinGap);
    // write max speed
    device.writeAttr(SUMO_ATTR_MAXSPEED, myMaxSpeed);
    // write speed factor
    device.writeAttr(SUMO_ATTR_SPEEDFACTOR, mySpeedFactor);
    // write speed dev
    device.writeAttr(SUMO_ATTR_SPEEDDEV, mySpeedDev);
    // write color
    device.writeAttr(SUMO_ATTR_COLOR, myColor);
    // write vehicle class
    device.writeAttr(SUMO_ATTR_VCLASS, myVClass);
    // write emission class
    device.writeAttr(SUMO_ATTR_EMISSIONCLASS, myEmissionClass);
    // write shape
    device.writeAttr(SUMO_ATTR_GUISHAPE, myShape);
    // write width
    device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    // write filename
    device.writeAttr(SUMO_ATTR_IMGFILE, myFilename);
    // write impatience
    device.writeAttr(SUMO_ATTR_IMPATIENCE, myImpatience);
    // write lane change model
    device.writeAttr(SUMO_ATTR_LANE_CHANGE_MODEL, myLaneChangeModel);
    // write car follow model
    device.writeAttr(SUMO_ATTR_CAR_FOLLOW_MODEL, myCarFollowModel);
    // write person capacity
    device.writeAttr(SUMO_ATTR_PERSON_CAPACITY, myPersonCapacity);
    // write container capacity
    device.writeAttr(SUMO_ATTR_CONTAINER_CAPACITY, myContainerCapacity);
    // write boarding duration
    device.writeAttr(SUMO_ATTR_BOARDING_DURATION, myBoardingDuration);
    // write loading duration
    device.writeAttr(SUMO_ATTR_LOADING_DURATION, myLoadingDuration);
    // write get lat alignment
    device.writeAttr(SUMO_ATTR_LATALIGNMENT, myLatAlignment);
    // write min gap lat
    device.writeAttr(SUMO_ATTR_MINGAP_LAT, myMinGapLat);
    // write max speed lat
    device.writeAttr(SUMO_ATTR_MAXSPEED_LAT, myMaxSpeedLat);
    // Close vehicle type tag
    device.closeTag();
}


GNECalibrator*
GNECalibratorVehicleType::getCalibratorParent() const {
    return myCalibratorParent;
}


std::string
GNECalibratorVehicleType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myVehicleTypeID;
        case SUMO_ATTR_ACCEL:
            return toString(myAccel);
        case SUMO_ATTR_DECEL:
            return toString(myDecel);
        case SUMO_ATTR_SIGMA:
            return toString(mySigma);
        case SUMO_ATTR_TAU:
            return toString(myTau);
        case SUMO_ATTR_LENGTH:
            return toString(myLength);
        case SUMO_ATTR_MINGAP:
            return toString(myMinGap);
        case SUMO_ATTR_MAXSPEED:
            return toString(myMaxSpeed);
        case SUMO_ATTR_SPEEDFACTOR:
            return toString(mySpeedFactor);
        case SUMO_ATTR_SPEEDDEV:
            return toString(mySpeedDev);
        case SUMO_ATTR_COLOR:
            return toString(myColor);
        case SUMO_ATTR_VCLASS:
            return toString(myVClass);
        case SUMO_ATTR_EMISSIONCLASS:
            return myEmissionClass;
        case SUMO_ATTR_GUISHAPE:
            return getVehicleShapeName(myShape);
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_IMGFILE:
            return myFilename;
        case SUMO_ATTR_IMPATIENCE:
            return toString(myImpatience);
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            return myLaneChangeModel;
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            return myCarFollowModel;
        case SUMO_ATTR_PERSON_CAPACITY:
            return toString(myPersonCapacity);
        case SUMO_ATTR_CONTAINER_CAPACITY:
            return toString(myContainerCapacity);
        case SUMO_ATTR_BOARDING_DURATION:
            return toString(myBoardingDuration);
        case SUMO_ATTR_LOADING_DURATION:
            return toString(myLoadingDuration);
        case SUMO_ATTR_LATALIGNMENT:
            return myLatAlignment;
        case SUMO_ATTR_MINGAP_LAT:
            return toString(myMinGapLat);
        case SUMO_ATTR_MAXSPEED_LAT:
            return toString(myMaxSpeedLat);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibratorVehicleType::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_ACCEL:
        case SUMO_ATTR_DECEL:
        case SUMO_ATTR_SIGMA:
        case SUMO_ATTR_TAU:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_MINGAP:
        case SUMO_ATTR_MAXSPEED:
        case SUMO_ATTR_SPEEDFACTOR:
        case SUMO_ATTR_SPEEDDEV:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_VCLASS:
        case SUMO_ATTR_EMISSIONCLASS:
        case SUMO_ATTR_GUISHAPE:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_IMPATIENCE:
        case SUMO_ATTR_LANE_CHANGE_MODEL:
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
        case SUMO_ATTR_PERSON_CAPACITY:
        case SUMO_ATTR_CONTAINER_CAPACITY:
        case SUMO_ATTR_BOARDING_DURATION:
        case SUMO_ATTR_LOADING_DURATION:
        case SUMO_ATTR_LATALIGNMENT:
        case SUMO_ATTR_MINGAP_LAT:
        case SUMO_ATTR_MAXSPEED_LAT:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNECalibratorVehicleType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidID(value) && (myCalibratorParent->getViewNet()->getNet()->retrieveCalibratorVehicleType(value, false) == NULL);
        case SUMO_ATTR_ACCEL:
            return canParse<double>(value);
        case SUMO_ATTR_DECEL:
            return canParse<double>(value);
        case SUMO_ATTR_SIGMA:
            return canParse<double>(value);
        case SUMO_ATTR_TAU:
            return canParse<double>(value);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value);
        case SUMO_ATTR_MINGAP:
            return canParse<double>(value);
        case SUMO_ATTR_MAXSPEED:
            return canParse<double>(value);
        case SUMO_ATTR_SPEEDFACTOR:
            return canParse<double>(value);
        case SUMO_ATTR_SPEEDDEV:
            return canParse<double>(value);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_VCLASS:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_EMISSIONCLASS:
            return true;
        case SUMO_ATTR_GUISHAPE:
            return canParseVehicleShape(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value);
        case SUMO_ATTR_IMGFILE:
            return isValidFilename(value);
        case SUMO_ATTR_IMPATIENCE:
            return canParse<double>(value);
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            return (value == "LC2013") || (value == "SL2015") || (value == "DK2008");
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            return (value == "Krauss") || (value == "KraussOrig1")  || (value == "PWagner2009")  ||
                   (value == "BKerner")  || (value == "IDM")  || (value == "IDMM")  || (value == "KraussPS")  ||
                   (value == "KraussAB") || (value == "SmartSK") || (value == "Wiedemann") || (value == "Daniel1");
        case SUMO_ATTR_PERSON_CAPACITY:
            return canParse<int>(value);
        case SUMO_ATTR_CONTAINER_CAPACITY:
            return canParse<int>(value);
        case SUMO_ATTR_BOARDING_DURATION:
            return canParse<double>(value);
        case SUMO_ATTR_LOADING_DURATION:
            return canParse<double>(value);
        case SUMO_ATTR_LATALIGNMENT:
            if ((value == "") || (value == "left") || (value == "right") || (value == "center")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_MINGAP_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_MAXSPEED_LAT:
            return canParse<double>(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibratorVehicleType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            std::string oldID = myVehicleTypeID;
            myVehicleTypeID = value;
            myCalibratorParent->getViewNet()->getNet()->changeCalibratorVehicleTypeID(this, oldID);
            break;
        }
        case SUMO_ATTR_ACCEL:
            myAccel = parse<double>(value);
            break;
        case SUMO_ATTR_DECEL:
            myDecel = parse<double>(value);
            break;
        case SUMO_ATTR_SIGMA:
            mySigma = parse<double>(value);
            break;
        case SUMO_ATTR_TAU:
            myTau = parse<double>(value);
            break;
        case SUMO_ATTR_LENGTH:
            myLength = parse<double>(value);
            break;
        case SUMO_ATTR_MINGAP:
            myMinGap = parse<double>(value);
            break;
        case SUMO_ATTR_MAXSPEED:
            myMaxSpeed = parse<double>(value);
            break;
        case SUMO_ATTR_SPEEDFACTOR:
            mySpeedFactor = parse<double>(value);
            break;
        case SUMO_ATTR_SPEEDDEV:
            mySpeedDev = parse<double>(value);
            break;
        case SUMO_ATTR_COLOR:
            myColor = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_VCLASS:
            myVClass = getVehicleClassID(value);
            break;
        case SUMO_ATTR_EMISSIONCLASS:
            myEmissionClass = value;
            break;
        case SUMO_ATTR_GUISHAPE:
            myShape = getVehicleShapeID(value);
            break;
        case SUMO_ATTR_WIDTH:
            myWidth = parse<double>(value);
            break;
        case SUMO_ATTR_IMGFILE:
            myFilename = value;
            break;
        case SUMO_ATTR_IMPATIENCE:
            myImpatience = parse<double>(value);
            break;
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            myLaneChangeModel = value;
            break;
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            myCarFollowModel = value;
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            myPersonCapacity = parse<int>(value);
            break;
        case SUMO_ATTR_CONTAINER_CAPACITY:
            myContainerCapacity = parse<int>(value);
            break;
        case SUMO_ATTR_BOARDING_DURATION:
            myBoardingDuration = parse<double>(value);
            break;
        case SUMO_ATTR_LOADING_DURATION:
            myLoadingDuration = parse<double>(value);
            break;
        case SUMO_ATTR_LATALIGNMENT:
            myLatAlignment = value;
            break;
        case SUMO_ATTR_MINGAP_LAT:
            myMinGapLat = parse<double>(value);
            break;
        case SUMO_ATTR_MAXSPEED_LAT:
            myMaxSpeedLat = parse<double>(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
