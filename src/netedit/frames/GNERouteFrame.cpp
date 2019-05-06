/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/demandelements/GNERouteHandler.h>
#include <netedit/demandelements/GNERoute.h>
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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode),
};

FXDEFMAP(GNERouteFrame::ConsecutiveEdges) ConsecutiveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_HOTKEY_ENTER,    GNERouteFrame::ConsecutiveEdges::onCmdCreateRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_HOTKEY_ESC,      GNERouteFrame::ConsecutiveEdges::onCmdAbortCreateRoute)
};

FXDEFMAP(GNERouteFrame::NonConsecutiveEdges) NonConsecutiveEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_ABORT,          GNERouteFrame::NonConsecutiveEdges::onCmdAbortRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_FINISHCREATION, GNERouteFrame::NonConsecutiveEdges::onCmdFinishRouteCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GNERouteFrame::NonConsecutiveEdges::onCmdRemoveLastRouteEdge)
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
    myCurrentRouteMode(ROUTEMODE_CONSECUTIVE_EDGES) {
    // first fill myRouteModesStrings
    myRouteModesStrings.push_back(std::make_pair(ROUTEMODE_CONSECUTIVE_EDGES, "consecutive edges"));
    myRouteModesStrings.push_back(std::make_pair(ROUTEMODE_NONCONSECUTIVE_EDGES, "non consecutive edges"));
    // Create FXComboBox
    myTypeMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // fill myTypeMatchBox with route modes
    for (const auto& i : myRouteModesStrings) {
        myTypeMatchBox->appendItem(i.second.c_str());
    }
    // Set visible items
    myTypeMatchBox->setNumVisible((int)myTypeMatchBox->getNumItems());
    // RouteModeSelector is always shown
    show();
}


GNERouteFrame::RouteModeSelector::~RouteModeSelector() {}


const GNERouteFrame::RouteMode&
GNERouteFrame::RouteModeSelector::getCurrenRouteMode() const {
    return myCurrentRouteMode;
}


