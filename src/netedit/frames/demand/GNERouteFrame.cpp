/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for remove network-elements
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEJunction.h>
#include <netedit/elements/demand/GNERoute.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>

#include "GNERouteFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERouteFrame::RouteModeSelector) RouteModeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ROUTEFRAME_ROUTEMODE,    GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ROUTEFRAME_VCLASS,       GNERouteFrame::RouteModeSelector::onCmdSelectVClass),
};

// Object implementation
FXIMPLEMENT(GNERouteFrame::RouteModeSelector,   FXGroupBox,     RouteModeSelectorMap,   ARRAYNUMBER(RouteModeSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERouteFrame::RouteModeSelector - methods
// ---------------------------------------------------------------------------

GNERouteFrame::RouteModeSelector::RouteModeSelector(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Route mode", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent),
    myCurrentRouteMode(RouteMode::NONCONSECUTIVE_EDGES),
    myValidVClass(true) {
    // first fill myRouteModesStrings
    myRouteModesStrings.push_back(std::make_pair(RouteMode::NONCONSECUTIVE_EDGES, "non consecutive edges"));
    myRouteModesStrings.push_back(std::make_pair(RouteMode::CONSECUTIVE_EDGES, "consecutive edges"));
    // Create FXComboBox for Route mode
    myRouteModeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_ROUTEFRAME_ROUTEMODE, GUIDesignComboBox);
    // fill myRouteModeMatchBox with route modes
    for (const auto& routeMode : myRouteModesStrings) {
        myRouteModeMatchBox->appendItem(routeMode.second.c_str());
    }
    // Set visible items
    myRouteModeMatchBox->setNumVisible((int)myRouteModeMatchBox->getNumItems());
    // Create FXComboBox for VClass
    myVClassMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_ROUTEFRAME_VCLASS, GUIDesignComboBox);
    // fill myVClassMatchBox with all VCLass
    for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
        myVClassMatchBox->appendItem(vClass.c_str());
    }
    // set Passenger als default VCLass
    myVClassMatchBox->setCurrentItem(7);
    // Set visible items
    myVClassMatchBox->setNumVisible((int)myVClassMatchBox->getNumItems());
    // RouteModeSelector is always shown
    show();
}


GNERouteFrame::RouteModeSelector::~RouteModeSelector() {}


const GNERouteFrame::RouteMode&
GNERouteFrame::RouteModeSelector::getCurrentRouteMode() const {
    return myCurrentRouteMode;
}


bool
GNERouteFrame::RouteModeSelector::isValidMode() const {
    return (myCurrentRouteMode != RouteMode::INVALID);
}


bool
GNERouteFrame::RouteModeSelector::isValidVehicleClass() const {
    return myValidVClass;
}


