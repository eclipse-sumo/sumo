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
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Person elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonFrame - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::GNEPersonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Persons"),
    myRouteHandler("", viewNet->getNet(), true),
    myPersonBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector modul for persons
    myPersonTagSelector = new GNEFrameModules::TagSelector(this, GNETagProperties::TagType::PERSON, SUMO_TAG_PERSON);

    // create person types selector modul
    myPTypeSelector = new GNEFrameModules::DemandElementSelector(this, SUMO_TAG_PTYPE);

    // create person attributes
    myPersonAttributes = new GNEFrameAttributeModules::AttributesCreator(this);

    // create tag Selector modul for person plans
    myPersonPlanTagSelector = new GNEFrameModules::TagSelector(this, GNETagProperties::TagType::PERSONPLAN, GNE_TAG_PERSONTRIP_EDGE);

    // create person plan attributes
    myPersonPlanAttributes = new GNEFrameAttributeModules::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributeModules::NeteditAttributes(this);

    // create PathCreator Module
    myPathCreator = new GNEFrameModules::PathCreator(this);

    // limit path creator to pedestrians
    myPathCreator->setVClass(SVC_PEDESTRIAN);
}


GNEPersonFrame::~GNEPersonFrame() {
    delete myPersonBaseObject;
}


void
GNEPersonFrame::show() {
    // refresh tag selector
    myPersonTagSelector->refreshTagSelector();
    myPTypeSelector->refreshDemandElementSelector();
    myPersonPlanTagSelector->refreshTagSelector();
    // update VClass of myPathCreator
    if (myPersonPlanTagSelector->getCurrentTemplateAC() && 
        myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
        myPathCreator->setVClass(SVC_PASSENGER);
    } else {
        myPathCreator->setVClass(SVC_PEDESTRIAN);
    }
    // show frame
    GNEFrame::show();
}


void
GNEPersonFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check that we clicked over an AC
    if (objectsUnderCursor.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    SumoXMLTag clickedACTag = objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag();
    // first check that current selected person is valid
    if (personTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person isn't valid.");
        return false;
    }
    // now check that pType is valid
    if (myPTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText("Current selected person type isn't valid.");
        return false;
    }
    // finally check that person plan selected is valid
    if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected person plan isn't valid.");
        return false;
    }
    // add elements to path creator
    if (clickedACTag == SUMO_TAG_LANE) {
        const bool result = myPathCreator->addEdge(objectsUnderCursor.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
        // if we're creating a stop, create it immediately
        if (result && myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson()) {
            createPath();
        }
        return result;
    } else if (clickedACTag == SUMO_TAG_BUS_STOP) {
        const bool result = myPathCreator->addStoppingPlace(objectsUnderCursor.getAdditionalFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
        // if we're creating a stop, create it immediately
        if (result && myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson()) {
            createPath();
        }
        return result;
    } else if (clickedACTag == SUMO_TAG_ROUTE) {
        const bool result = myPathCreator->addRoute(objectsUnderCursor.getDemandElementFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
        // if we're creating a walk route, create it immediately
        if (result && (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE)) {
            createPath();
            myPathCreator->removeRoute();
        }
        return result;
    } else {
        return false;
    }
}


GNEFrameModules::PathCreator*
GNEPersonFrame::getPathCreator() const {
    return myPathCreator;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::tagSelected() {
    // first check if person is valid
    if (myPersonTagSelector->getCurrentTemplateAC()) {
        // show PType selector and person plan selector
        myPTypeSelector->showDemandElementSelector();
        // check if current person type selected is valid
        if (myPTypeSelector->getCurrentDemandElement()) {
            // show person attributes depending of myPersonPlanTagSelector
            if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson()) {
                myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {});
            }
            // show person plan tag selector
            myPersonPlanTagSelector->showTagSelector();
            // now check if person plan selected is valid
            if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
                // show person plan attributes
                myPersonPlanAttributes->showAttributesCreatorModule(myPersonPlanTagSelector->getCurrentTemplateAC(), {});
                // show Netedit attributes modul
                myNeteditAttributes->showNeteditAttributesModule(myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty());
                // show path creator depending of tag
                if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson() || 
                    (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() == GNE_TAG_WALK_ROUTE)) {
                    myPathCreator->hidePathCreatorModule();
                } else {
                    // update VClass of myPathCreator depending if person is a ride
                    if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                        myPathCreator->setVClass(SVC_PASSENGER);
                    } else {
                        myPathCreator->setVClass(SVC_PEDESTRIAN);
                    }
                    // show edge path creator modul
                    myPathCreator->showPathCreatorModule(myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
                }
            } else {
                // hide modules
                myPersonPlanAttributes->hideAttributesCreatorModule();
                myNeteditAttributes->hideNeteditAttributesModule();
                myPathCreator->hidePathCreatorModule();
            }
        } else {
            // hide modules
            myPersonPlanTagSelector->hideTagSelector();
            myPersonAttributes->hideAttributesCreatorModule();
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPathCreator->hidePathCreatorModule();
        }
    } else {
        // hide all moduls if person isn't valid
        myPTypeSelector->hideDemandElementSelector();
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModule();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPathCreator->hidePathCreatorModule();
    }
}


