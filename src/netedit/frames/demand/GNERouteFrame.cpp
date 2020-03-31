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

FXDEFMAP(GNERouteFrame::ConsecutiveEdges) ConsecutiveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,      GNERouteFrame::ConsecutiveEdges::onCmdAbortRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,     GNERouteFrame::ConsecutiveEdges::onCmdCreateRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST, GNERouteFrame::ConsecutiveEdges::onCmdRemoveLastRouteEdge)
};

FXDEFMAP(GNERouteFrame::NonConsecutiveEdges) NonConsecutiveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_ABORT,      GNERouteFrame::NonConsecutiveEdges::onCmdAbortRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_FINISH,     GNERouteFrame::NonConsecutiveEdges::onCmdCreateRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGEPATH_REMOVELAST, GNERouteFrame::NonConsecutiveEdges::onCmdRemoveLastRouteEdge)
};

// Object implementation
FXIMPLEMENT(GNERouteFrame::RouteModeSelector,   FXGroupBox,     RouteModeSelectorMap,   ARRAYNUMBER(RouteModeSelectorMap))
FXIMPLEMENT(GNERouteFrame::ConsecutiveEdges,    FXGroupBox,     ConsecutiveEdgesMap,    ARRAYNUMBER(ConsecutiveEdgesMap))
FXIMPLEMENT(GNERouteFrame::NonConsecutiveEdges, FXGroupBox,     NonConsecutiveEdgesMap, ARRAYNUMBER(NonConsecutiveEdgesMap))


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
            myRouteFrameParent->myConsecutiveEdges->showConsecutiveEdgesModul();
            myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
        } else if ((routemode == RouteMode::NONCONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
            myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
            myRouteFrameParent->myNonConsecutiveEdges->showNonConsecutiveEdgesModul();
        }
    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myRouteAttributes->hideAttributesCreatorModul();
        myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
        myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
    }
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
    myRouteFrameParent->myNonConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myRouteModesStrings) {
        if (i.second == myRouteModeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myRouteModeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentRouteMode = i.first;
            // show route attributes modul
            myRouteFrameParent->myRouteAttributes->showAttributesCreatorModul(GNEAttributeCarrier::getTagProperties(SUMO_TAG_ROUTE), {});
            // show modes moduls
            if ((myCurrentRouteMode == RouteMode::CONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
                myRouteFrameParent->myConsecutiveEdges->showConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
            } else if ((myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) && (myCurrentVehicleClass != SVC_IGNORING)) {
                myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->showNonConsecutiveEdgesModul();
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
    myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
    myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
    // set color of myTypeMatchBox to red (invalid)
    myRouteModeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid RouteMode in RouteModeSelector");
    return 1;
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectVClass(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
    myRouteFrameParent->myNonConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : SumoVehicleClassStrings.getStrings()) {
        if (i == myVClassMatchBox->getText().text()) {
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
                myRouteFrameParent->myConsecutiveEdges->showConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
            } else if (myCurrentRouteMode == RouteMode::NONCONSECUTIVE_EDGES) {
                myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->showNonConsecutiveEdgesModul();
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
    myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
    myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
    // set color of myTypeMatchBox to red (invalid)
    myVClassMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid VClass in RouteModeSelector");
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteFrame::ConsecutiveEdges - methods
// ---------------------------------------------------------------------------

GNERouteFrame::ConsecutiveEdges::ConsecutiveEdges(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Consecutive edges", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(this, "No edges selected", 0, GUIDesignLabelFrameInformation);
    // Create button for create routes
    myCreateRouteButton = new FXButton(this, "Create route", 0, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myCreateRouteButton->disable();
    // Create button for create routes
    myAbortCreationButton = new FXButton(this, "Abort creation", 0, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted edge
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
    // ConsecutiveEdges is by default shown
    show();
}


GNERouteFrame::ConsecutiveEdges::~ConsecutiveEdges() {}


void
GNERouteFrame::ConsecutiveEdges::showConsecutiveEdgesModul() {
    // recalc before show (to avoid graphic problems)
    recalc();
    // show modul
    show();
}


void
GNERouteFrame::ConsecutiveEdges::hideConsecutiveEdgesModul() {
    // first abort route creation
    onCmdAbortRoute(0, 0, 0);
    // now hide modul
    hide();
}


bool
GNERouteFrame::ConsecutiveEdges::addEdge(GNEEdge* edge) {
    // check if currently we're creating a new route
    if (myRouteEdges.empty()) {
        // block undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
        // add edge into list
        myRouteEdges.push_back(edge);
        // refresh edge candidates
        refreshEdgeCandidates();
        // enable create route and abort edge route
        myCreateRouteButton->enable();
        myAbortCreationButton->enable();
        // edge added, then return true
        return true;
    } else {
        // check if clicked edge is in the candidate edges
        for (const auto& i : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            if ((i == edge) && myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->consecutiveEdgesConnected(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), myRouteEdges.back(), edge)) {
                // restore colors of outgoing edges
                for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
                    for (const auto& k : j->getLanes()) {
                        k->setSpecialColor(nullptr);
                    }
                }
                // add new edge in the list of route edges
                myRouteEdges.push_back(edge);
                // enable remove last inserted edge
                myRemoveLastInsertedEdge->enable();
                // refresh edge candidates
                refreshEdgeCandidates();
                // edge added, then return true
                return true;
            }
        }
        // edge isn't a candidate edge, then return false
        return false;
    }
}


void
GNERouteFrame::ConsecutiveEdges::refreshEdgeCandidates() {
    // first check that at least there is a candidate edge
    if (myRouteEdges.size() > 0) {
        // set selected color in all edges
        for (const auto& j : myRouteEdges) {
            for (const auto& k : j->getLanes()) {
                k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateColor());
            }
        }
        // set new candidate colors
        for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            // check if exist a connection between both edges
            if (myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->consecutiveEdgesConnected(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), myRouteEdges.back(), j)) {
                for (const auto& k : j->getLanes()) {
                    k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
                }
            }
        }
        // update route label
        updateInfoRouteLabel();
        // update view
        myRouteFrameParent->getViewNet()->updateViewNet();
    }
}


void
GNERouteFrame::ConsecutiveEdges::clearEdges() {
    // disable special color in candidate edges
    for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
        for (const auto& k : j->getLanes()) {
            k->setSpecialColor(nullptr);
        }
    }
    // disable special color in current route edges
    for (const auto& j : myRouteEdges) {
        for (const auto& k : j->getLanes()) {
            k->setSpecialColor(nullptr);
        }
    }
    // clear route edges
    myRouteEdges.clear();
}


const std::vector<GNEEdge*>&
GNERouteFrame::ConsecutiveEdges::getRouteEdges() const {
    return myRouteEdges;
}


long
GNERouteFrame::ConsecutiveEdges::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    // check that route attributes are valid
    if (!myRouteFrameParent->myRouteAttributes->areValuesValid()) {
        myRouteFrameParent->myRouteAttributes->showWarningMessage();
    } else if (myRouteEdges.size() > 0) {
        // obtain attributes
        std::map<SumoXMLAttr, std::string> valuesMap = myRouteFrameParent->myRouteAttributes->getAttributesAndValues(true);
        // declare a route parameter
        GNERouteHandler::RouteParameter routeParameters;
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            routeParameters.routeID = myRouteFrameParent->getViewNet()->getNet()->generateDemandElementID("", SUMO_TAG_ROUTE);
        } else {
            routeParameters.routeID = valuesMap[SUMO_ATTR_ID];
        }
        // fill rest of elements
        routeParameters.color = GNEAttributeCarrier::parse<RGBColor>(valuesMap.at(SUMO_ATTR_COLOR));
        routeParameters.edges = myRouteEdges;
        routeParameters.vClass = myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass();
        // create route
        GNERoute* route = new GNERoute(myRouteFrameParent->getViewNet(), routeParameters);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myRouteFrameParent->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
        myRouteFrameParent->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myRouteFrameParent->getViewNet()->getUndoList()->p_end();
        // abort route creation (because route was already created and vector/colors has to be cleaned)
        onCmdAbortRoute(0, 0, 0);
        // refresh route attributes
        myRouteFrameParent->myRouteAttributes->refreshRows();
    }
    return 1;
}


