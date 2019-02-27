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
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/gui/div/GUIGlobalSelection.h>

#include "GNEVehicleType.h"


// ===========================================================================
// member method definitions
// ===========================================================================


GNEVehicleType::GNEVehicleType(GNEViewNet* viewNet, const std::string &vTypeID) :
    GNEDemandElement(vTypeID, viewNet, GLO_VTYPE, SUMO_TAG_VTYPE),
    SUMOVTypeParameter(vTypeID),
    myDefaultVehicleType(true),
    myDefaultVehicleTypeModified(false) {
}


GNEVehicleType::GNEVehicleType(GNEViewNet* viewNet, const SUMOVTypeParameter &vTypeParameter) :
    GNEDemandElement(vTypeParameter.id, viewNet, GLO_VTYPE, SUMO_TAG_VTYPE),
    SUMOVTypeParameter(vTypeParameter),
    myDefaultVehicleType(false),
    myDefaultVehicleTypeModified(false)   {
}


GNEVehicleType::GNEVehicleType(GNEViewNet* viewNet, const std::string &vTypeID, GNEVehicleType *vTypeOriginal) :
    GNEDemandElement(vTypeID, viewNet, GLO_VTYPE, SUMO_TAG_VTYPE),
    SUMOVTypeParameter(*vTypeOriginal),
    myDefaultVehicleType(false),
    myDefaultVehicleTypeModified(false)  {
}


GNEVehicleType::~GNEVehicleType() {}


const RGBColor &
GNEVehicleType::getColor() const {
    return color;
}


