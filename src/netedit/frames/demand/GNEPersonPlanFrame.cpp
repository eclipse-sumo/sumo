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
/// @file    GNEPersonPlanFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2019
///
// The Widget for add PersonPlan elements
/****************************************************************************/
#include <config.h>

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
    GNEFrame(viewParent, viewNet, "PersonPlans"),
    myRouteHandler("", viewNet->getNet(), true, false) {

    // create person types selector modul
    myPersonSelector = new DemandElementSelector(this, {GNETagProperties::TagType::PERSON});

    // Create tag selector for person plan
    myPersonPlanTagSelector = new GNETagSelector(this, GNETagProperties::TagType::PERSONPLAN, GNE_TAG_PERSONTRIP_EDGE);

    // Create person parameters
    myPersonPlanAttributes = new GNEAttributesCreator(this);

    // create myPathCreator Module
    myPathCreator = new GNEPathCreator(this);

    // Create GNEElementTree modul
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
    // Only show moduls if there is at least one person
    if ((persons.size() > 0) || (personFlows.size() > 0)) {
        // show person selector
        myPersonSelector->showDemandElementSelector();
        // refresh tag selector
        myPersonPlanTagSelector->refreshTagSelector();
        // set first person as demand element (this will call demandElementSelected() function)
        if (myViewNet->getFrontAttributeCarrier() && myViewNet->getFrontAttributeCarrier()->getTagProperty().isPerson()) {
            GNEDemandElement* personFound = nullptr;
            // search person
            for (const auto& person : persons) {
                if (myViewNet->getFrontAttributeCarrier()->getID() == person->getID()) {
                    personFound = person;
                }
            }
            // search personFlow
            for (const auto& personFlow : personFlows) {
                if (myViewNet->getFrontAttributeCarrier()->getID() == personFlow->getID()) {
                    personFound = personFlow;
                }
            }
            // check personFound
            if (personFound) {
                myPersonSelector->setDemandElement(personFound);
            }
        } else {
            myPersonSelector->setDemandElement(nullptr);
        }
    } else {
        // hide all moduls
        myPersonSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
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
    if (myPersonPlanTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected person plan isn't valid."));
        return false;
    }
    // Obtain current person plan tag (only for improve code legibility)
    SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // declare flags for requirements
    const bool requireBusStop = ((personPlanTag == GNE_TAG_PERSONTRIP_BUSSTOP) || (personPlanTag == GNE_TAG_WALK_BUSSTOP) ||
                                 (personPlanTag == GNE_TAG_RIDE_BUSSTOP) || (personPlanTag == GNE_TAG_STOPPERSON_BUSSTOP));
    const bool requireEdge = ((personPlanTag == GNE_TAG_PERSONTRIP_EDGE) || (personPlanTag == GNE_TAG_WALK_EDGE) ||
                              (personPlanTag == GNE_TAG_RIDE_EDGE) || (personPlanTag == GNE_TAG_WALK_EDGES) ||
                              (personPlanTag == GNE_TAG_STOPPERSON_EDGE));
    const bool requireJunction = ((personPlanTag == GNE_TAG_PERSONTRIP_JUNCTIONS) || (personPlanTag == GNE_TAG_WALK_JUNCTIONS));
    // continue depending of tag
    if ((personPlanTag == GNE_TAG_WALK_ROUTE) && objectsUnderCursor.getDemandElementFront() && (objectsUnderCursor.getDemandElementFront()->getTagProperty().getTag() == SUMO_TAG_ROUTE)) {
        return myPathCreator->addRoute(objectsUnderCursor.getDemandElementFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (requireBusStop && objectsUnderCursor.getAdditionalFront() && (objectsUnderCursor.getAdditionalFront()->getTagProperty().getTag() == SUMO_TAG_BUS_STOP)) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (requireEdge && objectsUnderCursor.getEdgeFront()) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (requireJunction && objectsUnderCursor.getJunctionFront()) {
        return myPathCreator->addJunction(objectsUnderCursor.getJunctionFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
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

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonPlanFrame::tagSelected() {
    // first check if person is valid
    if (myPersonPlanTagSelector->getCurrentTemplateAC()) {
        // Obtain current person plan tag (only for improve code legibility)
        SumoXMLTag personPlanTag = myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
        // show person attributes
        myPersonPlanAttributes->showAttributesCreatorModule(myPersonPlanTagSelector->getCurrentTemplateAC(), {});
        // get previous person plan
        GNEEdge* previousEdge = myPersonSelector->getPersonPlanPreviousEdge();
        // update VClass of myPathCreator depending if person is a ride
        if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
            myPathCreator->setVClass(SVC_PASSENGER);
        } else {
            myPathCreator->setVClass(SVC_PEDESTRIAN);
        }
        // set path creator mode depending if previousEdge exist
        if (previousEdge) {
            // set path creator mode
            myPathCreator->showPathCreatorModule(personPlanTag, true, false);
            // show legend
            myPathLegend->showPathLegendModule();
            // add previous edge or junction
            if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().hasAttribute(SUMO_ATTR_FROMJUNCTION)) {
                myPathCreator->addJunction(previousEdge->getToJunction(), false, false);
            } else if (!myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson()) {
                myPathCreator->addEdge(previousEdge, false, false);
            }
        } else {
            // set path creator mode
            myPathCreator->showPathCreatorModule(personPlanTag, false, false);
            // show legend
            myPathLegend->showPathLegendModule();
        }
        // show person hierarchy
        myPersonHierarchy->showHierarchicalElementTree(myPersonSelector->getCurrentDemandElement());
    } else {
        // hide moduls if tag selecte isn't valid
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
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTemplateAC()) {
            // call tag selected
            tagSelected();
        } else {
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myPathCreator->hidePathCreatorModule();
            myPersonHierarchy->hideHierarchicalElementTree();
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide moduls if person selected isn't valid
        myPersonPlanTagSelector->hideTagSelector();
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
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " parameters.");
        return false;
    } else {
        // check if person plan can be created
        if (myRouteHandler.buildPersonPlan(
                    myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(),
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
}

/****************************************************************************/
