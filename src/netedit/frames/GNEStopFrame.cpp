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
#include <netedit/demandelements/GNEDemandElement.h>
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

FXDEFMAP(GNEStopFrame::StopParentSelector) StopParentSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEStopFrame::StopParentSelector::onCmdSelectStopParent),
};

// Object implementation
FXIMPLEMENT(GNEStopFrame::StopParentSelector,     FXGroupBox, StopParentSelectorMap,       ARRAYNUMBER(StopParentSelectorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEStopFrame::StopParentSelector - methods
// ---------------------------------------------------------------------------

GNEStopFrame::StopParentSelector::StopParentSelector(GNEStopFrame* StopFrameParent) :
    FXGroupBox(StopFrameParent->myContentFrame, "Stop parent", GUIDesignGroupBoxFrame),
    myStopFrameParent(StopFrameParent),
    myCurrentStopParent(nullptr) {
    // Create FXComboBox
    myStopParentMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh myStopParentMatchBox
    refreshStopParentSelector();
    // StopParentSelector is always shown
    show();
}


GNEStopFrame::StopParentSelector::~StopParentSelector() {}


const GNEDemandElement*
GNEStopFrame::StopParentSelector::getCurrentStopParent() const {
    return myCurrentStopParent;
}


void 
GNEStopFrame::StopParentSelector::setStopParent(GNEDemandElement *stopParent) {

}


void 
GNEStopFrame::StopParentSelector::showStopParentSelector(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // refresh stop parent selector
    refreshStopParentSelector();
    // show VType selector
    show();
}


void 
GNEStopFrame::StopParentSelector::hideStopParentSelector() {
    hide();
}


void 
GNEStopFrame::StopParentSelector::refreshStopParentSelector() {
    // clear comboBox and show
    myStopParentMatchBox->clearItems();
    myStopParentMatchBox->show();
    myStopParentCandidates.clear();
    // reserve stop parent cantidadtes
    myStopParentCandidates.reserve(
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTE).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VEHICLE).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_FLOW).size() +
        myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_TRIP).size());
    // fill myStopParentMatchBox with list of routes
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_ROUTE)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of vehicles
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_VEHICLE)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of flows
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_FLOW)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // fill myStopParentMatchBox with list of trips
    for (const auto& i : myStopFrameParent->getViewNet()->getNet()->getDemandElementByType(SUMO_TAG_TRIP)) {
        myStopParentMatchBox->appendItem(i.first.c_str());
        myStopParentCandidates.push_back(i.second);
    }
    // Set visible items
    if ((int)myStopParentMatchBox->getNumItems() < 20) {
        myStopParentMatchBox->setNumVisible((int)myStopParentMatchBox->getNumItems());
    } else {
        myStopParentMatchBox->setNumVisible(20);
    }
    // check if myCurrentStopParent exists
    bool found = false;
    for (int i = 0; i < myStopParentCandidates.size(); i++) {
        if (myStopParentCandidates.at(i) == myCurrentStopParent) {
            myStopParentMatchBox->setCurrentItem(i);
            found = true;
        }
    }
    if (!found) {
        if (myStopParentCandidates.size() > 0) {
            myStopParentMatchBox->setCurrentItem(0);
        } else {
            myStopParentMatchBox->hide();
        }
    }
}


long
GNEStopFrame::StopParentSelector::onCmdSelectStopParent(FXObject*, FXSelector, void*) {
    // Check if value of myStopParentMatchBox correspond to a existent stop parent candidate
    for (int i = 0; i < myStopParentCandidates.size(); i++) {
        if (myStopParentCandidates.at(i)->getID() == myStopParentMatchBox->getText().text()) {
            // set color of myStopParentMatchBox to black (valid)
            myStopParentMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current VType
            myCurrentStopParent = myStopParentCandidates.at(i);
            // show Stop selector, attributes and help creation moduls
            myStopFrameParent->myStopTypeSelector->showItemSelector();
            myStopFrameParent->myStopAttributes->showAttributesCreatorModul(myStopFrameParent->myStopTypeSelector->getCurrentTagProperties());
            myStopFrameParent->myHelpCreation->showHelpCreation();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myStopParentMatchBox->getText() + "' in StopParentSelector").text());
            return 1;
        }
    }
    // if VType selecte is invalid, select
    myCurrentStopParent = nullptr;
    // hide all moduls if selected item isn't valid
    myStopFrameParent->myStopTypeSelector->hideItemSelector();
    myStopFrameParent->myStopAttributes->hideAttributesCreatorModul();
    myStopFrameParent->myHelpCreation->hideHelpCreation();
    // set color of myStopParentMatchBox to red (invalid)
    myStopParentMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in StopParentSelector");
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
    switch (myStopFrameParent->myStopTypeSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_STOP_BUSSTOP:
            information
                << "- Click over a bus stop\n"
                << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_CONTAINERSTOP:
            information
                << "- Click over a container stop\n"
                << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_CHARGINGSTATION:
            information
                << "- Click over a charging \n"
                << "  station to create a stop.";
            break;
        case SUMO_TAG_STOP_PARKINGAREA:
            information
                << "- Click over a parking area\n"
                << "  to create a stop.";
            break;
        case SUMO_TAG_STOP_LANE:
            information
                << "- Click over a lane to\n"
                << "  create a stop.";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEStopFrame - methods
// ---------------------------------------------------------------------------

GNEStopFrame::GNEStopFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Stops") {

    // Create Stop parent selector
    myStopParentSelector = new StopParentSelector(this);

    // Create item Selector modul for Stops
    myStopTypeSelector = new ItemSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_STOP);

    // Create Stop parameters
    myStopAttributes = new AttributesCreator(this);
    
    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);
}


GNEStopFrame::~GNEStopFrame() {}


void
GNEStopFrame::show() {
    // refresh vType selector
    myStopParentSelector->refreshStopParentSelector();
    // refresh item selector
    myStopTypeSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEStopFrame::addStop(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag StopTag = myStopTypeSelector->getCurrentTagProperties().getTag();

    // first check that current selected Stop is valid
    if (StopTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected Stop isn't valid.");
        return false;
    }

    // now check if VType is valid
    if (myStopParentSelector->getCurrentStopParent() == nullptr) {
        myViewNet->setStatusBarText("Current selected Stop type isn't valid.");
        return false;
    }

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myStopAttributes->getAttributesAndValues(false);

    // add ID
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID(StopTag);
/*
    // add VType
    valuesMap[SUMO_ATTR_TYPE] = myStopParentSelector->getCurrentStopParent()->getID();

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
    myStopAttributes->showAttributesCreatorModul(tagProperties);
    myHelpCreation->showHelpCreation();
}


void
GNEStopFrame::disableModuls() {
    // hide all moduls if Stop isn't valid
    myStopAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
