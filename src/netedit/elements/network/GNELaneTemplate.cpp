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
/// @file    GNELaneTemplate.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2021
///
// Template for lanes
/****************************************************************************/

#include <netedit/elements/network/GNELane.h>

#include "GNELaneTemplate.h"

// ===========================================================================
// members methods
// ===========================================================================

GNELaneTemplate::GNELaneTemplate(const GNELane* lane) :
    GNEAttributeCarrier(SUMO_TAG_LANE, lane->getNet(), "", false),
    myLane(lane) {
}


GNELaneTemplate::~GNELaneTemplate() {
}


GNEHierarchicalElement*
GNELaneTemplate::getHierarchicalElement() {
    return nullptr;
}


GNEMoveElement*
GNELaneTemplate::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNELaneTemplate::getParameters() {
    return nullptr;
}


const Parameterised*
GNELaneTemplate::getParameters() const {
    return nullptr;
}


GUIGlObject*
GNELaneTemplate::getGUIGlObject() {
    return nullptr;
}


const GUIGlObject*
GNELaneTemplate::getGUIGlObject() const {
    return nullptr;
}


void
GNELaneTemplate::updateGeometry() {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNELaneTemplate::checkDrawFromContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawToContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawRelatedContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawOverContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawDeleteContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNELaneTemplate::checkDrawSelectContour() const {
    return false;
}


bool
GNELaneTemplate::checkDrawMoveContour() const {
    return false;
}


std::string
GNELaneTemplate::getAttribute(SumoXMLAttr key) const {
    return myLane->getAttribute(key);
}


double
GNELaneTemplate::getAttributeDouble(SumoXMLAttr key) const {
    return myLane->getAttributeDouble(key);
}


Position
GNELaneTemplate::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNELaneTemplate::getAttributePositionVector(SumoXMLAttr key) const {
    return myLane->getAttributePositionVector(key);
}


void
GNELaneTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNELaneTemplate::isValid(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}


bool
GNELaneTemplate::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    // All attributes are disabled in templates
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

// ===========================================================================
// private
// ===========================================================================

void
GNELaneTemplate::setAttribute(SumoXMLAttr /*key*/, const std::string& /*value*/) {
    throw InvalidArgument("cannot be called in templates");
}

/****************************************************************************/
