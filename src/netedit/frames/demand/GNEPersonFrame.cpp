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
/// @file    GNEPersonFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    May 2019
///
// The Widget for add Person elements
/****************************************************************************/
#include <config.h>

#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEPersonFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPersonFrame - methods
// ---------------------------------------------------------------------------

GNEPersonFrame::GNEPersonFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Persons")),
    myRouteHandler("", viewNet->getNet(), true, false),
    myPersonBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create tag Selector module for persons
    myPersonTagSelector = new GNETagSelector(this, GNETagProperties::TagType::PERSON, SUMO_TAG_PERSON);

    // create person types selector module and set DEFAULT_PEDTYPE_ID as default element
    myTypeSelector = new GNEDemandElementSelector(this, SUMO_TAG_VTYPE, GNETagProperties::TagType::PERSON);

    // create person attributes
    myPersonAttributes = new GNEAttributesCreator(this);

    // create plan selector module for person plans
    myPlanSelector = new GNEPlanSelector(this, SUMO_TAG_PERSON);

    // create person plan attributes
    myPersonPlanAttributes = new GNEAttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNENeteditAttributes(this);

    // create GNEPlanCreator Module
    myPlanCreator = new GNEPlanCreator(this);

    // create plan creator legend
    myPlanCreatorLegend = new GNEPlanCreatorLegend(this);
}


GNEPersonFrame::~GNEPersonFrame() {
    delete myPersonBaseObject;
}


void
GNEPersonFrame::show() {
    // refresh tag selector
    myPersonTagSelector->refreshTagSelector();
    myTypeSelector->refreshDemandElementSelector();
    myPlanSelector->refreshPlanSelector();
    // show frame
    GNEFrame::show();
}


void
GNEPersonFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEPersonFrame::addPerson(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that we clicked over an AC
    if (viewObjects.getAttributeCarrierFront() == nullptr) {
        return false;
    }
    // obtain tags (only for improve code legibility)
    SumoXMLTag personTag = myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getTag();
    // first check that current selected person is valid
    if (personTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText(TL("Current selected person isn't valid."));
        return false;
    }
    // now check that pType is valid
    if (myTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected person type isn't valid."));
        return false;
    }
    // finally check that person plan selected is valid
    if (myPlanSelector->getCurrentPlanTemplate() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected person plan isn't valid."));
        return false;
    }
    for (GNEAdditional* o : viewObjects.getAdditionals()) {
        if (o->getTagProperty().isStoppingPlace()) {
            return myPlanCreator->addStoppingPlace(o);
        }
    }
    for (GNEDemandElement* o : viewObjects.getDemandElements()) {
        if (o->getTagProperty().getTag() == SUMO_TAG_ROUTE) {
            return myPlanCreator->addRoute(o);
        }
    }
    if (!viewObjects.getLanes().empty()) {
        return myPlanCreator->addEdge(viewObjects.getLanes().front());
    }
    if (!viewObjects.getJunctions().empty()) {
        return myPlanCreator->addJunction(viewObjects.getJunctions().front());
    }
    if (!viewObjects.getTAZs().empty()) {
        return myPlanCreator->addTAZ(viewObjects.getTAZs().front());
    }
    return false;
}


GNEPlanCreator*
GNEPersonFrame::getPlanCreator() const {
    return myPlanCreator;
}


GNEDemandElementSelector*
GNEPersonFrame::getTypeSelector() const {
    return myTypeSelector;
}


GNEPlanSelector*
GNEPersonFrame::getPlanSelector() const {
    return myPlanSelector;
}


