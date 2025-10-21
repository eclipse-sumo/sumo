/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNECalibratorFlow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2016
///
//
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNECalibratorFlow.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibratorFlow::GNECalibratorFlow(GNENet* net) :
    GNEAdditional("", net, "", GNE_TAG_CALIBRATOR_FLOW, ""),
    GNEAdditionalListed(this) {
    // set VPH and speed enabled
    toggleAttribute(SUMO_ATTR_VEHSPERHOUR, true);
    toggleAttribute(SUMO_ATTR_SPEED, true);
    // reset ID
    id.clear();
}


GNECalibratorFlow::GNECalibratorFlow(GNEAdditional* calibratorParent, GNEDemandElement* vehicleType, GNEDemandElement* route) :
    GNEAdditional(calibratorParent, GNE_TAG_CALIBRATOR_FLOW, ""),
    GNEAdditionalListed(this) {
    // set parents
    setParent<GNEAdditional*>(calibratorParent);
    setParents<GNEDemandElement*>({vehicleType, route});
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // reset ID
    id.clear();
}


GNECalibratorFlow::GNECalibratorFlow(GNEAdditional* calibratorParent, GNEDemandElement* vehicleType, GNEDemandElement* route,
                                     const SUMOVehicleParameter& vehicleParameters) :
    GNEAdditional(calibratorParent, GNE_TAG_CALIBRATOR_FLOW, ""),
    SUMOVehicleParameter(vehicleParameters),
    GNEAdditionalListed(this) {
    // set parents
    setParent<GNEAdditional*>(calibratorParent);
    setParents<GNEDemandElement*>({vehicleType, route});
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // reset ID
    id.clear();
}


GNECalibratorFlow::~GNECalibratorFlow() {}


GNEMoveElement*
GNECalibratorFlow::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNECalibratorFlow::getParameters() {
    return this;
}


const Parameterised*
GNECalibratorFlow::getParameters() const {
    return this;
}


void
GNECalibratorFlow::writeAdditional(OutputDevice& device) const {
    if (isAttributeEnabled(SUMO_ATTR_TYPE) || isAttributeEnabled(SUMO_ATTR_VEHSPERHOUR) || isAttributeEnabled(SUMO_ATTR_SPEED)) {
        // open tag
        device.openTag(SUMO_TAG_FLOW);
        // write vehicle attributes
        write(device, OptionsCont::getOptions(), SUMO_TAG_FLOW, getParentDemandElements().at(0)->getID());
        // write end
        device.writeAttr(SUMO_ATTR_END, getAttribute(SUMO_ATTR_END));
        // write route
        device.writeAttr(SUMO_ATTR_ROUTE, getParentDemandElements().at(1)->getID());
        // write parameters
        SUMOVehicleParameter::writeParams(device);
        // close vehicle tag
        device.closeTag();
    } else {
        WRITE_WARNING(TLF("calibratorFlow of calibrator '%' cannot be written. Either type or vehsPerHour or speed must be enabled", getParentAdditionals().front()->getID()));
    }
}


bool
GNECalibratorFlow::isAdditionalValid() const {
    return true;
}


std::string
GNECalibratorFlow::getAdditionalProblem() const {
    return "";
}


void
GNECalibratorFlow::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNECalibratorFlow::checkDrawMoveContour() const {
    return false;
}


void
GNECalibratorFlow::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNECalibratorFlow::getPositionInView() const {
    return getListedPositionInView();
}


void
GNECalibratorFlow::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNECalibratorFlow::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNECalibratorFlow::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNECalibratorFlow::drawGL(const GUIVisualizationSettings& s) const {
    // draw closing reroute as listed attribute
    drawListedAdditional(s, s.additionalSettings.calibratorColor, RGBColor::BLACK, GUITexture::VARIABLESPEEDSIGN_STEP,
                         "Flow: " + getID());
}


