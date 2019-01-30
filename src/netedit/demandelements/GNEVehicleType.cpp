/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEVehicleType.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Definition of Vehicle Types in NETEDIT
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

#include "GNEVehicleType.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNEVehicleType::GNEVehicleType(GNEViewNet* viewNet, const std::string &vTypeID) :
    GNEDemandElement(vTypeID, viewNet, GLO_VTYPE, SUMO_TAG_VTYPE),
    SUMOVTypeParameter(vTypeID) {
}


GNEVehicleType::GNEVehicleType(GNEViewNet* viewNet, const SUMOVTypeParameter &vTypeParameter) :
    GNEDemandElement(vTypeParameter.id, viewNet, GLO_VTYPE, SUMO_TAG_VTYPE),
    SUMOVTypeParameter(vTypeParameter) {
}


GNEVehicleType::~GNEVehicleType() {}


void
GNEVehicleType::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNEVehicleType::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNEVehicleType::updateGeometry(bool /*updateGrid*/) {
    // Currently this additional doesn't own a Geometry
}


Position
GNEVehicleType::getPositionInView() const {
    return Position();
}


std::string
GNEVehicleType::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNEVehicleType::drawGL(const GUIVisualizationSettings&) const {
    // Currently This additional isn't drawn
}


void
GNEVehicleType::selectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.select(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // add object of list into selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_ROUTE);
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEVehicleType::unselectAttributeCarrier(bool changeFlag) {
    if (!myViewNet) {
        throw ProcessError("ViewNet cannot be nullptr");
    } else {
        gSelected.deselect(dynamic_cast<GUIGlObject*>(this)->getGlID());
        // remove object of list of selected objects
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_ROUTE);
        if (changeFlag) {
            mySelected = false;

        }
    }
}


std::string
GNEVehicleType::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getDemandElementID();
        /*
        case SUMO_ATTR_ACCEL:
            return toString(myAccel);
        case SUMO_ATTR_DECEL:
            return toString(myDecel);
        case SUMO_ATTR_SIGMA:
            return toString(mySigma);
        case SUMO_ATTR_TAU:
            return toString(myTau);
        case SUMO_ATTR_LENGTH:
            return toString(length);
        case SUMO_ATTR_MINGAP:
            return toString(minGap);
        case SUMO_ATTR_MAXSPEED:
            return toString(myMaxSpeed);
        case SUMO_ATTR_SPEEDFACTOR:
            return toString(speedFactor);
        case SUMO_ATTR_SPEEDDEV:
            return toString(mySpeedDev);
        case SUMO_ATTR_COLOR:
            return toString(color);
        case SUMO_ATTR_VCLASS:
            return toString(myVClass);
        case SUMO_ATTR_EMISSIONCLASS:
            return toString(emissionClass);
        case SUMO_ATTR_GUISHAPE:
            return getVehicleShapeName(shape);
        case SUMO_ATTR_WIDTH:
            return toString(width);
        case SUMO_ATTR_IMGFILE:
            return imgFile;
        case SUMO_ATTR_IMPATIENCE:
            return toString(impatience);
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            return myLaneChangeModel;
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            return myCarFollowModel;
        case SUMO_ATTR_PERSON_CAPACITY:
            return toString(personCapacity);
        case SUMO_ATTR_CONTAINER_CAPACITY:
            return toString(containerCapacity);
        case SUMO_ATTR_BOARDING_DURATION:
            return toString(boardingDuration);
        case SUMO_ATTR_LOADING_DURATION:
            return toString(loadingDuration);
        case SUMO_ATTR_LATALIGNMENT:
            return toString(latAlignment);
        case SUMO_ATTR_MINGAP_LAT:
            return toString(minGapLat);
        case SUMO_ATTR_MAXSPEED_LAT:
            return toString(maxSpeedLat);
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
            */
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEVehicleType::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVehicleType::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDemandElementID(value);
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEVehicleType::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEVehicleType::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicleType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        /*
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
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
            length = parse<double>(value);
            break;
        case SUMO_ATTR_MINGAP:
            minGap = parse<double>(value);
            break;
        case SUMO_ATTR_MAXSPEED:
            myMaxSpeed = parse<double>(value);
            break;
        case SUMO_ATTR_SPEEDFACTOR:
            speedFactor = parse<double>(value);
            break;
        case SUMO_ATTR_SPEEDDEV:
            mySpeedDev = parse<double>(value);
            break;
        case SUMO_ATTR_COLOR:
            color = parse<RGBColor>(value);
            break;
        case SUMO_ATTR_VCLASS:
            myVClass = getVehicleClassID(value);
            break;
        case SUMO_ATTR_EMISSIONCLASS:
            emissionClass = value;
            break;
        case SUMO_ATTR_GUISHAPE:
            shape = getVehicleShapeID(value);
            break;
        case SUMO_ATTR_WIDTH:
            width = parse<double>(value);
            break;
        case SUMO_ATTR_IMGFILE:
            imgFile = value;
            break;
        case SUMO_ATTR_IMPATIENCE:
            impatience = parse<double>(value);
            break;
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            myLaneChangeModel = value;
            break;
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            myCarFollowModel = value;
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            personCapacity = parse<int>(value);
            break;
        case SUMO_ATTR_CONTAINER_CAPACITY:
            containerCapacity = parse<int>(value);
            break;
        case SUMO_ATTR_BOARDING_DURATION:
            boardingDuration = parse<double>(value);
            break;
        case SUMO_ATTR_LOADING_DURATION:
            loadingDuration = parse<double>(value);
            break;
        case SUMO_ATTR_LATALIGNMENT:
            latAlignment = value;
            break;
        case SUMO_ATTR_MINGAP_LAT:
            minGapLat = parse<double>(value);
            break;
        case SUMO_ATTR_MAXSPEED_LAT:
            maxSpeedLat = parse<double>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
            */
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
