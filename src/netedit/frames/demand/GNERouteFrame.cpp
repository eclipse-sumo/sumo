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

FXDEFMAP(GNERouteFrame::PathCreator) PathCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,          GNERouteFrame::PathCreator::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,         GNERouteFrame::PathCreator::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST,     GNERouteFrame::PathCreator::onCmdRemoveLastElement),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_SHOWCANDIDATES, GNERouteFrame::PathCreator::onCmdShowCandidateEdges)
};

// Object implementation
FXIMPLEMENT(GNERouteFrame::RouteModeSelector,   FXGroupBox,     RouteModeSelectorMap,   ARRAYNUMBER(RouteModeSelectorMap))
FXIMPLEMENT(GNERouteFrame::PathCreator,         FXGroupBox,     PathCreatorMap,         ARRAYNUMBER(PathCreatorMap))


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
    myCurrentVehicleClass(SVC_PASSENGER),
    myValidVClass(true) {
    // first fill myRouteModesStrings
    myRouteModesStrings.push_back(std::make_pair(RouteMode::NONCONSECUTIVE_EDGES, "non consecutive edges"));
    myRouteModesStrings.push_back(std::make_pair(RouteMode::CONSECUTIVE_EDGES, "consecutive edges"));
    // Create FXComboBox for Route mode
    myRouteModeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_ROUTEFRAME_ROUTEMODE, GUIDesignComboBox);
    // fill myRouteModeMatchBox with route modes
    for (const auto& i : myRouteModesStrings) {
        myRouteModeMatchBox->appendItem(i.second.c_str());
    }
    // Set visible items
    myRouteModeMatchBox->setNumVisible((int)myRouteModeMatchBox->getNumItems());
    // Create FXComboBox for VClass
    myVClassMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_ROUTEFRAME_VCLASS, GUIDesignComboBox);
    // fill myVClassMatchBox with all VCLass
    for (const auto& i : SumoVehicleClassStrings.getStrings()) {
        myVClassMatchBox->appendItem(i.c_str());
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


SUMOVehicleClass
GNERouteFrame::RouteModeSelector::getCurrentVehicleClass() const {
    return myCurrentVehicleClass;
}


bool
GNERouteFrame::RouteModeSelector::isValidVehicleClass() const {
    return myValidVClass;
}


void
GNERouteFrame::RouteModeSelector::setCurrentRouteMode(RouteMode routemode) {
    // make sure that route isn't invalid
    if (routemode != RouteMode::INVALID) {
        // show route creator
        myRouteFrameParent->myPathCreator->showPathCreatorModul();
        // restore color
        myRouteModeMatchBox->setTextColor(FXRGB(0, 0, 0));
        // set current route mode
        myCurrentRouteMode = routemode;
        // set item in myTypeMatchBox
        for (int i = 0; i < (int)myRouteModesStrings.size(); i++) {
            if (myRouteModesStrings.at(i).first == myCurrentRouteMode) {
                myRouteModeMatchBox->setCurrentItem(i);
            }
        }
        // show route attributes modul
        myRouteFrameParent->myRouteAttributes->showAttributesCreatorModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_ROUTE), {});
        // show modes moduls
        if ((routemode == RouteMode::CONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
            myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::CONSECUTIVE);
        } else if ((routemode == RouteMode::NONCONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
            myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::NOCONSECUTIVE);
        }
    } else {
        // hide all moduls if route mode isn't valid
        myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
        myRouteFrameParent->myPathCreator->hidePathCreatorModul();
    }
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myPathCreator->onCmdAbortPathCreation(0, 0, 0);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myRouteModesStrings) {
        if (i.second == myRouteModeMatchBox->getText().text()) {
            // show route creator
            myRouteFrameParent->myPathCreator->showPathCreatorModul();
            // set color of myTypeMatchBox to black (valid)
            myRouteModeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentRouteMode = i.first;
            // show route attributes modul
            myRouteFrameParent->myRouteAttributes->showAttributesCreatorModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_ROUTE), {});
            // show modes moduls
            if ((myCurrentRouteMode == RouteMode::CONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
                myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::CONSECUTIVE);
            } else if ((myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
                myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::NOCONSECUTIVE);
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected RouteMode '" + myRouteModeMatchBox->getText() + "' in RouteModeSelector").text());
            return 1;
        }
    }
    // if Route mode isn't correct, set RouteMode::INVALID as current route mde
    myCurrentRouteMode = RouteMode::INVALID;
    // hide all moduls if route mode isn't valid
    myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
    myRouteFrameParent->myPathCreator->hidePathCreatorModul();
    // set color of myTypeMatchBox to red (invalid)
    myRouteModeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid RouteMode in RouteModeSelector");
    return 1;
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectVClass(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myPathCreator->onCmdAbortPathCreation(0, 0, 0);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : SumoVehicleClassStrings.getStrings()) {
        if (i == myVClassMatchBox->getText().text()) {
            // show route creator
            myRouteFrameParent->myPathCreator->showPathCreatorModul();
            // set color of myTypeMatchBox to black (valid)
            myVClassMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentVehicleClass = SumoVehicleClassStrings.get(i);
            // change flag
            myValidVClass = true;
            // show route attributes modul
            myRouteFrameParent->myRouteAttributes->showAttributesCreatorModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_ROUTE), {});
            // enable moduls if current route is valid
            if (myCurrentRouteMode == RouteMode::CONSECUTIVE_EDGES) {
                myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::CONSECUTIVE);
            } else if (myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) {
                myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::NOCONSECUTIVE);
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected VClass '" + myVClassMatchBox->getText() + "' in RouteModeSelector").text());
            return 1;
        }
    }
    // if VClass name isn't correct, set SVC_IGNORING as current type
    myCurrentVehicleClass = SVC_IGNORING;
    // change flag
    myValidVClass = false;
    // hide all moduls if route mode isnt' valid
    myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
    myRouteFrameParent->myPathCreator->hidePathCreatorModul();
    // set color of myTypeMatchBox to red (invalid)
    myVClassMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid VClass in RouteModeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteFrame::PathCreator - methods