void
GNERouteFrame::RouteModeSelector::areParametersValid() {
    // check if current mode is valid
    if ((myCurrentRouteMode != RouteMode::INVALID) && myValidVClass) {
        // show route attributes modul
        myRouteFrameParent->myRouteAttributes->showAttributesCreatorModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_ROUTE), {});
        // show modes moduls
        if (myCurrentRouteMode == RouteMode::CONSECUTIVE_EDGES) {
            myRouteFrameParent->myPathCreator->setPathCreatorMode(GNEFrameModuls::PathCreator::Mode::CONSECUTIVE);
        } else if (myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) {
            myRouteFrameParent->myPathCreator->setPathCreatorMode(GNEFrameModuls::PathCreator::Mode::NOCONSECUTIVE);
        }
        // show route creator
        myRouteFrameParent->myPathCreator->showPathCreatorModul();
        // update edge colors
        myRouteFrameParent->myPathCreator->updateEdgeColors();
        // show legend
        myRouteFrameParent->myLegend->showLegendModul();
    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
        myRouteFrameParent->myPathCreator->hidePathCreatorModul();
        myRouteFrameParent->myLegend->hideLegendModul();
        // reset all flags
        for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
            edge.second->resetCandidateFlags();
        }
        // update view net
        myRouteFrameParent->myViewNet->update();
    }
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myPathCreator->onCmdAbortPathCreation(0, 0, 0);
    // set invalid current route mode
    myCurrentRouteMode = RouteMode::INVALID;
    // set color of myTypeMatchBox to red (invalid)
    myRouteModeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& routeMode : myRouteModesStrings) {
        if (routeMode.second == myRouteModeMatchBox->getText().text()) {
            // Set new current type
            myCurrentRouteMode = routeMode.first;
            // set color of myTypeMatchBox to black (valid)
            myRouteModeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected RouteMode '" + myRouteModeMatchBox->getText() + "' in RouteModeSelector").text());
        }
    }
    // check if parameters are valid
    areParametersValid();
    return 1;
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectVClass(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myPathCreator->onCmdAbortPathCreation(0, 0, 0);
    // set vClass flag invalid
    myValidVClass = false;
    // set color of myTypeMatchBox to red (invalid)
    myVClassMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
        if (vClass == myVClassMatchBox->getText().text()) {
            // change flag
            myValidVClass = true;
            // set color of myTypeMatchBox to black (valid)
            myVClassMatchBox->setTextColor(FXRGB(0, 0, 0));
            // set vClass in Path creator
            myRouteFrameParent->myPathCreator->setVClass(SumoVehicleClassStrings.get(vClass));
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected VClass '" + myVClassMatchBox->getText() + "' in RouteModeSelector").text());
        }
    }
    // check if parameters are valid
    areParametersValid();
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteFrame::Legend - methods
// ---------------------------------------------------------------------------

