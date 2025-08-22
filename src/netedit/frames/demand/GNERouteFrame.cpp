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
/// @file    GNERouteFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for remove network-elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEPathCreator.h>
#include <netedit/frames/GNEPathLegendModule.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/MsgHandler.h>

#include "GNERouteFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERouteFrame::RouteModeSelector) RouteModeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ROUTEFRAME_ROUTEMODE,    GNERouteFrame::RouteModeSelector::onCmdSelectRouteMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ROUTEFRAME_VCLASS,       GNERouteFrame::RouteModeSelector::onCmdSelectVClass),
};

// Object implementation
FXIMPLEMENT(GNERouteFrame::RouteModeSelector,   MFXGroupBoxModule,     RouteModeSelectorMap,   ARRAYNUMBER(RouteModeSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERouteFrame::RouteModeSelector - methods
// ---------------------------------------------------------------------------

GNERouteFrame::RouteModeSelector::RouteModeSelector(GNERouteFrame* routeFrameParent) :
    MFXGroupBoxModule(routeFrameParent, TL("Route mode")),
    myRouteFrameParent(routeFrameParent) {
    // first fill myRouteModesStrings
    myRouteModesStrings.push_back(std::make_pair(RouteMode::NONCONSECUTIVE_EDGES, TL("non consecutive edges")));
    myRouteModesStrings.push_back(std::make_pair(RouteMode::CONSECUTIVE_EDGES, TL("consecutive edges")));
    // Create MFXComboBoxIcon for Route mode
    myRouteModeMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_ROUTEFRAME_ROUTEMODE, GUIDesignComboBox);
    // fill myRouteModeMatchBox with route modes
    for (const auto& routeMode : myRouteModesStrings) {
        myRouteModeMatchBox->appendIconItem(routeMode.second.c_str());
    }
    // Create MFXComboBoxIcon for VClass
    myVClassMatchBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, false, GUIDesignComboBoxVisibleItems,
                                           this, MID_GNE_ROUTEFRAME_VCLASS, GUIDesignComboBox);
    // fill myVClassMatchBox with all VCLass
    for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
        myVClassMatchBox->appendIconItem(vClass.c_str());
    }
    // set Passenger als default VCLass
    myVClassMatchBox->setCurrentItem(7);
    // RouteModeSelector is always shown
    show();
}


GNERouteFrame::RouteModeSelector::~RouteModeSelector() {
}


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
    const auto routeTemplate = myRouteFrameParent->getViewNet()->getNet()->getACTemplates()->getTemplateAC(SUMO_TAG_ROUTE);
    // check if current mode is valid
    if ((myCurrentRouteMode != RouteMode::INVALID) && myValidVClass) {
        // check if create routes consecutively
        const bool consecutiveEdges = (myCurrentRouteMode == RouteMode::CONSECUTIVE_EDGES);
        // show route attributes modul
        myRouteFrameParent->myRouteAttributesEditor->showAttributesEditor(routeTemplate, true);
        // show path creator
        myRouteFrameParent->myPathCreator->showPathCreatorModule(routeTemplate->getTagProperty(), consecutiveEdges);
        // update edge colors
        myRouteFrameParent->myPathCreator->updateEdgeColors();
        // show legend
        myRouteFrameParent->myPathLegend->showPathLegendModule();
    } else {
        // hide all moduls if route mode isnt' valid
        myRouteFrameParent->myRouteAttributesEditor->hideAttributesEditor();
        myRouteFrameParent->myPathCreator->hidePathCreatorModule();
        myRouteFrameParent->myPathLegend->hidePathLegendModule();
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
    myRouteModeMatchBox->setTextColor(GUIDesignTextColorRed);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& routeMode : myRouteModesStrings) {
        if (routeMode.second == myRouteModeMatchBox->getText().text()) {
            // Set new current type
            myCurrentRouteMode = routeMode.first;
            // set color of myTypeMatchBox to black (valid)
            myRouteModeMatchBox->setTextColor(GUIDesignTextColorBlack);
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
    myVClassMatchBox->setTextColor(GUIDesignTextColorRed);
    // Check if value of myTypeMatchBox correspond of an allowed additional tags
    for (const auto& vClass : SumoVehicleClassStrings.getStrings()) {
        if (vClass == myVClassMatchBox->getText().text()) {
            // change flag
            myValidVClass = true;
            // set color of myTypeMatchBox to black (valid)
            myVClassMatchBox->setTextColor(GUIDesignTextColorBlack);
            // set vClass in Path creator
            myRouteFrameParent->myPathCreator->setVClass(SumoVehicleClassStrings.get(vClass));
        }
    }
    // check if parameters are valid
    areParametersValid();
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteFrame - methods
// ---------------------------------------------------------------------------

GNERouteFrame::GNERouteFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Routes")),
    myRouteBaseObject(new CommonXMLStructure::SumoBaseObject(nullptr)) {

    // create route mode Selector module
    myRouteModeSelector = new RouteModeSelector(this);

    // Create route parameters
    myRouteAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // create consecutive edges module
    myPathCreator = new GNEPathCreator(this, viewNet->getNet()->getDemandPathManager());

    // create legend label
    myPathLegend = new GNEPathLegendModule(this);
}


GNERouteFrame::~GNERouteFrame() {
    delete myRouteBaseObject;
}


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
        edge.second->resetCandidateFlags();
    }
    GNEFrame::hide();
}