void 
GNEVehicleType::writeDemandElement(OutputDevice& device) const {
    // only write default vehicle types if it was modified
    if (myDefaultVehicleType) {
        if (myDefaultVehicleTypeModified) {
            write(device);
        }
    } else {
        write(device);
    }
}


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
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(GLO_VTYPE);
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
        myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(GLO_VTYPE);
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
        // CFM Values
        case SUMO_ATTR_ACCEL:
        case SUMO_ATTR_DECEL:
        case SUMO_ATTR_APPARENTDECEL:
        case SUMO_ATTR_EMERGENCYDECEL:
        case SUMO_ATTR_SIGMA:
        case SUMO_ATTR_TAU:
        case SUMO_ATTR_COLLISION_MINGAP_FACTOR:
        case SUMO_ATTR_TMP1:
        case SUMO_ATTR_TMP2:
        case SUMO_ATTR_TMP3:
        case SUMO_ATTR_TMP4:
        case SUMO_ATTR_TMP5:
        case SUMO_ATTR_CF_PWAGNER2009_TAULAST:
        case SUMO_ATTR_CF_PWAGNER2009_APPROB:
        case SUMO_ATTR_CF_IDMM_ADAPT_FACTOR:
        case SUMO_ATTR_CF_IDMM_ADAPT_TIME:
        case SUMO_ATTR_CF_WIEDEMANN_SECURITY:
        case SUMO_ATTR_CF_WIEDEMANN_ESTIMATION:
        case SUMO_ATTR_TRAIN_TYPE:
        case SUMO_ATTR_K:
        case SUMO_ATTR_CF_KERNER_PHI:
        case SUMO_ATTR_CF_IDM_DELTA:
        case SUMO_ATTR_CF_IDM_STEPPING:
            return getCFParamString(key, "0");
        //
        case SUMO_ATTR_LENGTH:
            return toString(length);
        case SUMO_ATTR_MINGAP:
            return toString(minGap);
        case SUMO_ATTR_MAXSPEED:
            return toString(maxSpeed);
        case SUMO_ATTR_SPEEDFACTOR:
            return toString(speedFactor.getParameter()[0]);
        case SUMO_ATTR_SPEEDDEV:
            return toString(speedFactor.getParameter()[1]);
        case SUMO_ATTR_COLOR:
            return toString(color);
        case SUMO_ATTR_VCLASS:
            return toString(vehicleClass);
        case SUMO_ATTR_EMISSIONCLASS:
            return PollutantsInterface::getName(emissionClass);
        case SUMO_ATTR_GUISHAPE:
            return SumoVehicleShapeStrings.getString(shape);
        case SUMO_ATTR_WIDTH:
            return toString(width);
        case SUMO_ATTR_IMGFILE:
            return imgFile;
        case SUMO_ATTR_IMPATIENCE:
            return toString(impatience);
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            return SUMOXMLDefinitions::LaneChangeModels.getString(lcModel);
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            return SUMOXMLDefinitions::CarFollowModels.getString(cfModel);
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
        case SUMO_ATTR_ACTIONSTEPLENGTH:
            return toString(actionStepLength);
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
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
        // CFM
        case SUMO_ATTR_ACCEL:
        case SUMO_ATTR_DECEL:
        case SUMO_ATTR_APPARENTDECEL:
        case SUMO_ATTR_EMERGENCYDECEL:
        case SUMO_ATTR_SIGMA:
        case SUMO_ATTR_TAU:
        case SUMO_ATTR_COLLISION_MINGAP_FACTOR:
        case SUMO_ATTR_TMP1:
        case SUMO_ATTR_TMP2:
        case SUMO_ATTR_TMP3:
        case SUMO_ATTR_TMP4:
        case SUMO_ATTR_TMP5:
        case SUMO_ATTR_CF_PWAGNER2009_TAULAST:
        case SUMO_ATTR_CF_PWAGNER2009_APPROB:
        case SUMO_ATTR_CF_IDMM_ADAPT_FACTOR:
        case SUMO_ATTR_CF_IDMM_ADAPT_TIME:
        case SUMO_ATTR_CF_WIEDEMANN_SECURITY:
        case SUMO_ATTR_CF_WIEDEMANN_ESTIMATION:
        case SUMO_ATTR_TRAIN_TYPE:
        case SUMO_ATTR_K:
        case SUMO_ATTR_CF_KERNER_PHI:
        case SUMO_ATTR_CF_IDM_DELTA:
        case SUMO_ATTR_CF_IDM_STEPPING:
        //
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
        case SUMO_ATTR_ACTIONSTEPLENGTH:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
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
        // CFM parameters
        case SUMO_ATTR_SIGMA:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) <= 1);
        case SUMO_ATTR_ACCEL:
        case SUMO_ATTR_DECEL:
        case SUMO_ATTR_APPARENTDECEL:
        case SUMO_ATTR_EMERGENCYDECEL:
        case SUMO_ATTR_TAU:
        case SUMO_ATTR_COLLISION_MINGAP_FACTOR:
        case SUMO_ATTR_TMP1:
        case SUMO_ATTR_TMP2:
        case SUMO_ATTR_TMP3:
        case SUMO_ATTR_TMP4:
        case SUMO_ATTR_TMP5:
        case SUMO_ATTR_CF_PWAGNER2009_TAULAST:
        case SUMO_ATTR_CF_PWAGNER2009_APPROB:
        case SUMO_ATTR_CF_IDMM_ADAPT_FACTOR:
        case SUMO_ATTR_CF_IDMM_ADAPT_TIME:
        case SUMO_ATTR_CF_WIEDEMANN_SECURITY:
        case SUMO_ATTR_CF_WIEDEMANN_ESTIMATION:
        case SUMO_ATTR_TRAIN_TYPE:
        case SUMO_ATTR_K:
        case SUMO_ATTR_CF_KERNER_PHI:
        case SUMO_ATTR_CF_IDM_DELTA:
        case SUMO_ATTR_CF_IDM_STEPPING:
            return canParse<double>(value);
            // rail string
        //
        case SUMO_ATTR_LENGTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_MINGAP:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_MAXSPEED:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_SPEEDFACTOR:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_SPEEDDEV:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_VCLASS:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_EMISSIONCLASS:
            /** check **/
            return true;
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
        case SUMO_ATTR_ACTIONSTEPLENGTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool 
GNEVehicleType::isDisjointAttributeSet(const SumoXMLAttr attr) const {
    switch (attr) {
        case SUMO_ATTR_COLOR:
            return (parametersSet & VTYPEPARS_COLOR_SET) != 0;
        default:
            return true;
    };
}