GNERouteFrame::Legend::Legend(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Legend", GUIDesignGroupBoxFrame) {
    // declare label
    FXLabel* legendLabel = nullptr;
    legendLabel = new FXLabel(this, " edge candidate", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(routeFrameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.possible));
    legendLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    legendLabel = new FXLabel(this, " last edge selected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(routeFrameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.target));
    legendLabel = new FXLabel(this, " edge selected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(routeFrameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.source));
    legendLabel = new FXLabel(this, " edge conflic (vClass)", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(routeFrameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.special));
    legendLabel = new FXLabel(this, " edge disconnected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(routeFrameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.conflict));
}


GNERouteFrame::Legend::~Legend() {}


void
GNERouteFrame::Legend::showLegendModul() {
    show();
}

void
GNERouteFrame::Legend::hideLegendModul() {
    hide();
}

// ---------------------------------------------------------------------------
// GNERouteFrame - methods
// ---------------------------------------------------------------------------

GNERouteFrame::GNERouteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Routes") {

    // create route mode Selector modul
    myRouteModeSelector = new RouteModeSelector(this);

    // Create route parameters
    myRouteAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // create consecutive edges modul
    myPathCreator = new GNEFrameModuls::PathCreator(this, GNEFrameModuls::PathCreator::Mode::NOCONSECUTIVE);

    // create legend label
    myLegend = new Legend(this);
}


GNERouteFrame::~GNERouteFrame() {}


void
GNERouteFrame::show() {
    // call are parameters valid
    myRouteModeSelector->areParametersValid();
    // show route frame
    GNEFrame::show();
}


void
GNERouteFrame::hide() {
    // reset candidate edges
    for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->setPossibleCandidate(false);
    }
    GNEFrame::hide();
}


void
GNERouteFrame::handleEdgeClick(GNEEdge* clickedEdge, const bool shiftKeyPressed, const bool controlKeyPressed) {
    // first check if current vClass and mode are valid and edge exist
    if (clickedEdge && myRouteModeSelector->isValidVehicleClass() && myRouteModeSelector->isValidMode()) {
        // add edge in path
        myPathCreator->addEdge(clickedEdge, shiftKeyPressed, controlKeyPressed);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEnter() {
    if (myRouteModeSelector->isValidVehicleClass() && myRouteModeSelector->isValidMode()) {
        // create route
        myPathCreator->onCmdCreatePath(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyBackSpace() {
    if (myRouteModeSelector->isValidVehicleClass() && myRouteModeSelector->isValidMode()) {
        // remove last edge
        myPathCreator->onCmdRemoveLastElement(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEsc() {
    if (myRouteModeSelector->isValidVehicleClass() && myRouteModeSelector->isValidMode()) {
        // abort route
        myPathCreator->onCmdAbortPathCreation(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::drawTemporalRoute(const GUIVisualizationSettings* s) const {
    if (myPathCreator->getPath().size() > 0) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // iterate over path
        for (int i = 0; i < (int)myPathCreator->getPath().size(); i++) {
            // get path
            const GNEFrameModuls::PathCreator::Path &path = myPathCreator->getPath().at(i);
            // set path color color
            if (path.isConflictDisconnected()) {
                GLHelper::setColor(s->candidateColorSettings.conflict);
            } else if (path.isConflictVClass()) {
                GLHelper::setColor(s->candidateColorSettings.special);
            } else {
                GLHelper::setColor(RGBColor::ORANGE);
            }
            // draw line over 
            for (int j = 0; j < (int)path.getSubPath().size(); j++) {
                const GNELane* lane = path.getSubPath().at(j)->getLanes().back();
                if (((i == 0) && (j == 0)) || (j > 0)) {
                    GLHelper::drawBoxLines(lane->getLaneShape(), 0.3);
                }
                // draw connection between lanes
                if ((j+1) < (int)path.getSubPath().size()) {
                    const GNELane* nextLane = path.getSubPath().at(j+1)->getLanes().back();
                    if (lane->getLane2laneConnections().connectionsMap.count(nextLane) > 0) {
                        GLHelper::drawBoxLines(lane->getLane2laneConnections().connectionsMap.at(nextLane).getShape(), 0.3);
                    } else {
                        GLHelper::drawBoxLines({lane->getLaneShape().back(), nextLane->getLaneShape().front()}, 0.3);
                    }
                }
            }
        }
        // Pop last matrix
        glPopMatrix();
    }
}


GNEFrameModuls::PathCreator*
GNERouteFrame::getPathCreator() const {
    return myPathCreator;
}


void 
GNERouteFrame::createPath() {
    // check that route attributes are valid
    if (!myRouteAttributes->areValuesValid()) {
        myRouteAttributes->showWarningMessage();
    } else if (myPathCreator->getSelectedEdges().size() > 0) {
        // obtain attributes
        std::map<SumoXMLAttr, std::string> valuesMap = myRouteAttributes->getAttributesAndValues(true);
        // declare a route parameter
        GNERouteHandler::RouteParameter routeParameters;
        for (const auto& path : myPathCreator->getPath()) {
            for (const auto& edgeID : path.getSubPath()) {
                // get edge
                GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID->getID());
                // avoid double edges
                if (routeParameters.edges.empty() || (routeParameters.edges.back() != edge)) {
                    routeParameters.edges.push_back(edge);
                }
            }
        }
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            routeParameters.routeID = myViewNet->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE);
        } else {
            routeParameters.routeID = valuesMap[SUMO_ATTR_ID];
        }
        // fill rest of elements
        routeParameters.color = GNEAttributeCarrier::parse<RGBColor>(valuesMap.at(SUMO_ATTR_COLOR));
        routeParameters.vClass = myPathCreator->getVClass();
        // create route
        GNERoute* route = new GNERoute(myViewNet->getNet(), routeParameters);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myViewNet->getUndoList()->p_begin("add " + route->getTagStr());
        myViewNet->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myViewNet->getUndoList()->p_end();
        // abort route creation (because route was already created and vector/colors has to be cleaned)
        myPathCreator->onCmdAbortPathCreation(0, 0, 0);
        // refresh route attributes
        myRouteAttributes->refreshRows();
    }
}

/****************************************************************************/