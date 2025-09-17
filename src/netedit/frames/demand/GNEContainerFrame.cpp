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
/// @file    GNEContainerFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Container elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEPlanCreator.h>
#include <netedit/frames/GNEPlanCreatorLegend.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEContainerFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEContainerFrame::GNEContainerFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Containers")),
    myContainerBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector module for containers
    myContainerTagSelector = new GNETagSelector(this, GNETagProperties::Type::CONTAINER, SUMO_TAG_CONTAINER);

    // create container types selector module and set DEFAULT_PEDTYPE_ID as default element
    myTypeSelector = new GNEDemandElementSelector(this, SUMO_TAG_VTYPE, GNETagProperties::Type::CONTAINER);

    // Create attributes editor
    myContainerAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create plan selector module for container plans
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_CONTAINER);

    // Create attributes editor
    myContainerPlanAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create GNEPlanCreator Module
    myPlanCreator = new GNEPlanCreator(this, viewNet->getNet()->getDemandPathManager());

    // create plan creator legend
    myPlanCreatorLegend = new GNEPlanCreatorLegend(this);
}


GNEContainerFrame::~GNEContainerFrame() {
    delete myContainerBaseObject;
}


void
GNEContainerFrame::show() {
    // refresh tag selector
    myContainerTagSelector->refreshTagSelector();
    myTypeSelector->refreshDemandElementSelector();
    myPlanSelector->refreshPlanSelector();
    // show frame
    GNEFrame::show();
}


