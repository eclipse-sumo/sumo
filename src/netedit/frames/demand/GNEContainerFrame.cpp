/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @date    Jun 2021
///
// The Widget for add Container elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEContainerFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEContainerFrame - methods
// ---------------------------------------------------------------------------

GNEContainerFrame::GNEContainerFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Containers"),
    myRouteHandler("", viewNet->getNet(), true),
    myContainerBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector modul for containers
    myContainerTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::CONTAINER, SUMO_TAG_CONTAINER);

    // create container types selector modul
    myPTypeSelector = new GNEFrameModuls::DemandElementSelector(this, SUMO_TAG_PTYPE);

    // create container attributes
    myContainerAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create tag Selector modul for container plans
    myContainerPlanTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::CONTAINERPLAN, GNE_TAG_TRANSPORT_EDGE);

    // create container plan attributes
    myContainerPlanAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributesModuls::NeteditAttributes(this);

    // create PathCreator Modul
    myPathCreator = new GNEFrameModuls::PathCreator(this);

    // limit path creator to pedestrians
    myPathCreator->setVClass(SVC_PEDESTRIAN);
}


GNEContainerFrame::~GNEContainerFrame() {
    delete myContainerBaseObject;
}


void
GNEContainerFrame::show() {
    // refresh tag selector
    myContainerTagSelector->refreshTagSelector();
    myPTypeSelector->refreshDemandElementSelector();
    myContainerPlanTagSelector->refreshTagSelector();
    // update VClass of myPathCreator
    if (myContainerPlanTagSelector->getCurrentTemplateAC() && 
        myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isTransportPlan()) {
        myPathCreator->setVClass(SVC_PASSENGER);
    } else {
        myPathCreator->setVClass(SVC_PEDESTRIAN);
    }
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
GNEContainerFrame::addContainer(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check that we clicked over an AC
    if (objectsUnderCursor.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag clickedACTag = objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag();
    // first check that current selected container is valid
    if (myContainerTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText("Current selected container isn't valid.");
        return false;
    }
    // now check that pType is valid
    if (myPTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected container type isn't valid.");
        return false;
    }
    // finally check that container plan selected is valid
    if (myContainerPlanTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText("Current selected container plan isn't valid.");
        return false;
    }
    // add elements to path creator
    if (clickedACTag == SUMO_TAG_LANE) {
        return myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (clickedACTag == SUMO_TAG_CONTAINER_STOP) {
        return myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (clickedACTag == SUMO_TAG_ROUTE) {
        return myPathCreator->addRoute(objectsUnderCursor.getDemandElementFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else {
        return false;
    }
}


GNEFrameModuls::PathCreator*
GNEContainerFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEContainerFrame::tagSelected() {
    // first check if container is valid
    if (myContainerTagSelector->getCurrentTemplateAC()) {
        // show PType selector and container plan selector
        myPTypeSelector->showDemandElementSelector();
        // check if current container type selected is valid
        if (myPTypeSelector->getCurrentDemandElement()) {
            // show container attributes depending of myContainerPlanTagSelector
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopContainer()) {
                myContainerAttributes->showAttributesCreatorModul(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myContainerAttributes->showAttributesCreatorModul(myContainerTagSelector->getCurrentTemplateAC(), {});
            }
            // show container plan tag selector
            myContainerPlanTagSelector->showTagSelector();
            // now check if container plan selected is valid
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
                // update VClass of myPathCreator depending if container is a ride
                if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                    myPathCreator->setVClass(SVC_PASSENGER);
                } else {
                    myPathCreator->setVClass(SVC_PEDESTRIAN);
                }
                // show container plan attributes
                myContainerPlanAttributes->showAttributesCreatorModul(myContainerPlanTagSelector->getCurrentTemplateAC(), {});
                // show Netedit attributes modul
                myNeteditAttributes->showNeteditAttributesModul(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty());
                // show edge path creator modul
                myPathCreator->showPathCreatorModul(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
            } else {
                // hide modules
                myContainerPlanAttributes->hideAttributesCreatorModul();
                myNeteditAttributes->hideNeteditAttributesModul();
                myPathCreator->hidePathCreatorModul();
            }
        } else {
            // hide modules
            myContainerPlanTagSelector->hideTagSelector();
            myContainerAttributes->hideAttributesCreatorModul();
            myContainerPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide all moduls if container isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myContainerPlanTagSelector->hideTagSelector();
        myContainerAttributes->hideAttributesCreatorModul();
        myContainerPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
    }
}


void
GNEContainerFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        // show container attributes depending of myContainerPlanTagSelector
        if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopContainer()) {
            myContainerAttributes->showAttributesCreatorModul(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myContainerAttributes->showAttributesCreatorModul(myContainerTagSelector->getCurrentTemplateAC(), {});
        }
        // show container plan tag selector
        myContainerPlanTagSelector->showTagSelector();
        // now check if container plan selected is valid
        if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
            // update VClass of myPathCreator depending if container is a ride
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                myPathCreator->setVClass(SVC_PASSENGER);
            } else {
                myPathCreator->setVClass(SVC_PEDESTRIAN);
            }
            // show container plan attributes
            myContainerPlanAttributes->showAttributesCreatorModul(myContainerPlanTagSelector->getCurrentTemplateAC(), {});
            // show Netedit attributes modul
            myNeteditAttributes->showNeteditAttributesModul(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty());
            // show edge path creator modul
            myPathCreator->showPathCreatorModul(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
        } else {
            // hide modules
            myContainerPlanAttributes->hideAttributesCreatorModul();
            myNeteditAttributes->hideNeteditAttributesModul();
            myPathCreator->hidePathCreatorModul();
        }
    } else {
        // hide modules
        myContainerPlanTagSelector->hideTagSelector();
        myContainerAttributes->hideAttributesCreatorModul();
        myContainerPlanAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myPathCreator->hidePathCreatorModul();
    }
}


void
GNEContainerFrame::createPath() {
    // first check that all attributes are valid
    if (!myContainerAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid container parameters.");
    } else if (!myContainerPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " parameters.");
    } else {
        /*
                // begin undo-redo operation
                myViewNet->getUndoList()->begin("create " + myContainerTagSelector->getCurrentTagProperties().getTagStr() + " and " + myContainerPlanTagSelector->getCurrentTagProperties().getTagStr());
                // create container
                GNEDemandElement* container = buildContainer();
                // check if container and container plan can be created
                if (GNERouteHandler::buildContainerPlan(
                            myContainerPlanTagSelector->getCurrentTagProperties().getTag(),
                            container, myContainerPlanAttributes, myPathCreator)) {
                    // end undo-redo operation
                    myViewNet->getUndoList()->end();
                    // abort path creation
                    myPathCreator->abortPathCreation();
                    // refresh container and containerPlan attributes
                    myContainerAttributes->refreshRows();
                    myContainerPlanAttributes->refreshRows();
                    // compute container
                    container->computePathElement();
                } else {
                    // abort container creation
                    myViewNet->getUndoList()->p_abort();
                }
        */
    }
}

// ---------------------------------------------------------------------------
// GNEContainerFrame - private methods
// ---------------------------------------------------------------------------

GNEDemandElement*
GNEContainerFrame::buildContainer() {
    // obtain container tag (only for improve code legibility)
    SumoXMLTag containerTag = myContainerTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // Declare map to keep attributes from myContainerAttributes
    myContainerAttributes->getAttributesAndValues(myContainerBaseObject, false);
    // Check if ID has to be generated
    if (!myContainerBaseObject->hasStringAttribute(SUMO_ATTR_ID)) {
        myContainerBaseObject->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(containerTag));
    }
    // add pType parameter
    myContainerBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myPTypeSelector->getCurrentDemandElement()->getID());
    // check if we're creating a container or containerFlow
    if (containerTag == SUMO_TAG_CONTAINER) {
        // Add parameter departure
        if (myContainerBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) && myContainerBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myContainerBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(containerTag));
        // obtain container parameters
        SUMOVehicleParameter* containerParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_CONTAINER, SUMOSAXAttrs, false, false, false);
        // check personParameters
        if (containerParameters) {
            myContainerBaseObject->setVehicleParameter(containerParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete personParameters
            delete containerParameters;
        }
    } else {
        // set begin and end attributes
        if (myContainerBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) && myContainerBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
        }
        if (myContainerBaseObject->hasStringAttribute(SUMO_ATTR_END) && myContainerBaseObject->getStringAttribute(SUMO_ATTR_END).empty()) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_END, "3600");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myContainerBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(containerTag));
        // obtain containerFlow parameters
        SUMOVehicleParameter* containerFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_CONTAINERFLOW, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
        // check personParameters
        if (containerFlowParameters) {
            myContainerBaseObject->setVehicleParameter(containerFlowParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myContainerBaseObject);
            // delete personParameters
            delete containerFlowParameters;
        }
    }
    // refresh container and containerPlan attributes
    myContainerAttributes->refreshRows();
    myContainerPlanAttributes->refreshRows();
    // return created container
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(containerTag, myContainerBaseObject->getStringAttribute(SUMO_ATTR_ID));
}


/****************************************************************************/
