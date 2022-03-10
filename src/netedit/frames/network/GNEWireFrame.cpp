/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2021-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEWireFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
// The Widget for editing wires
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNELane2laneConnection.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEWireFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

/*
FXDEFMAP(GNEWireFrame) GNEWireFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,     GNEWireFrame::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_OK,         GNEWireFrame::onCmdOK)
};
*/

// Object implementation
/* FXIMPLEMENT(GNEWireFrame, FXVerticalFrame, GNEWireFrameMap, ARRAYNUMBER(GNEWireFrameMap)) */

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEWireFrame - methods
// ---------------------------------------------------------------------------

GNEWireFrame::GNEWireFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Wires") {

    // create item Selector modul for wires
    myWireTagSelector = new GNEFrameModules::TagSelector(this, GNETagProperties::TagType::WIRE, SUMO_TAG_TRACTION_SUBSTATION);

    // Create wire parameters
    myWireAttributes = new GNEFrameAttributeModules::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributeModules::NeteditAttributes(this);

    // Create selector parent
    mySelectorWireParent = new GNEFrameModules::SelectorParent(this);
}


GNEWireFrame::~GNEWireFrame() {
    // check if we have to delete base wire object
    if (myBaseWire) {
        delete myBaseWire;
    }
}


bool
GNEWireFrame::addWire(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that current selected wire is valid
    if (myWireTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText("Current selected wire isn't valid.");
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myWireAttributes->areValuesValid()) {
        myWireAttributes->showWarningMessage();
        return false;
    }
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myWireTagSelector->getCurrentTemplateAC()->getTagProperty();
    // create base wire
    if (!createBaseWireObject(tagProperties)) {
        return false;
    }
    // obtain attributes and values
    myWireAttributes->getAttributesAndValues(myBaseWire, true);
    // fill netedit attributes
    if (!myNeteditAttributes->getNeteditAttributesAndValues(myBaseWire, objectsUnderCursor.getLaneFront())) {
        return false;
    }
    // check if wire element has lanes
    if (tagProperties.hasAttribute(SUMO_ATTR_LANES)) {
        return buildWireOverLanes(objectsUnderCursor.getLaneFront(), tagProperties);
    } else {
        return buildWireOverView(tagProperties);
    }
}