// ---------------------------------------------------------------------------

GNERouteFrame::PathCreator::PathCreator(GNERouteFrame* routeFrameParent, GNERouteFrame::PathCreator::Mode mode) :
    FXGroupBox(routeFrameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent),
    myMode(mode) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(this, "No edges selected", 0, GUIDesignLabelFrameThicked);
    // create button for finish route creation
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted edge
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
    // create check button
    myShowCandidateEdges = new FXCheckButton(this, "Show candidate edges", this, MID_GNE_EDGEPATH_SHOWCANDIDATES, GUIDesignCheckButton);
    myShowCandidateEdges->setCheck(TRUE);
}


GNERouteFrame::PathCreator::~PathCreator() {}


void
GNERouteFrame::PathCreator::showPathCreatorModul() {
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    // recalc before show (to avoid graphic problems)
    recalc();
    // show modul
    show();
}


void
GNERouteFrame::PathCreator::hidePathCreatorModul() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


std::vector<GNEEdge*>
GNERouteFrame::PathCreator::getSelectedEdges() const {
    return mySelectedElements;
}


void 
GNERouteFrame::PathCreator::setPathCreatorMode(GNERouteFrame::PathCreator::Mode mode) {
    // first abort route
    onCmdAbortPathCreation(nullptr, 0, nullptr);
    myMode = mode;
}


bool
GNERouteFrame::PathCreator::addEdge(GNEEdge* edge, const bool shiftKeyPressed) {
    // check double edges
    if ((mySelectedElements.size() > 0) && (mySelectedElements.back() == edge)) {
        // Write warning
        WRITE_WARNING("Double edges aren't allowed");
        // abort add edge
        return false;
    }
    // check consecutive edges
    if (myMode == Mode::CONSECUTIVE) {
        // check that new edge is consecutive
        const auto &outgoingEdges = mySelectedElements.back()->getSecondParentJunction()->getGNEOutgoingEdges();
        if (std::find(outgoingEdges.begin(), outgoingEdges.end(), edge) == outgoingEdges.end()) {
            // Write warning
            WRITE_WARNING("Only consecutives edges are allowed");
            // abort add edge
            return false;
        }
    }
    // check candidate edge
    if ((mySelectedElements.size() > 0) && !edge->isPossibleCandidate() && !shiftKeyPressed) {
        // Write warning
        WRITE_WARNING("Only candidate edges are allowed");
        // abort add edge
        return false;
    }
    // All checks ok, then add it in selected elements
    mySelectedElements.push_back(edge);
    // set selected color (check)
    if ((mySelectedElements.size() == 1) || edge->isPossibleCandidate()) {
        edge->setTargetCandidate(true);
    } else {
        edge->setConflictedCandidate(true);
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
    // calculate route if there is more than two edges
    if (mySelectedElements.size() > 1) {
        // enable remove last edge button
        myRemoveLastInsertedEdge->enable();
        // calculate temporal route
        myTemporalPath = myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), mySelectedElements);
    } else {
        // Routes with only one edge are allowed
        myTemporalPath.clear();
        myTemporalPath.push_back(mySelectedElements.front());
    }
    // update info route label
    updateInfoRouteLabel();
    // update reachability
    updateReachability();
    return true;
}


void
GNERouteFrame::PathCreator::clearPath() {
    // reset all flags
    for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // clear edges
    mySelectedElements.clear();
    myTemporalPath.clear();
    // update info route label
    updateInfoRouteLabel();
}