void
GNERouteFrame::RouteModeSelector::setCurrentRouteMode(RouteMode routemode) {
    // make sure that route isn't invalid
    if (routemode != ROUTEMODE_INVALID) {
        // restore color
        myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
        // set current route mode
        myCurrentRouteMode = routemode;
        // set item in myTypeMatchBox
        for (int i = 0; i < (int)myRouteModesStrings.size(); i++) {
            if (myRouteModesStrings.at(i).first == myCurrentRouteMode) {
                myTypeMatchBox->setCurrentItem(i);
            }
        }
        // show moduls
        if (routemode == ROUTEMODE_CONSECUTIVE_EDGES) {
            myRouteFrameParent->myConsecutiveEdges->showConsecutiveEdgesModul();
            myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
        } else if (routemode == ROUTEMODE_NONCONSECUTIVE_EDGES) {
            myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
            myRouteFrameParent->myNonConsecutiveEdges->showNonConsecutiveEdgesModul();
        }

    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
        myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
    }
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode(FXObject*, FXSelector, void*) {
    // first abort all current operations in moduls
    myRouteFrameParent->myConsecutiveEdges->abortRouteCreation();
    myRouteFrameParent->myNonConsecutiveEdges->onCmdAbortRouteCreation(0,0,0);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myRouteModesStrings) {
        if (i.second == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentRouteMode = i.first;
            // enable moduls
            if (myCurrentRouteMode == ROUTEMODE_CONSECUTIVE_EDGES) {
                myRouteFrameParent->myConsecutiveEdges->showConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
            } else if (myCurrentRouteMode == ROUTEMODE_NONCONSECUTIVE_EDGES) {
                myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
                myRouteFrameParent->myNonConsecutiveEdges->showNonConsecutiveEdgesModul();
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in RouteModeSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentRouteMode = ROUTEMODE_INVALID;
    // hide all moduls if route mode isnt' valid
    myRouteFrameParent->myConsecutiveEdges->hideConsecutiveEdgesModul();
    myRouteFrameParent->myNonConsecutiveEdges->hideNonConsecutiveEdgesModul();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in RouteModeSelector");
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
    myCreateRouteButton = new FXButton(this, "Create route", 0, this, MID_GNE_HOTKEY_ENTER, GUIDesignButton);
    myCreateRouteButton->disable();
    // Create button for create routes
    myAbortCreationButton = new FXButton(this, "Abort creation", 0, this, MID_GNE_HOTKEY_ESC, GUIDesignButton);
    myAbortCreationButton->disable();
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
    abortRouteCreation();
    // now hide modul
    hide();
}


bool
GNERouteFrame::ConsecutiveEdges::addEdgeIntoRoute(GNEEdge* edge) {
    // check if currently we're creating a new route
    if (myRouteEdges.empty()) {
        // block undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
        // add edge into list
        myRouteEdges.push_back(edge);
        // set selected color in all edges
        for (const auto& j : edge->getLanes()) {
            j->setSpecialColor(&myRouteFrameParent->getEdgeCandidateColor());
        }
        // enable buttons
        myCreateRouteButton->enable();
        myAbortCreationButton->enable();
        // Change colors of candidate edges
        for (const auto& i : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            if (i != edge) {
                // set color in every lane
                for (const auto& j : i->getLanes()) {
                    j->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
                }
            }
        }
        // update route label
        updateInfoRouteLabel();
        // edge added, then return true
        return true;
    } else {
        // check if clicked edge is in the candidate edges
        for (const auto& i : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
            if (i == edge) {
                // restore colors of outgoing edges
                for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
                    for (const auto& k : j->getLanes()) {
                        k->setSpecialColor(nullptr);
                    }
                }
                // add new edge in the list of route edges
                myRouteEdges.push_back(edge);
                // set selected color in all edges
                for (const auto& j : myRouteEdges) {
                    for (const auto& k : j->getLanes()) {
                        k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateColor());
                    }
                }
                // set new candidate colors
                for (const auto& j : myRouteEdges.back()->getGNEJunctionDestiny()->getGNEOutgoingEdges()) {
                    if (j != edge) {
                        for (const auto& k : j->getLanes()) {
                            k->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
                        }
                    }
                }
                // update route label
                updateInfoRouteLabel();
                // edge added, then return true
                return true;
            }
        }
        // edge isn't a candidate edge, then return false
        return false;
    }
}


void
GNERouteFrame::ConsecutiveEdges::createRoute() {
    // create edge only if there is route edges
    if (myRouteEdges.size() > 0) {
        // cenerate Route ID
        std::string routeID = myRouteFrameParent->getViewNet()->getNet()->generateDemandElementID(SUMO_TAG_ROUTE);
        // create route
        GNERoute* route = new GNERoute(myRouteFrameParent->getViewNet(), routeID, myRouteEdges, RGBColor::BLUE);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myRouteFrameParent->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
        myRouteFrameParent->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myRouteFrameParent->getViewNet()->getUndoList()->p_end();
        // abort route creation (because route it was already seleted and vector/colors has to be cleaned)
        abortRouteCreation();
    }
}


void
GNERouteFrame::ConsecutiveEdges::abortRouteCreation() {
    // first check that there is route edges selected
    if (myRouteEdges.size() > 0) {
        // unblock undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
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
        // disable buttons
        myCreateRouteButton->disable();
        myAbortCreationButton->disable();
        // update route label
        updateInfoRouteLabel();
        // update view
        myRouteFrameParent->getViewNet()->update();
    }
}

long
GNERouteFrame::ConsecutiveEdges::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    // create route
    createRoute();
    return 1;
}


long
GNERouteFrame::ConsecutiveEdges::onCmdAbortCreateRoute(FXObject*, FXSelector, void*) {
    // abort route creation
    abortRouteCreation();
    return 1;
}