void
GNEContainerFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEContainerFrame::addContainer(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that we clicked over an AC
    if (viewObjects.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag containerTag = myContainerTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
    // first check that current selected container is valid
    if (containerTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText(TL("Current selected container isn't valid."));
        return false;
    }
    // now check that pType is valid
    if (myTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected container type isn't valid."));
        return false;
    }
    // finally check that container plan selected is valid
    if (myPlanSelector->getCurrentPlanTemplate() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected container plan isn't valid."));
        return false;
    }
    for (GNEAdditional* o : viewObjects.getAdditionals()) {
        if (o->getTagProperty()->isStoppingPlace()) {
            return myPlanCreator->addStoppingPlace(o);
        }
    }
    for (GNEDemandElement* o : viewObjects.getDemandElements()) {
        if (o->getTagProperty()->getTag() == SUMO_TAG_ROUTE) {
            return myPlanCreator->addRoute(o);
        }
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getJunctionFront()) {
        return myPlanCreator->addJunction(viewObjects.getJunctions().front());
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getLaneFront()) {
        return myPlanCreator->addEdge(viewObjects.getLanes().front());
    }
    if (viewObjects.getAttributeCarrierFront() == viewObjects.getTAZFront()) {
        return myPlanCreator->addTAZ(viewObjects.getTAZs().front());
    }
    return false;
}


GNEPlanCreator*
GNEContainerFrame::getPlanCreator() const {
    return myPlanCreator;
}


GNEDemandElementSelector*
GNEContainerFrame::getTypeSelector() const {
    return myTypeSelector;
}


GNEPlanSelector*
GNEContainerFrame::getPlanSelector() const {
    return myPlanSelector;
}


GNEAttributesEditor*
GNEContainerFrame::getContainerAttributesEditor() const {
    return myContainerAttributesEditor;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainerFrame::tagSelected() {
    // first check if container is valid
    if (myContainerTagSelector->getCurrentTemplateAC()) {
        // show PType selector and container plan selector
        myTypeSelector->showDemandElementSelector();
        // check if current container type selected is valid
        if (myTypeSelector->getCurrentDemandElement()) {
            // show container attributes
            myContainerAttributesEditor->showAttributesEditor(myContainerTagSelector->getCurrentTemplateAC(), true);
            // show container plan tag selector
            myPlanSelector->showPlanSelector();
            // check current plan template
            if (myPlanSelector->getCurrentPlanTemplate()) {
                // show container plan attributes
                myContainerPlanAttributesEditor->showAttributesEditor(myPlanSelector->getCurrentPlanTemplate(), false);
                // show edge path creator module
                myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
                // show path legend
                myPlanCreatorLegend->showPlanCreatorLegend();
            } else {
                // hide modules
                myContainerAttributesEditor->hideAttributesEditor();
                myContainerPlanAttributesEditor->hideAttributesEditor();
                myPlanCreator->hidePathCreatorModule();
                myPlanCreatorLegend->hidePlanCreatorLegend();
            }
        } else {
            // hide modules
            myPlanSelector->hidePlanSelector();
            myContainerAttributesEditor->hideAttributesEditor();
            myContainerPlanAttributesEditor->hideAttributesEditor();
            myPlanCreator->hidePathCreatorModule();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide all modules if container isn't valid
        myTypeSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myContainerAttributesEditor->hideAttributesEditor();
        myContainerPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


void
GNEContainerFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement() && myPlanSelector->getCurrentPlanTemplate()) {
        // show container attributes
        myContainerAttributesEditor->showAttributesEditor(myContainerTagSelector->getCurrentTemplateAC(), true);
        // show container plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if container plan selected is valid
        if (myPlanSelector->getCurrentPlanTagProperties()->getTag() != SUMO_TAG_NOTHING) {
            // show container plan attributes
            myContainerPlanAttributesEditor->showAttributesEditor(myPlanSelector->getCurrentPlanTemplate(), false);
            // show edge path creator module
            myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
            // show legend
            myPlanCreatorLegend->showPlanCreatorLegend();
        } else {
            // hide modules
            myContainerAttributesEditor->hideAttributesEditor();
            myContainerPlanAttributesEditor->hideAttributesEditor();
            myPlanCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myPlanSelector->hidePlanSelector();
        myContainerAttributesEditor->hideAttributesEditor();
        myContainerPlanAttributesEditor->hideAttributesEditor();
        myPlanCreator->hidePathCreatorModule();
    }
}


bool
GNEContainerFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myContainerAttributesEditor->checkAttributes(true) || !myContainerPlanAttributesEditor->checkAttributes(true)) {
        return false;
    } else if (myPlanCreator->planCanBeCreated(myPlanSelector->getCurrentPlanTemplate())) {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myContainerTagSelector->getCurrentTemplateAC(), "create " +
                                        myContainerTagSelector->getCurrentTemplateAC()->getTagProperty()->getTagStr() + " and " +
                                        myPlanSelector->getCurrentPlanTagProperties()->getTagStr());
        // create container
        GNEDemandElement* container = buildContainer();
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), container->getAttribute(GNE_ATTR_DEMAND_FILE),
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
        // check if container and container plan can be created
        if (routeHandler.buildContainerPlan(myPlanSelector->getCurrentPlanTemplate(),
                                            container, myContainerPlanAttributesEditor, myPlanCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh container and containerPlan attributes
            myContainerAttributesEditor->refreshAttributesEditor();
            myContainerPlanAttributesEditor->refreshAttributesEditor();
            // compute container
            container->computePathElement();
            // enable show all container plans
            myViewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->setChecked(TRUE);
            return true;
        } else {
            // abort container creation
            myViewNet->getUndoList()->abortAllChangeGroups();
            return false;
        }
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEContainerFrame - private methods
// ---------------------------------------------------------------------------

GNEDemandElement*
GNEContainerFrame::buildContainer() {
    // first container base object
    myContainerBaseObject->clear();
    // obtain container tag (only for improve code legibility)
    SumoXMLTag containerTag = myContainerTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
    // set tag
    myContainerBaseObject->setTag(containerTag);
    // get attributes
    myContainerAttributesEditor->fillSumoBaseObject(myContainerBaseObject);
    // add pType parameter
    myContainerBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
    // declare route handler
    GNERouteHandler routeHandler(myViewNet->getNet(), myContainerBaseObject->hasStringAttribute(GNE_ATTR_DEMAND_FILE) ? myContainerBaseObject->getStringAttribute(GNE_ATTR_DEMAND_FILE) : "",
                                 myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
    // check if we're creating a container or containerFlow
    if (containerTag == SUMO_TAG_CONTAINER) {
        // Add parameter departure
        if (!myContainerBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myContainerBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myContainerBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(containerTag));
        // obtain container parameters
        SUMOVehicleParameter* containerParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_CONTAINER, SUMOSAXAttrs, false, false, false);
        // check containerParameters
        if (containerParameters) {
            myContainerBaseObject->setVehicleParameter(containerParameters);
            // parse vehicle
            routeHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete containerParameters
            delete containerParameters;
        }
    } else {
        // set begin and end attributes
        if (!myContainerBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myContainerBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
        }
        // adjust poisson value
        if (myContainerBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myContainerBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myContainerBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(containerTag));
        // obtain containerFlow parameters
        SUMOVehicleParameter* containerFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_CONTAINERFLOW, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
        // check containerParameters
        if (containerFlowParameters) {
            myContainerBaseObject->setVehicleParameter(containerFlowParameters);
            // parse vehicle
            routeHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete containerParameters
            delete containerFlowParameters;
        }
    }
    // refresh container and containerPlan attributes
    myContainerAttributesEditor->refreshAttributesEditor();
    myContainerPlanAttributesEditor->refreshAttributesEditor();
    // return created container
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(containerTag, myContainerBaseObject->getStringAttribute(SUMO_ATTR_ID));
}


/****************************************************************************/
