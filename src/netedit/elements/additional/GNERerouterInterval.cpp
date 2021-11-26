/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
    GNEAdditional("", net, GLO_REROUTER_INTERVAL, SUMO_TAG_INTERVAL, "",
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myBegin(0),
    myEnd(0) {
    // reset default values
    resetDefaultValues();
}


GNERerouterInterval::GNERerouterInterval(GNERerouterDialog* rerouterDialog) :
    GNEAdditional(rerouterDialog->getEditedAdditional()->getNet(), GLO_REROUTER_INTERVAL, SUMO_TAG_INTERVAL, "",
        {}, {}, {}, {rerouterDialog->getEditedAdditional()}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myBegin(0),
    myEnd(0) {
    // reset default values
    resetDefaultValues();
    // update boundary of rerouter parent
    rerouterDialog->getEditedAdditional()->updateCenteringBoundary(true);
}


GNERerouterInterval::GNERerouterInterval(GNEAdditional* rerouterParent, SUMOTime begin, SUMOTime end) :
    GNEAdditional(rerouterParent->getNet(), GLO_REROUTER, SUMO_TAG_INTERVAL, "",
        {}, {}, {}, {rerouterParent}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    myBegin(begin),
    myEnd(end) {
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
    myAdditionalBoundary.reset();
    myAdditionalBoundary.add(getPositionInView());
    myAdditionalBoundary.grow(5);
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
    // draw rerouter interval as listed attribute
    drawListedAddtional(s, getParentAdditionals().front()->getPositionInView(),
                        0, 0, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_INTERVAL,
                        getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END));
    // iterate over additionals and check if drawn
    for (const auto& rerouterElement : getChildAdditionals()) {
        // if rerouter or their child is selected, then draw
        if (isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(this) ||
                rerouterElement->isAttributeCarrierSelected() || myNet->getViewNet()->isAttributeCarrierInspected(rerouterElement) ||
                (myNet->getViewNet()->getFrontAttributeCarrier() == rerouterElement)) {
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
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
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


void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case GNE_ATTR_SELECTED:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouterInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) < myEnd);
        case SUMO_ATTR_END:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) > myBegin);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouterInterval::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
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
GNERerouterInterval::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNERerouterInterval::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
