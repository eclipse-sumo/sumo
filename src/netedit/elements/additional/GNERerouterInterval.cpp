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

#include <netedit/GNENet.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNERerouterInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNENet* net) :
    GNEAdditional("", net, "", SUMO_TAG_INTERVAL, ""),
    GNEAdditionalListed(this) {
}


GNERerouterInterval::GNERerouterInterval(GNEAdditional* rerouterParent, SUMOTime begin, SUMOTime end) :
    GNEAdditional(rerouterParent, SUMO_TAG_INTERVAL, ""),
    GNEAdditionalListed(this),
    myBegin(begin),
    myEnd(end) {
    // set parents
    setParent<GNEAdditional*>(rerouterParent);
    // update boundary of rerouter parent
    rerouterParent->updateCenteringBoundary(true);
}


GNERerouterInterval::~GNERerouterInterval() {}


GNEMoveElement*
GNERerouterInterval::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNERerouterInterval::getParameters() {
    return nullptr;
}


const Parameterised*
GNERerouterInterval::getParameters() const {
    return nullptr;
}


void
GNERerouterInterval::writeAdditional(OutputDevice& device) const {
    // avoid write empty intervals
    if (getChildAdditionals().size() > 0) {
        device.openTag(SUMO_TAG_INTERVAL);
        // write common additional attributes
        writeAdditionalAttributes(device);
        // write specific attributes
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
    updateGeometryListedAdditional();
}


Position
GNERerouterInterval::getPositionInView() const {
    return getListedPositionInView();
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
    // draw rerouter interval as listed attribute
    drawListedAdditional(s, RGBColor::RED, RGBColor::YELLOW, GUITexture::REROUTER_INTERVAL,
                         getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END));
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
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
            return getCommonAttribute(key);
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
            return getCommonAttributeDouble(key);
    }
}


Position
GNERerouterInterval::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNERerouterInterval::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
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
            return isCommonAttributeValid(key, value);
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
            setCommonAttribute(key, value);
            break;
    }
}

/****************************************************************************/
