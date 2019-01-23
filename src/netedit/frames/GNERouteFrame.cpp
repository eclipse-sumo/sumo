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
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/demandelements/GNEDemandHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>

#include "GNERouteFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERouteFrame::RouteModeSelector) RouteModeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TYPE,    GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode),
};

FXDEFMAP(GNERouteFrame::EdgeToEdge) EdgeToEdgeMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_HOTKEY_ENTER,    GNERouteFrame::EdgeToEdge::onCmdCreateRoute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_HOTKEY_ESC,      GNERouteFrame::EdgeToEdge::onCmdAbortCreateRoute),
};

// Object implementation
FXIMPLEMENT(GNERouteFrame::RouteModeSelector,   FXGroupBox,     RouteModeSelectorMap,   ARRAYNUMBER(RouteModeSelectorMap))
FXIMPLEMENT(GNERouteFrame::EdgeToEdge,          FXGroupBox,     EdgeToEdgeMap,          ARRAYNUMBER(EdgeToEdgeMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERouteFrame::RouteModeSelector - methods
// ---------------------------------------------------------------------------

GNERouteFrame::RouteModeSelector::RouteModeSelector(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Route mode", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent),
    myCurrentRouteMode(ROUTEMODE_EDGETOEDGE) {
    // first fill myRouteModesStrings
    myRouteModesStrings.push_back(std::make_pair(ROUTEMODE_EDGETOEDGE, "edge to edge"));
    myRouteModesStrings.push_back(std::make_pair(ROUTEMODE_MAXVELOCITY, "maximum velocity"));
    myRouteModesStrings.push_back(std::make_pair(ROUTEMODE_MINIMUMLENGHT, "minimum lenght"));
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
        // enable moduls
        myRouteFrameParent->myEdgeToEdge->showEdgeToEdgeModul();
    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myEdgeToEdge->hideEdgeToEdgeModul();
    }
}


long
GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode(FXObject*, FXSelector, void*) {
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& i : myRouteModesStrings) {
        if (i.second == myTypeMatchBox->getText().text()) {
            // set color of myTypeMatchBox to black (valid)
            myTypeMatchBox->setTextColor(FXRGB(0, 0, 0));
            // Set new current type
            myCurrentRouteMode = i.first;
            // enable moduls
            myRouteFrameParent->myEdgeToEdge->showEdgeToEdgeModul();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myTypeMatchBox->getText() + "' in RouteModeSelector").text());
            return 1;
        }
    }
    // if additional name isn't correct, set SUMO_TAG_NOTHING as current type
    myCurrentRouteMode = ROUTEMODE_INVALID;
    // hide all moduls if route mode isnt' valid
    myRouteFrameParent->myEdgeToEdge->hideEdgeToEdgeModul();
    // set color of myTypeMatchBox to red (invalid)
    myTypeMatchBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in RouteModeSelector");
    return 1;
}


// ---------------------------------------------------------------------------
// GNERouteFrame::EdgeToEdge - methods
// ---------------------------------------------------------------------------

GNERouteFrame::EdgeToEdge::EdgeToEdge(GNERouteFrame* routeFrameParent) :
    FXGroupBox(routeFrameParent->myContentFrame, "Edge to edge", GUIDesignGroupBoxFrame),
    myRouteFrameParent(routeFrameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(this, "No edges selected", 0, GUIDesignLabelFrameInformation);
    // Create button for create routes
    myCreateRouteButton = new FXButton(this, "Create route", 0, this, MID_GNE_HOTKEY_ENTER, GUIDesignButton);
    myCreateRouteButton->disable();
    // Create button for create routes
    myAbortCreationButton = new FXButton(this, "Abort creation", 0, this, MID_GNE_HOTKEY_ESC, GUIDesignButton);
    myAbortCreationButton->disable();
    // EdgeToEdge is by default shown
    show();
}


GNERouteFrame::EdgeToEdge::~EdgeToEdge() {}


void
GNERouteFrame::EdgeToEdge::showEdgeToEdgeModul() {
    // only show modul if current route mode is valid
    if (myRouteFrameParent->myRouteModeSelector->getCurrenRouteMode() == ROUTEMODE_EDGETOEDGE) {
        show();
    } else {
        hide();
    }
}


void
GNERouteFrame::EdgeToEdge::hideEdgeToEdgeModul() {
    // first abort route creation
    abortRouteCreation();
    // now hide modul
    hide();
}


bool
GNERouteFrame::EdgeToEdge::addEdgeIntoRoute(GNEEdge* edge) {
    // check if currently we're creating a new route
    if (myRouteEdges.empty()) {
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
GNERouteFrame::EdgeToEdge::createRoute() {
    // create edge only if there is route edges
    if (myRouteEdges.size() > 0) {
        // creaste edge using buildRoute function of GNEDemandHandler
        GNEDemandHandler::buildRoute(myRouteFrameParent->getViewNet(), true, myRouteFrameParent->getViewNet()->getNet()->generateDemandElementID(SUMO_TAG_ROUTE), myRouteEdges, RGBColor::BLUE);
        // abort route creation (because route it was already seleted and vector/colors has to be cleaned)
        abortRouteCreation();
    }
}


void
GNERouteFrame::EdgeToEdge::abortRouteCreation() {
    // first check that there is route edges selected
    if (myRouteEdges.size() > 0) {
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
GNERouteFrame::EdgeToEdge::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    // create route
    createRoute();
    return 1;
}


long
GNERouteFrame::EdgeToEdge::onCmdAbortCreateRoute(FXObject*, FXSelector, void*) {
    // abort route creation
    abortRouteCreation();
    return 1;
}


void
GNERouteFrame::EdgeToEdge::updateInfoRouteLabel() {
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
// GNERouteFrame - methods
// ---------------------------------------------------------------------------

GNERouteFrame::GNERouteFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Routes") {

    // create route mode Selector modul
    myRouteModeSelector = new RouteModeSelector(this);

    // create Edge To Edge modul
    myEdgeToEdge = new EdgeToEdge(this);

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
            case ROUTEMODE_EDGETOEDGE:
                // check if edge can be inserted in edge to edge modul
                if (myEdgeToEdge->addEdgeIntoRoute(clickedEdge)) {
                    WRITE_DEBUG("Edge added in EdgeToEdge mode");
                } else {
                    WRITE_DEBUG("Edge wasn't added in EdgeToEdge mode");
                }
                break;
            case ROUTEMODE_MAXVELOCITY:
                break;
            case ROUTEMODE_MINIMUMLENGHT:
                break;
            default:
                break;
        }
    }
}


void
GNERouteFrame::hotKeyEnter() {
    myEdgeToEdge->createRoute();
}


void
GNERouteFrame::hotKeyEsc() {
    myEdgeToEdge->abortRouteCreation();
}

/****************************************************************************/