std::string
GNEVehicleType::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVehicleType::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_ID) ;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVehicleType::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeDemandElementID(value);
            break;
        // CFM
        case SUMO_ATTR_ACCEL:
        case SUMO_ATTR_DECEL:
        case SUMO_ATTR_APPARENTDECEL:
        case SUMO_ATTR_EMERGENCYDECEL:
        case SUMO_ATTR_SIGMA:
        case SUMO_ATTR_TAU:
        case SUMO_ATTR_COLLISION_MINGAP_FACTOR:
        case SUMO_ATTR_TMP1:
        case SUMO_ATTR_TMP2:
        case SUMO_ATTR_TMP3:
        case SUMO_ATTR_TMP4:
        case SUMO_ATTR_TMP5:
        case SUMO_ATTR_CF_PWAGNER2009_TAULAST:
        case SUMO_ATTR_CF_PWAGNER2009_APPROB:
        case SUMO_ATTR_CF_IDMM_ADAPT_FACTOR:
        case SUMO_ATTR_CF_IDMM_ADAPT_TIME:
        case SUMO_ATTR_CF_WIEDEMANN_SECURITY:
        case SUMO_ATTR_CF_WIEDEMANN_ESTIMATION:
        case SUMO_ATTR_TRAIN_TYPE:
        case SUMO_ATTR_K:
        case SUMO_ATTR_CF_KERNER_PHI:
        case SUMO_ATTR_CF_IDM_DELTA:
        case SUMO_ATTR_CF_IDM_STEPPING:
            cfParameter[key] = value;
            break;
        //
        case SUMO_ATTR_LENGTH:
            length = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_LENGTH_SET;
            break;
        case SUMO_ATTR_MINGAP:
            minGap = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_MINGAP_SET;
            break;
        case SUMO_ATTR_MAXSPEED:
            maxSpeed = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_MAXSPEED_SET;
            break;
        case SUMO_ATTR_SPEEDFACTOR:
            speedFactor.getParameter()[0] = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_SPEEDFACTOR_SET;
            break;
        case SUMO_ATTR_SPEEDDEV:
            speedFactor.getParameter()[1] = parse<double>(value);
            break;
        case SUMO_ATTR_COLOR:
            color = parse<RGBColor>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_COLOR_SET;
            break;
        case SUMO_ATTR_VCLASS:
            vehicleClass = getVehicleClassID(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
            break;
        case SUMO_ATTR_EMISSIONCLASS:
            emissionClass = PollutantsInterface::getClassByName("value");
            // mark parameter as set
            parametersSet |= VTYPEPARS_EMISSIONCLASS_SET;
            break;
        case SUMO_ATTR_GUISHAPE:
            shape = getVehicleShapeID(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_SHAPE_SET;
            break;
        case SUMO_ATTR_WIDTH:
            width = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_WIDTH_SET;
            break;
        case SUMO_ATTR_IMGFILE:
            imgFile = value;
            // mark parameter as set
            parametersSet |= VTYPEPARS_IMGFILE_SET;
            break;
        case SUMO_ATTR_IMPATIENCE:
            impatience = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_IMPATIENCE_SET;
            break;
        case SUMO_ATTR_LANE_CHANGE_MODEL:
            // mark parameter as set
            parametersSet |= VTYPEPARS_LANE_CHANGE_MODEL_SET;
            break;
        case SUMO_ATTR_CAR_FOLLOW_MODEL:
            // mark parameter as set
            parametersSet |= VTYPEPARS_CAR_FOLLOW_MODEL;
            break;
        case SUMO_ATTR_PERSON_CAPACITY:
            // mark parameter as set
            parametersSet |= VTYPEPARS_PERSON_CAPACITY;
            break;
        case SUMO_ATTR_CONTAINER_CAPACITY:
            containerCapacity = parse<int>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_CONTAINER_CAPACITY;
            break;
        case SUMO_ATTR_BOARDING_DURATION:
            boardingDuration = parse<int>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_BOARDING_DURATION;
            break;
        case SUMO_ATTR_LOADING_DURATION:
            loadingDuration = parse<int>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_LOADING_DURATION;
            break;
        case SUMO_ATTR_LATALIGNMENT:
            latAlignment = SUMOXMLDefinitions::LateralAlignments.get(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_LATALIGNMENT_SET;
            break;
        case SUMO_ATTR_MINGAP_LAT:
            minGapLat = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_MINGAP_LAT_SET;
            break;
        case SUMO_ATTR_MAXSPEED_LAT:
            maxSpeedLat = parse<double>(value);
            // mark parameter as set
            parametersSet |= VTYPEPARS_MAXSPEED_LAT_SET;
            break;
        case SUMO_ATTR_ACTIONSTEPLENGTH:
            actionStepLength = parse<int>(value);
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            /*
            // mark parameter as set
            parametersSet |= VTYPEPARS_HASDRIVERSTATE_SET;
            // mark parameter as set
            parametersSet |= VTYPEPARS_PROBABILITY_SET;
            // mark parameter as set
            parametersSet |= VTYPEPARS_HEIGHT_SET;
            // mark parameter as set
            parametersSet |= VTYPEPARS_OSGFILE_SET;
            */
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
