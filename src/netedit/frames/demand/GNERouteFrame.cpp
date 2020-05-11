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


SUMOVehicleClass
GNERouteFrame::RouteModeSelector::getCurrentVehicleClass() const {
    return myCurrentVehicleClass;
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
            myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::CONSECUTIVE);
        } else if (myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) {
            myRouteFrameParent->myPathCreator->setPathCreatorMode(PathCreator::Mode::NOCONSECUTIVE);
        }
        // show route creator
        myRouteFrameParent->myPathCreator->showPathCreatorModul();
        // update edge colors
        myRouteFrameParent->myPathCreator->updateEdgeColors();
        // show information and label
        myRouteFrameParent->myInformation->showInformationModul();
        myRouteFrameParent->myLegend->showLegendModul();
    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
        myRouteFrameParent->myPathCreator->hidePathCreatorModul();
        myRouteFrameParent->myInformation->hideInformationModul();
        myRouteFrameParent->myLegend->hideLegendModul();
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
    // if VClass name isn't correct, set SVC_IGNORING as current type
    myCurrentVehicleClass = SVC_IGNORING;
    // set color of myTypeMatchBox to red (invalid)
    myVClassMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
        if (vClass == myVClassMatchBox->getText().text()) {
            // change flag
            myValidVClass = true;
            // set color of myTypeMatchBox to black (valid)
            myVClassMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentVehicleClass = SumoVehicleClassStrings.get(vClass);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected VClass '" + myVClassMatchBox->getText() + "' in RouteModeSelector").text());
        }
    }
    // check if parameters are valid
    areParametersValid();
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteFrame::PathCreator - methods
// ---------------------------------------------------------------------------

GNERouteFrame::PathCreator::Path::Path(const SUMOVehicleClass vClass, GNEEdge* edge) :
    subPath({edge}),
    conflictVClass(false),
    conflictDisconnected(false) {
    // check if we have to change vClass flag
    if (edge->getNBEdge()->getNumLanesThatAllow(vClass) == 0) {
        conflictVClass = true;
    }
}


GNERouteFrame::PathCreator::Path::Path(GNEViewNet* viewNet, const SUMOVehicleClass vClass, GNEEdge* edgeFrom, GNEEdge* edgeTo) :
    conflictVClass(false),
    conflictDisconnected(false) {
    // calculate subpath
    subPath = viewNet->getNet()->getPathCalculator()->calculatePath(vClass, {edgeFrom, edgeTo});
    // if subPath is empty, try it with pedestrian (i.e. ignoring vCass)
    if (subPath.empty()) {
        subPath = viewNet->getNet()->getPathCalculator()->calculatePath(SVC_PEDESTRIAN, { edgeFrom, edgeTo });
        if (subPath.empty()) {
            subPath = {edgeFrom, edgeTo};
            conflictDisconnected = true;
        } else {
            conflictVClass = true;
        }
    }
}


