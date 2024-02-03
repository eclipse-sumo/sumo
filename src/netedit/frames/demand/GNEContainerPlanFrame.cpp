/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEContainerPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add ContainerPlan elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNEContainerPlanFrame.h"


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEContainerPlanFrame - methods
// ---------------------------------------------------------------------------

GNEContainerPlanFrame::GNEContainerPlanFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("ContainerPlans")),
    myRouteHandler("", viewNet->getNet(), true, false) {

    // create container types selector module
    myContainerSelector = new GNEDemandElementSelector(this, {GNETagProperties::TagType::CONTAINER});

    // Create plan selector
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_CONTAINER);

    // Create container parameters
    myContainerPlanAttributes = new GNEAttributesCreator(this);

    // create plan creator Module
    myPlanCreator = new GNEPlanCreator(this);

    // Create GNEElementTree module
    myContainerHierarchy = new GNEElementTree(this);

    // create plan creator legend
    myPlanCreatorLegend = new GNEPlanCreatorLegend(this);
}


GNEContainerPlanFrame::~GNEContainerPlanFrame() {}


void
GNEContainerPlanFrame::show() {
    // get containers maps
    const auto& containers = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_CONTAINER);
    const auto& containerFlows = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_CONTAINERFLOW);
    // Only show modules if there is at least one container
    if ((containers.size() > 0) || (containerFlows.size() > 0)) {
        // show container selector
        myContainerSelector->showDemandElementSelector();
        // refresh tag selector
        myPlanSelector->refreshPlanSelector();
    } else {
        // hide all modules
        myContainerSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPlanCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
    // show frame
    GNEFrame::show();
}


void
GNEContainerPlanFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second.second->resetCandidateFlags();
    }
    // enable undo/redo
    myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
    // hide frame
    GNEFrame::hide();
}


bool
GNEContainerPlanFrame::addContainerPlanElement(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that we clicked over an AC
    if (viewObjects.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // check if we have to select a new container
    if (myContainerSelector->getCurrentDemandElement() == nullptr) {
        if (viewObjects.getDemandElementFront() && viewObjects.getDemandElementFront()->getTagProperty().isContainer()) {
            // continue depending of number of demand elements under cursor
            if (viewObjects.getDemandElements().size() > 1) {
                // Filter containers
                myContainerSelector->setDemandElements(viewObjects.getDemandElements());
            } else {
                // select new container
                myContainerSelector->setDemandElement(viewObjects.getDemandElementFront());
            }
            return true;
        } else {
            myViewNet->setStatusBarText(TL("Current selected container isn't valid."));
            return false;
        }
    }
    // finally check that container plan selected is valid
    if (!myPlanSelector->getCurrentPlanTemplate()) {
        myViewNet->setStatusBarText(TL("Current selected container plan isn't valid."));
        return false;
    }
    // continue depending of marked elements
    if (myPlanSelector->markRoutes() && viewObjects.getDemandElementFront() &&
            (viewObjects.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        return myPlanCreator->addRoute(viewObjects.getDemandElementFront());
    } else if (myPlanSelector->markContainerStops() && viewObjects.getAdditionalFront() &&
               (viewObjects.getAdditionalFront()->getTagProperty().isStoppingPlace())) {
        return myPlanCreator->addStoppingPlace(viewObjects.getAdditionalFront());
    } else if (myPlanSelector->markEdges() && viewObjects.getLaneFront()) {
        return myPlanCreator->addEdge(viewObjects.getLaneFront());
    } else if (myPlanSelector->markJunctions() && viewObjects.getJunctionFront()) {
        return myPlanCreator->addJunction(viewObjects.getJunctionFront());
    } else if (myPlanSelector->markTAZs() && viewObjects.getTAZFront()) {
        return myPlanCreator->addTAZ(viewObjects.getTAZFront());
    } else {
        return false;
    }
}


void
GNEContainerPlanFrame::resetSelectedContainer() {
    myContainerSelector->setDemandElement(nullptr);
}


GNEPlanCreator*
GNEContainerPlanFrame::getPlanCreator() const {
    return myPlanCreator;
}


GNEElementTree*
GNEContainerPlanFrame::getContainerHierarchy() const {
    return myContainerHierarchy;
}


GNEDemandElementSelector*
GNEContainerPlanFrame::getContainerSelector() const {
    return myContainerSelector;
}


GNEPlanSelector*
GNEContainerPlanFrame::getPlanSelector() const {
    return myPlanSelector;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainerPlanFrame::tagSelected() {
    // first check if container is valid
    if (myPlanSelector->getCurrentPlanTemplate()) {
        // show container attributes
        myContainerPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
        // set path creator mode depending if previousEdge exist
        if (myContainerSelector) {
            // show path creator mode
            myPlanCreator->showPlanCreatorModule(myPlanSelector, myContainerSelector->getPreviousPlanElement());
            // show legend
            myPlanCreatorLegend->showPlanCreatorLegend();
            // show container hierarchy
            myContainerHierarchy->showHierarchicalElementTree(myContainerSelector->getCurrentDemandElement());
        } else {
            // hide modules
            myPlanCreator->hidePathCreatorModule();
            myContainerHierarchy->hideHierarchicalElementTree();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide modules if tag selected isn't valid
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPlanCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


void
GNEContainerPlanFrame::demandElementSelected() {
    // check if a valid container was selected
    if (myContainerSelector->getCurrentDemandElement()) {
        // show container plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if container plan selected is valid
        if (myPlanSelector->getCurrentPlanTemplate()) {
            // call tag selected
            tagSelected();
        } else {
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myPlanCreator->hidePathCreatorModule();
            myContainerHierarchy->hideHierarchicalElementTree();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide modules if container selected isn't valid
        myPlanSelector->hidePlanSelector();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPlanCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


bool
GNEContainerPlanFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myContainerPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPlanSelector->getCurrentPlanTagProperties().getTagStr() + " parameters.");
        return false;
    } else {
        // check if container plan can be created
        if (myRouteHandler.buildContainerPlan(myPlanSelector->getCurrentPlanTemplate(), myContainerSelector->getCurrentDemandElement(),
                                              myContainerPlanAttributes, myPlanCreator, false)) {
            // refresh GNEElementTree
            myContainerHierarchy->refreshHierarchicalElementTree();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh using tagSelected
            tagSelected();
            // refresh containerPlan attributes
            myContainerPlanAttributes->refreshAttributesCreator();
            // enable show all container plans
            myViewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->setChecked(TRUE);
            return true;
        } else {
            return false;
        }
    }
}

/****************************************************************************/
