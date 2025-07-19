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
/// @file    GNERerouterInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>

#include "GNERerouterInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_INTERVAL, "") {
}


GNERerouterInterval::GNERerouterInterval(GNERerouterDialog* rerouterDialog) :
    GNEAdditional(rerouterDialog->getEditedAdditional(), SUMO_TAG_INTERVAL, "") {
    // set parents
    setParent<GNEAdditional*>(rerouterDialog->getEditedAdditional());
    // update boundary of rerouter parent
    rerouterDialog->getEditedAdditional()->updateCenteringBoundary(true);
}


GNERerouterInterval::GNERerouterInterval(GNEAdditional* rerouterParent, SUMOTime begin, SUMOTime end) :
    GNEAdditional(rerouterParent, SUMO_TAG_INTERVAL, ""),
    myBegin(begin),
    myEnd(end) {
    // set parents
    setParent<GNEAdditional*>(rerouterParent);
    // update boundary of rerouter parent
    rerouterParent->updateCenteringBoundary(true);
}


GNERerouterInterval::~GNERerouterInterval() {}


void
GNERerouterInterval::writeAdditional(OutputDevice& device) const {
    // avoid write empty intervals
    if (getChildAdditionals().size() > 0) {
        device.openTag(SUMO_TAG_INTERVAL);
        device.writeAttr(SUMO_ATTR_BEGIN, getAttribute(SUMO_ATTR_BEGIN));
        device.writeAttr(SUMO_ATTR_END, getAttribute(SUMO_ATTR_END));
        // write all rerouter interval
        for (const auto& rerouterElement : getChildAdditionals()) {
            rerouterElement->writeAdditional(device);
        }
        device.closeTag();
    }
}


bool
GNERerouterInterval::isAdditionalValid() const {
    return true;
}


std::string
GNERerouterInterval::getAdditionalProblem() const {
    return "";
}


void
GNERerouterInterval::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNERerouterInterval::checkDrawMoveContour() const {
    return false;
}


GNEMoveOperation*
GNERerouterInterval::getMoveOperation() {
    // rerouter intervals cannot be moved
    return nullptr;
}


void
GNERerouterInterval::updateGeometry() {
    // update centering boundary (needed for centering)
    updateCenteringBoundary(false);
    // update geometries (boundaries of all children)
    for (const auto& rerouterElement : getChildAdditionals()) {
        rerouterElement->updateGeometry();
    }
}


Position
GNERerouterInterval::getPositionInView() const {
    // get rerouter parent position
    Position signPosition = getParentAdditionals().front()->getPositionInView();
    // set position depending of indexes
    signPosition.add(4.5, (getDrawPositionIndex() * -1) + 1, 0);
    // return signPosition
    return signPosition;
}


void
GNERerouterInterval::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNERerouterInterval::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNERerouterInterval::getParentName() const {
    return getParentAdditionals().at(0)->getID();
}


void
GNERerouterInterval::drawGL(const GUIVisualizationSettings& s) const {
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // draw rerouter interval as listed attribute
    drawListedAdditional(s, getParentAdditionals().front()->getPositionInView(),
                         0, 0, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_INTERVAL,
                         getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END));
    // iterate over additionals and check if drawn
    for (const auto& rerouterElement : getChildAdditionals()) {
        // if rerouter or their child is selected, then draw
        if (isAttributeCarrierSelected() || inspectedElements.isACInspected(this) ||
                rerouterElement->isAttributeCarrierSelected() || inspectedElements.isACInspected(rerouterElement) ||
                rerouterElement->isMarkedForDrawingFront()) {
            rerouterElement->drawGL(s);
        }
    }
}


std::string
GNERerouterInterval::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_BEGIN:
            return time2string(myBegin);
        case SUMO_ATTR_END:
            return time2string(myEnd);
        case GNE_ATTR_PARENT:
            return getParentAdditionals().at(0)->getID();
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNERerouterInterval::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(myBegin);
        case SUMO_ATTR_END:
            return STEPS2TIME(myEnd);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNERerouterInterval::getACParametersMap() const {
    return getParametersMap();
}


void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNERerouterInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            if (canParse<SUMOTime>(value)) {
                const auto begin = parse<SUMOTime>(value);
                if (begin < 0) {
                    return false;
                } else {
                    return (begin <= myEnd);
                }
            } else {
                return false;
            }
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) < myEnd);
        case SUMO_ATTR_END:
            if (canParse<SUMOTime>(value)) {
                const auto end = parse<SUMOTime>(value);
                if (end < 0) {
                    return false;
                } else {
                    return (myBegin <= end);
                }
            } else {
                return false;
            }
        default:
            return isCommonValid(key, value);
    }
}


std::string
GNERerouterInterval::getPopUpID() const {
    return getTagStr();
}


std::string
GNERerouterInterval::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}

// ===========================================================================
// private
// ===========================================================================

void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            myBegin = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<SUMOTime>(value);
            break;
        default:
            setCommonAttribute(this, key, value);
            break;
    }
}


void
GNERerouterInterval::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNERerouterInterval::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