std::string
GNECalibratorFlow::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_TYPE:
            return vtypeid;
        case SUMO_ATTR_ROUTE:
            return getParentDemandElements().at(1)->getID();
        case SUMO_ATTR_VEHSPERHOUR:
            if (parametersSet & VEHPARS_VPH_SET) {
                return toString(3600 / STEPS2TIME(repetitionOffset));
            } else {
                return "";
            }
        case SUMO_ATTR_SPEED:
            if (parametersSet & VEHPARS_CALIBRATORSPEED_SET) {
                return toString(calibratorSpeed);
            } else {
                return "";
            }
        case SUMO_ATTR_COLOR:
            if (wasSet(VEHPARS_COLOR_SET)) {
                return toString(color);
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_COLOR);
            }
        case SUMO_ATTR_BEGIN:
            return time2string(depart);
        case SUMO_ATTR_END:
            return time2string(repetitionEnd);
        case SUMO_ATTR_DEPARTLANE:
            if (wasSet(VEHPARS_DEPARTLANE_SET)) {
                return getDepartLane();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTLANE);
            }
        case SUMO_ATTR_DEPARTPOS:
            if (wasSet(VEHPARS_DEPARTPOS_SET)) {
                return getDepartPos();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTPOS);
            }
        case SUMO_ATTR_DEPARTSPEED:
            if (wasSet(VEHPARS_DEPARTSPEED_SET)) {
                return getDepartSpeed();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTSPEED);
            }
        case SUMO_ATTR_ARRIVALLANE:
            if (wasSet(VEHPARS_ARRIVALLANE_SET)) {
                return getArrivalLane();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_ARRIVALLANE);
            }
        case SUMO_ATTR_ARRIVALPOS:
            if (wasSet(VEHPARS_ARRIVALPOS_SET)) {
                return getArrivalPos();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_ARRIVALPOS);
            }
        case SUMO_ATTR_ARRIVALSPEED:
            if (wasSet(VEHPARS_ARRIVALSPEED_SET)) {
                return getArrivalSpeed();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_ARRIVALSPEED);
            }
        case SUMO_ATTR_LINE:
            if (wasSet(VEHPARS_LINE_SET)) {
                return line;
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_LINE);
            }
        case SUMO_ATTR_PERSON_NUMBER:
            if (wasSet(VEHPARS_PERSON_NUMBER_SET)) {
                return toString(personNumber);
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_PERSON_NUMBER);
            }
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return toString(containerNumber);
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_CONTAINER_NUMBER);
            }
        case SUMO_ATTR_REROUTE:
            if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
                return "true";
            } else {
                return "false";
            }
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (wasSet(VEHPARS_DEPARTPOSLAT_SET)) {
                return getDepartPosLat();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_DEPARTPOS_LAT);
            }
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (wasSet(VEHPARS_ARRIVALPOSLAT_SET)) {
                return getArrivalPosLat();
            } else {
                return myTagProperty->getDefaultStringValue(SUMO_ATTR_ARRIVALPOS_LAT);
            }
        case SUMO_ATTR_INSERTIONCHECKS:
            return getInsertionChecks();
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNECalibratorFlow::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            return STEPS2TIME(depart);
        case SUMO_ATTR_DEPARTPOS:
            // only return departPos it if is given
            if (departPosProcedure == DepartPosDefinition::GIVEN) {
                return departPos;
            } else {
                return 0;
            }
        case SUMO_ATTR_ARRIVALPOS:
            // only return departPos it if is given
            if (arrivalPosProcedure == ArrivalPosDefinition::GIVEN) {
                return arrivalPos;
            } else {
                return -1;
            }
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_MINGAP:
            return getParentDemandElements().at(0)->getAttributeDouble(key);
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNECalibratorFlow::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNECalibratorFlow::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
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
        case SUMO_ATTR_INSERTIONCHECKS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNECalibratorFlow::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_TYPE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::types, value, false) == nullptr);
        case SUMO_ATTR_ROUTE:
            return (myNet->getAttributeCarriers()->retrieveDemandElements(NamespaceIDs::routes, value, false) == nullptr);
        case SUMO_ATTR_VEHSPERHOUR:
            if (value.empty()) {
                // speed and vehsPerHour cannot be empty at the same time
                if (!isAttributeEnabled(SUMO_ATTR_SPEED)) {
                    return false;
                } else {
                    return true;
                }
            } else if (canParse<double>(value)) {
                const double dVal = parse<double>(value);
                return (dVal >= 0 || dVal == -1);
            } else {
                return false;
            }
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
                // speed and vehsPerHour cannot be empty at the same time
                if (!isAttributeEnabled(SUMO_ATTR_VEHSPERHOUR)) {
                    return false;
                } else {
                    return true;
                }
            } else if (canParse<double>(value)) {
                const double dVal = parse<double>(value);
                return (dVal >= 0 || dVal == -1);
            } else {
                return false;
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_END:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_DEPARTLANE:
            if ((value == "random") || (value == "free") || (value == "allowed") || (value == "best") || (value == "first")) {
                return true;
            } else {
                return (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr);
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
                return (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr);
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
            if ((value == "random") || (value == "free") || (value == "random_free") || (value == "left") || (value == "right") || (value == "center")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if ((value == "left") || (value == "right") || (value == "center")) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_INSERTIONCHECKS:
            return areInsertionChecksValid(value);
        default:
            return isCommonAttributeValid(key, value);
    }
}


bool
GNECalibratorFlow::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_TYPE:
            return (parametersSet & VEHPARS_VTYPE_SET) != 0;
        case SUMO_ATTR_VEHSPERHOUR:
            return (parametersSet & VEHPARS_VPH_SET) != 0;
        case SUMO_ATTR_SPEED:
            return (parametersSet & VEHPARS_CALIBRATORSPEED_SET) != 0;
        default:
            return true;
    }
}