void
GNEWireFrame::show() {
    // refresh tag selector
    myWireTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


void
GNEWireFrame::tagSelected() {
    if (myWireTagSelector->getCurrentTemplateAC()) {
        // show wire attributes modul
        myWireAttributes->showAttributesCreatorModule(myWireTagSelector->getCurrentTemplateAC(), {});
        // show netedit attributes
        myNeteditAttributes->showNeteditAttributesModule(myWireTagSelector->getCurrentTemplateAC()->getTagProperty());
    } else {
        // hide all moduls if wire isn't valid
        myWireAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
    }
}


bool
GNEWireFrame::createBaseWireObject(const GNETagProperties& tagProperty) {
    // check if baseWire exist, and if yes, delete it
    if (myBaseWire) {
        // go to base wire root
        while (myBaseWire->getParentSumoBaseObject()) {
            myBaseWire = myBaseWire->getParentSumoBaseObject();
        }
        // delete baseWire (and all children)
        delete myBaseWire;
        // reset baseWire
        myBaseWire = nullptr;
    }
    // check if wire is child
    if (tagProperty.isChild()) {
        // get wire under cursor
        const GNEAdditional* wireUnderCursor = myViewNet->getObjectsUnderCursor().getAdditionalFront();
        // if user click over an wire element parent, mark int in ParentWireSelector
        if (wireUnderCursor && (wireUnderCursor->getTagProperty().getTag() == tagProperty.getParentTags().front())) {
            // update parent wire selected
            mySelectorWireParent->setIDSelected(wireUnderCursor->getID());
        }
        // stop if currently there isn't a valid selected parent
        if (mySelectorWireParent->getIdSelected().empty()) {
            myWireAttributes->showWarningMessage("A " + toString(tagProperty.getParentTags().front()) + " must be selected before insertion of " + myWireTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + ".");
            return false;
        } else {
            // create baseWire parent
            myBaseWire = new CommonXMLStructure::SumoBaseObject(nullptr);
            // set parent tag
            myBaseWire->setTag(tagProperty.getParentTags().front());
            // add ID
            myBaseWire->addStringAttribute(SUMO_ATTR_ID, mySelectorWireParent->getIdSelected());
            // create baseWire again as child of current myBaseWire
            myBaseWire = new CommonXMLStructure::SumoBaseObject(myBaseWire);
        }
    } else {
        // just create a base wire
        myBaseWire = new CommonXMLStructure::SumoBaseObject(nullptr);
    }
    // set baseWire tag
    myBaseWire->setTag(tagProperty.getTag());
    // BaseWire created, then return true
    return true;
}


bool
GNEWireFrame::buildWireOverLanes(GNELane* lane, const GNETagProperties& tagProperties) {
    // check that lane exist
    if (lane != nullptr) {
        // Get attribute lane
        myBaseWire->addStringAttribute(SUMO_ATTR_LANE, lane->getID());
        // Check if ID has to be generated
        if (!myBaseWire->hasStringAttribute(SUMO_ATTR_ID)) {
            myBaseWire->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperties.getTag()));
        }
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    const double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // set attribute position as mouse position over lane
    myBaseWire->addDoubleAttribute(SUMO_ATTR_POSITION, mousePositionOverLane);
    // show warning dialogbox and stop check if input parameters are valid
    if (!myWireAttributes->areValuesValid()) {
        myWireAttributes->showWarningMessage();
        return false;
    } else {
    // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true);
        // build wire
        additionalHandler.parseSumoBaseObject(myBaseWire);
        // Refresh wire Parent Selector (For wires that have a limited number of children)
        mySelectorWireParent->refreshSelectorParentModule();
        // refresh wire attributes
        myWireAttributes->refreshAttributesCreator();
        return true;
    }
}


bool
GNEWireFrame::buildWireOverView(const GNETagProperties& tagProperties) {
    // disable intervals (temporal)
    if ((tagProperties.getTag() == SUMO_TAG_INTERVAL) ||
        (tagProperties.getTag() == SUMO_TAG_DEST_PROB_REROUTE) ||
        (tagProperties.getTag() == SUMO_TAG_CLOSING_REROUTE) ||
        (tagProperties.getTag() == SUMO_TAG_CLOSING_LANE_REROUTE) ||
        (tagProperties.getTag() == SUMO_TAG_ROUTE_PROB_REROUTE) ||
        (tagProperties.getTag() == SUMO_TAG_PARKING_AREA_REROUTE)) {
        WRITE_WARNING("Currently unsuported. Create rerouter elements using rerouter dialog");
        return false;
    }
        // disable intervals (temporal)
    if (tagProperties.getTag() == SUMO_TAG_STEP) {
        WRITE_WARNING("Currently unsuported. Create VSS steps elements using VSS dialog");
        return false;
    }
    // Check if ID has to be generated
    if (!myBaseWire->hasStringAttribute(SUMO_ATTR_ID)) {
        myBaseWire->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperties.getTag()));
    }
    // Obtain position as the clicked position over view
    const Position viewPos = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());
    // add position and X-Y-Z attributes
    myBaseWire->addPositionAttribute(SUMO_ATTR_POSITION, viewPos);
    myBaseWire->addDoubleAttribute(SUMO_ATTR_X, viewPos.x());
    myBaseWire->addDoubleAttribute(SUMO_ATTR_Y, viewPos.y());
    myBaseWire->addDoubleAttribute(SUMO_ATTR_Z, viewPos.z());
    // show warning dialogbox and stop check if input parameters are valid
    if (myWireAttributes->areValuesValid() == false) {
        myWireAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true);
        // build wire
        additionalHandler.parseSumoBaseObject(myBaseWire);
        // Refresh wire Parent Selector (For wires that have a limited number of children)
        mySelectorWireParent->refreshSelectorParentModule();
        // refresh wire attributes
        myWireAttributes->refreshAttributesCreator();
        return true;
    }
}

/****************************************************************************/