long
GNERouteFrame::ConsecutiveEdges::onCmdAbortRoute(FXObject*, FXSelector, void*) {
    // first check that there is route edges selected
    if (myRouteEdges.size() > 0) {
        // unblock undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
        // clear edges
        clearEdges();
        // disable buttons
        myCreateRouteButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
        // update route label
        updateInfoRouteLabel();
        // update view
        myRouteFrameParent->getViewNet()->updateViewNet();
    }
    return 1;
}


long
GNERouteFrame::ConsecutiveEdges::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (myRouteEdges.size() > 1) {
        // restore colors of last inserted edge edges
        for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            for (const auto& k : j->getLanes()) {
                k->setSpecialColor(nullptr);
            }
        }
        // add new edge in the list of route edges
        myRouteEdges.pop_back();
        // set selected color in all edges
        for (const auto& j : myRouteEdges) {
            for (const auto& k : j->getLanes()) {
                k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateColor());
            }
        }
        // set new candidate colors
        for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            if (j != myRouteEdges.back()) {
                for (const auto& k : j->getLanes()) {
                    k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
                }
            }
        }
        // disable remove last edge button if there is only one edge
        if (myRouteEdges.size() == 1) {
            myRemoveLastInsertedEdge->disable();
        }
        // update route label
        updateInfoRouteLabel();
        // update view
        myRouteFrameParent->getViewNet()->updateViewNet();
        // edge added, then return true
        return true;
    } else {
        return false;
    }
}