bool
GNERouteFrame::addEdgeRoute(GNEEdge* clickedEdge, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed) {
    // first check if current vClass and mode are valid and edge exist
    if (clickedEdge && myRouteModeSelector->isValidVehicleClass() && myRouteModeSelector->isValidMode()) {
        // add edge in path
        myPathCreator->addEdge(clickedEdge, mouseButtonKeyPressed.shiftKeyPressed(), mouseButtonKeyPressed.controlKeyPressed());
        // update view
        myViewNet->updateViewNet();
        return true;
    } else {
        return false;
    }
}


GNEPathCreator*
GNERouteFrame::getPathCreator() const {
    return myPathCreator;
}


bool
GNERouteFrame::createPath(const bool /*useLastRoute*/) {
    // check that route attributes are valid
    if (!myRouteAttributesEditor->checkAttributes(true)) {
        return false;
    } else if (myPathCreator->getSelectedEdges().size() > 0) {
        // clear base object
        myRouteBaseObject->clear();
        // set tag
        myRouteBaseObject->setTag(SUMO_TAG_ROUTE);
        // obtain attributes
        myRouteAttributesEditor->fillSumoBaseObject(myRouteBaseObject);
        // add probability (needed for distributions)
        myRouteBaseObject->addDoubleAttribute(SUMO_ATTR_PROB, 1.0);
        // declare edge vector
        std::vector<std::string> edges;
        for (const auto& path : myPathCreator->getPath()) {
            for (const auto& edgeID : path.getSubPath()) {
                // get edge
                GNEEdge* edge = myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(edgeID->getID());
                // avoid double edges
                if (edges.empty() || (edges.back() != edge->getID())) {
                    edges.push_back(edge->getID());
                }
            }
        }
        // set edges in route base object
        myRouteBaseObject->addStringListAttribute(SUMO_ATTR_EDGES, edges);
        // declare route handler
        GNERouteHandler routeHandler(myViewNet->getNet(), myRouteBaseObject->hasStringAttribute(GNE_ATTR_DEMAND_FILE) ?
                                     myRouteBaseObject->getStringAttribute(GNE_ATTR_DEMAND_FILE) : "",
                                     myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed(), false);
        // create route
        routeHandler.parseSumoBaseObject(myRouteBaseObject);
        // abort path creation
        myPathCreator->abortPathCreation();
        // refresh route attributes
        myRouteAttributesEditor->refreshAttributesEditor();
        // get new route
        auto newRoute = myViewNet->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE, myRouteBaseObject->getStringAttribute(SUMO_ATTR_ID));
        // compute path route
        newRoute->computePathElement();
        // set as last created route
        myViewNet->setLastCreatedRoute(newRoute);
        return true;
    }
    return false;
}

/****************************************************************************/