GNERouteFrame::PathCreator::Path::Path() :
    conflictVClass(false),
    conflictDisconnected(false) {
}


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
GNERouteFrame::PathCreator::addEdge(GNEEdge* edge, const bool shiftKeyPressed, const bool controlKeyPressed) {
    if (mySelectedElements.size() > 0) {
        // check double edges
        if ((mySelectedElements.back() == edge)) {
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
    }
    // check candidate edge
    if (!edge->isPossibleCandidate()) {
        if (!edge->isSpecialCandidate() && !shiftKeyPressed) {
            // Write warning
            WRITE_WARNING("Invalid edge (SHIFT + click to add an invalid vClass edge)");
            // abort add edge
            return false;
        } else if (!edge->isConflictedCandidate() && !controlKeyPressed) {
            // Write warning
            WRITE_WARNING("Invalid edge (CTRL + click to add an invalid disjoint edge)");
            // abort add edge
            return false;
        }
    }
    // change last edge flag
    if (mySelectedElements.size() > 0 && mySelectedElements.back()->isTargetCandidate()) {
        mySelectedElements.back()->setTargetCandidate(false);
        mySelectedElements.back()->setSourceCandidate(true);
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
    // enable or disable remove last edge button
    if (mySelectedElements.size() > 1) {
        myRemoveLastInsertedEdge->enable();
    } else {
        myRemoveLastInsertedEdge->disable();
    }
    // recalculate path
    recalculatePath();
    // update info route label
    updateInfoRouteLabel();
    // update edge colors
    updateEdgeColors();
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
    myPath.clear();
    // update info route label
    updateInfoRouteLabel();
}


const std::vector<GNERouteFrame::PathCreator::Path>&
GNERouteFrame::PathCreator::getPath() const {
    return myPath;
}


bool 
GNERouteFrame::PathCreator::drawCandidateEdgesWithSpecialColor() const {
    return (myShowCandidateEdges->getCheck() == TRUE);
}


void
GNERouteFrame::PathCreator::updateEdgeColors() {
    // reset all flags
    for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
        edge.second->resetCandidateFlags();
    }
    // check that vClass is valid
    if (myRouteFrameParent->myRouteModeSelector->isValidVehicleClass()) {
        // get vehicle class 
        const SUMOVehicleClass vClass = myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass();
        // set reachability
        if (mySelectedElements.size() > 0) {
            // mark all edges as conflicted (to mark special candidates) 
            for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
                edge.second->setConflictedCandidate(true);
            }
            // call recursively setSpecialCandidates(...)
            setSpecialCandidates(mySelectedElements.back());
            // mark again all edges as conflicted (to mark possible candidates)
            for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
                edge.second->setConflictedCandidate(true);
            }
            // call recursively setSpecialCandidates(...)
            setPossibleCandidates(mySelectedElements.back(), vClass);
            // now mark selected eges
            for (const auto& edge : mySelectedElements) {
                edge->resetCandidateFlags();
                edge->setSourceCandidate(true);
            }
            // finally mark last selected element as target
            mySelectedElements.back()->resetCandidateFlags();
            mySelectedElements.back()->setTargetCandidate(true);
        } else {
            // mark all edges that have at least one lane that allow given vClass
            for (const auto& edge : myRouteFrameParent->myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
                if (edge.second->getNBEdge()->getNumLanesThatAllow(vClass) > 0) {
                    edge.second->setPossibleCandidate(true);
                } else {
                    edge.second->setSpecialCandidate(true);
                }
            }
        }
    }
    // update view net
    myRouteFrameParent->myViewNet->updateViewNet();
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
        for (const auto &path : myPath) {
            for (const auto& edgeID : path.subPath) {
                // get edge
                GNEEdge *edge = myRouteFrameParent->myViewNet->getNet()->retrieveEdge(edgeID->getID());
                // avoid double edges
                if (routeParameters.edges.empty() || (routeParameters.edges.back() != edge)) {
                    routeParameters.edges.push_back(edge);
                }
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
        // update reachability
        updateEdgeColors();
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
        // change last edge flag
        if (mySelectedElements.size() > 0 && mySelectedElements.back()->isSourceCandidate()) {
            mySelectedElements.back()->setSourceCandidate(false);
            mySelectedElements.back()->setTargetCandidate(true);
        }
        // enable or disable remove last edge button
        if (mySelectedElements.size() > 1) {
            myRemoveLastInsertedEdge->enable();
        } else {
            myRemoveLastInsertedEdge->disable();
        }
        // recalculate path
        recalculatePath();
        // update info route label
        updateInfoRouteLabel();
        // update reachability
        updateEdgeColors();
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
    if (myPath.size() > 0) {
        // declare variables for route info
        double length = 0;
        double speed = 0;
        int pathSize = 0;
        for (const auto& path : myPath) {
            for (const auto& edge : path.subPath) {
                length += edge->getNBEdge()->getLength();
                speed += edge->getNBEdge()->getSpeed();
            }
            pathSize += (int)path.subPath.size();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
            << "- Selected edges: " << toString(mySelectedElements.size()) << "\n"
            << "- Path edges: " << toString(pathSize) << "\n"
            << "- Length: " << toString(length) << "\n"
            << "- Average speed: " << toString(speed / pathSize);
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText("No edges selected");
    }
}


void
GNERouteFrame::PathCreator::recalculatePath() {
    // get vehicle class 
    const SUMOVehicleClass vClass = myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass();
    // first clear path
    myPath.clear();
    // check if remove last route edge button has to be disabled
    if (mySelectedElements.size() == 1) {
        myPath.push_back(Path(vClass, mySelectedElements.front()));
    } else {
        // add every segment
        for (int i = 1; i < (int)mySelectedElements.size(); i++) {
            myPath.push_back(Path(myRouteFrameParent->getViewNet(), vClass, mySelectedElements.at(i-1), mySelectedElements.at(i)));
        }
    }
}


void
GNERouteFrame::PathCreator::setSpecialCandidates(GNEEdge* edge) {
    // set edge as special candidate
    edge->setSpecialCandidate(true);
    // disable edge as invalid candidate (this is the stop for recursive function)
    edge->setConflictedCandidate(false);
    // iterate over outgoing edges of second junction's edge
    for (const auto& nextEdge : edge->getSecondParentJunction()->getGNEOutgoingEdges()) {
        // check that isn't conflicted
        if (nextEdge->isConflictedCandidate() && edge->getNBEdge()->isConnectedTo(nextEdge->getNBEdge(), true)) {
            // now continue depending of mode
            if (myMode == Mode::CONSECUTIVE) {
                nextEdge->setSpecialCandidate(true);
                nextEdge->setConflictedCandidate(false);
            } else if (myMode == Mode::NOCONSECUTIVE) {
                setSpecialCandidates(nextEdge);
            }
        }
    }
}

void
GNERouteFrame::PathCreator::setPossibleCandidates(GNEEdge* edge, SUMOVehicleClass vClass) {
    // set edge as special candidate
    edge->setPossibleCandidate(true);
    // disable edge as invalid candidate (this is the stop for recursive function)
    edge->setConflictedCandidate(false);
    // iterate over outgoing edges of second junction's edge
    for (const auto& nextEdge : edge->getSecondParentJunction()->getGNEOutgoingEdges()) {
        // check that isn't conflicted
        if (nextEdge->isConflictedCandidate() && myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->consecutiveEdgesConnected(vClass, edge, nextEdge)) {
            // now continue depending of mode
            if (myMode == Mode::CONSECUTIVE) {
                nextEdge->setPossibleCandidate(true);
                nextEdge->setConflictedCandidate(false);
            } else if (myMode == Mode::NOCONSECUTIVE) {
                setPossibleCandidates(nextEdge, vClass);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// GNERouteFrame::Information - methods
// ---------------------------------------------------------------------------

GNERouteFrame::Information::Information(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Information", GUIDesignGroupBoxFrame) {
    // create keys Hint
    new FXLabel(this,
        "- Hold SHIFT while clicking\n  to add an invalid vclass edge.",
        0, GUIDesignLabelFrameInformation);
    new FXLabel(this,
        "- Hold CONTROL while clicking\n  to add an disjoint edge.",
        0, GUIDesignLabelFrameInformation);
    new FXLabel(this,
        "- Press BACKSPACE to remove\n  last inserted edge.",
        0, GUIDesignLabelFrameInformation);
}


GNERouteFrame::Information::~Information() {}


void 
GNERouteFrame::Information::showInformationModul() {
    show();
}

void 
GNERouteFrame::Information::hideInformationModul() {
    hide();
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
    legendLabel = new FXLabel(this, " edge conflict (disjointed)", 0, GUIDesignLabelLeft);
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
    myPathCreator = new PathCreator(this, PathCreator::Mode::NOCONSECUTIVE);

    // create information modul
    myInformation = new Information(this);

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
        for (int i = 0; i < myPathCreator->getPath().size(); i++) {
            // get path
            const GNERouteFrame::PathCreator::Path &path = myPathCreator->getPath().at(i);
            // set path color color
            if (path.conflictDisconnected) {
                GLHelper::setColor(s->candidateColorSettings.conflict);
            } else if (path.conflictVClass) {
                GLHelper::setColor(s->candidateColorSettings.special);
            } else {
                GLHelper::setColor(RGBColor::ORANGE);
            }
            // draw line over 
            for (int j = 0; j < path.subPath.size(); j++) {
                const GNELane* lane = path.subPath.at(j)->getLanes().back();
                if (j > 0) {
                    GLHelper::drawBoxLines(lane->getLaneShape(), 0.3);
                }
                // draw connection between lanes
                if ((j+1) < path.subPath.size()) {
                    const GNELane* nextLane = path.subPath.at(j+1)->getLanes().back();
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


GNERouteFrame::PathCreator* 
GNERouteFrame::getPathCreator() const {
    return myPathCreator;
}

/****************************************************************************/