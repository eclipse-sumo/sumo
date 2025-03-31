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
/// @file    GNEVehicleFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2018
///
// The Widget for add Vehicles/Flows/Trips/etc. elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEDemandSelector.h>
#include <netedit/frames/GNEPathCreator.h>
#include <netedit/frames/GNEPathLegendModule.h>
#include <netedit/frames/GNETagSelector.h>
#include <utils/foxtools/MFXDynamicLabel.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNEVehicleFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEVehicleFrame::HelpCreation - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::HelpCreation::HelpCreation(GNEVehicleFrame* vehicleFrameParent) :
    MFXGroupBoxModule(vehicleFrameParent, TL("Help")),
    myVehicleFrameParent(vehicleFrameParent) {
    myInformationLabel = new MFXDynamicLabel(getCollapsableFrame(), "", 0, GUIDesignLabelFrameInformation);
}


GNEVehicleFrame::HelpCreation::~HelpCreation() {}


void
GNEVehicleFrame::HelpCreation::showHelpCreation() {
    // first update help cration
    updateHelpCreation();
    // show modul
    show();
}


void
GNEVehicleFrame::HelpCreation::hideHelpCreation() {
    hide();
}

void
GNEVehicleFrame::HelpCreation::updateHelpCreation() {
    // create information label
    std::ostringstream information;
    // set text depending of selected vehicle type
    switch (myVehicleFrameParent->myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag()) {
        // vehicles
        case SUMO_TAG_VEHICLE:
            information
                    << "- " << TL("Click over a route to create a vehicle.");
            break;
        case SUMO_TAG_TRIP:
            information
                    << "- " << TL("Select two edges to create a trip.");
            break;
        case GNE_TAG_VEHICLE_WITHROUTE:
            information
                    << "- " << TL("Select two edges to create a vehicle with embedded route.");
            break;
        case GNE_TAG_TRIP_JUNCTIONS:
            information
                    << "- " << TL("Select two junctions to create a trip.");
            break;
        case GNE_TAG_TRIP_TAZS:
            information
                    << "- " << TL("Select two TAZS to create a trip.");
            break;
        // flows
        case GNE_TAG_FLOW_ROUTE:
            information
                    << "- " << TL("Click over a route to create a routeFlow.");
            break;
        case SUMO_TAG_FLOW:
            information
                    << "- " << TL("Select two edges to create a flow.");
            break;
        case GNE_TAG_FLOW_WITHROUTE:
            information
                    << "- " << TL("Select two edges to create a flow with embedded route.");
            break;
        case GNE_TAG_FLOW_JUNCTIONS:
            information
                    << "- " << TL("Select two junctions to create a flow.");
            break;
        case GNE_TAG_FLOW_TAZS:
            information
                    << "- " << TL("Select two TAZs to create a flow.");
            break;
        default:
            break;
    }
    // set information label
    myInformationLabel->setText(information.str().c_str());
}

// ---------------------------------------------------------------------------
// GNEVehicleFrame - methods
// ---------------------------------------------------------------------------

GNEVehicleFrame::GNEVehicleFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Vehicles")),
    myVehicleBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // Create item Selector module for vehicles
    myVehicleTagSelector = new GNETagSelector(this, GNETagProperties::TagType::VEHICLE, SUMO_TAG_TRIP);

    // Create vehicle type selector and set DEFAULT_VTYPE_ID as default element
    myTypeSelector = new GNEDemandElementSelector(this, SUMO_TAG_VTYPE, GNETagProperties::TagType::VEHICLE);

    // Create attributes editor
    myVehicleAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create GNEPathCreator Module
    myPathCreator = new GNEPathCreator(this, viewNet->getNet()->getDemandPathManager());

    // Create Help Creation Module
    myHelpCreation = new HelpCreation(this);

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
}


GNEVehicleFrame::~GNEVehicleFrame() {
    delete myVehicleBaseObject;
}


