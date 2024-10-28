/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <netedit/elements/network/GNEEdge.h>

#include "GNEEdgeTemplate.h"
#include "GNELaneTemplate.h"

// ===========================================================================
// members methods
// ===========================================================================

GNEEdgeTemplate::GNEEdgeTemplate(const GNEEdge* edge) :
    GNEAttributeCarrier(SUMO_TAG_EDGE, edge->getNet()),
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
    for (const auto& lane : myEdge->getLanes()) {
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


const Parameterised::Map&
GNEEdgeTemplate::getACParametersMap() const {
    return myEdge->getACParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEEdgeTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

/****************************************************************************/