void
GNERouteFrame::ConsecutiveEdges::updateInfoRouteLabel() {
    if (myRouteEdges.size() > 0) {
        // declare variables for route info
        double lenght = 0;
        double speed = 0;
        for (const auto& i : myRouteEdges) {
            lenght += i->getNBEdge()->getLength();
            speed += i->getNBEdge()->getSpeed();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << "- Number of Edges: " << toString(myRouteEdges.size()) << "\n"
                << "- Lenght: " << toString(lenght) << "\n"
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
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_VEHICLEFRAME_FINISHCREATION, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_VEHICLEFRAME_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted edge
    myRemoveLastInsertedEdge = new FXButton(this, "Remove last inserted edge", nullptr, this, MID_GNE_VEHICLEFRAME_REMOVELASTEDGE, GUIDesignButton);
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
        // disable undo/redo
        myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->disableUndoRedo("trip creation");
        // set special color
        for (auto i : edge->getLanes()) {
            i->setSpecialColor(&myRouteFrameParent->getEdgeCandidateSelectedColor());
        }
        // calculate route if there is more than two edges
        if (mySelectedEdges.size() > 1) {
            // enable remove last edge button
            myRemoveLastInsertedEdge->enable();
            // enable finish button
            myFinishCreationButton->enable();
            // calculate temporal route
            myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PASSENGER, mySelectedEdges);         
            // update info route label
            updateInfoRouteLabel();
        }
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
    // enable undo/redo
    myRouteFrameParent->myViewNet->getViewParent()->getGNEAppWindows()->enableUndoRedo();
}


void
GNERouteFrame::NonConsecutiveEdges::drawTemporalRoute() const {
    // only draw if there is at least two edges
    if (myTemporalRoute.size() > 1) {
        // Add a draw matrix
        glPushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_MAX);
        // set orange color
        GLHelper::setColor(RGBColor::ORANGE);
        // set line width
        glLineWidth(5);
        // draw first line
        GLHelper::drawLine(myTemporalRoute.at(0)->getLanes().front().shape.front(),
                           myTemporalRoute.at(0)->getLanes().front().shape.back());
        // draw rest of lines
        for (int i = 1; i < (int)myTemporalRoute.size(); i++) {
            GLHelper::drawLine(myTemporalRoute.at(i - 1)->getLanes().front().shape.back(),
                               myTemporalRoute.at(i)->getLanes().front().shape.front());
            GLHelper::drawLine(myTemporalRoute.at(i)->getLanes().front().shape.front(),
                               myTemporalRoute.at(i)->getLanes().front().shape.back());
        }
        // Pop last matrix
        glPopMatrix();
    }
}


bool
GNERouteFrame::NonConsecutiveEdges::isValid(SUMOVehicleClass /* vehicleClass */) const {
    return mySelectedEdges.size() > 0;
}


long
GNERouteFrame::NonConsecutiveEdges::onCmdAbortRouteCreation(FXObject*, FXSelector, void*) {
    clearEdges();
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedEdge->disable();
    // update info route label
    updateInfoRouteLabel();
    return 1;
}


long
GNERouteFrame::NonConsecutiveEdges::onCmdFinishRouteCreation(FXObject*, FXSelector, void*) {
     // create edge only if there is route edges
    if (mySelectedEdges.size() > 1) {
        // obtain GNERoutes
        std::vector<GNEEdge*> routeEdges;
        routeEdges.reserve(myTemporalRoute.size());
        for (const auto &i : myTemporalRoute) {
            routeEdges.push_back(myRouteFrameParent->myViewNet->getNet()->retrieveEdge(i->getID()));
        }
        // generate Route ID
        std::string routeID = myRouteFrameParent->getViewNet()->getNet()->generateDemandElementID(SUMO_TAG_ROUTE);
        // create route
        GNERoute* route = new GNERoute(myRouteFrameParent->getViewNet(), routeID, routeEdges, RGBColor::BLUE);
        // add it into GNENet using GNEChange_DemandElement (to allow undo-redo)
        myRouteFrameParent->getViewNet()->getUndoList()->p_begin("add " + route->getTagStr());
        myRouteFrameParent->getViewNet()->getUndoList()->add(new GNEChange_DemandElement(route, true), true);
        myRouteFrameParent->getViewNet()->getUndoList()->p_end();
        // clear edges
        clearEdges();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedEdge->disable();
        // update info route label
        updateInfoRouteLabel();
    }
    return 1;
}


