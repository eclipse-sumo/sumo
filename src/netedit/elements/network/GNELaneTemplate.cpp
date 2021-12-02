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
/// @file    GNELaneTemplate.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
// Template for lanes
/****************************************************************************/
#include <config.h>

#include <netedit/elements/network/GNELane.h>

#include "GNELaneTemplate.h"

// ===========================================================================
// members methods
// ===========================================================================

GNELaneTemplate::GNELaneTemplate(const GNELane *lane) :
    GNEAttributeCarrier(SUMO_TAG_LANE, lane->getNet()),
    myLane(lane) {
}


GNELaneTemplate::~GNELaneTemplate() {
}


const std::string& 
GNELaneTemplate::getID() const {
    return myLane->getID();
}


GUIGlObject* 
GNELaneTemplate::getGUIGlObject() {
    return nullptr;
}


void
GNELaneTemplate::updateGeometry() {
    throw InvalidArgument("cannot be called in templates");
}


std::string
GNELaneTemplate::getAttribute(SumoXMLAttr key) const {
        return myLane->getAttribute(key);
}


void
GNELaneTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNELaneTemplate::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

void 
GNELaneTemplate::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


void 
GNELaneTemplate::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNELaneTemplate::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    return false;
}


bool
GNELaneTemplate::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
}


std::string 
GNELaneTemplate::getPopUpID() const {
    return myLane->getPopUpID();
}


std::string 
GNELaneTemplate::getHierarchyName() const {
    return myLane->getHierarchyName();
}


const std::map<std::string, std::string>&
GNELaneTemplate::getACParametersMap() const {
    return myLane->getACParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNELaneTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

void 
GNELaneTemplate::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/, const int /*previousParameters*/) {
    throw InvalidArgument("cannot be called in templates");
}

/****************************************************************************/
