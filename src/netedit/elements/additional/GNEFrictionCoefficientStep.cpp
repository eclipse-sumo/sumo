/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrictionCoefficientStep.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2017
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/GNEUndoList.h>

#include "GNEFrictionCoefficientStep.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEFrictionCoefficientStep::GNEFrictionCoefficientStep(GNENet* net) :
    GNEAdditional("", net, GLO_COF_STEP, SUMO_TAG_STEP_COF, "",
{}, {}, {}, {}, {}, {}, {}, {},
std::map<std::string, std::string>()),
myTime(0) {
    // reset default values
    resetDefaultValues();
}


GNEFrictionCoefficientStep::GNEFrictionCoefficientStep(GNEAdditional* variableFrictionSignParent, SUMOTime time, const std::string& friction) :
    GNEAdditional(variableFrictionSignParent->getNet(), GLO_COF_STEP, SUMO_TAG_STEP_COF, "",
{}, {}, {}, {variableFrictionSignParent}, {}, {}, {}, {},
std::map<std::string, std::string>()),
myTime(time),
myFriction(friction) {
    // update boundary of rerouter parent
    variableFrictionSignParent->updateCenteringBoundary(true);
}


GNEFrictionCoefficientStep::~GNEFrictionCoefficientStep() {}


void
GNEFrictionCoefficientStep::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_STEP_COF);
    device.writeAttr(SUMO_ATTR_TIME, time2string(myTime));
    device.writeAttr(SUMO_ATTR_FRICTION, myFriction);
    device.closeTag();
}


GNEMoveOperation*
GNEFrictionCoefficientStep::getMoveOperation() {
    // COF Steps cannot be moved
    return nullptr;
}


SUMOTime
GNEFrictionCoefficientStep::getTime() const {
    return myTime;
}


void
GNEFrictionCoefficientStep::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
}


Position
GNEFrictionCoefficientStep::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5, (getDrawPositionIndex() * -1) + 1, 0);
    // return signPosition
    return signPosition;
}


void
GNEFrictionCoefficientStep::updateCenteringBoundary(const bool /*updateGrid*/) {
    myAdditionalBoundary.reset();
    myAdditionalBoundary.add(getPositionInView());
    myAdditionalBoundary.grow(5);
}


void
GNEFrictionCoefficientStep::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEFrictionCoefficientStep::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNEFrictionCoefficientStep::drawGL(const GUIVisualizationSettings& s) const {
    // draw rerouter interval as listed attribute
    drawListedAddtional(s, getParentAdditionals().front()->getPositionInView(),
                        0, 0, RGBColor::WHITE, RGBColor::BLACK, GUITexture::FRICTIONCOEFFICIENT_STEP,
                        getAttribute(SUMO_ATTR_TIME) + ": " + getAttribute(SUMO_ATTR_FRICTION) + "[0..1]");
}


std::string
GNEFrictionCoefficientStep::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_TIME:
            return time2string(myTime);
        case SUMO_ATTR_FRICTION:
            return myFriction;
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEFrictionCoefficientStep::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_TIME:
            return (double)myTime;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNEFrictionCoefficientStep::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_TIME:
        case SUMO_ATTR_FRICTION:
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEFrictionCoefficientStep::isValid(SumoXMLAttr key, const std::string& value) {
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
                for (const auto& COFChild : getParentAdditionals().at(0)->getChildAdditionals()) {
                    if (!COFChild->getTagProperty().isSymbol() && COFChild->getAttributeDouble(SUMO_ATTR_TIME) == newTime) {
                        counter++;
                    }
                }
                return (counter <= 1);
            } else {
                return false;
            }
        case SUMO_ATTR_FRICTION:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case GNE_ATTR_SELECTED:
            return canParse<double>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEFrictionCoefficientStep::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNEFrictionCoefficientStep::getPopUpID() const {
    return getTagStr();
}


std::string
GNEFrictionCoefficientStep::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_TIME);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEFrictionCoefficientStep::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_TIME:
            myTime = string2time(value);
            break;
        case SUMO_ATTR_FRICTION:
            myFriction = value;
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEFrictionCoefficientStep::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEFrictionCoefficientStep::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}


/****************************************************************************/
