/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEStopFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    March 2019
/// @version $Id$
///
// The Widget for add Stops elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEUndoList.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEStopFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEStopFrame::StopSelector) StopSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEStopFrame::StopSelector::onCmdSelectVType),
};

// Object implementation
FXIMPLEMENT(GNEStopFrame::StopSelector,     FXGroupBox, StopSelectorMap,       ARRAYNUMBER(StopSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEStopFrame::StopSelector - methods
// ---------------------------------------------------------------------------

GNEStopFrame::StopSelector::StopSelector(GNEStopFrame* StopFrameParent) :
    FXGroupBox(StopFrameParent->myContentFrame, "Stop Type", GUIDesignGroupBoxFrame),
    myStopFrameParent(StopFrameParent),
    myCurrentStopType(nullptr) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh TypeMatchBox
    refreshStopSelector();
    // StopSelector is always shown
    show();
}


GNEStopFrame::StopSelector::~StopSelector() {}


const GNEDemandElement*
GNEStopFrame::StopSelector::getCurrentStopType() const {
    return myCurrentStopType;
}


void 
GNEStopFrame::StopSelector::showStopSelector(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // if current selected item isn't valid, set DEFAULT_VEHTYPE
    if (myCurrentStopType) {
        // show Stop attributes modul
        myStopFrameParent->myStopAttributes->showAttributesCreatorModul(tagProperties);
        // show help creation
        myStopFrameParent->myHelpCreation->showHelpCreation();
    } else {
        // set DEFAULT_VTYPE as current VType
        myTypeMatchBox->setText(DEFAULT_VTYPE_ID.c_str());
        // call manually onCmdSelectVType to update comboBox
        onCmdSelectVType(nullptr, 0, nullptr);
    }
    // show VType selector
    show();
}


void 
GNEStopFrame::StopSelector::hideStopSelector() {
    hide();
}


void 
GNEStopFrame::StopSelector::refreshStopSelector() {
    // clear comboBox
    myTypeMatchBox->clearItems();
    // get list of VTypes
    const auto &vTypes = myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE);
    // fill myTypeMatchBox with list of tags
    for (const auto& i : vTypes) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
}


long
GNEStopFrame::StopSelector::onCmdSelectVType(FXObject*, FXSelector, void*) {
    // get list of VTypes
    const auto &vTypes = myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VTYPE);
    // Check if value of myTypeMatchBox correspond to a VType
    for (const auto& i : vTypes) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current VType
            myCurrentStopType = i.second;
            // show Stop attributes modul
            myStopFrameParent->myStopAttributes->showAttributesCreatorModul(myStopFrameParent->myItemSelector->getCurrentTagProperties());
            // show help creation
            myStopFrameParent->myHelpCreation->showHelpCreation();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in StopSelector").text());
            return 1;
        }
    }
    // if VType selecte is invalid, select
    myCurrentStopType = nullptr;
    // hide all moduls if selected item isn't valid
    myStopFrameParent->myStopAttributes->hideAttributesCreatorModul();
    // hide help creation
    myStopFrameParent->myHelpCreation->hideHelpCreation();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in StopSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEStopFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEStopFrame::HelpCreation::HelpCreation(GNEStopFrame* StopFrameParent) :
    FXGroupBox(StopFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myStopFrameParent(StopFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEStopFrame::HelpCreation::~HelpCreation() {}


void 
GNEStopFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void 
GNEStopFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void 
GNEStopFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected Stop type
    /*
    switch (myStopFrameParent->myItemSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_Stop:
            information
                << "- Click over a route to\n"
                << "  create a Stop.";
            break;
        case SUMO_TAG_FLOW:
            information
                << "- Click over a route to\n"
                << "  create a flow.";
            break;
        case SUMO_TAG_TRIP:
            information
                << "- Select two edges to\n"
                << "  create a Trip.";
            break;
        default:
            break;
    }
    */
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEStopFrame - methods
// ---------------------------------------------------------------------------

GNEStopFrame::GNEStopFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Stops") {

    // Create item Selector modul for Stops
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_STOP);

    // Create Stop type selector
    myStopSelector = new StopSelector(this);

    // Create Stop parameters
    myStopAttributes = new AttributesCreator(this);
    
    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);
}


GNEStopFrame::~GNEStopFrame() {}


void
GNEStopFrame::show() {
    // refresh item selector
    myItemSelector->refreshTagProperties();
    // refresh vType selector
    myStopSelector->refreshStopSelector();
    // show frame
    GNEFrame::show();
}


bool
GNEStopFrame::addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag StopTag = myItemSelector->getCurrentTagProperties().getTag();

    // first check that current selected Stop is valid
    if (StopTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected Stop isn't valid.");
        return false;
    }

    // now check if VType is valid
    if (myStopSelector->getCurrentStopType() == nullptr) {
        myViewNet->setStatusBarText("Current selected Stop type isn't valid.");
        return false;
    }

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myStopAttributes->getAttributesAndValues(false);

    // add ID
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID(StopTag);
/*
    // add VType
    valuesMap[SUMO_ATTR_TYPE] = myStopSelector->getCurrentStopType()->getID();

    // set route or edges depending of Stop type
    if ((StopTag == SUMO_TAG_Stop || StopTag == SUMO_TAG_FLOW)) {
        if (objectsUnderCursor.getDemandElementFront() && 
           (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
            // obtain route
            valuesMap[SUMO_ATTR_ROUTE] = objectsUnderCursor.getDemandElementFront()->getID();
            if(StopTag == SUMO_TAG_Stop) {
                // Add parameter departure
                if(valuesMap.count(SUMO_ATTR_DEPART) == 0) {
                    valuesMap[SUMO_ATTR_DEPART] = "0";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(StopTag));
                // obtain Stop parameters in StopParameters
                SUMOStopParameter* StopParameters = SUMOStopParserHelper::parseStopAttributes(SUMOSAXAttrs);
                // create it in RouteFrame
                GNERouteHandler::buildStop(myViewNet, true, StopParameters);
                // delete StopParameters
                delete StopParameters;
            } else {
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(StopTag));
                // obtain flow parameters in flowParameters
                SUMOStopParameter* flowParameters = SUMOStopParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
                // create it in RouteFrame
                GNERouteHandler::buildFlow(myViewNet, true, flowParameters);
                // delete StopParameters
                delete flowParameters;
            }
            // all ok, then return true;
            return true;
        } else {
            myViewNet->setStatusBarText(toString(StopTag) + " has to be placed within a route.");
            return false;
        }
    } else if ((StopTag == SUMO_TAG_TRIP) && objectsUnderCursor.getEdgeFront()) {
        // add clicked edge in TripRouteCreator
        myTripRouteCreator->addEdge(objectsUnderCursor.getEdgeFront());
    }
*/
    // nothing crated
    return false;
}


// ===========================================================================
// protected
// ===========================================================================

void
GNEStopFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show Stop type selector modul
    myStopSelector->showStopSelector(tagProperties);
}


void
GNEStopFrame::disableModuls() {
    // hide all moduls if Stop isn't valid
    myStopSelector->hideStopSelector();
    myStopAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