void
GNEVehicleFrame::show() {
    // refresh tag selector
    myVehicleTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


void
GNEVehicleFrame::hide() {
    // reset edge candidates
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // reset junctioncandidates
    for (const auto& junction : myViewNet->getNet()->getAttributeCarriers()->getJunctions()) {
        junction.second->resetCandidateFlags();
    }
    // hide frame
    GNEFrame::hide();
}


bool
GNEVehicleFrame::addVehicle(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // check template AC
    if (myVehicleTagSelector->getCurrentTemplateAC() == nullptr) {
        return false;
    }
    // begin cleaning vehicle base object
    myVehicleBaseObject->clear();
    // obtain tag (only for improve code legibility)
    SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
    const bool addEdge = ((vehicleTag == SUMO_TAG_TRIP) || (vehicleTag == GNE_TAG_VEHICLE_WITHROUTE) || (vehicleTag == SUMO_TAG_FLOW) || (vehicleTag == GNE_TAG_FLOW_WITHROUTE));
    const bool addJunction = ((vehicleTag == GNE_TAG_TRIP_JUNCTIONS) || (vehicleTag == GNE_TAG_FLOW_JUNCTIONS));
    const bool addTAZ = ((vehicleTag == GNE_TAG_TRIP_TAZS) || (vehicleTag == GNE_TAG_FLOW_TAZS));
    // first check that current selected vehicle is valid
    if (vehicleTag == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText(TL("Current selected vehicle isn't valid."));
        return false;
    }
    // now check if VType is valid
    if (myTypeSelector->getCurrentDemandElement() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected vehicle type isn't valid."));
        return false;
    }
    // add VType
    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
    // set route or edges depending of vehicle type
    if (myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty()->vehicleRoute()) {
        return buildVehicleOverRoute(vehicleTag, viewObjects.getDemandElementFront());
    } else if (addEdge && viewObjects.getEdgeFront()) {
        // add clicked edge in GNEPathCreator
        return myPathCreator->addEdge(viewObjects.getEdgeFront(), mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
    } else if (addJunction && viewObjects.getJunctionFront()) {
        // add clicked junction in GNEPathCreator
        return myPathCreator->addJunction(viewObjects.getJunctionFront());
    } else if (addTAZ && viewObjects.getTAZFront()) {
        // add clicked TAZ in GNEPathCreator
        return myPathCreator->addTAZ(viewObjects.getTAZFront());
    } else {
        return false;
    }
}


GNETagSelector*
GNEVehicleFrame::getVehicleTagSelector() const {
    return myVehicleTagSelector;
}


GNEDemandElementSelector*
GNEVehicleFrame::getTypeSelector() const {
    return myTypeSelector;
}


GNEPathCreator*
GNEVehicleFrame::getPathCreator() const {
    return myPathCreator;
}


GNEAttributesEditor*
GNEVehicleFrame::getVehicleAttributesEditor() const {
    return myVehicleAttributesEditor;
}

// ===========================================================================
// protected
// ===========================================================================

void
GNEVehicleFrame::tagSelected() {
    if (myVehicleTagSelector->getCurrentTemplateAC()) {
        // show vehicle type selector modul
        myTypeSelector->showDemandElementSelector();
        // show path creator modul
        myPathCreator->showPathCreatorModule(myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty(), false);
        // check if show path legend
        if (myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty()->vehicleRouteEmbedded()) {
            myPathLegend->hidePathLegendModule();
        } else {
            myPathLegend->showPathLegendModule();
        }
    } else {
        // hide all moduls if tag isn't valid
        myTypeSelector->hideDemandElementSelector();
        myVehicleAttributesEditor->hideAttributesEditor();
        myPathCreator->hidePathCreatorModule();
        myHelpCreation->hideHelpCreation();
        myPathLegend->hidePathLegendModule();
    }
}


void
GNEVehicleFrame::demandElementSelected() {
    if (myTypeSelector->getCurrentDemandElement()) {
        // show vehicle attributes modul
        myVehicleAttributesEditor->showAttributesEditor(myVehicleTagSelector->getCurrentTemplateAC(), true);
        // clear colors
        myPathCreator->clearJunctionColors();
        myPathCreator->clearEdgeColors();
        // set current VTypeClass in pathCreator
        myPathCreator->setVClass(myTypeSelector->getCurrentDemandElement()->getVClass());
        // show path creator module
        myPathCreator->showPathCreatorModule(myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty(), false);
        // show help creation
        myHelpCreation->showHelpCreation();
    } else {
        // hide all moduls if selected item isn't valid
        myVehicleAttributesEditor->hideAttributesEditor();
        myPathCreator->hidePathCreatorModule();
        myPathLegend->hidePathLegendModule();
        myHelpCreation->hideHelpCreation();
    }
}


bool
GNEVehicleFrame::createPath(const bool useLastRoute) {
    // first check if parameters are valid
    if (myVehicleAttributesEditor->checkAttributes(true) && myTypeSelector->getCurrentDemandElement()) {
        // obtain tag (only for improve code legibility)
        SumoXMLTag vehicleTag = myVehicleTagSelector->getCurrentTemplateAC()->getTagProperty()->getTag();
        // begin cleaning vehicle base object
        myVehicleBaseObject->clear();
        // Updated myVehicleBaseObject
        myVehicleAttributesEditor->fillSumoBaseObject(myVehicleBaseObject);
        // add VType
        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TYPE, myTypeSelector->getCurrentDemandElement()->getID());
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), myVehicleBaseObject->hasStringAttribute(GNE_ATTR_DEMAND_FILE) ?
                                     myVehicleBaseObject->getStringAttribute(GNE_ATTR_DEMAND_FILE) : "",
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed(), false);
        // check if use last route
        if (useLastRoute) {
            // build vehicle using last route
            return buildVehicleOverRoute(vehicleTag, myViewNet->getLastCreatedRoute());
        } else {
            // extract via attribute
            std::vector<std::string> viaEdges;
            for (int i = 1; i < ((int)myPathCreator->getSelectedEdges().size() - 1); i++) {
                viaEdges.push_back(myPathCreator->getSelectedEdges().at(i)->getID());
            }
            // continue depending of tag
            if ((vehicleTag == SUMO_TAG_TRIP) && (myPathCreator->getSelectedEdges().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_TRIP);
                // Add parameter departure
                if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain trip parameters
                SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                tripParameters->setParameters(myVehicleBaseObject->getParameters());
                // check trip parameters
                if (tripParameters) {
                    myVehicleBaseObject->setVehicleParameter(tripParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM, myPathCreator->getSelectedEdges().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO, myPathCreator->getSelectedEdges().back()->getID());
                    myVehicleBaseObject->addStringListAttribute(SUMO_ATTR_VIA, viaEdges);
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete tripParameters and base object
                    delete tripParameters;
                }
            } else if ((vehicleTag == GNE_TAG_VEHICLE_WITHROUTE) && (myPathCreator->getPath().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_VEHICLE);
                // Add parameter departure
                if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                }
                // get route edges
                std::vector<std::string> routeEdges;
                for (const auto& subPath : myPathCreator->getPath()) {
                    for (const auto& edge : subPath.getSubPath()) {
                        routeEdges.push_back(edge->getID());
                    }
                }
                // avoid consecutive duplicated edges
                routeEdges.erase(std::unique(routeEdges.begin(), routeEdges.end()), routeEdges.end());
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain vehicle parameters
                SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                vehicleParameters->setParameters(myVehicleBaseObject->getParameters());
                // continue depending of vehicleParameters
                if (vehicleParameters) {
                    myVehicleBaseObject->setVehicleParameter(vehicleParameters);
                    // create route base object
                    CommonXMLStructure::SumoBaseObject* embeddedRouteObject = new CommonXMLStructure::SumoBaseObject(myVehicleBaseObject);
                    embeddedRouteObject->setTag(SUMO_TAG_ROUTE);
                    embeddedRouteObject->addStringListAttribute(SUMO_ATTR_EDGES, routeEdges);
                    embeddedRouteObject->addColorAttribute(SUMO_ATTR_COLOR, RGBColor::INVISIBLE);
                    embeddedRouteObject->addIntAttribute(SUMO_ATTR_REPEAT, 0);
                    embeddedRouteObject->addTimeAttribute(SUMO_ATTR_CYCLETIME, 0);
                    embeddedRouteObject->addDoubleAttribute(SUMO_ATTR_PROB, 1.0);
                    // parse route
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete vehicleParamters
                    delete vehicleParameters;
                }
            } else if ((vehicleTag == SUMO_TAG_FLOW) && (myPathCreator->getSelectedEdges().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                // set flow attributes
                updateFlowAttributes();
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain flow parameters
                SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                flowParameters->setParameters(myVehicleBaseObject->getParameters());
                // check flowParameters
                if (flowParameters) {
                    myVehicleBaseObject->setVehicleParameter(flowParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM, myPathCreator->getSelectedEdges().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO, myPathCreator->getSelectedEdges().back()->getID());
                    myVehicleBaseObject->addStringListAttribute(SUMO_ATTR_VIA, viaEdges);
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete flowParameters and base object
                    delete flowParameters;
                }
            } else if ((vehicleTag == GNE_TAG_FLOW_WITHROUTE) && (myPathCreator->getPath().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                // set flow attributes
                updateFlowAttributes();
                // get route edges
                std::vector<std::string> routeEdges;
                for (const auto& subPath : myPathCreator->getPath()) {
                    for (const auto& edge : subPath.getSubPath()) {
                        routeEdges.push_back(edge->getID());
                    }
                }
                // avoid consecutive duplicated edges
                routeEdges.erase(std::unique(routeEdges.begin(), routeEdges.end()), routeEdges.end());
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain flow parameters
                SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                flowParameters->setParameters(myVehicleBaseObject->getParameters());
                // continue depending of vehicleParameters
                if (flowParameters) {
                    myVehicleBaseObject->setVehicleParameter(flowParameters);
                    // create under base object
                    CommonXMLStructure::SumoBaseObject* embeddedRouteObject = new CommonXMLStructure::SumoBaseObject(myVehicleBaseObject);
                    embeddedRouteObject->setTag(SUMO_TAG_ROUTE);
                    embeddedRouteObject->addStringListAttribute(SUMO_ATTR_EDGES, routeEdges);
                    embeddedRouteObject->addColorAttribute(SUMO_ATTR_COLOR, RGBColor::INVISIBLE);
                    embeddedRouteObject->addIntAttribute(SUMO_ATTR_REPEAT, 0);
                    embeddedRouteObject->addTimeAttribute(SUMO_ATTR_CYCLETIME, 0);
                    embeddedRouteObject->addDoubleAttribute(SUMO_ATTR_PROB, 1.0);
                    // parse route
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete vehicleParamters
                    delete flowParameters;
                }
            } else if ((vehicleTag == GNE_TAG_TRIP_JUNCTIONS) && (myPathCreator->getSelectedJunctions().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_TRIP);
                // Add parameter departure
                if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain trip parameters
                SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                tripParameters->setParameters(myVehicleBaseObject->getParameters());
                // check trip parameters
                if (tripParameters) {
                    myVehicleBaseObject->setVehicleParameter(tripParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM_JUNCTION, myPathCreator->getSelectedJunctions().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO_JUNCTION, myPathCreator->getSelectedJunctions().back()->getID());
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete tripParameters and base object
                    delete tripParameters;
                }
            } else if ((vehicleTag == GNE_TAG_TRIP_TAZS) && (myPathCreator->getSelectedTAZs().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_TRIP);
                // Add parameter departure
                if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
                }
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain trip parameters
                SUMOVehicleParameter* tripParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
                tripParameters->setParameters(myVehicleBaseObject->getParameters());
                // check trip parameters
                if (tripParameters) {
                    myVehicleBaseObject->setVehicleParameter(tripParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM_TAZ, myPathCreator->getSelectedTAZs().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO_TAZ, myPathCreator->getSelectedTAZs().back()->getID());
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete tripParameters and base object
                    delete tripParameters;
                }
            } else if ((vehicleTag == GNE_TAG_FLOW_JUNCTIONS) && (myPathCreator->getSelectedJunctions().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                // set flow attributes
                updateFlowAttributes();
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain flow parameters
                SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                flowParameters->setParameters(myVehicleBaseObject->getParameters());
                // check flowParameters
                if (flowParameters) {
                    myVehicleBaseObject->setVehicleParameter(flowParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM_JUNCTION, myPathCreator->getSelectedJunctions().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO_JUNCTION, myPathCreator->getSelectedJunctions().back()->getID());
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete flowParameters and base object
                    delete flowParameters;
                }
            } else if ((vehicleTag == GNE_TAG_FLOW_TAZS) && (myPathCreator->getSelectedTAZs().size() > 0)) {
                // set tag
                myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
                // set flow attributes
                updateFlowAttributes();
                // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
                SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
                // obtain flow parameters
                SUMOVehicleParameter* flowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
                flowParameters->setParameters(myVehicleBaseObject->getParameters());
                // check flowParameters
                if (flowParameters) {
                    myVehicleBaseObject->setVehicleParameter(flowParameters);
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_FROM_TAZ, myPathCreator->getSelectedTAZs().front()->getID());
                    myVehicleBaseObject->addStringAttribute(SUMO_ATTR_TO_TAZ, myPathCreator->getSelectedTAZs().back()->getID());
                    // parse vehicle
                    routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                    // delete flowParameters and base object
                    delete flowParameters;
                }
            }
            // abort path creation
            myPathCreator->abortPathCreation();
            // refresh attributes editor
            myVehicleAttributesEditor->refreshAttributesEditor();
            return true;
        }
    }
    return false;
}


