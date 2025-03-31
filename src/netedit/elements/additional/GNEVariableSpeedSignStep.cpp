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
#include <config.h>

#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEVariableSpeedSignStep.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNENet* net) :
    GNEAdditional("", net, "", GLO_VSS_STEP, SUMO_TAG_STEP, GUIIcon::VSSSTEP, "") {
}


GNEVariableSpeedSignStep::GNEVariableSpeedSignStep(GNEAdditional* variableSpeedSignParent, SUMOTime time, const std::string& speed) :
    GNEAdditional(variableSpeedSignParent, GLO_VSS_STEP, SUMO_TAG_STEP, GUIIcon::VSSSTEP, ""),
    myTime(time),
    mySpeed(speed) {
    // set parents
    setParent<GNEAdditional*>(variableSpeedSignParent);
    // update boundary of rerouter parent
    variableSpeedSignParent->updateCenteringBoundary(true);
}


GNEVariableSpeedSignStep::~GNEVariableSpeedSignStep() {}


void
GNEVariableSpeedSignStep::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_STEP);
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


GNEMoveOperation*
GNEVariableSpeedSignStep::getMoveOperation() {
    // VSS Steps cannot be moved
    return nullptr;
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
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


Position
GNEVariableSpeedSignStep::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5, (getDrawPositionIndex() * -1) + 1, 0);
    // return signPosition
    return signPosition;
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
    // draw rerouter interval as listed attribute
    drawListedAdditional(s, getParentAdditionals().front()->getPositionInView(),
                         0, 0, RGBColor::WHITE, RGBColor::BLACK, GUITexture::VARIABLESPEEDSIGN_STEP,
                         getAttribute(SUMO_ATTR_TIME) + ": " + getAttribute(SUMO_ATTR_SPEED) + "km/h");
}


std::string
GNEVariableSpeedSignStep::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_TIME:
            return time2string(myTime);
        case SUMO_ATTR_SPEED:
            return mySpeed;
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNEVariableSpeedSignStep::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_TIME:
            return (double)myTime;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNEVariableSpeedSignStep::getACParametersMap() const {
    return getParametersMap();
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
            return isCommonValid(key, value);
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
            mySpeed = value;
            break;
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}


void
GNEVariableSpeedSignStep::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEVariableSpeedSignStep::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
