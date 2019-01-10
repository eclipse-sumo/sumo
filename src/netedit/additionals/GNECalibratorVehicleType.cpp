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

#include "GNECalibratorVehicleType.h"
#include "GNECalibrator.h"
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorVehicleType::GNECalibratorVehicleType(GNEViewNet* viewNet) :
    GNEAdditional(viewNet->getNet()->generateAdditionalID(SUMO_TAG_VTYPE), viewNet, GLO_CALIBRATOR, SUMO_TAG_VTYPE, "", false) {
    // fill calibrator vehicle type with default values
    setDefaultValues();
}


GNECalibratorVehicleType::GNECalibratorVehicleType(GNEViewNet* viewNet, const std::string& id) :
    GNEAdditional(id, viewNet, GLO_CALIBRATOR, SUMO_TAG_VTYPE, "", false) {
    // fill calibrator vehicle type with default values
    setDefaultValues();
}


GNECalibratorVehicleType::GNECalibratorVehicleType(GNEViewNet* viewNet, std::string vehicleTypeID,
        double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
        double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
        SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
        const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
        double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat) :
    GNEAdditional(vehicleTypeID, viewNet, GLO_CALIBRATOR, SUMO_TAG_VTYPE, "", false),
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
GNECalibratorVehicleType::moveGeometry(const Position&, const Position&) {
    // This additional cannot be moved
}


void
GNECalibratorVehicleType::commitGeometryMoving(const Position&, GNEUndoList*) {
    // This additional cannot be moved
}


void
GNECalibratorVehicleType::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNECalibratorVehicleType::getPositionInView() const {
    return Position();
}


std::string
GNECalibratorVehicleType::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNECalibratorVehicleType::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


std::string
GNECalibratorVehicleType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
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
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
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
        case GNE_ATTR_GENERIC:
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
            return isValidAdditionalID(value);
        case SUMO_ATTR_ACCEL:
            return canParse<double>(value);
        case SUMO_ATTR_DECEL:
            return canParse<double>(value);
        case SUMO_ATTR_SIGMA:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) <= 1);
        case SUMO_ATTR_TAU:
            return canParse<double>(value);
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
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
            if ((value == "zero") || (value == "LDV") || (value == "LDV_G_EU0") || (value == "LDV_G_EU1") || (value == "LDV_G_EU2") || (value == "LDV_G_EU3") || (value == "LDV_G_EU4") || (value == "LDV_G_EU5") ||
                    (value == "LDV_G_EU6") || (value == "LDV_G_East") || (value == "LDV_D_EU0") || (value == "LDV_D_EU1") || (value == "LDV_D_EU2") || (value == "LDV_D_EU3") || (value == "LDV_D_EU4") || (value == "LDV_D_EU5") || (value == "LDV_D_EU6") ||
                    (value == "PC") || (value == "PC_Alternative") || (value == "PC_G_EU0") || (value == "PC_G_EU1") || (value == "PC_G_EU2") || (value == "PC_G_EU3") || (value == "PC_G_EU4") || (value == "PC_G_EU5") || (value == "PC_G_EU6") || (value == "PC_G_East") ||
                    (value == "PC_D_EU0") || (value == "PC_D_EU1") || (value == "PC_D_EU2") || (value == "PC_D_EU3") || (value == "PC_D_EU4") || (value == "PC_D_EU5") || (value == "PC_D_EU6") || (value == "Bus") || (value == "Coach") || (value == "HDV") || (value == "HDV_G") ||
                    (value == "HDV_D_EU0") || (value == "HDV_D_EU1") || (value == "HDV_D_EU2") || (value == "HDV_D_EU3") || (value == "HDV_D_EU4") || (value == "HDV_D_EU5") || (value == "HDV_D_EU6") || (value == "HDV_D_East")) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_GUISHAPE:
            return canParseVehicleShape(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value);
        case SUMO_ATTR_IMGFILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_IMPATIENCE:
            return canParse<double>(value);
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            return SUMOXMLDefinitions::LaneChangeModels.hasString(value);
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            return SUMOXMLDefinitions::CarFollowModels.hasString(value);
        case SUMO_ATTR_PERSON_CAPACITY:
            return canParse<int>(value);
        case SUMO_ATTR_CONTAINER_CAPACITY:
            return canParse<int>(value);
        case SUMO_ATTR_BOARDING_DURATION:
            return canParse<double>(value);
        case SUMO_ATTR_LOADING_DURATION:
            return canParse<double>(value);
        case SUMO_ATTR_LATALIGNMENT:
            return SUMOXMLDefinitions::LateralAlignments.hasString(value);
        case SUMO_ATTR_MINGAP_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_MAXSPEED_LAT:
            return canParse<double>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNECalibratorVehicleType::getPopUpID() const {
    return toString(getTag()) + ": " + getID();
}


std::string
GNECalibratorVehicleType::getHierarchyName() const {
    return toString(getTag());
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibratorVehicleType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
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
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
