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
/// @file    GNEContainerFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Container elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEContainerFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEContainerFrame - methods
// ---------------------------------------------------------------------------

GNEContainerFrame::GNEContainerFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Containers")),
    myRouteHandler("", viewNet->getNet(), true, false),
    myContainerBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector module for containers
    myContainerTagSelector = new GNETagSelector(this, GNETagProperties::TagType::CONTAINER, SUMO_TAG_CONTAINER);

    // create container types selector module and set DEFAULT_PEDTYPE_ID as default element
    myTypeSelector = new GNEDemandElementSelector(this, SUMO_TAG_VTYPE, GNETagProperties::TagType::CONTAINER);

    // create container attributes
    myContainerAttributes = new GNEAttributesCreator(this);

    // create plan selector module for container plans
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_CONTAINER);

    // create container plan attributes
    myContainerPlanAttributes = new GNEAttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNENeteditAttributes(this);

    // create GNEPlanCreator Module
    myPlanCreator = new GNEPlanCreator(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
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
GNEContainerFrame::addContainer(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that we clicked over an AC
    if (objectsUnderCursor.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag containerTag = myContainerTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    SumoXMLTag clickedACTag = objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag();
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
    // add elements to path creator
    if (clickedACTag == SUMO_TAG_LANE) {
        return myPlanCreator->addEdge(objectsUnderCursor.getLaneFront());
    } else if (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isStoppingPlace()) {
        return myPlanCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront());
    } else if (clickedACTag == SUMO_TAG_ROUTE) {
        return myPlanCreator->addRoute(objectsUnderCursor.getDemandElementFront());
    } else if (clickedACTag == SUMO_TAG_JUNCTION) {
        return myPlanCreator->addJunction(objectsUnderCursor.getJunctionFront());
    } else if (clickedACTag == SUMO_TAG_TAZ) {
        return myPlanCreator->addTAZ(objectsUnderCursor.getTAZFront());
    } else {
        return false;
    }
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


GNEAttributesCreator*
GNEContainerFrame::getContainerAttributes() const {
    return myContainerAttributes;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainerFrame::tagSelected() {
    // first check if container is valid
    if (myContainerTagSelector->getCurrentTemplateAC() && myPlanSelector->getCurrentPlanTemplate()) {
        // show PType selector and container plan selector
        myTypeSelector->showDemandElementSelector();
        // check if current container type selected is valid
        if (myTypeSelector->getCurrentDemandElement()) {
            // show container attributes depending of myPlanSelector
            if (myPlanSelector->getCurrentPlanTagProperties().isPlanStopContainer()) {
                myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {});
            }
            // show container plan tag selector
            myPlanSelector->showPlanSelector();
            // now check if container plan selected is valid
            if (myPlanSelector->getCurrentPlanTagProperties().getTag() != SUMO_TAG_NOTHING) {
                // show container plan attributes
                myContainerPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
                // show Netedit attributes module
                myNeteditAttributes->showNeteditAttributesModule(myPlanSelector->getCurrentPlanTemplate());
                // show edge path creator module
                myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
                // show path legend
                myPathLegend->showPathLegendModule();
            } else {
                // hide modules
                myContainerPlanAttributes->hideAttributesCreatorModule();
                myNeteditAttributes->hideNeteditAttributesModule();
                myPlanCreator->hidePathCreatorModule();
                myPathLegend->hidePathLegendModule();
            }
        } else {
            // hide modules
            myPlanSelector->hidePlanSelector();
            myContainerAttributes->hideAttributesCreatorModule();
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide all modules if container isn't valid
        myTypeSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myContainerAttributes->hideAttributesCreatorModule();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEContainerFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement() && myPlanSelector->getCurrentPlanTemplate()) {
        // show container attributes depending of myPlanSelector
        if (myPlanSelector->getCurrentPlanTagProperties().isPlanStopContainer()) {
            myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {});
        }
        // show container plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if container plan selected is valid
        if (myPlanSelector->getCurrentPlanTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // show container plan attributes
            myContainerPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
            // show Netedit attributes module
            myNeteditAttributes->showNeteditAttributesModule(myPlanSelector->getCurrentPlanTemplate());
            // show edge path creator module
            myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
            // show legend
            myPathLegend->showPathLegendModule();
            // show warning if we have selected a vType oriented to containers or vehicles
            if (myTypeSelector->getCurrentDemandElement()->getVClass() == SVC_IGNORING) {
                WRITE_WARNING(TL("VType with vClass == 'ignoring' is oriented to containers"));
            } else if (myTypeSelector->getCurrentDemandElement()->getVClass() != SVC_PEDESTRIAN) {
                WRITE_WARNING(TL("VType with vClass != 'pedestrian' is not oriented to containers"));
            }
        } else {
            // hide modules
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myPlanSelector->hidePlanSelector();
        myContainerAttributes->hideAttributesCreatorModule();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
    }
}


bool
GNEContainerFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myContainerAttributes->areValuesValid()) {
        myViewNet->setStatusBarText(TL("Invalid container parameters."));
    } else if (!myContainerPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPlanSelector->getCurrentPlanTagProperties().getTagStr() + " parameters.");
    } else if (myPlanCreator->planCanBeCreated(myPlanSelector->getCurrentPlanTemplate())) {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myContainerTagSelector->getCurrentTemplateAC(), "create " +
                                        myContainerTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " and " +
                                        myPlanSelector->getCurrentPlanTagProperties().getTagStr());
        // create container
        GNEDemandElement* container = buildContainer();
        // check if container and container plan can be created
        if (myRouteHandler.buildContainerPlan(myPlanSelector->getCurrentPlanTemplate(),
                                              container, myContainerPlanAttributes, myPlanCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh container and containerPlan attributes
            myContainerAttributes->refreshAttributesCreator();
            myContainerPlanAttributes->refreshAttributesCreator();
            // compute container
            container->computePathElement();
            // enable show all container plans
            myViewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->setChecked(TRUE);
            return true;
        } else {
            // abort container creation
            myViewNet->getUndoList()->abortAllChangeGroups();
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNEContainerFrame - private methods
// ---------------------------------------------------------------------------

GNEDemandElement*
GNEContainerFrame::buildContainer() {
    // first container base object
    myContainerBaseObject->clear();
    // obtain container tag (only for improve code legibility)
    SumoXMLTag containerTag = myContainerTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // set tag
    myContainerBaseObject->setTag(containerTag);
    // get attribute ad values
    myContainerAttributes->getAttributesAndValues(myContainerBaseObject, false);
    // Check if ID has to be generated
    if (!myContainerBaseObject->hasStringAttribute(SUMO_ATTR_ID)) {
        myContainerBaseObject->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(containerTag));
    }
    // add pType parameter
    myContainerBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
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
            myRouteHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete containerParameters
            delete containerParameters;
        }
    } else {
        // set begin and end attributes
        if (!myContainerBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myContainerBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
        }
        // adjust poisson value
        if (myContainerBaseObject->hasTimeAttribute(GNE_ATTR_POISSON)) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + time2string(myContainerBaseObject->getTimeAttribute(GNE_ATTR_POISSON), false) + ")");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myContainerBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(containerTag));
        // obtain containerFlow parameters
        SUMOVehicleParameter* containerFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_CONTAINERFLOW, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
        // check containerParameters
        if (containerFlowParameters) {
            myContainerBaseObject->setVehicleParameter(containerFlowParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete containerParameters
            delete containerFlowParameters;
        }
    }
    // refresh container and containerPlan attributes
    myContainerAttributes->refreshAttributesCreator();
    myContainerPlanAttributes->refreshAttributesCreator();
    // return created container
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(containerTag, myContainerBaseObject->getStringAttribute(SUMO_ATTR_ID));
}


/****************************************************************************/
