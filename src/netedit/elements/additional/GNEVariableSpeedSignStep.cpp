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
/// @file    GNEVariableSpeedSignStep.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
///
//
/****************************************************************************/

#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEVariableSpeedSignStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_STEP, ""),
    GNEAdditionalListed(this) {
}


GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEAdditional* variableSpeedSign,
        const SUMOTime time, const double speed) :
    GNEAdditional(variableSpeedSign, SUMO_TAG_STEP, ""),
    GNEAdditionalListed(this),
    myTime(time),
    mySpeed(speed) {
    // set parents
    setParent<GNEAdditional*>(variableSpeedSign);
    // update boundary of rerouter parent
    variableSpeedSign->updateCenteringBoundary(true);
}


GNEVariableSpeedSignStep::~GNEVariableSpeedSignStep() {}


GNEMoveElement*
GNEVariableSpeedSignStep::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEVariableSpeedSignStep::getParameters() {
    return nullptr;
}


const Parameterised*
GNEVariableSpeedSignStep::getParameters() const {
    return nullptr;
}


void
GNEVariableSpeedSignStep::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_STEP);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write specific attributes
    device.writeAttr(SUMO_ATTR_TIME, time2string(myTime));
    device.writeAttr(SUMO_ATTR_SPEED, mySpeed);
    device.closeTag();
}


bool
GNEVariableSpeedSignStep::isAdditionalValid() const {
    return true;
}


std::string
GNEVariableSpeedSignStep::getAdditionalProblem() const {
    return "";
}


void
GNEVariableSpeedSignStep::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEVariableSpeedSignStep::checkDrawMoveContour() const {
    return false;
}


SUMOTime
GNEVariableSpeedSignStep::getTime() const {
    return myTime;
}


void
GNEVariableSpeedSignStep::updateGeometry() {
    updateGeometryListedAdditional();
}


Position
GNEVariableSpeedSignStep::getPositionInView() const {
    return getListedPositionInView();
}


void
GNEVariableSpeedSignStep::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNEVariableSpeedSignStep::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEVariableSpeedSignStep::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEVariableSpeedSignStep::drawGL(const GUIVisualizationSettings& s) const {
    // draw VSS step as listed attribute
    drawListedAdditional(s, RGBColor::WHITE, RGBColor::BLACK, GUITexture::VARIABLESPEEDSIGN_STEP,
                         getAttribute(SUMO_ATTR_TIME) + ": " + getAttribute(SUMO_ATTR_SPEED) + " km/h");
}


std::string
GNEVariableSpeedSignStep::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_TIME:
            return time2string(myTime);
        case SUMO_ATTR_SPEED:
            return toString(mySpeed);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(key);
    }
}


double
GNEVariableSpeedSignStep::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_TIME:
            return (double)myTime;
        default:
            return getCommonAttributeDouble(key);
    }
}


Position
GNEVariableSpeedSignStep::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEVariableSpeedSignStep::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEVariableSpeedSignStep::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_TIME:
        case SUMO_ATTR_SPEED:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEVariableSpeedSignStep::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_TIME:
            if (canParse<double>(value)) {
                // Check that
                double newTime = parse<double>(value);
                // Only allowed positiv times
                if (newTime < 0) {
                    return false;
                }
                // check that there isn't duplicate times
                int counter = 0;
                for (const auto& VSSChild : getParentAdditionals().at(0)->getChildAdditionals()) {
                    if (!VSSChild->getTagProperty()->isSymbol() && VSSChild->getAttributeDouble(SUMO_ATTR_TIME) == newTime) {
                        counter++;
                    }
                }
                return (counter <= 1);
            } else {
                return false;
            }
        case SUMO_ATTR_SPEED:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        default:
            return isCommonAttributeValid(key, value);
    }
}


std::string
GNEVariableSpeedSignStep::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVariableSpeedSignStep::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_TIME);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVariableSpeedSignStep::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_TIME:
            myTime = string2time(value);
            break;
        case SUMO_ATTR_SPEED:
            mySpeed = parse<double>(value);
            break;
        default:
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
