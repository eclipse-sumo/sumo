/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
    for (const auto &i : myRouteModesStrings) {
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
    if(routemode != ROUTEMODE_INVALID) {
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
    for (const auto &i : myRouteModesStrings) {
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
    // EdgeToEdge is always shown
    show();
}


GNERouteFrame::EdgeToEdge::~EdgeToEdge() {}


void 
GNERouteFrame::EdgeToEdge::showEdgeToEdgeModul() {
    // only show modul if current route mode is valid
    if(myRouteFrameParent->myRouteModeSelector->getCurrenRouteMode() == ROUTEMODE_EDGETOEDGE) {
        show();
    } else {
        hide();
    }
}


void 
GNERouteFrame::EdgeToEdge::hideEdgeToEdgeModul() {
    hide();
}


void 
GNERouteFrame::EdgeToEdge::addEdgeIntoRoute(GNEEdge* edge) {
    
}


long
GNERouteFrame::EdgeToEdge::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNERouteFrame::EdgeToEdge::onCmdAbortCreateRoute(FXObject*, FXSelector, void*) {
    return 1;
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
    if(clickedEdge) {
        switch (myRouteModeSelector->getCurrenRouteMode())
        {
        case ROUTEMODE_EDGETOEDGE:
            myEdgeToEdge->addEdgeIntoRoute(clickedEdge);
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

/****************************************************************************/
