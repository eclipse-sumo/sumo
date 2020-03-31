/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEPersonPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add PersonPlan elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEBusStop.h>
#include <netedit/elements/demand/GNEPerson.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEPersonPlanFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPersonPlanFrame::PersonPlanCreator) PersonPlanCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,      GNEPersonPlanFrame::PersonPlanCreator::onCmdAbortPersonPlanCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,     GNEPersonPlanFrame::PersonPlanCreator::onCmdFinishPersonPlanCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST, GNEPersonPlanFrame::PersonPlanCreator::onCmdRemoveLastElement)
};

// Object implementation
FXIMPLEMENT(GNEPersonPlanFrame::PersonPlanCreator,  FXGroupBox, PersonPlanCreatorMap, ARRAYNUMBER(PersonPlanCreatorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::HelpCreation::HelpCreation(GNEPersonPlanFrame* vehicleFrameParent) :
    FXGroupBox(vehicleFrameParent->myContentFrame, "Help", GUIDesignGroupBoxFrame),
    myPersonPlanFrameParent(vehicleFrameParent) {
    myInformationLabel = new FXLabel(this, "", 0, GUIDesignLabelFrameInformation);
}


GNEPersonPlanFrame::HelpCreation::~HelpCreation() {}


void
GNEPersonPlanFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEPersonPlanFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEPersonPlanFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // first check if Person Plan selector is shown
    if (myPersonPlanFrameParent->myPersonSelector->isDemandElementSelectorShown()) {
        // set text depending of selected person plan
        switch (myPersonPlanFrameParent->myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
            case SUMO_TAG_PERSONTRIP_FROMTO:
                information
                        << "- Click over edges to\n"
                        << "  create a trip.";
                break;
            case SUMO_TAG_PERSONTRIP_BUSSTOP:
                information
                        << "- Click over an edge and\n"
                        << "  a bus to create a trip.";
                break;
            case SUMO_TAG_WALK_EDGES:
                information
                        << "- Click over a sequenz of\n"
                        << "  consecutive edges to\n"
                        << "  create a walk.";
                break;
            case SUMO_TAG_WALK_FROMTO:
                information
                        << "- Click over edges to\n"
                        << "  create a walk.";
                break;
            case SUMO_TAG_WALK_BUSSTOP:
                information
                        << "- Click over an edge and\n"
                        << "  a bus to create a walk.";
                break;
            case SUMO_TAG_WALK_ROUTE:
                information
                        << "- Click over a route";
                break;
            case SUMO_TAG_RIDE_FROMTO:
                information
                        << "- Click over edges to\n"
                        << "  create a ride.";
                break;
            case SUMO_TAG_RIDE_BUSSTOP:
                information
                        << "- Click over an edge and\n"
                        << "  a bus to create a ride";
                break;
            default:
                break;
        }
    } else {
        information << "-  There aren't persons or\n"
                    << "   personFlows in network.";
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame::PersonPlanCreator - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::PersonPlanCreator::PersonPlanCreator(GNEPersonPlanFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myPersonPlanFrameParent(frameParent) {
    // create label for curren begin element
    new FXLabel(this, "Current from edge:", 0, GUIDesignLabelCenterThick);
    myCurrentBeginElementLabel = new FXLabel(this, "", 0, GUIDesignLabelCenterThick);

    // create button for finish person plan creation
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();

    // create button for abort person plan creation
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();

    // create button for remove last element
    myRemoveLastEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastEdge->disable();
}


GNEPersonPlanFrame::PersonPlanCreator::~PersonPlanCreator() {}


void
GNEPersonPlanFrame::PersonPlanCreator::edgePathCreatorName(const std::string& name) {
    // header needs the first capitalized letter
    std::string nameWithFirstCapitalizedLetter = name;
    nameWithFirstCapitalizedLetter[0] = (char)toupper(nameWithFirstCapitalizedLetter.at(0));
    setText((nameWithFirstCapitalizedLetter + " creator").c_str());
    myFinishCreationButton->setText(("Finish " + name + " creation").c_str());
    myAbortCreationButton->setText(("Abort " + name + " creation").c_str());
}


void
GNEPersonPlanFrame::PersonPlanCreator::showPersonPlanCreator() {
    // simply refresh person plan creator
    refreshPersonPlanCreator();
    // show
    show();
}


void
GNEPersonPlanFrame::PersonPlanCreator::hidePersonPlanCreator() {
    // disable buttons
    myAbortCreationButton->disable();
    myFinishCreationButton->disable();
    myRemoveLastEdge->disable();
    // restore colors
    for (const auto& i : myClickedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    myClickedEdges.clear();
    // clear myTemporalEdgePath
    myTemporalEdgePath.clear();
    // hide
    hide();
}


void
GNEPersonPlanFrame::PersonPlanCreator::refreshPersonPlanCreator() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastEdge->disable();
    // restore colors
    for (const auto& i : myClickedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    myClickedEdges.clear();
    myTemporalEdgePath.clear();
    // first check if person has already child demand elements
    if (myPersonPlanFrameParent->myPersonSelector->getCurrentDemandElement() &&
            (myPersonPlanFrameParent->myPersonSelector->getCurrentDemandElement()->getChildDemandElements().size() > 0)) {
        // obtain last person plan element tag and pointer (to improve code legibliy)
        SumoXMLTag lastPersonPlanElementTag = myPersonPlanFrameParent->myPersonSelector->getCurrentDemandElement()->getChildDemandElements().back()->getTagProperty().getTag();
        GNEDemandElement* lastPersonPlanElement = myPersonPlanFrameParent->myPersonSelector->getCurrentDemandElement()->getChildDemandElements().back();
        // add edge of last person plan of current edited person
        if (lastPersonPlanElementTag == SUMO_TAG_PERSONSTOP_LANE) {
            // obtan edge's lane of stop lane
            addEdge(lastPersonPlanElement->getParentLanes().front()->getParentEdge());
        } else if (lastPersonPlanElementTag == SUMO_TAG_PERSONSTOP_BUSSTOP) {
            // obtan edge's lane of stop stopping place
            addEdge(lastPersonPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge());
        } else if ((lastPersonPlanElementTag == SUMO_TAG_PERSONTRIP_BUSSTOP) || (lastPersonPlanElementTag == SUMO_TAG_WALK_BUSSTOP) || (lastPersonPlanElementTag == SUMO_TAG_RIDE_BUSSTOP)) {
            // obtan edge's lane of Person Plans placed over stopping places
            addEdge(lastPersonPlanElement->getParentAdditionals().front()->getParentLanes().front()->getParentEdge());
        } else if (lastPersonPlanElementTag == SUMO_TAG_WALK_ROUTE) {
            // obtan edge's lane of Person Plans placed over stopping places
            addEdge(lastPersonPlanElement->getParentDemandElements().back()->getParentEdges().back());
        } else {
            // all rest of person plans have parent edges
            addEdge(lastPersonPlanElement->getParentEdges().back());
        }
        // set current begin element information
        myCurrentBeginElementLabel->setText((myClickedEdges.front()->getID()).c_str());
        // show label
        show();
    } else {
        hide();
    }
}


std::vector<GNEEdge*>
GNEPersonPlanFrame::PersonPlanCreator::getClickedEdges() const {
    return myClickedEdges;
}


const std::vector<GNEEdge*>&
GNEPersonPlanFrame::PersonPlanCreator::getEdgePath() const {
    return myTemporalEdgePath;
}


bool
GNEPersonPlanFrame::PersonPlanCreator::addEdge(GNEEdge* edge) {
    // if a certain BusStop was already defined, a new edge cannot be added
    if (myClickedEdges.empty() || ((myClickedEdges.size() > 0) && (myClickedEdges.back() != edge))) {
        myClickedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // disable undo/redo
        myPersonPlanFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myPersonPlanFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (myClickedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            if (myPersonPlanFrameParent->myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myTemporalEdgePath = myPersonPlanFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(SVC_PASSENGER, myClickedEdges);
            } else {
                myTemporalEdgePath = myPersonPlanFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(SVC_PEDESTRIAN, myClickedEdges);
            }
        }
        return true;
    } else {
        return false;
    }
}


void
GNEPersonPlanFrame::PersonPlanCreator::drawTemporalRoute() const {
    // only draw if there is at least two edges
    if (myTemporalEdgePath.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(myTemporalEdgePath.at(0)->getNBEdge()->getLanes().front().shape.front(),
                           myTemporalEdgePath.at(0)->getNBEdge()->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)myTemporalEdgePath.size(); i++) {
            GLHelper::drawLine(myTemporalEdgePath.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                               myTemporalEdgePath.at(i)->getNBEdge()->getLanes().front().shape.front());
            GLHelper::drawLine(myTemporalEdgePath.at(i)->getNBEdge()->getLanes().front().shape.front(),
                               myTemporalEdgePath.at(i)->getNBEdge()->getLanes().front().shape.back());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


void
GNEPersonPlanFrame::PersonPlanCreator::abortPersonPlanCreation() {
    if (myAbortCreationButton->isEnabled()) {
        onCmdAbortPersonPlanCreation(nullptr, 0, nullptr);
    }
}


void
GNEPersonPlanFrame::PersonPlanCreator::finishPersonPlanCreation() {
    if (myFinishCreationButton->isEnabled()) {
        onCmdFinishPersonPlanCreation(nullptr, 0, nullptr);
    }
}


void
GNEPersonPlanFrame::PersonPlanCreator::removeLastAddedElement() {
    if (myRemoveLastEdge->isEnabled()) {
        onCmdRemoveLastElement(nullptr, 0, nullptr);
    }
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdAbortPersonPlanCreation(FXObject*, FXSelector, void*) {
    // refresh person plan creator
    refreshPersonPlanCreator();
    // enable undo/redo
    myPersonPlanFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
    return 1;
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdFinishPersonPlanCreation(FXObject*, FXSelector, void*) {
    // only create route if there is more than two edges
    if (myClickedEdges.size() > 1) {
        // call edgePathCreated
        myPersonPlanFrameParent->personPlanCreated(nullptr, nullptr);
        // update view
        myPersonPlanFrameParent->myViewNet->updateViewNet();
        // refresh person plan creator
        refreshPersonPlanCreator();
        // enable undo/redo
        myPersonPlanFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
    }
    return 1;
}


long
GNEPersonPlanFrame::PersonPlanCreator::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    if (myClickedEdges.size() > 1) {
        // remove last edge
        myClickedEdges.pop_back();
        // calculate temporal route
        if (myPersonPlanFrameParent->myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
            myTemporalEdgePath = myPersonPlanFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(SVC_PASSENGER, myClickedEdges);
        } else {
            myTemporalEdgePath = myPersonPlanFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(SVC_PEDESTRIAN, myClickedEdges);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::GNEPersonPlanFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "PersonPlans") {

    // create person types selector modul
    myPersonSelector = new GNEFrameModuls::DemandElementSelector(this, {GNETagProperties::TagType::PERSON});

    // Create tag selector for person plan
    myPersonPlanTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::PERSONPLAN);

    // Create person parameters
    myPersonPlanAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create PersonPlanCreator Modul
    myPersonPlanCreator = new PersonPlanCreator(this);

    // Create Help Creation Modul
    myHelpCreation = new HelpCreation(this);

    // Create AttributeCarrierHierarchy modul
    myPersonHierarchy = new GNEFrameModuls::AttributeCarrierHierarchy(this);

    // set PersonPlan tag type in tag selector
    myPersonPlanTagSelector->setCurrentTagType(GNETagProperties::TagType::PERSONPLAN);
}


GNEPersonPlanFrame::~GNEPersonPlanFrame() {}


void
GNEPersonPlanFrame::show() {
    // Only show moduls if there is at least one person
    if ((myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON).size() > 0) ||
            (myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW).size() > 0)) {
        // refresh demand element selector
        myPersonSelector->refreshDemandElementSelector();
        // refresh item selector
        myPersonPlanTagSelector->refreshTagProperties();
        // show myPersonPlanCreator
        myPersonPlanCreator->showPersonPlanCreator();
        // set first person as demand element
        if (myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON).size() > 0) {
            myPersonSelector->setDemandElement(myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON).begin()->second);
        } else {
            myPersonSelector->setDemandElement(myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW).begin()->second);
        }
    } else {
        // hide all moduls except helpCreation
        myPersonSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPersonPlanCreator->hidePersonPlanCreator();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
        myHelpCreation->showHelpCreation();
    }
    // show frame
    GNEFrame::show();
}


void
GNEPersonPlanFrame::hide() {
    // abort plan creation
    myPersonPlanCreator->hidePersonPlanCreator();
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonPlanFrame::addPersonPlan(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if person selected is valid
    if (myPersonSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected person isn't valid.");
        return false;
    }
    // finally check that person plan selected is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // Obtain current person plan tag (only for improve code legibility)
    SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTagProperties().getTag();
    // declare flags to check required elements
    /*SUMO_TAG_PERSONSTOP_LANE;*/
    bool requireRoute = (personPlanTag == SUMO_TAG_WALK_ROUTE);
    bool requireBusStop = ((personPlanTag == SUMO_TAG_PERSONTRIP_BUSSTOP) || (personPlanTag == SUMO_TAG_WALK_BUSSTOP) ||
                           (personPlanTag == SUMO_TAG_RIDE_BUSSTOP) || (personPlanTag == SUMO_TAG_PERSONSTOP_BUSSTOP));
    bool requireEdge = ((personPlanTag == SUMO_TAG_PERSONTRIP_FROMTO) || (personPlanTag == SUMO_TAG_WALK_EDGES) ||
                        (personPlanTag == SUMO_TAG_WALK_FROMTO) || (personPlanTag == SUMO_TAG_RIDE_FROMTO));
    // process personPlanCreated(...) depending of required element
    if (requireBusStop && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return personPlanCreated(objectsUnderCursor.getAdditionalFront(), nullptr);
    } else if (requireRoute && objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        return personPlanCreated(nullptr, objectsUnderCursor.getDemandElementFront());
    } else if (requireEdge && objectsUnderCursor.getEdgeFront()) {
        return myPersonPlanCreator->addEdge(objectsUnderCursor.getEdgeFront());
    } else {
        return false;
    }
}


GNEPersonPlanFrame::PersonPlanCreator*
GNEPersonPlanFrame::getPersonPlanCreator() const {
    return myPersonPlanCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonPlanFrame::tagSelected() {
    // first check if person is valid
    if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
        // set edge path creator name
        if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
            myPersonPlanCreator->edgePathCreatorName("person trip");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
            myPersonPlanCreator->edgePathCreatorName("walk");
        } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
            myPersonPlanCreator->edgePathCreatorName("ride");
        }
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
        // show edge path creator
        myPersonPlanCreator->showPersonPlanCreator();
        // show help creation
        myHelpCreation->showHelpCreation();
        // show person hierarchy
        myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPersonPlanCreator->hidePersonPlanCreator();
        myHelpCreation->hideHelpCreation();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
}


void
GNEPersonPlanFrame::demandElementSelected() {
    // check if a valid person was selected
    if (myPersonSelector->getCurrentDemandElement()) {
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // set edge path creator name
            if (myPersonPlanTagSelector->getCurrentTagProperties().isPersonTrip()) {
                myPersonPlanCreator->edgePathCreatorName("person trip");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isWalk()) {
                myPersonPlanCreator->edgePathCreatorName("walk");
            } else if (myPersonPlanTagSelector->getCurrentTagProperties().isRide()) {
                myPersonPlanCreator->edgePathCreatorName("ride");
            }
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModul(myPersonPlanTagSelector->getCurrentTagProperties(), {});
            // show edge path creator
            myPersonPlanCreator->showPersonPlanCreator();
            // show help creation
            myHelpCreation->showHelpCreation();
            // Show the person's children
            myPersonHierarchy->showAttributeCarrierHierarchy(myPersonSelector->getCurrentDemandElement());
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModul();
            myPersonPlanCreator->hidePersonPlanCreator();
            myHelpCreation->hideHelpCreation();
            myPersonHierarchy->hideAttributeCarrierHierarchy();
        }
    } else {
        // hide moduls if person selected isn't valid
        myPersonPlanTagSelector->hideTagSelector();
        myPersonPlanAttributes->hideAttributesCreatorModul();
        myPersonPlanCreator->hidePersonPlanCreator();
        myHelpCreation->hideHelpCreation();
        myPersonHierarchy->hideAttributeCarrierHierarchy();
    }
}


bool
GNEPersonPlanFrame::personPlanCreated(GNEAdditional* busStop, GNEDemandElement* route) {
    // first check that all attributes are valid
    if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTagProperties().getTagStr() + " parameters.");
        return false;
    } else {
        // Declare map to keep attributes from myPersonPlanAttributes
        std::map<SumoXMLAttr, std::string> valuesMap = myPersonPlanAttributes->getAttributesAndValues(true);
        // check what PersonPlan we're creating
        switch (myPersonPlanTagSelector->getCurrentTagProperties().getTag()) {
            case SUMO_TAG_PERSONTRIP_FROMTO: {
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if person trip fromto can be created
                if (myPersonPlanCreator->getEdgePath().size() > 1) {
                    GNERouteHandler::buildPersonTripFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), myPersonPlanCreator->getEdgePath().back(), arrivalPos, types, modes);
                } else {
                    myViewNet->setStatusBarText("A person trip with from-to attributes needs at least two edge.");
                    return false;
                }
                break;
            }
            case SUMO_TAG_PERSONTRIP_BUSSTOP: {
                std::vector<std::string> types = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_VTYPES]);
                std::vector<std::string> modes = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_MODES]);
                // check if person trip busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A person trip with from and busStop attributes needs one edge and one busStop");
                    return false;
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPersonPlanCreator->addEdge(busStop->getParentLanes().front()->getParentEdge());
                    if (myPersonPlanCreator->getEdgePath().size() > 0) {
                        GNERouteHandler::buildPersonTripBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), busStop, types, modes);
                    } else {
                        myViewNet->setStatusBarText("A person trip with from and busStop attributes needs one edge and one busStop");
                        return false;
                    }
                }
                break;
            }
            case SUMO_TAG_WALK_EDGES: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if walk edges can be created
                if (myPersonPlanCreator->getEdgePath().size() > 0) {
                    GNERouteHandler::buildWalkEdges(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath(), arrivalPos);
                } else if ((myPersonPlanCreator->getClickedEdges().size() == 1) || (myPersonPlanCreator->getClickedEdges().size() == 2)) {
                    GNERouteHandler::buildWalkEdges(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getClickedEdges(), arrivalPos);
                } else {
                    myViewNet->setStatusBarText("A walk with edges attribute needs a list of edges");
                    return false;
                }
                break;
            }
            case SUMO_TAG_WALK_FROMTO: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if walk fromto can be created
                if (myPersonPlanCreator->getEdgePath().size() > 1) {
                    GNERouteHandler::buildWalkFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), myPersonPlanCreator->getEdgePath().back(), arrivalPos);
                } else {
                    myViewNet->setStatusBarText("A walk with from-to attributes needs at least two edges.");
                    return false;
                }
                break;
            }
            case SUMO_TAG_WALK_BUSSTOP: {
                // check if walk busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A walk with from and busStop attributes needs one edge and one busStop");
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPersonPlanCreator->addEdge(busStop->getParentLanes().front()->getParentEdge());
                    if (myPersonPlanCreator->getEdgePath().size() > 0) {
                        GNERouteHandler::buildWalkBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), busStop);
                    } else {
                        myViewNet->setStatusBarText("A walk with from and busStop attributes needs one edge and one busStop");
                        return false;
                    }
                }
                break;
            }
            case SUMO_TAG_WALK_ROUTE: {
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if walk route can be created
                if (route != nullptr) {
                    GNERouteHandler::buildWalkRoute(myViewNet, true, myPersonSelector->getCurrentDemandElement(), route, arrivalPos);
                } else {
                    myViewNet->setStatusBarText("A ride with route attributes needs one route");
                    return false;
                }
                break;
            }
            case SUMO_TAG_RIDE_FROMTO: {
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                double arrivalPos = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_ARRIVALPOS]);
                // check if ride fromto can be created
                if (myPersonPlanCreator->getEdgePath().size() > 2) {
                    GNERouteHandler::buildRideFromTo(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), myPersonPlanCreator->getEdgePath().back(), lines, arrivalPos);
                } else {
                    myViewNet->setStatusBarText("A ride with from-to attributes needs at least two edge.");
                    return false;
                }
                break;
            }
            case SUMO_TAG_RIDE_BUSSTOP: {
                std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(valuesMap[SUMO_ATTR_LINES]);
                // check if ride busstop can be created
                if (busStop == nullptr) {
                    myViewNet->setStatusBarText("A ride with from and busStop attributes needs one edge and one busStop");
                } else {
                    // add busstop's edge to personPlan creator (To calculate a temporal route)
                    myPersonPlanCreator->addEdge(busStop->getParentLanes().front()->getParentEdge());
                    if (myPersonPlanCreator->getEdgePath().size() > 0) {
                        GNERouteHandler::buildRideBusStop(myViewNet, true, myPersonSelector->getCurrentDemandElement(), myPersonPlanCreator->getEdgePath().front(), busStop, lines);
                    } else {
                        myViewNet->setStatusBarText("A ride with from and busStop attributes needs one edge and one busStop");
                        return false;
                    }
                }
                break;
            }
            default:
                throw InvalidArgument("Invalid person plan tag");
        }
        // refresh AttributeCarrierHierarchy
        myPersonHierarchy->refreshAttributeCarrierHierarchy();
        // refresh also Person Plan creator
        myPersonPlanCreator->refreshPersonPlanCreator();
        // refresh personPlan attributes
        myPersonPlanAttributes->refreshRows();
        // person plan element created, then return true
        return true;
    }
}


/****************************************************************************/
