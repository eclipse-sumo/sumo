/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
/// @version $Id$
///
// The Widget for add Person elements
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
#include <netedit/demandelements/GNEPerson.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEUndoList.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBVehicle.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPersonFrame::VTypeSelector) VTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNEPersonFrame::VTypeSelector::onCmdSelectVType),
};

FXDEFMAP(GNEPersonFrame::TripRouteCreator) TripRouteCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_ABORT,          GNEPersonFrame::TripRouteCreator::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_FINISHCREATION, GNEPersonFrame::TripRouteCreator::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GNEPersonFrame::TripRouteCreator::onCmdRemoveLastRouteEdge)
};

// Object implementation
FXIMPLEMENT(GNEPersonFrame::VTypeSelector,     FXGroupBox, VTypeSelectorMap,       ARRAYNUMBER(VTypeSelectorMap))
FXIMPLEMENT(GNEPersonFrame::TripRouteCreator,  FXGroupBox, TripRouteCreatorMap,    ARRAYNUMBER(TripRouteCreatorMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonFrame::VTypeSelector - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::VTypeSelector::VTypeSelector(GNEPersonFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Person Type", GUIDesignGroupBoxFrame),
    myPersonFrameParent(vehicleFrameParent),
    myCurrentPersonType(nullptr) {
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // refresh TypeMatchBox
    refreshVTypeSelector();
    // VTypeSelector is always shown
    show();
}


GNEPersonFrame::VTypeSelector::~VTypeSelector() {}


const GNEDemandElement*
GNEPersonFrame::VTypeSelector::getCurrentPersonType() const {
    return myCurrentPersonType;
}


void
GNEPersonFrame::VTypeSelector::showVTypeSelector(const GNEAttributeCarrier::TagProperties& /*tagProperties*/) {
    refreshVTypeSelector();
    // if current selected item isn't valid, set DEFAULT_VEHTYPE
    if (myCurrentPersonType) {
        // set DEFAULT_VTYPE as current VType
        myTypeMatchBox->setText(myCurrentPersonType->getID().c_str());
    } else {
        myTypeMatchBox->setText(DEFAULT_VTYPE_ID.c_str());
    }
    onCmdSelectVType(nullptr, 0, nullptr);
}


void
GNEPersonFrame::VTypeSelector::hideVTypeSelector() {
    hide();
}


void
GNEPersonFrame::VTypeSelector::refreshVTypeSelector() {
    // clear comboBox
    myTypeMatchBox->clearItems();
    // get list of VTypes
    const auto& vTypes = myPersonFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE);
    // fill myTypeMatchBox with list of tags
    for (const auto& i : vTypes) {
        myTypeMatchBox->appendItem(i.first.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
}


long
GNEPersonFrame::VTypeSelector::onCmdSelectVType(FXObject*, FXSelector, void*) {
    // get list of VTypes
    const auto& vTypes = myPersonFrameParent->getViewNet()->getNet()->getAttributeCarriers().demandElements.at(SUMO_TAG_VTYPE);
    // Check if value of myTypeMatchBox correspond to a VType
    for (const auto& i : vTypes) {
        if (i.first == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current VType
            myCurrentPersonType = i.second;
            // show vehicle attributes modul
            myPersonFrameParent->myPersonAttributes->showAttributesCreatorModul(myPersonFrameParent->myItemSelector->getCurrentTagProperties());
            // show help creation
            myPersonFrameParent->myHelpCreation->showHelpCreation();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in VTypeSelector").text());
            return 1;
        }
    }
    // if VType selecte is invalid, select
    myCurrentPersonType = nullptr;
    // hide all moduls if selected item isn't valid
    myPersonFrameParent->myPersonAttributes->hideAttributesCreatorModul();
    // hide help creation
    myPersonFrameParent->myHelpCreation->hideHelpCreation();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in VTypeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::HelpCreation::HelpCreation(GNEPersonFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myPersonFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEPersonFrame::HelpCreation::~HelpCreation() {}


void
GNEPersonFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEPersonFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEPersonFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected vehicle type
    switch (myPersonFrameParent->myItemSelector->getCurrentTagProperties().getTag()) {
        case SUMO_TAG_VEHICLE:
            information
                    << "- Click over a route to\n"
                    << "  create a vehicle.";
            break;
        case SUMO_TAG_TRIP:
            information
                    << "- Select two edges to\n"
                    << "  create a Trip.";
            break;
        case SUMO_TAG_ROUTEFLOW:
            information
                    << "- Click over a route to\n"
                    << "  create a routeFlow.";
            break;
        case SUMO_TAG_FLOW:
            information
                    << "- Select two edges to\n"
                    << "  create a flow.";
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEPersonFrame::TripRouteCreator - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::TripRouteCreator::TripRouteCreator(GNEPersonFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myPersonFrameParent(vehicleFrameParent) {

    // create button for create GEO POIs
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_VEHICLEFRAME_FINISHCREATION, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for create GEO POIs
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_VEHICLEFRAME_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for create GEO POIs
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNEPersonFrame::TripRouteCreator::~TripRouteCreator() {
}


void
GNEPersonFrame::TripRouteCreator::showTripRouteCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    show();
}


void
GNEPersonFrame::TripRouteCreator::hideTripRouteCreator() {
    hide();
}


std::vector<GNEEdge*>
GNEPersonFrame::TripRouteCreator::getSelectedEdges() const {
    return mySelectedEdges;
}


void
GNEPersonFrame::TripRouteCreator::addEdge(GNEEdge* edge) {
    if (mySelectedEdges.empty() || ((mySelectedEdges.size() > 0) && (mySelectedEdges.back() != edge))) {
        mySelectedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myPersonFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myPersonFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (mySelectedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(myPersonFrameParent->myVTypeSelector->getCurrentPersonType()->getVClass(), mySelectedEdges);
        }
    }
}


void
GNEPersonFrame::TripRouteCreator::clearEdges() {
    // restore colors
    for (const auto& i : mySelectedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    mySelectedEdges.clear();
    myTemporalRoute.clear();
    // enable undo/redo
    myPersonFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
}


void
GNEPersonFrame::TripRouteCreator::drawTemporalRoute() const {
    // only draw if there is at least two edges
    if (myTemporalRoute.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.front(),
                           myTemporalRoute.at(0)->getNBEdge()->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)myTemporalRoute.size(); i++) {
            GLHelper::drawLine(myTemporalRoute.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front());
            GLHelper::drawLine(myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front(),
                               myTemporalRoute.at(i)->getNBEdge()->getLanes().front().shape.back());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


bool
GNEPersonFrame::TripRouteCreator::isValid(SUMOVehicleClass /* vehicleClass */) const {
    return mySelectedEdges.size() > 0;
}


long
GNEPersonFrame::TripRouteCreator::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    clearEdges();
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    return 1;
}


long
GNEPersonFrame::TripRouteCreator::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (mySelectedEdges.size() > 1) {
        // obtain tag (only for improve code legibility)
        SumoXMLTag vehicleTag = myPersonFrameParent->myItemSelector->getCurrentTagProperties().getTag();
        // Declare map to keep attributes from Frames from Frame
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonFrameParent->myPersonAttributes->getAttributesAndValues(false);
        // add ID parameter
        valuesMap[SUMO_ATTR_ID] = myPersonFrameParent->myViewNet->getNet()->generateDemandElementID("", vehicleTag);
        // add VType parameter
        valuesMap[SUMO_ATTR_TYPE] = myPersonFrameParent->myVTypeSelector->getCurrentPersonType()->getID();
        // check if we're creating a trip or flow
        if (vehicleTag == SUMO_TAG_TRIP) {
            // Add parameter departure
            if (valuesMap[SUMO_ATTR_DEPART].empty()) {
                valuesMap[SUMO_ATTR_DEPART] = "0";
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, myPersonFrameParent->getPredefinedTagsMML(), toString(vehicleTag));
            // obtain trip parameters
            SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
            // build trip in GNERouteHandler
            GNERouteHandler::buildTrip(myPersonFrameParent->myViewNet, true, *tripParameters, mySelectedEdges);
            // delete tripParameters
            delete tripParameters;
        } else {
            // set begin and end attributes
            if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
                valuesMap[SUMO_ATTR_BEGIN] = "0";
            }
            if (valuesMap[SUMO_ATTR_END].empty()) {
                valuesMap[SUMO_ATTR_END] = "3600";
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, myPersonFrameParent->getPredefinedTagsMML(), toString(vehicleTag));
            // obtain flow parameters
            SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
            // build flow in GNERouteHandler
            GNERouteHandler::buildFlow(myPersonFrameParent->myViewNet, true, *flowParameters, mySelectedEdges);
            // delete flowParameters
            delete flowParameters;
        }
        // clear edges
        clearEdges();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
    }
    return 1;
}


long
GNEPersonFrame::TripRouteCreator::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (mySelectedEdges.size() > 1) {
        // remove last edge
        mySelectedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(myPersonFrameParent->myVTypeSelector->getCurrentPersonType()->getVClass(), mySelectedEdges);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonFrame - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::GNEPersonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Persons") {

    // Create item Selector modul for vehicles
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TagType::TAGTYPE_VEHICLE);

    // Create vehicle type selector
    myVTypeSelector = new VTypeSelector(this);

    // Create vehicle parameters
    myPersonAttributes = new AttributesCreator(this);

    // create TripRouteCreator Modul
    myTripRouteCreator = new TripRouteCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // set Person as default vehicle
    myItemSelector->setCurrentTypeTag(SUMO_TAG_VEHICLE);
}


GNEPersonFrame::~GNEPersonFrame() {}


void
GNEPersonFrame::show() {
    // refresh item selector
    myItemSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myItemSelector->getCurrentTagProperties().getTag();
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected vehicle isn't valid.");
        return false;
    }
    // now check if VType is valid
    if (myVTypeSelector->getCurrentPersonType() == nullptr) {
        myViewNet->setStatusBarText("Current selected vehicle type isn't valid.");
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myPersonAttributes->getAttributesAndValues(false);
    // add ID parameter
    valuesMap[SUMO_ATTR_ID] = myViewNet->getNet()->generateDemandElementID("", vehicleTag);
    // add VType
    valuesMap[SUMO_ATTR_TYPE] = myVTypeSelector->getCurrentPersonType()->getID();
    // set route or edges depending of vehicle type
    if ((vehicleTag == SUMO_TAG_VEHICLE) || (vehicleTag == SUMO_TAG_ROUTEFLOW)) {
        if (objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().isRoute())) {
            // obtain route
            valuesMap[SUMO_ATTR_ROUTE] = (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)? objectsUnderCursor.getDemandElementFront()->getID() : "embedded";
            // check if we're creating a vehicle or a flow
            if (vehicleTag == SUMO_TAG_VEHICLE) {
                // Add parameter departure
                if (valuesMap[SUMO_ATTR_DEPART].empty()) {
                    valuesMap[SUMO_ATTR_DEPART] = "0";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain vehicle parameters in vehicleParameters
                SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMOSAXAttrs);
                // check if we're creating a vehicle over a existent route or over a embedded route
                if (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
                    GNERouteHandler::buildVehicleOverRoute(myViewNet, true, *vehicleParameters);
                } else {
                    GNERouteHandler::buildVehicleWithEmbeddedRoute(myViewNet, true, *vehicleParameters, objectsUnderCursor.getDemandElementFront());
                }
                // delete vehicleParameters
                delete vehicleParameters;
            } else {
                // set begin and end attributes
                if (valuesMap[SUMO_ATTR_BEGIN].empty()) {
                    valuesMap[SUMO_ATTR_BEGIN] = "0";
                }
                if (valuesMap[SUMO_ATTR_END].empty()) {
                    valuesMap[SUMO_ATTR_END] = "3600";
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(vehicleTag));
                // obtain routeFlow parameters in routeFlowParameters
                SUMOVehicleParameter* routeFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMOSAXAttrs, 0, SUMOTime_MAX);
                // check if we're creating a vehicle over a existent route or over a embedded route
                if (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
                    GNERouteHandler::buildFlowOverRoute(myViewNet, true, *routeFlowParameters);
                } else {
                    GNERouteHandler::buildFlowWithEmbeddedRoute(myViewNet, true, *routeFlowParameters, objectsUnderCursor.getDemandElementFront());
                }
                // delete routeFlowParameters
                delete routeFlowParameters;
            }
            // all ok, then return true;
            return true;
        } else {
            myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed within a route.");
            return false;
        }
    } else if (((vehicleTag == SUMO_TAG_TRIP) || (vehicleTag == SUMO_TAG_FLOW)) && objectsUnderCursor.getEdgeFront()) {
        // add clicked edge in TripRouteCreator
        myTripRouteCreator->addEdge(objectsUnderCursor.getEdgeFront());
    }
    // nothing crated
    return false;
}


GNEPersonFrame::TripRouteCreator*
GNEPersonFrame::getTripRouteCreator() const {
    return myTripRouteCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show vehicle type selector modul
    myVTypeSelector->showVTypeSelector(tagProperties);
    // show AutoRute creator if we're editing a trip
    if ((myItemSelector->getCurrentTagProperties().getTag() == SUMO_TAG_TRIP) || (myItemSelector->getCurrentTagProperties().getTag() == SUMO_TAG_FLOW)) {
        myTripRouteCreator->showTripRouteCreator();
    } else {
        myTripRouteCreator->hideTripRouteCreator();
    }
}


void
GNEPersonFrame::disableModuls() {
    // hide all moduls if vehicle isn't valid
    myVTypeSelector->hideVTypeSelector();
    myPersonAttributes->hideAttributesCreatorModul();
    myHelpCreation->hideHelpCreation();
}

/****************************************************************************/