const std::vector<GNEEdge*>&
GNERouteFrame::PathCreator::getPathRoute() const {
    return myTemporalPath;
}


bool 
GNERouteFrame::PathCreator::showCandidateEdges() const {
    return (myShowCandidateEdges->getCheck() == TRUE);
}


long
GNERouteFrame::PathCreator::onCmdCreatePath(FXObject*, FXSelector, void*) {
    // check that route attributes are valid
    if (!myRouteFrameParent->myRouteAttributes->areValuesValid()) {
        myRouteFrameParent->myRouteAttributes->showWarningMessage();
    } else if (mySelectedElements.size() > 0) {
        // obtain attributes
        std::map<SumoXMLAttr, std::string> valuesMap = myRouteFrameParent->myRouteAttributes->getAttributesAndValues(true);
        // declare a route parameter
        GNERouteHandler::RouteParameter routeParameters;
        if (myTemporalPath.size() > 0) {
            routeParameters.edges.reserve(myTemporalPath.size());
            for (const auto& edge : myTemporalPath) {
                routeParameters.edges.push_back(myRouteFrameParent->myViewNet->getNet()->retrieveEdge(edge->getID()));
            }
        } else {
            for (const auto& edge : mySelectedElements) {
                routeParameters.edges.push_back(myRouteFrameParent->myViewNet->getNet()->retrieveEdge(edge->getID()));
            }
        }
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            routeParameters.routeID = myRouteFrameParent->getViewNet()->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE);
        } else {
            routeParameters.routeID = valuesMap[SUMO_ATTR_ID];
        }
        // fill rest of elements
        routeParameters.color = GNEAttributeCarrier::parse<RGBColor>(valuesMap.at(SUMO_ATTR_COLOR));
        routeParameters.vClass = myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass();
        // create route
        GNERoute* route = new GNERoute(myRouteFrameParent->getViewNet()->getNet(), routeParameters);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myRouteFrameParent->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
        myRouteFrameParent->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myRouteFrameParent->getViewNet()->getUndoList()->p_end();
        // abort route creation (because route was already created and vector/colors has to be cleaned)
        onCmdAbortPathCreation(0, 0, 0);
        // refresh route attributes
        myRouteFrameParent->myRouteAttributes->refreshRows();
    }
    return 1;
}


long
GNERouteFrame::PathCreator::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // first check that there is route edges selected
    if (mySelectedElements.size() > 0) {
        // unblock undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
        // clear edges
        clearPath();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
        // update info route label
        updateInfoRouteLabel();
        // update view (to see the new route)
        myRouteFrameParent->getViewNet()->updateViewNet();
    }
    return 1;
}


long
GNERouteFrame::PathCreator::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    if (mySelectedElements.size() > 1) {
        // remove special color of last selected edge
        mySelectedElements.back()->resetCandidateFlags();
        // remove last edge
        mySelectedElements.pop_back();
        // check if remove last route edge button has to be disabled
        if (mySelectedElements.size() == 1) {
            // avoid remove last edge
            myRemoveLastInsertedEdge->disable();
            // Routes with only one edge are allowed
            myTemporalPath.clear();
            myTemporalPath.push_back(mySelectedElements.front());
        } else {
            // calculate temporal route
            myTemporalPath = myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), mySelectedElements);
        }
        // update info route label
        updateInfoRouteLabel();
        // update reachability
        updateReachability();
        // update view
        myRouteFrameParent->myViewNet->updateViewNet();
        return true;
    } else {
        return false;
    }
}


long 
GNERouteFrame::PathCreator::onCmdShowCandidateEdges(FXObject*, FXSelector, void*) {
    // just update view
    myRouteFrameParent->myViewNet->updateViewNet();
    return 1;
}


void
GNERouteFrame::PathCreator::updateInfoRouteLabel() {
    if (myTemporalPath.size() > 0) {
        // declare variables for route info
        double length = 0;
        double speed = 0;
        for (const auto& edge : myTemporalPath) {
            length += edge->getNBEdge()->getLength();
            speed += edge->getNBEdge()->getSpeed();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
            << "- Number of Edges: " << toString(myTemporalPath.size()) << "\n"
            << "- Length: " << toString(length) << "\n"
            << "- Average speed: " << toString(speed / myTemporalPath.size());
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText("No edges selected");
    }
}


void 
GNERouteFrame::PathCreator::updateReachability() {
    // reset candidate edges
    for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->setPossibleCandidate(false);
    }
    // set reachability
    if (mySelectedElements.size() > 0) {
        setEdgesReachability(mySelectedElements.back(), myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass());
    }
}