void
GNERouteFrame::ConsecutiveEdges::updateInfoRouteLabel() {
    if (myRouteEdges.size() > 0) {
        // declare variables for route info
        double length = 0;
        double speed = 0;
        for (const auto& i : myRouteEdges) {
            length += i->getNBEdge()->getLength();
            speed += i->getNBEdge()->getSpeed();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << "- Number of Edges: " << toString(myRouteEdges.size()) << "\n"
                << "- Length: " << toString(length) << "\n"
                << "- Average speed: " << toString(speed / myRouteEdges.size());
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText("No edges selected");
    }
}

// ---------------------------------------------------------------------------
// GNERouteFrame::NonConsecutiveEdges - methods
// ---------------------------------------------------------------------------

GNERouteFrame::NonConsecutiveEdges::NonConsecutiveEdges(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Route creator", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(this, "No edges selected", 0, GUIDesignLabelFrameInformation);
    // create button for finish route creation
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_EDGEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_EDGEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted edge
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_EDGEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedEdge->disable();
}


GNERouteFrame::NonConsecutiveEdges::~NonConsecutiveEdges() {
}


void
GNERouteFrame::NonConsecutiveEdges::showNonConsecutiveEdgesModul() {
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
GNERouteFrame::NonConsecutiveEdges::hideNonConsecutiveEdgesModul() {
    hide();
}


std::vector<GNEEdge*>
GNERouteFrame::NonConsecutiveEdges::getSelectedEdges() const {
    return mySelectedEdges;
}


bool
GNERouteFrame::NonConsecutiveEdges::addEdge(GNEEdge* edge) {
    if (mySelectedEdges.empty() || ((mySelectedEdges.size() > 0) && (mySelectedEdges.back() != edge))) {
        mySelectedEdges.push_back(edge);
        // enable abort route button
        myAbortCreationButton->enable();
        // enable finish button
        myFinishCreationButton->enable();
        // disable undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (mySelectedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // calculate temporal route
            myTemporalRoute = myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), mySelectedEdges);
        } else {
            // Routes with only one edge are allowed
            myTemporalRoute.clear();
            myTemporalRoute.push_back(mySelectedEdges.front());
        }
        // update info route label
        updateInfoRouteLabel();
        return true;
    } else {
        return false;
    }
}


