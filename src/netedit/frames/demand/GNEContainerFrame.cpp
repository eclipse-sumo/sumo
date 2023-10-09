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
/// @date    Jun 2021
///
// The Widget for add Container elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
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

    // create container types selector module and set DEFAULT_CONTAINERTYPE_ID as default element
    myTypeSelector = new DemandElementSelector(this, SUMO_TAG_VTYPE, viewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, DEFAULT_CONTAINERTYPE_ID));

    // create container attributes
    myContainerAttributes = new GNEAttributesCreator(this);

    // create tag Selector module for container plans
    myContainerPlanTagSelector = new GNETagSelector(this, GNETagProperties::TagType::CONTAINERPLAN, GNE_TAG_TRANSPORT_EDGE_EDGE);

    // create container plan attributes
    myContainerPlanAttributes = new GNEAttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNENeteditAttributes(this);

    // create GNEPlanCreator Module
    myPlanCreator = new GNEPlanCreator(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);

    // limit path creator to pedestrians
    myPlanCreator->setVClass(SVC_PEDESTRIAN);
}


GNEContainerFrame::~GNEContainerFrame() {
    delete myContainerBaseObject;
}


void
GNEContainerFrame::show() {
    // refresh tag selector
    myContainerTagSelector->refreshTagSelector();
    myTypeSelector->refreshDemandElementSelector();
    myContainerPlanTagSelector->refreshTagSelector();
    // update VClass of myPlanCreator
    if (myContainerPlanTagSelector->getCurrentTemplateAC() &&
            myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isTransportPlan()) {
        myPlanCreator->setVClass(SVC_PASSENGER);
    } else {
        myPlanCreator->setVClass(SVC_PEDESTRIAN);
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
    if (myContainerPlanTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected container plan isn't valid."));
        return false;
    }
    // add elements to path creator
    if (clickedACTag == SUMO_TAG_LANE) {
        return myPlanCreator->addEdge(objectsUnderCursor.getEdgeFront());
    } else if (clickedACTag == SUMO_TAG_CONTAINER_STOP) {
        return myPlanCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront());
    } else if (clickedACTag == SUMO_TAG_JUNCTION) {
        return myPlanCreator->addJunction(objectsUnderCursor.getJunctionFront());
    } else {
        return false;
    }
}


GNEPlanCreator*
GNEContainerFrame::getPlanCreator() const {
    return myPlanCreator;
}


DemandElementSelector*
GNEContainerFrame::getTypeSelector() const {
    return myTypeSelector;
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
    if (myContainerTagSelector->getCurrentTemplateAC()) {
        // show PType selector and container plan selector
        myTypeSelector->showDemandElementSelector();
        // check if current container type selected is valid
        if (myTypeSelector->getCurrentDemandElement()) {
            // show container attributes depending of myContainerPlanTagSelector
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopContainer()) {
                myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {});
            }
            // show container plan tag selector
            myContainerPlanTagSelector->showTagSelector();
            // now check if container plan selected is valid
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
                // update VClass of myPlanCreator depending if container is a ride
                if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                    myPlanCreator->setVClass(SVC_PASSENGER);
                } else {
                    myPlanCreator->setVClass(SVC_PEDESTRIAN);
                }
                // show container plan attributes
                myContainerPlanAttributes->showAttributesCreatorModule(myContainerPlanTagSelector->getCurrentTemplateAC(), {});
                // show Netedit attributes module
                myNeteditAttributes->showNeteditAttributesModule(myContainerPlanTagSelector->getCurrentTemplateAC());
                // show edge path creator module
//              myPlanCreator->showPlanCreatorModule(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty(), false);
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
            myContainerPlanTagSelector->hideTagSelector();
            myContainerAttributes->hideAttributesCreatorModule();
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
            myPathLegend->hidePathLegendModule();
        }
    } else {
        // hide all modules if container isn't valid
        myTypeSelector->hideDemandElementSelector();
        myContainerPlanTagSelector->hideTagSelector();
        myContainerAttributes->hideAttributesCreatorModule();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEContainerFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement()) {
        // show container attributes depending of myContainerPlanTagSelector
        if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopContainer()) {
            myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myContainerAttributes->showAttributesCreatorModule(myContainerTagSelector->getCurrentTemplateAC(), {});
        }
        // show container plan tag selector
        myContainerPlanTagSelector->showTagSelector();
        // now check if container plan selected is valid
        if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
            // update VClass of myPlanCreator depending if container is a ride
            if (myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                myPlanCreator->setVClass(SVC_PASSENGER);
            } else {
                myPlanCreator->setVClass(SVC_PEDESTRIAN);
            }
            // show container plan attributes
            myContainerPlanAttributes->showAttributesCreatorModule(myContainerPlanTagSelector->getCurrentTemplateAC(), {});
            // show Netedit attributes module
            myNeteditAttributes->showNeteditAttributesModule(myContainerPlanTagSelector->getCurrentTemplateAC());
            // show edge path creator module
//          myPlanCreator->showPlanCreatorModule(myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty(), false);
            // show path legend
            myPathLegend->showPathLegendModule();
            // show warning if we have selected a vType oriented to persons or vehicles
            if (myTypeSelector->getCurrentDemandElement()->getVClass() == SVC_PEDESTRIAN) {
                WRITE_WARNING(TL("VType with vClass == 'pedestrian' is oriented to pedestrians"));
            } else if (myTypeSelector->getCurrentDemandElement()->getVClass() != SVC_IGNORING) {
                WRITE_WARNING(TL("VType with vClass != 'ignoring' is not oriented to containers"));
            }
        } else {
            // hide modules
            myContainerPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myContainerPlanTagSelector->hideTagSelector();
        myContainerAttributes->hideAttributesCreatorModule();
        myContainerPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
    }
}


bool
GNEContainerFrame::createPath(const bool /* useLastRoute */) {
    // first check that all attributes are valid
    if (!myContainerAttributes->areValuesValid()) {
        myViewNet->setStatusBarText(TL("Invalid container parameters."));
    } else if (!myContainerPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " parameters.");
    } else {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myContainerTagSelector->getCurrentTemplateAC(), "create " +
                                        myContainerTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " and " +
                                        myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr());
        // create person
        GNEDemandElement* person = buildContainer();
        // check if person and person plan can be created
        if (myRouteHandler.buildContainerPlan(
                    myContainerPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(),
                    person, myContainerPlanAttributes, myPlanCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh person and personPlan attributes
            myContainerAttributes->refreshAttributesCreator();
            myContainerPlanAttributes->refreshAttributesCreator();
            // compute person
            person->computePathElement();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->setChecked(TRUE);
            return true;
        } else {
            // abort person creation
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
    // Declare map to keep attributes from myContainerAttributes
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
        // adjust poisson value
        if (myContainerBaseObject->hasTimeAttribute(GNE_ATTR_POISSON)) {
            myContainerBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + time2string(myContainerBaseObject->getTimeAttribute(GNE_ATTR_POISSON), false) + ")");
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
    myContainerAttributes->refreshAttributesCreator();
    myContainerPlanAttributes->refreshAttributesCreator();
    // return created container
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(containerTag, myContainerBaseObject->getStringAttribute(SUMO_ATTR_ID));
}


/****************************************************************************/
