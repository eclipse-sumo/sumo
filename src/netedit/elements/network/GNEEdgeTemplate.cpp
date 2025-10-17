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
/// @file    GNEEdgeTemplate.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
// Template for edges
/****************************************************************************/

#include <netedit/elements/network/GNEEdge.h>

#include "GNEEdgeTemplate.h"
#include "GNELaneTemplate.h"

// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeTemplate::GNEEdgeTemplate(const GNEEdge* edge) :
    GNEAttributeCarrier(SUMO_TAG_EDGE, edge->getNet(), "", false),
    myEdge(edge) {
    // update lane templates
    updateLaneTemplates();
}


GNEEdgeTemplate::~GNEEdgeTemplate() {
    for (const auto& laneTemplate : myLaneTemplates) {
        delete laneTemplate;
    }
}


GNEHierarchicalElement*
GNEEdgeTemplate::getHierarchicalElement() {
    return nullptr;
}


GNEMoveElement*
GNEEdgeTemplate::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEEdgeTemplate::getParameters() {
    return nullptr;
}


const Parameterised*
GNEEdgeTemplate::getParameters() const {
    return nullptr;
}


const std::vector<GNELaneTemplate*>&
GNEEdgeTemplate::getLaneTemplates() const {
    return myLaneTemplates;
}


void
GNEEdgeTemplate::updateLaneTemplates() {
    // first remove all laneTemplates
    for (const auto& laneTemplate : myLaneTemplates) {
        delete laneTemplate;
    }
    myLaneTemplates.clear();
    // now set new laneTemplates
    for (const auto& lane : myEdge->getChildLanes()) {
        myLaneTemplates.push_back(new GNELaneTemplate(lane));
    }
}


GUIGlObject*
GNEEdgeTemplate::getGUIGlObject() {
    return nullptr;
}


const GUIGlObject*
GNEEdgeTemplate::getGUIGlObject() const {
    return nullptr;
}


void
GNEEdgeTemplate::updateGeometry() {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNEEdgeTemplate::checkDrawFromContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawToContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawRelatedContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawOverContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawDeleteContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawSelectContour() const {
    return false;
}


bool
GNEEdgeTemplate::checkDrawMoveContour() const {
    return false;
}


std::string
GNEEdgeTemplate::getAttribute(SumoXMLAttr key) const {
    return myEdge->getAttribute(key);
}


double
GNEEdgeTemplate::getAttributeDouble(SumoXMLAttr key) const {
    return myEdge->getAttributeDouble(key);
}


Position
GNEEdgeTemplate::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEEdgeTemplate::getAttributePositionVector(SumoXMLAttr key) const {
    return myEdge->getAttributePositionVector(key);
}


void
GNEEdgeTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNEEdgeTemplate::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNEEdgeTemplate::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    // All attributes are disabled in templates
    return false;
}


std::string
GNEEdgeTemplate::getPopUpID() const {
    return myEdge->getPopUpID();
}


std::string
GNEEdgeTemplate::getHierarchyName() const {
    return myEdge->getHierarchyName();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEEdgeTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

/****************************************************************************/