void
GNERouteFrame::NonConsecutiveEdges::clearEdges() {
    // restore colors
    for (const auto& i : mySelectedEdges) {
        for (const auto& j : i->getLanes()) {
            j->setSpecialColor(nullptr);
        }
    }
    // clear edges
    mySelectedEdges.clear();
    myTemporalRoute.clear();
}


const std::vector<GNEEdge*>&
GNERouteFrame::NonConsecutiveEdges::getTemporalRoute() const {
    return myTemporalRoute;
}


bool
GNERouteFrame::NonConsecutiveEdges::isValid(SUMOVehicleClass /* vehicleClass */) const {
    return mySelectedEdges.size() > 0;
}


long
GNERouteFrame::NonConsecutiveEdges::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    // check that route attributes are valid
    if (!myRouteFrameParent->myRouteAttributes->areValuesValid()) {
        myRouteFrameParent->myRouteAttributes->showWarningMessage();
    } else if (mySelectedEdges.size() > 0) {
        // obtain attributes
        std::map<SumoXMLAttr, std::string> valuesMap = myRouteFrameParent->myRouteAttributes->getAttributesAndValues(true);
        // declare a route parameter
        GNERouteHandler::RouteParameter routeParameters;
        routeParameters.edges.reserve(myTemporalRoute.size());
        for (const auto& i : myTemporalRoute) {
            routeParameters.edges.push_back(myRouteFrameParent->myViewNet->getNet()->retrieveEdge(i->getID()));
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
        GNERoute* route = new GNERoute(myRouteFrameParent->getViewNet(), routeParameters);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myRouteFrameParent->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
        myRouteFrameParent->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myRouteFrameParent->getViewNet()->getUndoList()->p_end();
        // abort route creation (because route was already created and vector/colors has to be cleaned)
        onCmdAbortRoute(0, 0, 0);
        // refresh route attributes
        myRouteFrameParent->myRouteAttributes->refreshRows();
    }
    return 1;
}


