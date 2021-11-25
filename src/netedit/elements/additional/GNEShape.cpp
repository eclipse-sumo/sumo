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
/// @file    GNEShape.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// Abstract class for Shapes uses in netedit
/****************************************************************************/
#include <config.h>

#include <utils/gui/images/GUITextureSubSys.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEShape.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEShape::GNEShape(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                   const std::vector<GNEJunction*>& junctionParents,
                   const std::vector<GNEEdge*>& edgeParents,
                   const std::vector<GNELane*>& laneParents,
                   const std::vector<GNEAdditional*>& additionalParents,
                   const std::vector<GNEShape*>& shapeParents,
                   const std::vector<GNETAZElement*>& TAZElementParents,
                   const std::vector<GNEDemandElement*>& demandElementParents,
                   const std::vector<GNEGenericData*>& genericDataParents) :
    GUIGlObject(type, id),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents) {
}


GNEShape::~GNEShape() {}


const std::string&
GNEShape::getID() const {
    return getMicrosimID();
}


GUIGlObject*
GNEShape::getGUIGlObject() {
    return this;
}


Boundary
GNEShape::getCenteringBoundary() const {
    return myBoundary;
}


void
GNEShape::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNEShape::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNEShape::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
}


std::string
GNEShape::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEShape::getHierarchyName() const {
    return getTagStr();
}


void
GNEShape::replaceShapeParentLanes(const std::string& value) {
    replaceParentElements(this, parse<std::vector<GNELane*> >(getNet(), value));
}


void
GNEShape::shiftLaneIndex() {
    // get new lane parent vector
    std::vector<GNELane*> newLane = {getParentLanes().front()->getParentEdge()->getLanes().at(getParentLanes().front()->getIndex() + 1)};
    // replace parent elements
    replaceParentElements(this, newLane);
}


void
GNEShape::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw InvalidArgument("Nothing to enable");
}

/****************************************************************************/
