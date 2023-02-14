/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2021-2023 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>

#include "GNEWireFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================


GNEWireFrame::GNEWireFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Wires") {

    // create item Selector module for wires
    myWireTagSelector = new GNETagSelector(this, GNETagProperties::TagType::WIRE, SUMO_TAG_TRACTION_SUBSTATION);

    // Create wire parameters
    myWireAttributes = new GNEAttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNENeteditAttributes(this);

    // Create selector parent
    mySelectorWireParent = new GNESelectorParent(this);

    // Create list for E2Multilane lane selector
    myConsecutiveLaneSelector = new GNEConsecutiveSelector(this, true);
}


GNEWireFrame::~GNEWireFrame() {
    // check if we have to delete base wire object
    if (myBaseWire) {
        delete myBaseWire;
    }
}


void
GNEWireFrame::show() {
    // refresh tag selector
    myWireTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


bool
GNEWireFrame::addWire(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that current selected wire is valid
    if (myWireTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected wire isn't valid."));
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
    if (tagProperties.getTag() == SUMO_TAG_OVERHEAD_WIRE_SECTION) {
        return myConsecutiveLaneSelector->addLane(objectsUnderCursor.getLaneFront());
    } else {
        // build wire over view
        return buildWireOverView(tagProperties);
    }
}


GNEConsecutiveSelector*
GNEWireFrame::getConsecutiveLaneSelector() const {
    return myConsecutiveLaneSelector;
}


bool
GNEWireFrame::createPath(const bool /* useLastRoute */) {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperty = myWireTagSelector->getCurrentTemplateAC()->getTagProperty();
    // first check that current tag is valid (currently only for overhead wires)
    if (tagProperty.getTag() == SUMO_TAG_OVERHEAD_WIRE_SECTION) {
        if (myConsecutiveLaneSelector->getLanePath().size() == 1) {
            WRITE_WARNINGF(TL("A % needs at least two lane positions"), toString(SUMO_TAG_OVERHEAD_WIRE_SECTION));
        } else if (createBaseWireObject(tagProperty)) {
            // get attributes and values
            myWireAttributes->getAttributesAndValues(myBaseWire, true);
            // fill netedit attributes
            if (myNeteditAttributes->getNeteditAttributesAndValues(myBaseWire, nullptr)) {
                // Check if ID has to be generated
                if (!myBaseWire->hasStringAttribute(SUMO_ATTR_ID)) {
                    myBaseWire->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperty.getTag()));
                }
                // add lane IDs
                myBaseWire->addStringListAttribute(SUMO_ATTR_LANES, myConsecutiveLaneSelector->getLaneIDPath());
                // set positions
                myBaseWire->addDoubleAttribute(SUMO_ATTR_STARTPOS, myConsecutiveLaneSelector->getLanePath().front().second);
                myBaseWire->addDoubleAttribute(SUMO_ATTR_ENDPOS, myConsecutiveLaneSelector->getLanePath().back().second);
                // show warning dialogbox and stop check if input parameters are valid
                if (myWireAttributes->areValuesValid() == false) {
                    myWireAttributes->showWarningMessage();
                } else {
                    // declare additional handler
                    GNEAdditionalHandler additionalHandler(getViewNet()->getNet(), true, false);
                    // build additional
                    additionalHandler.parseSumoBaseObject(myBaseWire);
                    // Refresh wire Parent Selector (For additionals that have a limited number of children)
                    mySelectorWireParent->refreshSelectorParentModule();
                    // abort overhead wire creation
                    myConsecutiveLaneSelector->abortPathCreation();
                    // refresh additional attributes
                    myWireAttributes->refreshAttributesCreator();
                    return true;
                }
            }
        }
    }
    return false;
}


void
GNEWireFrame::tagSelected() {
    // get template AC
    const auto templateAC = myWireTagSelector->getCurrentTemplateAC();
    // check if templateAC Exist
    if (templateAC) {
        // show wire attributes module
        myWireAttributes->showAttributesCreatorModule(templateAC, {});
        // show netedit attributes
        myNeteditAttributes->showNeteditAttributesModule(templateAC);
        // check if we're creating a overhead wire section
        if (templateAC->getTagProperty().getTag() == SUMO_TAG_OVERHEAD_WIRE_SECTION) {
            myConsecutiveLaneSelector->showConsecutiveLaneSelectorModule();
            mySelectorWireParent->showSelectorParentModule({SUMO_TAG_TRACTION_SUBSTATION});
        } else {
            myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
            mySelectorWireParent->hideSelectorParentModule();
        }
    } else {
        // hide all modules if wire isn't valid
        myWireAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
        mySelectorWireParent->hideSelectorParentModule();
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
    // create a base wire object
    myBaseWire = new CommonXMLStructure::SumoBaseObject(nullptr);
    // check if wire is a overheadWIre
    if (tagProperty.getTag() == SUMO_TAG_OVERHEAD_WIRE_SECTION) {
        // get wire under cursor
        const GNEAdditional* wireUnderCursor = myViewNet->getObjectsUnderCursor().getAdditionalFront();
        // if user click over a traction substation, mark int in ParentWireSelector
        if (wireUnderCursor && (wireUnderCursor->getTagProperty().getTag() == SUMO_TAG_TRACTION_SUBSTATION)) {
            // update parent wire selected
            mySelectorWireParent->setIDSelected(wireUnderCursor->getID());
        }
        // stop if currently there isn't a valid selected parent
        if (mySelectorWireParent->getIdSelected().empty()) {
            myWireAttributes->showWarningMessage("A " + toString(SUMO_TAG_TRACTION_SUBSTATION) + " must be selected before insertion of " + toString(SUMO_TAG_TRACTION_SUBSTATION) + ".");
            return false;
        } else {
            // add tractionsubstation id
            myBaseWire->addStringAttribute(SUMO_ATTR_SUBSTATIONID, mySelectorWireParent->getIdSelected());
        }
    }
    // set baseWire tag
    myBaseWire->setTag(tagProperty.getTag());
    // BaseWire created, then return true
    return true;
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
        WRITE_WARNING(TL("Currently unsupported. Create rerouter elements using rerouter dialog"));
        return false;
    }
    // disable intervals (temporal)
    if (tagProperties.getTag() == SUMO_TAG_STEP) {
        WRITE_WARNING(TL("Currently unsupported. Create VSS steps elements using VSS dialog"));
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
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true, false);
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