void 
GNERouteFrame::PathCreator::setEdgesReachability(GNEEdge* edge, SUMOVehicleClass vClass) {
    // set edge reachable
    edge->setPossibleCandidate(true);
    // iterate over outgoing edges of second junction's edge
    for (const auto &nextEdge : edge->getSecondParentJunction()->getGNEOutgoingEdges()) {
        if (!nextEdge->isPossibleCandidate() && myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->consecutiveEdgesConnected(vClass, edge, nextEdge)) {
            if (myMode == Mode::CONSECUTIVE) {
                nextEdge->setPossibleCandidate(true);
            } else if (myMode == Mode::NOCONSECUTIVE) {
                setEdgesReachability(nextEdge, vClass);
            }
        }
    }
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
    myPathCreator = new PathCreator(this, PathCreator::Mode::NOCONSECUTIVE);

    // set RouteMode::NONCONSECUTIVE_EDGES as default mode
    myRouteModeSelector->setCurrentRouteMode(RouteMode::NONCONSECUTIVE_EDGES);

    // Create groupbox for information
    FXGroupBox* groupBoxInformation = new FXGroupBox(myContentFrame, "Information", GUIDesignGroupBoxFrame);
    
    // create keys Hint
    new FXLabel(groupBoxInformation, "- Hold SHIFT key while clicking\n  to add an invalid edge.", 0, GUIDesignLabelFrameInformation);
    new FXLabel(groupBoxInformation, "- Press BACKSPACE key to remove\n  last inserted edge.", 0, GUIDesignLabelFrameInformation);
    
    // Create groupbox and labels for legends
    FXGroupBox* groupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);
    FXLabel* colorCandidateLabel = new FXLabel(groupBoxLegend, " edge candidate", 0, GUIDesignLabelLeft);
    colorCandidateLabel->setBackColor(MFXUtils::getFXColor(viewNet->getVisualisationSettings().candidateColorSettings.possible));
    colorCandidateLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    FXLabel* colorSelectedLabel = new FXLabel(groupBoxLegend, " edge selected", 0, GUIDesignLabelLeft);
    colorSelectedLabel->setBackColor(MFXUtils::getFXColor(viewNet->getVisualisationSettings().candidateColorSettings.target));
    FXLabel* colorConflictLabel = new FXLabel(groupBoxLegend, " edge selected (conflict)", 0, GUIDesignLabelLeft);
    colorConflictLabel->setBackColor(MFXUtils::getFXColor(viewNet->getVisualisationSettings().candidateColorSettings.conflict));
}


GNERouteFrame::~GNERouteFrame() {}


void
GNERouteFrame::show() {
    // refresh myRouteModeSelector
    myRouteModeSelector->setCurrentRouteMode(myRouteModeSelector->getCurrentRouteMode());
    // show route frame
    GNEFrame::show();
}


void
GNERouteFrame::hide() {
    GNEFrame::hide();
}


void
GNERouteFrame::handleEdgeClick(GNEEdge* clickedEdge, const bool shiftKeyPressed) {
    // first check if current vClass is valid and edge exist
    if (myRouteModeSelector->isValidVehicleClass() && clickedEdge &&
        (myRouteModeSelector->getCurrentRouteMode() != RouteMode::INVALID)) {
        // add edge in path
        myPathCreator->addEdge(clickedEdge, shiftKeyPressed);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEnter() {
    if (myRouteModeSelector->getCurrentRouteMode() != RouteMode::INVALID) {
        // create route
        myPathCreator->onCmdCreatePath(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyBackSpace() {
    if (myRouteModeSelector->getCurrentRouteMode() != RouteMode::INVALID) {
        // remove last edge
        myPathCreator->onCmdRemoveLastElement(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEsc() {
    if (myRouteModeSelector->getCurrentRouteMode() != RouteMode::INVALID) {
        // abort route
        myPathCreator->onCmdAbortPathCreation(0, 0, 0);
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::drawTemporalRoute() const {
    // declare a vector with temporal route edges
    std::vector<GNEEdge*> temporalRoute;
    if (myPathCreator->getPathRoute().size() > 0) {
        temporalRoute = myPathCreator->getPathRoute();
    } else {
        temporalRoute = myPathCreator->getSelectedEdges();
    }
    // only draw if there is at least two edges
    if (temporalRoute.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(temporalRoute.at(0)->getNBEdge()->getLanes().front().shape.front(),
                           temporalRoute.at(0)->getNBEdge()->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)temporalRoute.size(); i++) {
            GLHelper::drawLine(temporalRoute.at(i - 1)->getNBEdge()->getLanes().front().shape.back(),
                               temporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front());
            GLHelper::drawLine(temporalRoute.at(i)->getNBEdge()->getLanes().front().shape.front(),
                               temporalRoute.at(i)->getNBEdge()->getLanes().front().shape.back());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


GNERouteFrame::PathCreator* 
GNERouteFrame::getPathCreator() const {
    return myPathCreator;
}

/****************************************************************************/