long
GNERouteFrame::NonConsecutiveEdges::onCmdRemoveLastRouteEdge(FXObject*, FXSelector, void*) {
    if (mySelectedEdges.size() > 1) {
        // remove last edge
        mySelectedEdges.pop_back();
        // calculate temporal route
        myTemporalRoute = GNEDemandElement::getRouteCalculatorInstance()->calculateDijkstraRoute(SVC_PASSENGER, mySelectedEdges);
    }
    return 1;
}

void
GNERouteFrame::NonConsecutiveEdges::updateInfoRouteLabel() {
    if (myTemporalRoute.size() > 0) {
        // declare variables for route info
        double lenght = 0;
        double speed = 0;
        for (const auto& i : myTemporalRoute) {
            lenght += i->getLength();
            speed += i->getSpeed();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << "- Number of Edges: " << toString(myTemporalRoute.size()) << "\n"
                << "- Lenght: " << toString(lenght) << "\n"
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

    // create consecutive edges modul
    myConsecutiveEdges = new ConsecutiveEdges(this);

    // create non consecutive edges modul
    myNonConsecutiveEdges = new NonConsecutiveEdges(this);

    // set ROUTEMODE_CONSECUTIVE_EDGES as default mode
    myRouteModeSelector->setCurrentRouteMode(ROUTEMODE_CONSECUTIVE_EDGES);
}


GNERouteFrame::~GNERouteFrame() {}


void
GNERouteFrame::show() {

    GNEFrame::show();
}


void
GNERouteFrame::hide() {

    GNEFrame::hide();
}


void
GNERouteFrame::handleEdgeClick(GNEEdge* clickedEdge) {
    // make sure that Edge exist
    if (clickedEdge) {
        switch (myRouteModeSelector->getCurrenRouteMode()) {
            case ROUTEMODE_CONSECUTIVE_EDGES:
                // check if edge can be inserted in consecutive edges modul modul
                if (myConsecutiveEdges->addEdgeIntoRoute(clickedEdge)) {
                    WRITE_DEBUG("Edge added in ConsecutiveEdges mode");
                } else {
                    WRITE_DEBUG("Edge wasn't added in ConsecutiveEdges mode");
                }
                break;
            case ROUTEMODE_NONCONSECUTIVE_EDGES:
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
    }
}


void
GNERouteFrame::hotKeyEnter() {
    switch (myRouteModeSelector->getCurrenRouteMode()) {
        case ROUTEMODE_CONSECUTIVE_EDGES:
            myConsecutiveEdges->createRoute();
            break;
        case ROUTEMODE_NONCONSECUTIVE_EDGES:
            myNonConsecutiveEdges->onCmdFinishRouteCreation(0,0,0);
            break;
        default:
            break;
    }
}


void
GNERouteFrame::hotKeyEsc() {
    switch (myRouteModeSelector->getCurrenRouteMode()) {
        case ROUTEMODE_CONSECUTIVE_EDGES:
            myConsecutiveEdges->abortRouteCreation();
            break;
        case ROUTEMODE_NONCONSECUTIVE_EDGES:
            myNonConsecutiveEdges->onCmdAbortRouteCreation(0,0,0);
            break;
        default:
            break;
    }
}


GNERouteFrame::NonConsecutiveEdges* 
GNERouteFrame::getNonConsecutiveEdges() const {
    return myNonConsecutiveEdges;
}

/****************************************************************************/