GNEAttributesCreator*
GNEPersonFrame::getPersonAttributes() const {
    return myPersonAttributes;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEPersonFrame::tagSelected() {
    // first check if person is valid
    if (myPersonTagSelector->getCurrentTemplateAC()) {
        // show PType selector and person plan selector
        myTypeSelector->showDemandElementSelector();
        // check if current person type selected is valid
        if (myTypeSelector->getCurrentDemandElement()) {
            // show person attributes depending of myPlanSelector
            if (myPlanSelector->getCurrentPlanTagProperties().isPlanStopPerson()) {
                myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
            } else {
                myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {});
            }
            // show person plan tag selector
            myPlanSelector->showPlanSelector();
            // check current plan template
            if (myPlanSelector->getCurrentPlanTemplate()) {
                // show person plan attributes
                myPersonPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
                // show Netedit attributes module
                myNeteditAttributes->showNeteditAttributesModule(myPlanSelector->getCurrentPlanTemplate());
                // show edge path creator module
                myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
                // show path legend
                myPlanCreatorLegend->showPlanCreatorLegend();
            } else {
                // hide modules
                myPersonPlanAttributes->hideAttributesCreatorModule();
                myNeteditAttributes->hideNeteditAttributesModule();
                myPlanCreator->hidePathCreatorModule();
                myPlanCreatorLegend->hidePlanCreatorLegend();
            }
        } else {
            // hide modules
            myPlanSelector->hidePlanSelector();
            myPersonAttributes->hideAttributesCreatorModule();
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
            myPlanCreatorLegend->hidePlanCreatorLegend();
        }
    } else {
        // hide all modules if person isn't valid
        myTypeSelector->hideDemandElementSelector();
        myPlanSelector->hidePlanSelector();
        myPersonAttributes->hideAttributesCreatorModule();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
        myPlanCreatorLegend->hidePlanCreatorLegend();
    }
}


void
GNEPersonFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement() && myPlanSelector->getCurrentPlanTemplate()) {
        // show person attributes depending of myPlanSelector
        if (myPlanSelector->getCurrentPlanTagProperties().isPlanStopPerson()) {
            myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {SUMO_ATTR_DEPARTPOS});
        } else {
            myPersonAttributes->showAttributesCreatorModule(myPersonTagSelector->getCurrentTemplateAC(), {});
        }
        // show person plan tag selector
        myPlanSelector->showPlanSelector();
        // now check if person plan selected is valid
        if (myPlanSelector->getCurrentPlanTagProperties().getTag() != SUMO_TAG_NOTHING) {
            // show person plan attributes
            myPersonPlanAttributes->showAttributesCreatorModule(myPlanSelector->getCurrentPlanTemplate(), {});
            // show Netedit attributes module
            myNeteditAttributes->showNeteditAttributesModule(myPlanSelector->getCurrentPlanTemplate());
            // show edge path creator module
            myPlanCreator->showPlanCreatorModule(myPlanSelector, nullptr);
            // show legend
            myPlanCreatorLegend->showPlanCreatorLegend();
        } else {
            // hide modules
            myPersonPlanAttributes->hideAttributesCreatorModule();
            myNeteditAttributes->hideNeteditAttributesModule();
            myPlanCreator->hidePathCreatorModule();
        }
    } else {
        // hide modules
        myPlanSelector->hidePlanSelector();
        myPersonAttributes->hideAttributesCreatorModule();
        myPersonPlanAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        myPlanCreator->hidePathCreatorModule();
    }
}


bool
GNEPersonFrame::createPath(const bool /*useLastRoute*/) {
    // first check that all attributes are valid
    if (!myPersonAttributes->areValuesValid()) {
        myViewNet->setStatusBarText(TL("Invalid person parameters."));
    } else if (!myPersonPlanAttributes->areValuesValid()) {
        myViewNet->setStatusBarText("Invalid " + myPlanSelector->getCurrentPlanTagProperties().getTagStr() + " parameters.");
    } else if (myPlanCreator->planCanBeCreated(myPlanSelector->getCurrentPlanTemplate())) {
        // begin undo-redo operation
        myViewNet->getUndoList()->begin(myPersonTagSelector->getCurrentTemplateAC(), "create " +
                                        myPersonTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " and " +
                                        myPlanSelector->getCurrentPlanTagProperties().getTagStr());
        // create person
        GNEDemandElement* person = buildPerson();
        // check if person and person plan can be created
        if (myRouteHandler.buildPersonPlan(myPlanSelector->getCurrentPlanTemplate(),
                                           person, myPersonPlanAttributes, myPlanCreator, true)) {
            // end undo-redo operation
            myViewNet->getUndoList()->end();
            // abort path creation
            myPlanCreator->abortPathCreation();
            // refresh person and personPlan attributes
            myPersonAttributes->refreshAttributesCreator();
            myPersonPlanAttributes->refreshAttributesCreator();
            // compute person
            person->computePathElement();
            // enable show all person plans
            myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->setChecked(TRUE);
            return true;
        } else {
            // abort person creation
            myViewNet->getUndoList()->abortAllChangeGroups();
        }
    }
    return false;
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
    myPersonBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
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
        // adjust poisson value
        if (myPersonBaseObject->hasTimeAttribute(GNE_ATTR_POISSON)) {
            myPersonBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + time2string(myPersonBaseObject->getTimeAttribute(GNE_ATTR_POISSON), false) + ")");
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
