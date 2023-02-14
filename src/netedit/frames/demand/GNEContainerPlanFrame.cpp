/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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
/// @file    GNEContainerPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The Widget for add ContainerPlan elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/demand/GNERouteHandler.h>
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
    GNEFrame(viewParent, viewNet, "ContainerPlans"),
    myRouteHandler("", viewNet->getNet(), true, false) {

    // create container types selector modul
    myContainerSelector = new DemandElementSelector(this, {GNETagProperties::TagType::CONTAINER});

    // Create tag selector for container plan
    myContainerPlanTagSelector = new GNETagSelector(this, GNETagProperties::TagType::CONTAINERPLAN, GNE_TAG_TRANSPORT_EDGE);

    // Create container parameters
    myContainerPlanAttributes = new GNEAttributesCreator(this);

    // create myPathCreator Module
    myPathCreator = new GNEPathCreator(this);

    // Create GNEElementTree modul
    myContainerHierarchy = new GNEElementTree(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
}


GNEContainerPlanFrame::~GNEContainerPlanFrame() {}


void
GNEContainerPlanFrame::show() {
    // get containers maps
    const auto& containers = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_CONTAINER);
    const auto& containerFlows = myViewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_CONTAINERFLOW);
    // Only show moduls if there is at least one container
    if ((containers.size() > 0) || (containerFlows.size() > 0)) {
        // show container selector
        myContainerSelector->showDemandElementSelector();
        // refresh tag selector
        myContainerPlanTagSelector->refreshTagSelector();
        // set first container as demand element (this will call demandElementSelected() function)
        if (containers.size() > 0) {
            myContainerSelector->setDemandElement(*containers.begin());
        } else {
            myContainerSelector->setDemandElement(*containerFlows.begin());
        }
    } else {
        // hide all moduls
        myContainerSelector->hideDemandElementSelector();
        myContainerPlanTagSelector->hideTagSelector();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
    }
    // show frame
    GNEFrame::show();
}


void
GNEContainerPlanFrame::hide() {
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
GNEContainerPlanFrame::addContainerPlanElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check if container selected is valid
    if (myContainerSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected container isn't valid."));
        return false;
    }
    // finally check that container plan selected is valid
    if (myContainerPlanTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected container plan isn't valid."));
        return false;
    }
    // Obtain current container plan tag (only for improve code legibility)
    SumoXMLTag containerPlanTag = myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // declare flags for requirements
    const bool requireContainerStop = ((containerPlanTag == GNE_TAG_TRANSPORT_CONTAINERSTOP) || (containerPlanTag == GNE_TAG_TRANSHIP_CONTAINERSTOP) ||
                                       (containerPlanTag == GNE_TAG_STOPCONTAINER_CONTAINERSTOP));
    const bool requireEdge = ((containerPlanTag == GNE_TAG_TRANSPORT_EDGE) || (containerPlanTag == GNE_TAG_TRANSHIP_EDGE) ||
                              (containerPlanTag == GNE_TAG_TRANSHIP_EDGES) || (containerPlanTag == GNE_TAG_STOPCONTAINER_EDGE));
    // continue depending of tag
    if (requireContainerStop && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_CONTAINER_STOP)) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (requireEdge && objectsUnderCursor.getEdgeFront()) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else {
        return false;
    }
}


GNEPathCreator*
GNEContainerPlanFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainerPlanFrame::tagSelected() {
    // first check if container is valid
    if (myContainerPlanTagSelector->getCurrentTemplateAC()) {
        // Obtain current container plan tag (only for improve code legibility)
        SumoXMLTag containerPlanTag = myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
        // show container attributes
        myContainerPlanAttributes->showAttributesCreatorModule(myContainerPlanTagSelector->getCurrentTemplateAC(), {});
        // get previous container plan
        GNEEdge* previousEdge = myContainerSelector->getContainerPlanPreviousEdge();
        // set path creator mode depending if previousEdge exist
        if (previousEdge) {
            // set path creator mode
            myPathCreator->showPathCreatorModule(containerPlanTag, true, false);
            // show legend
            myPathLegend->showPathLegendModule();
            // check if add previous edge
            if (!myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopContainer()) {
                myPathCreator->addEdge(previousEdge, false, false);
            }
        } else {
            // set path creator mode
            myPathCreator->showPathCreatorModule(containerPlanTag, false, false);
            // show legend
            myPathLegend->showPathLegendModule();
        }
        // show container hierarchy
        myContainerHierarchy->showHierarchicalElementTree(myContainerSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEContainerPlanFrame::demandElementSelected() {
    // check if a valid container was selected
    if (myContainerSelector->getCurrentDemandElement()) {
        // show container plan tag selector
        myContainerPlanTagSelector->showTagSelector();
        // now check if container plan selected is valid
        if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
            // call tag selected
            tagSelected();
        } else {
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myPathCreator->hidePathCreatorModule();
            myContainerHierarchy->hideHierarchicalElementTree();
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide moduls if container selected isn't valid
        myContainerPlanTagSelector->hideTagSelector();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myPathCreator->hidePathCreatorModule();
        myContainerHierarchy->hideHierarchicalElementTree();
        myPathLegend->hidePathLegendModule();
    }
}


bool
GNEContainerPlanFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myContainerPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " parameters.");
    } else {
        // check if container plan can be created
        if (myRouteHandler.buildContainerPlan(
                    myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(),
                    myContainerSelector->getCurrentDemandElement(),
                    myContainerPlanAttributes,
                    myPathCreator, true)) {
            // refresh GNEElementTree
            myContainerHierarchy->refreshHierarchicalElementTree();
            // abort path creation
            myPathCreator->abortPathCreation();
            // refresh using tagSelected
            tagSelected();
            // refresh containerPlan attributes
            myContainerPlanAttributes->refreshAttributesCreator();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            return true;
        }
    }
    return false;
}

/****************************************************************************/