long
GNERouteFrame::NonConsecutiveEdges::onCmdAbortRoute(FXObject*, FXSelector, void*) {
    // first check that there is route edges selected
    if (mySelectedEdges.size() > 0) {
        // unblock undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
        // clear edges
        clearEdges();
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
GNERouteFrame::NonConsecutiveEdges::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (mySelectedEdges.size() > 1) {
        // remove special color of last selected edge
        for (auto i : mySelectedEdges.back()->getLanes()) {
            i->setSpecialColor(0);
        }
        // remove last edge
        mySelectedEdges.pop_back();
        // check if remove last route edge button has to be disabled
        if (mySelectedEdges.size() == 1) {
            // avoid remove last edge
            myRemoveLastInsertedEdge->disable();
            // Routes with only one edge are allowed
            myTemporalRoute.clear();
            myTemporalRoute.push_back(mySelectedEdges.front());
        } else {
            // calculate temporal route
            myTemporalRoute = myRouteFrameParent->myViewNet->getNet()->getPathCalculator()->calculatePath(myRouteFrameParent->myRouteModeSelector->getCurrentVehicleClass(), mySelectedEdges);
        }
        // update info route label
        updateInfoRouteLabel();
        // update view
        myRouteFrameParent->myViewNet->updateViewNet();
        return true;
    } else {
        return false;
    }
}

void
GNERouteFrame::NonConsecutiveEdges::updateInfoRouteLabel() {
    if (myTemporalRoute.size() > 0) {
        // declare variables for route info
        double length = 0;
        double speed = 0;
        for (const auto& i : myTemporalRoute) {
            length += i->getNBEdge()->getLength();
            speed += i->getNBEdge()->getSpeed();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << "- Number of Edges: " << toString(myTemporalRoute.size()) << "\n"
                << "- Length: " << toString(length) << "\n"
                << "- Average speed: " << toString(speed / myTemporalRoute.size());
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText("No edges selected");
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
    myConsecutiveEdges = new ConsecutiveEdges(this);

    // create non consecutive edges modul
    myNonConsecutiveEdges = new NonConsecutiveEdges(this);

    // set RouteMode::NONCONSECUTIVE_EDGES as default mode
    myRouteModeSelector->setCurrentRouteMode(RouteMode::NONCONSECUTIVE_EDGES);
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
GNERouteFrame::handleEdgeClick(GNEEdge* clickedEdge) {
    // first check if current vClass is valid and edge exist
    if (myRouteModeSelector->isValidVehicleClass() && clickedEdge) {
        // continue dependig of current mode
        switch (myRouteModeSelector->getCurrentRouteMode()) {
            case RouteMode::CONSECUTIVE_EDGES:
                // check if edge can be inserted in consecutive edges modul modul
                if (myConsecutiveEdges->addEdge(clickedEdge)) {
                    WRITE_DEBUG("Edge added in ConsecutiveEdges mode");
                } else {
                    WRITE_DEBUG("Edge wasn't added in ConsecutiveEdges mode");
                }
                break;
            case RouteMode::NONCONSECUTIVE_EDGES:
                // check if edge can be inserted in non consecutive edges modul modul
                if (myNonConsecutiveEdges->addEdge(clickedEdge)) {
                    WRITE_DEBUG("Edge added in NonConsecutiveEdges mode");
                } else {
                    WRITE_DEBUG("Edge wasn't added in NonConsecutiveEdges mode");
                }
                break;
            default:
                break;
        }
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEnter() {
    // first check if current vClass is valid
    if (myRouteModeSelector->isValidVehicleClass()) {
        // continue dependig of current mode
        switch (myRouteModeSelector->getCurrentRouteMode()) {
            case RouteMode::CONSECUTIVE_EDGES:
                myConsecutiveEdges->onCmdCreateRoute(0, 0, 0);
                break;
            case RouteMode::NONCONSECUTIVE_EDGES:
                myNonConsecutiveEdges->onCmdCreateRoute(0, 0, 0);
                break;
            default:
                break;
        }
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyBackSpace() {
    // first check if current vClass is valid
    if (myRouteModeSelector->isValidVehicleClass()) {
        // continue dependig of current mode
        switch (myRouteModeSelector->getCurrentRouteMode()) {
            case RouteMode::CONSECUTIVE_EDGES:
                myConsecutiveEdges->onCmdRemoveLastRouteEdge(0, 0, 0);
                break;
            case RouteMode::NONCONSECUTIVE_EDGES:
                myNonConsecutiveEdges->onCmdRemoveLastRouteEdge(0, 0, 0);
                break;
            default:
                break;
        }
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::hotkeyEsc() {
    // first check if current vClass is valid
    if (myRouteModeSelector->isValidVehicleClass()) {
        // continue dependig of current mode
        switch (myRouteModeSelector->getCurrentRouteMode()) {
            case RouteMode::CONSECUTIVE_EDGES:
                myConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
                break;
            case RouteMode::NONCONSECUTIVE_EDGES:
                myNonConsecutiveEdges->onCmdAbortRoute(0, 0, 0);
                break;
            default:
                break;
        }
        // update view
        myViewNet->updateViewNet();
    }
}


void
GNERouteFrame::drawTemporalRoute() const {
    // declare a vector with temporal route edges
    std::vector<GNEEdge*> temporalRoute;
    // obtain temporal route depending of current route mode
    switch (myRouteModeSelector->getCurrentRouteMode()) {
        case RouteMode::CONSECUTIVE_EDGES:
            // convert GNEEdges to NBEdges
            temporalRoute.reserve(myConsecutiveEdges->getRouteEdges().size());
            for (const auto& i : myConsecutiveEdges->getRouteEdges()) {
                temporalRoute.push_back(i);
            }
            break;
        case RouteMode::NONCONSECUTIVE_EDGES:
            temporalRoute = myNonConsecutiveEdges->getTemporalRoute();
            break;
        default:
            break;
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


/****************************************************************************/
