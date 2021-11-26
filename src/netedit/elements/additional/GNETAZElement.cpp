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
/// @file    GNETAZElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2020
///
// Abstract class for TAZElements uses in netedit
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNETAZElement.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZElement::GNETAZElement(const std::string& id, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNETAZElement*>& TAZElementParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents,
                             const std::map<std::string, std::string>& parameters) :
    GUIGlObject(type, id),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    Parameterised(parameters) {
}


GNETAZElement::GNETAZElement(GNETAZElement* TAZElementParent, GNENet* net, GUIGlObjectType type, SumoXMLTag tag,
                             const std::vector<GNEJunction*>& junctionParents,
                             const std::vector<GNEEdge*>& edgeParents,
                             const std::vector<GNELane*>& laneParents,
                             const std::vector<GNEAdditional*>& additionalParents,
                             const std::vector<GNEShape*>& shapeParents,
                             const std::vector<GNETAZElement*>& TAZElementParents,
                             const std::vector<GNEDemandElement*>& demandElementParents,
                             const std::vector<GNEGenericData*>& genericDataParents,
                             const std::map<std::string, std::string>& parameters) :
    GUIGlObject(type, TAZElementParent->generateChildID(tag)),
    GNEHierarchicalElement(net, tag, junctionParents, edgeParents, laneParents, additionalParents, shapeParents, TAZElementParents, demandElementParents, genericDataParents),
    Parameterised(parameters) {
}


GNETAZElement::~GNETAZElement() {}


const std::string&
GNETAZElement::getID() const {
    return getMicrosimID();
}


GUIGlObject*
GNETAZElement::getGUIGlObject() {
    return this;
}


std::string
GNETAZElement::generateChildID(SumoXMLTag childTag) {
    int counter = (int)getChildTAZElements().size();
    while (myNet->getAttributeCarriers()->retrieveTAZElement(childTag, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


GUIParameterTableWindow*
GNETAZElement::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& i : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (i.isUnique()) {
            ret->mkItem(i.getAttrStr().c_str(), false, getAttribute(i.getAttr()));
        } else {
            ret->mkItem(i.getAttrStr().c_str(), true, getAttribute(i.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


const std::map<std::string, std::string>&
GNETAZElement::getACParametersMap() const {
    return getParametersMap();
}

// ---------------------------------------------------------------------------
// GNETAZElement - protected methods
// ---------------------------------------------------------------------------

void
GNETAZElement::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


void
GNETAZElement::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    //
}


bool
GNETAZElement::isAttributeComputed(SumoXMLAttr /*key*/) const {
    return false;
}


bool
GNETAZElement::checkChildTAZElementRestriction() const {
    // throw exception because this function mus be implemented in child (see GNEE3Detector)
    throw ProcessError("Calling non-implemented function checkChildTAZElementRestriction during saving of " + getTagStr() + ". It muss be reimplemented in child class");
}


void 
GNETAZElement::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/, const int /*previousParameters*/) {
    throw InvalidArgument("Nothing to enable");
}

/****************************************************************************/