std::string
GNECalibratorFlow::getPopUpID() const {
    return getTagStr();
}


std::string
GNECalibratorFlow::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibratorFlow::setAttribute(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_TYPE:
            if (!isTemplate()) {
                if (value.empty()) {
                    replaceDemandElementParent(SUMO_TAG_VTYPE, DEFAULT_VTYPE_ID, 0);
                } else {
                    replaceDemandElementParent(SUMO_TAG_VTYPE, value, 0);
                }
            }
            // set manually vtypeID (needed for saving)
            vtypeid = value;
            break;
        case SUMO_ATTR_ROUTE:
            replaceDemandElementParent(SUMO_TAG_ROUTE, value, 1);
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            if (value.empty()) {
                parametersSet &= ~VEHPARS_VPH_SET;
            } else {
                repetitionOffset = TIME2STEPS(3600 / parse<double>(value));
                // set parameters
                parametersSet |= VEHPARS_VPH_SET;
            }
            break;
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
                parametersSet &= ~VEHPARS_CALIBRATORSPEED_SET;
            } else {
                calibratorSpeed = parse<double>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_CALIBRATORSPEED_SET;
            }
            break;
        case SUMO_ATTR_COLOR:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                color = parse<RGBColor>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_COLOR_SET;
            } else {
                // set default value
                color = myTagProperty->getDefaultColorValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_COLOR_SET;
            }
            break;
        case SUMO_ATTR_BEGIN:
            depart = string2time(value);
            break;
        case SUMO_ATTR_END:
            repetitionEnd = string2time(value);
            break;
        case SUMO_ATTR_DEPARTLANE:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseDepartLane(value, myTagProperty->getTagStr(), id, departLane, departLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTLANE_SET;
            } else {
                // set default value
                parseDepartLane(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, departLane, departLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTLANE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseDepartPos(value, myTagProperty->getTagStr(), id, departPos, departPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOS_SET;
            } else {
                // set default value
                parseDepartPos(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, departPos, departPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOS_SET;
            }
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_DEPARTSPEED:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseDepartSpeed(value, myTagProperty->getTagStr(), id, departSpeed, departSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTSPEED_SET;
            } else {
                // set default value
                parseDepartSpeed(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, departSpeed, departSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTSPEED_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALLANE:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseArrivalLane(value, myTagProperty->getTagStr(), id, arrivalLane, arrivalLaneProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALLANE_SET;
            } else {
                // set default value
                parseArrivalLane(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, arrivalLane, arrivalLaneProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALLANE_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseArrivalPos(value, myTagProperty->getTagStr(), id, arrivalPos, arrivalPosProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOS_SET;
            } else {
                // set default value
                parseArrivalPos(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, arrivalPos, arrivalPosProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOS_SET;
            }
            // update geometry (except for template)
            if (getParentAdditionals().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseArrivalSpeed(value, myTagProperty->getTagStr(), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALSPEED_SET;
            } else {
                // set default value
                parseArrivalSpeed(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, arrivalSpeed, arrivalSpeedProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALSPEED_SET;
            }
            break;
        case SUMO_ATTR_LINE:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                line = value;
                // mark parameter as set
                parametersSet |= VEHPARS_LINE_SET;
            } else {
                // set default value
                line = myTagProperty->getDefaultStringValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_LINE_SET;
            }
            break;
        case SUMO_ATTR_PERSON_NUMBER:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                personNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_PERSON_NUMBER_SET;
            } else {
                // set default value
                personNumber = myTagProperty->getDefaultIntValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_PERSON_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_CONTAINER_NUMBER:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                containerNumber = parse<int>(value);
                // mark parameter as set
                parametersSet |= VEHPARS_CONTAINER_NUMBER_SET;
            } else {
                // set default value
                containerNumber = myTagProperty->getDefaultIntValue(key);
                // unset parameter
                parametersSet &= ~VEHPARS_CONTAINER_NUMBER_SET;
            }
            break;
        case SUMO_ATTR_REROUTE:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                // mark parameter as set
                parametersSet |= VEHPARS_ROUTE_SET;
            } else {
                // unset parameter
                parametersSet &= ~VEHPARS_ROUTE_SET;
            }
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseDepartPosLat(value, myTagProperty->getTagStr(), id, departPosLat, departPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_DEPARTPOSLAT_SET;
            } else {
                // set default value
                parseDepartPosLat(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, departPosLat, departPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_DEPARTPOSLAT_SET;
            }
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            if (!value.empty() && (value != myTagProperty->getDefaultStringValue(key))) {
                parseArrivalPosLat(value, myTagProperty->getTagStr(), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // mark parameter as set
                parametersSet |= VEHPARS_ARRIVALPOSLAT_SET;
            } else {
                // set default value
                parseArrivalPosLat(myTagProperty->getDefaultStringValue(key), myTagProperty->getTagStr(), id, arrivalPosLat, arrivalPosLatProcedure, error);
                // unset parameter
                parametersSet &= ~VEHPARS_ARRIVALPOSLAT_SET;
            }
            parseArrivalPosLat(value, myTagProperty->getTagStr(), id, arrivalPosLat, arrivalPosLatProcedure, error);
            break;
        case SUMO_ATTR_INSERTIONCHECKS:
            insertionChecks = parseInsertionChecks(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}


void
GNECalibratorFlow::toggleAttribute(SumoXMLAttr key, const bool value) {
    switch (key) {
        case SUMO_ATTR_TYPE:
            if (value) {
                parametersSet |= VEHPARS_VTYPE_SET;
            } else {
                parametersSet &= ~VEHPARS_VTYPE_SET;
            }
            break;
        case SUMO_ATTR_VEHSPERHOUR:
            if (value) {
                parametersSet |= VEHPARS_VPH_SET;
            } else {
                parametersSet &= ~VEHPARS_VPH_SET;
            }
            break;
        case SUMO_ATTR_SPEED:
            if (value) {
                parametersSet |= VEHPARS_CALIBRATORSPEED_SET;
            } else {
                parametersSet &= ~VEHPARS_CALIBRATORSPEED_SET;
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
