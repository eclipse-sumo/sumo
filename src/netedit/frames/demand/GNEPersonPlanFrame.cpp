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
/// @file    GNEPersonPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add PersonPlan elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEElementTree.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/GNEPlanCreatorLegend.h>

#include "GNEPersonPlanFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEPersonPlanFrame::GNEPersonPlanFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("PersonPlans")) {

    // create person types selector module
    myPersonSelector = new GNEDemandElementSelector(this, {GNETagProperties::Type::PERSON});

    // Create plan selector
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_PERSON);

    // Create person parameters
    myPersonPlanAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create plan creator Module
    myPlanCreator = new GNEPlanCreator(this, viewNet->getNet()->getDemandPathManager());

    // Create GNEElementTree module
    myPersonHierarchy = new GNEElementTree(this);

    // create plan creator legend
    myPlanCreatorLegend = new GNEPlanCreatorLegend(this);
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
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
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
    myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedoTemporally();
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonPlanFrame::addPersonPlanElement(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that we clicked over an AC
    if (viewObjects.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // check if we have to select a new person
    if (myPersonSelector->getCurrentDemandElement() == nullptr) {
        if (viewObjects.getDemandElementFront() && viewObjects.getDemandElementFront()->getTagProperty()->isPerson()) {
            // continue depending of number of demand elements under cursor
            if (viewObjects.getDemandElements().size() > 1) {
                // Filter persons
                myPersonSelector->setDemandElements(viewObjects.getDemandElements());
            } else {
                // select new person
                myPersonSelector->setDemandElement(viewObjects.getDemandElementFront());
            }
            return true;
        } else {
            myViewNet->setStatusBarText(TL("Current selected person isn't valid."));
            return false;
        }
    }
    // finally check that person plan selected is valid
    if (!myPlanSelector->getCurrentPlanTemplate()) {
        myViewNet->setStatusBarText(TL("Current selected person plan isn't valid."));
        return false;
    }
    // continue depending of marked elements
    if (myPlanSelector->markRoutes() && viewObjects.getDemandElementFront() &&
            (viewObjects.getDemandElementFront()->getTagProperty()->getTag() == SUMO_TAG_ROUTE)) {
        return myPlanCreator->addRoute(viewObjects.getDemandElementFront());
    } else if (myPlanSelector->markStoppingPlaces() && viewObjects.getAdditionalFront() &&
               (viewObjects.getAdditionalFront()->getTagProperty()->isStoppingPlace())) {
        return myPlanCreator->addStoppingPlace(viewObjects.getAdditionalFront());
    } else if (myPlanSelector->markJunctions() && viewObjects.getJunctionFront()) {
        return myPlanCreator->addJunction(viewObjects.getJunctionFront());
    } else if (myPlanSelector->markEdges() && viewObjects.getLaneFront()) {
        return myPlanCreator->addEdge(viewObjects.getLaneFront());
    } else if (myPlanSelector->markTAZs() && viewObjects.getTAZFront()) {
        return myPlanCreator->addTAZ(viewObjects.getTAZFront());
    } else {
        return false;
    }
}


void
GNEPersonPlanFrame::resetSelectedPerson() {
    myPersonSelector->setDemandElement(nullptr);
}


GNEPlanCreator*
GNEPersonPlanFrame::getPlanCreator() const {
    return myPlanCreator;
}


GNEElementTree*
GNEPersonPlanFrame::getPersonHierarchy() const {
    return myPersonHierarchy;
}


GNEDemandElementSelector*
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
        // show person attributes
        myPersonPlanAttributesEditor->showAttributesEditor(myPlanSelector->getCurrentPlanTemplate(), true);
        // set path creator mode depending if previousEdge exist
        if (myPersonSelector) {
            // show path creator mode
            myPlanCreator->showPlanCreatorModule(myPlanSelector, myPersonSelector->getPreviousPlanElement());
            // show legend
            myPlanCreatorLegend->showPlanCreatorLegend();
            // show person hierarchy
            myPersonHierarchy->showHierarchicalElementTree(myPersonSelector->getCurrentDemandElement());
        } else {
            // hide modules
            myPlanCreator->hidePathCreatorModule();
            myPersonHierarchy->hideHierarchicalElementTree();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide modules if tag selected isn't valid
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


void
GNEPersonPlanFrame::demandElementSelected() {
    // check if a valid person was selected
    if (myPersonSelector->getCurrentDemandElement()) {
        // show person plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if person plan selected is valid
        if (myPlanSelector->getCurrentPlanTemplate()) {
            // call tag selected
            tagSelected();
        } else {
            myPersonPlanAttributesEditor->hideAttributesEditor();
            myPlanCreator->hidePathCreatorModule();
            myPersonHierarchy->hideHierarchicalElementTree();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide modules if person selected isn't valid
        myPlanSelector->hidePlanSelector();
        myPersonPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
        myPersonHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


bool
GNEPersonPlanFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myPersonPlanAttributesEditor->checkAttributes(true)) {
        return false;
    } else {
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), myPersonSelector->getCurrentDemandElement()->getAttribute(GNE_ATTR_DEMAND_FILE),
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
        // check if person plan can be created
        if (routeHandler.buildPersonPlan(myPlanSelector->getCurrentPlanTemplate(), myPersonSelector->getCurrentDemandElement(),
                                         myPersonPlanAttributesEditor, myPlanCreator, false)) {
            // refresh GNEElementTree
            myPersonHierarchy->refreshHierarchicalElementTree();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh using tagSelected
            tagSelected();
            // refresh personPlan attributes
            myPersonPlanAttributesEditor->refreshAttributesEditor();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            return true;
        } else {
            return false;
        }
    }
}

/****************************************************************************/