void
GNEPersonFrame::demandElementSelected() {
    if (myPTypeSelector->getCurrentDemandElement()) {
        // show person attributes depending of myPersonPlanTagSelector
        if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isStopPerson()) {
            myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {});
        }
        // show person plan tag selector
        myPersonPlanTagSelector->showTagSelector();
        // now check if person plan selected is valid
        if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_NOTHING) {
            // update VClass of myPathCreator depending if person is a ride
            if (myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().isRide()) {
                myPathCreator->setVClass(SVC_PASSENGER);
            } else {
                myPathCreator->setVClass(SVC_PEDESTRIAN);
            }
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModule(myPersonPlanTagSelector->getCurrentTemplateAC(), {});
            // show Netedit attributes modul
            myNeteditAttributes->showNeteditAttributesModule(myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty());
            // show edge path creator modul
            myPathCreator->showPathCreatorModule(myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(), false, false);
        } else {
            // hide modules
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPathCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myPersonPlanTagSelector->hideTagSelector();
        myPersonAttributes->hideAttributesCreatorModule();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPathCreator->hidePathCreatorModule();
    }
}


void
GNEPersonFrame::createPath() {
    // first check that all attributes are valid
    if (!myPersonAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid person parameters.");
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " parameters.");
    } else {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getGUIIcon(), "create " + 
                                        myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " and " + 
                                        myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr());
        // create person
        GNEDemandElement* person = buildPerson();
        // check if person and person plan can be created
        if (myRouteHandler.buildPersonPlan(
                    myPersonPlanTagSelector->getCurrentTemplateAC()->getTagProperty().getTag(),
                    person, myPersonPlanAttributes, myPathCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPathCreator->abortPathCreation();
            // refresh person and personPlan attributes
            myPersonAttributes->refreshAttributesCreator();
            myPersonPlanAttributes->refreshAttributesCreator();
            // compute person
            person->computePathElement();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
        } else {
            // abort person creation
            myViewNet->getUndoList()->abortAllChangeGroups();
        }
    }
}

// ---------------------------------------------------------------------------
// GNEPersonFrame - private methods
// ---------------------------------------------------------------------------

GNEDemandElement*
GNEPersonFrame::buildPerson() {
    // first person base object
    myPersonBaseObject->clear();
    // obtain person tag (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // set tag
    myPersonBaseObject->setTag(personTag);
    // get attribute ad values
    myPersonAttributes->getAttributesAndValues(myPersonBaseObject, false);
    // Check if ID has to be generated
    if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_ID)) {
        myPersonBaseObject->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateDemandElementID(personTag));
    }
    // add pType parameter
    myPersonBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myPTypeSelector->getCurrentDemandElement()->getID());
    // check if we're creating a person or personFlow
    if (personTag == SUMO_TAG_PERSON) {
        // Add parameter departure
        if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myPersonBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myPersonBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(personTag));
        // obtain person parameters
        SUMOVehicleParameter* personParameters = SUMOVehicleParserHelper::parseVehicleAttributes(SUMO_TAG_PERSON, SUMOSAXAttrs, false, false, false);
        // check personParameters
        if (personParameters) {
            myPersonBaseObject->setVehicleParameter(personParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personParameters;
        }
    } else {
        // set begin and end attributes
        if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_BEGIN) || myPersonBaseObject->getStringAttribute(SUMO_ATTR_BEGIN).empty()) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_BEGIN, "0");
        }
        if (!myPersonBaseObject->hasStringAttribute(SUMO_ATTR_END) || myPersonBaseObject->getStringAttribute(SUMO_ATTR_END).empty()) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_END, "3600");
        }
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myPersonBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(personTag));
        // obtain personFlow parameters
        SUMOVehicleParameter* personFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(SUMO_TAG_PERSONFLOW, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
        // check personParameters
        if (personFlowParameters) {
            myPersonBaseObject->setVehicleParameter(personFlowParameters);
            // parse vehicle
            myRouteHandler.parseSumoBaseObject(myPersonBaseObject);
            // delete personParameters
            delete personFlowParameters;
        }
    }
    // refresh person and personPlan attributes
    myPersonAttributes->refreshAttributesCreator();
    myPersonPlanAttributes->refreshAttributesCreator();
    // return created person
    return myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(personTag, myPersonBaseObject->getStringAttribute(SUMO_ATTR_ID));
}


/****************************************************************************/
