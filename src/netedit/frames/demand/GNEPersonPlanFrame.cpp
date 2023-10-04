/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEPersonPlanFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonPlanFrame - methods
// ---------------------------------------------------------------------------

GNEPersonPlanFrame::GNEPersonPlanFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("PersonPlans")),
    myRouteHandler("", viewNet->getNet(), true, false) {

    // create person types selector module
    myPersonSelector = new DemandElementSelector(this, {GNETagProperties::TagType::PERSON});

    // Create plan selector
    myPlanSelector = new GNEPlanSelector(this);

    // Create person parameters
    myPersonPlanAttributes = new GNEAttributesCreator(this);

    // create myPathCreator Module
    myPathCreator = new GNEPathCreator(this);

    // Create GNEElementTree module
    myPersonHierarchy = new GNEElementTree(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
}


GNEPersonPlanFrame::~GNEPersonPlanFrame() {}


void
GNEPersonPlanFrame::show() {
    // get persons maps
    const auto& persons = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSON);
    const auto& personFlows = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_PERSONFLOW);
    // Only show modules if there is at least one person
    if ((persons.size() > 0) || (personFlows.size() > 0)) {
        // show person selector
        myPersonSelector->showDemandElementSelector();
        // refresh tag selector
        myPlanSelector->refreshPlanSelector();
    } else {
        // hide all modules
        myPersonSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
    }
    // show frame
    GNEFrame::show();
}


void
GNEPersonPlanFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // enable undo/redo
    myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonPlanFrame::addPersonPlanElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // check if we have to select a new person
    if (myPersonSelector->getCurrentDemandElement() == nullptr) {
        if (objectsUnderCursor.getDemandElementFront() && objectsUnderCursor.getDemandElementFront()->getTagProperty().isPerson()) {
            // continue depending of number of demand elements under cursor
            if (objectsUnderCursor.getClickedDemandElements().size() > 1) {
                // Filter persons
                myPersonSelector->setDemandElements(objectsUnderCursor.getClickedDemandElements());
            } else {
                // select new person
                myPersonSelector->setDemandElement(objectsUnderCursor.getDemandElementFront());
            }
            return true;
        } else {
            myViewNet->setStatusBarText(TL("Current selected person isn't valid."));
            return false;
        }
    }
    // finally check that person plan selected is valid
    if (!myPlanSelector->isPlanValid()) {
        myViewNet->setStatusBarText(TL("Current selected person plan isn't valid."));
        return false;
    }
    // continue depending of marked elements
    if (myPlanSelector->markRoutes() && objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        return myPathCreator->addRoute(objectsUnderCursor.getDemandElementFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (myPlanSelector->markBusStops() && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (myPlanSelector->markTrainStops() && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_TRAIN_STOP)) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if ((myPlanSelector->markSingleEdges() || myPlanSelector->markContinuousEdges()) && objectsUnderCursor.getEdgeFront()) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (myPlanSelector->markJunctions() && objectsUnderCursor.getJunctionFront()) {
        return myPathCreator->addJunction(objectsUnderCursor.getJunctionFront());
    } else if (myPlanSelector->markTAZs() && objectsUnderCursor.getTAZFront()) {
        return myPathCreator->addTAZ(objectsUnderCursor.getTAZFront());
    } else {
        return false;
    }
}


void
GNEPersonPlanFrame::resetSelectedPerson() {
    myPersonSelector->setDemandElement(nullptr);
}


GNEPathCreator*
GNEPersonPlanFrame::getPathCreator() const {
    return myPathCreator;
}


GNEElementTree*
GNEPersonPlanFrame::getPersonHierarchy() const {
    return myPersonHierarchy;
}


DemandElementSelector*
GNEPersonPlanFrame::getPersonSelector() const {
    return myPersonSelector;
}


GNEPlanSelector*
GNEPersonPlanFrame::getPlanSelector() const {
    return myPlanSelector;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonPlanFrame::tagSelected() {
    // first check if person is valid
    if (myPlanSelector->getCurrentPlanTemplate()) {
        // get ACs
        const auto &ACs = myViewNet->getNet()->getAttributeCarriers();
        // Obtain current person plan tag (only for improve code legibility)
        const auto personPlanProperty = myPlanSelector->getCurrentPlanTemplate()->getTagProperty();
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
        // get previous container plan element
        const auto previousElement = myPersonSelector->getPreviousPlanElement();
        // set path creator mode depending if previousEdge exist
        if (myPersonSelector) {
            // show path creator mode
            myPathCreator->showPathCreatorModule(personPlanProperty.getTag(), true, false);
            // add previous element
            if (previousElement->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
                // add junction
                myPathCreator->addJunction(ACs->retrieveJunction(previousElement->getID()));
            } else if (previousElement->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                // update VClass of myPathCreator depending if person is a ride
                if (personPlanProperty.isRide()) {
                    myPathCreator->setVClass(SVC_PASSENGER);
                } else {
                    myPathCreator->setVClass(SVC_PEDESTRIAN);
                }
                // add edge
                myPathCreator->addEdge(ACs->retrieveEdge(previousElement->getID()), true, true);
            } else if (previousElement->getTagProperty().isStoppingPlace()) {
                // add stopping place
                myPathCreator->addStoppingPlace(ACs->retrieveAdditional(previousElement->getTagProperty().getTag(), previousElement->getID()), true, true);
            } else if (previousElement->getTagProperty().getTag() == SUMO_TAG_TAZ) {
                // add TAZ
                myPathCreator->addTAZ(ACs->retrieveAdditional(SUMO_TAG_TAZ, previousElement->getID()));
            }
            // show legend
            myPathLegend->showPathLegendModule();
        } else {
            // set path creator mode
            myPathCreator->showPathCreatorModule(personPlanProperty.getTag(), false, false);
            // show legend
            myPathLegend->showPathLegendModule();
        }
        // show person hierarchy
        myPersonHierarchy->showHierarchicalElementTree(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide modules if tag selected isn't valid
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEPersonPlanFrame::demandElementSelected() {
    // check if a valid person was selected
    if (myPersonSelector->getCurrentDemandElement()) {
        // show person plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if person plan selected is valid
        if (myPlanSelector->isPlanValid()) {
            // call tag selected
            tagSelected();
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myPathCreator->hidePathCreatorModule();
            myPersonHierarchy->hideHierarchicalElementTree();
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide modules if person selected isn't valid
        myPlanSelector->hidePlanSelector();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
    }
}


bool
GNEPersonPlanFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPlanSelector->getCurrentPlanTemplate()->getTagProperty().getTagStr() + " parameters.");
        return false;
    } else {
        // check if person plan can be created
        if (myRouteHandler.buildPersonPlan(
                    myPlanSelector->getCurrentPlanTemplate()->getTagProperty().getTag(),
                    myPersonSelector->getCurrentDemandElement(),
                    myPersonPlanAttributes, myPathCreator, false)) {
            // refresh GNEElementTree
            myPersonHierarchy->refreshHierarchicalElementTree();
            // abort path creation
            myPathCreator->abortPathCreation();
            // refresh using tagSelected
            tagSelected();
            // refresh personPlan attributes
            myPersonPlanAttributes->refreshAttributesCreator();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

/****************************************************************************/