bool
GNEVehicleFrame::buildVehicleOverRoute(SumoXMLTag vehicleTag, GNEDemandElement* route) {
    if (route && (route->getTagProperty()->isRoute())) {
        // now check if parameters are valid
        if (!myVehicleAttributesEditor->checkAttributes(true)) {
            return false;
        }
        // get vehicle attributes
        myVehicleAttributesEditor->fillSumoBaseObject(myVehicleBaseObject);
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), myVehicleBaseObject->hasStringAttribute(GNE_ATTR_DEMAND_FILE) ?
                                     myVehicleBaseObject->getStringAttribute(GNE_ATTR_DEMAND_FILE) : "",
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed(), false);
        // check if departLane is valid
        if ((route->getTagProperty()->getTag() == SUMO_TAG_ROUTE) && myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPARTLANE) &&
                GNEAttributeCarrier::canParse<int>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTLANE))) {
            const int departLane = GNEAttributeCarrier::parse<int>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTLANE));
            if (departLane >= (int)route->getParentEdges().front()->getChildLanes().size()) {
                myViewNet->setStatusBarText("Invalid " + toString(SUMO_ATTR_DEPARTLANE));
                return false;
            }
        }
        // check if departSpeed is valid
        if (myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPARTSPEED) && GNEAttributeCarrier::canParse<double>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTSPEED))) {
            double departSpeed = GNEAttributeCarrier::parse<double>(myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPARTSPEED));
            if (departSpeed >= myTypeSelector->getCurrentDemandElement()->getAttributeDouble(SUMO_ATTR_MAXSPEED)) {
                myViewNet->setStatusBarText("Invalid " + toString(SUMO_ATTR_DEPARTSPEED));
                return false;
            }
        }
        // check if we're creating a vehicle or a flow
        if (vehicleTag == SUMO_TAG_VEHICLE) {
            // set tag
            myVehicleBaseObject->setTag(SUMO_TAG_VEHICLE);
            // Add parameter departure
            if (!myVehicleBaseObject->hasStringAttribute(SUMO_ATTR_DEPART) || myVehicleBaseObject->getStringAttribute(SUMO_ATTR_DEPART).empty()) {
                myVehicleBaseObject->addStringAttribute(SUMO_ATTR_DEPART, "0");
            }
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
            // obtain vehicle parameters in vehicleParameters
            SUMOVehicleParameter* vehicleParameters = SUMOVehicleParserHelper::parseVehicleAttributes(vehicleTag, SUMOSAXAttrs, false);
            vehicleParameters->setParameters(myVehicleBaseObject->getParameters());
            // check if vehicle was successfully created)
            if (vehicleParameters) {
                vehicleParameters->routeid = route->getID();
                myVehicleBaseObject->setVehicleParameter(vehicleParameters);
                // parse vehicle
                routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                // delete vehicleParameters and sumoBaseObject
                delete vehicleParameters;
            }
        } else {
            // set tag
            myVehicleBaseObject->setTag(SUMO_TAG_FLOW);
            // set flow attributes
            updateFlowAttributes();
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(myVehicleBaseObject->getAllAttributes(), getPredefinedTagsMML(), toString(vehicleTag));
            // obtain routeFlow parameters in routeFlowParameters
            SUMOVehicleParameter* routeFlowParameters = SUMOVehicleParserHelper::parseFlowAttributes(vehicleTag, SUMOSAXAttrs, false, true, 0, SUMOTime_MAX);
            routeFlowParameters->setParameters(myVehicleBaseObject->getParameters());
            // check if flow was successfully created)
            if (routeFlowParameters) {
                routeFlowParameters->routeid = route->getID();
                myVehicleBaseObject->setVehicleParameter(routeFlowParameters);
                // parse flow
                routeHandler.parseSumoBaseObject(myVehicleBaseObject);
                // delete vehicleParameters and sumoBaseObject
                delete routeFlowParameters;
            }
        }
        // center view after creation
        const auto* vehicle = myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(myVehicleBaseObject->getTag(), myVehicleBaseObject->getStringAttribute(SUMO_ATTR_ID), false);
        if (vehicle && !myViewNet->getVisibleBoundary().around(vehicle->getPositionInView())) {
            myViewNet->centerTo(vehicle->getPositionInView(), false);
        }
        // refresh attributes editor
        myVehicleAttributesEditor->refreshAttributesEditor();
        // all ok, then return true;
        return true;
    } else {
        myViewNet->setStatusBarText(toString(vehicleTag) + " has to be placed within a route.");
        return false;
    }
}


void
GNEVehicleFrame::updateFlowAttributes() {
    // adjust poisson value
    if (myVehicleBaseObject->hasDoubleAttribute(GNE_ATTR_POISSON)) {
        myVehicleBaseObject->addStringAttribute(SUMO_ATTR_PERIOD, "exp(" + toString(myVehicleBaseObject->getDoubleAttribute(GNE_ATTR_POISSON)) + ")");
    }
}

/****************************************************************************/
