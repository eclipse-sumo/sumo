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

GNEEdgeTemplate::GNEEdgeTemplate(const GNEEdge *edge) :
    GNEAttributeCarrier(SUMO_TAG_EDGE, edge->getNet()),
    myEdge(edge) {
    // update lane templates
    updateLaneTemplates();
}


GNEEdgeTemplate::~GNEEdgeTemplate() {
    for (const auto &laneTemplate : myLaneTemplates) {
        delete laneTemplate;
    }
}


const std::vector<GNELaneTemplate*>&
GNEEdgeTemplate::getLaneTemplates() const {
    return myLaneTemplates;
}


void
GNEEdgeTemplate::updateLaneTemplates() {
    // first remove all laneTemplates
    for (const auto &laneTemplate : myLaneTemplates) {
        delete laneTemplate;
    }
    // now set new laneTemplates
    for (const auto &lane : myEdge->getLanes()) {
        myLaneTemplates.push_back(new GNELaneTemplate(lane));
    }
}


const std::string& 
GNEEdgeTemplate::getID() const {
    return myEdge->getID();
}


GUIGlObject* 
GNEEdgeTemplate::getGUIGlObject() {
    return nullptr;
}


void
GNEEdgeTemplate::updateGeometry() {
    throw InvalidArgument("cannot be called in templates");
}


std::string
GNEEdgeTemplate::getAttribute(SumoXMLAttr key) const {
        return myEdge->getAttribute(key);
}


void
GNEEdgeTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNEEdgeTemplate::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

void 
GNEEdgeTemplate::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


void 
GNEEdgeTemplate::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNEEdgeTemplate::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return false;
}


bool
GNEEdgeTemplate::isAttributeComputed(SumoXMLAttr /*key*/) const {
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


const std::map<std::string, std::string>&
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

void 
GNEEdgeTemplate::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/, const int /*previousParameters*/) {
    throw InvalidArgument("cannot be called in templates");
}

/****************************************************************************/
