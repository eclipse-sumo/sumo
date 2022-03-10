/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNECommonNetworkModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNELane2laneConnection.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>

#include "GNECommonNetworkModules.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECommonNetworkModules::SelectorParentLanes) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_STOPSELECTION,  GNECommonNetworkModules::SelectorParentLanes::onCmdStopSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GNECommonNetworkModules::SelectorParentLanes::onCmdAbortSelection),
};

FXDEFMAP(GNECommonNetworkModules::SelectorChildEdges) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNECommonNetworkModules::SelectorChildEdges::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNECommonNetworkModules::SelectorChildEdges::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNECommonNetworkModules::SelectorChildEdges::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNECommonNetworkModules::SelectorChildEdges::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNECommonNetworkModules::SelectorChildEdges::onCmdSelectEdge),
};

FXDEFMAP(GNECommonNetworkModules::SelectorChildLanes) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNECommonNetworkModules::SelectorChildLanes::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNECommonNetworkModules::SelectorChildLanes::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNECommonNetworkModules::SelectorChildLanes::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNECommonNetworkModules::SelectorChildLanes::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNECommonNetworkModules::SelectorChildLanes::onCmdSelectLane),
};

FXDEFMAP(GNECommonNetworkModules::E2MultilaneLaneSelector) E2MultilaneLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_ABORT,          GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_FINISH,         GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_REMOVELAST,     GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdRemoveLastElement),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_SHOWCANDIDATES, GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdShowCandidateLanes)
};

// Object implementation
FXIMPLEMENT(GNECommonNetworkModules::SelectorParentLanes,        FXGroupBoxModule,     ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNECommonNetworkModules::SelectorChildEdges,         FXGroupBoxModule,     SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNECommonNetworkModules::SelectorChildLanes,         FXGroupBoxModule,     SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))
FXIMPLEMENT(GNECommonNetworkModules::E2MultilaneLaneSelector,    FXGroupBoxModule,     E2MultilaneLaneSelectorMap,     ARRAYNUMBER(E2MultilaneLaneSelectorMap))


// ---------------------------------------------------------------------------
// GNECommonNetworkModules::SelectorParentLanes - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::SelectorParentLanes::SelectorParentLanes(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Lane Selector"),
    myFrameParent(frameParent) {
    // create start and stop buttons
    myStopSelectingButton = new FXButton(getCollapsableFrame(), "Stop selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_STOPSELECTION, GUIDesignButton);
    myAbortSelectingButton = new FXButton(getCollapsableFrame(), "Abort selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GUIDesignButton);
    // disable stop and abort functions as init
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
}


GNECommonNetworkModules::SelectorParentLanes::~SelectorParentLanes() {}


void
GNECommonNetworkModules::SelectorParentLanes::showSelectorParentLanesModule() {
    // abort current selection before show
    abortConsecutiveLaneSelector();
    // show FXGroupBoxModule
    FXGroupBoxModule::show();
}


void
GNECommonNetworkModules::SelectorParentLanes::hideSelectorParentLanesModule() {
    // abort current selection before hide
    abortConsecutiveLaneSelector();
    // hide FXGroupBoxModule
    FXGroupBoxModule::hide();
}


void
GNECommonNetworkModules::SelectorParentLanes::startConsecutiveLaneSelector(GNELane* lane, const Position& clickedPosition) {
    // Only start selection if SelectorParentLanes modul is shown
    if (shown()) {
        // change buttons
        myStopSelectingButton->enable();
        myAbortSelectingButton->enable();
        // add lane
        addSelectedLane(lane, clickedPosition);
    }
}


void
GNECommonNetworkModules::SelectorParentLanes::abortConsecutiveLaneSelector() {
    // reset color of all candidate lanes
    for (const auto& lane : myCandidateLanes) {
        lane->resetCandidateFlags();
    }
    // clear candidate colors
    myCandidateLanes.clear();
    // reset color of all selected lanes
    for (const auto& lane : mySelectedLanes) {
        lane.first->resetCandidateFlags();
    }
    // clear selected lanes
    mySelectedLanes.clear();
    // disable buttons
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
    // update view (due colors)
    myFrameParent->getViewNet()->updateViewNet();
}


bool
GNECommonNetworkModules::SelectorParentLanes::addSelectedLane(GNELane* lane, const Position& clickedPosition) {
    // first check that lane exist
    if (lane == nullptr) {
        return false;
    }
    // check that lane wasn't already selected
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            WRITE_WARNING("Duplicated lanes aren't allowed");
            return false;
        }
    }
    // check that there is candidate lanes
    if (mySelectedLanes.size() > 0) {
        if (myCandidateLanes.empty()) {
            WRITE_WARNING("Only candidate lanes are allowed");
            return false;
        } else if ((myCandidateLanes.size() > 0) && (std::find(myCandidateLanes.begin(), myCandidateLanes.end(), lane) == myCandidateLanes.end())) {
            WRITE_WARNING("Only consecutive lanes are allowed");
            return false;
        }
    }
    // select lane and save the clicked position
    mySelectedLanes.push_back(std::make_pair(lane, lane->getLaneShape().nearest_offset_to_point2D(clickedPosition) / lane->getLengthGeometryFactor()));
    // change color of selected lane
    lane->setTargetCandidate(true);
    // restore original color of candidates (except already selected)
    for (const auto& candidateLane : myCandidateLanes) {
        candidateLane->setPossibleCandidate(false);
    }
    // clear candidate lanes
    myCandidateLanes.clear();
    // fill candidate lanes
    for (const auto& connection : lane->getParentEdge()->getGNEConnections()) {
        // check that possible candidate lane isn't already selected
        if ((lane == connection->getLaneFrom()) && (!isLaneSelected(connection->getLaneTo()))) {
            // set candidate lane
            connection->getLaneTo()->setPossibleCandidate(true);
            myCandidateLanes.push_back(connection->getLaneTo());
        }
    }
    // update view (due colors)
    myFrameParent->getViewNet()->updateViewNet();
    return true;
}


void
GNECommonNetworkModules::SelectorParentLanes::removeLastSelectedLane() {
    if (mySelectedLanes.size() > 1) {
        mySelectedLanes.pop_back();
    } else {
        WRITE_WARNING("First lane cannot be removed");
    }
}


bool
GNECommonNetworkModules::SelectorParentLanes::isSelectingLanes() const {
    return myStopSelectingButton->isEnabled();
}


bool
GNECommonNetworkModules::SelectorParentLanes::isShown() const {
    return shown();
}


const std::vector<std::pair<GNELane*, double> >&
GNECommonNetworkModules::SelectorParentLanes::getSelectedLanes() const {
    return mySelectedLanes;
}


long
GNECommonNetworkModules::SelectorParentLanes::onCmdStopSelection(FXObject*, FXSelector, void*) {
    myFrameParent->stopConsecutiveLaneSelector();
    return 0;
}


long
GNECommonNetworkModules::SelectorParentLanes::onCmdAbortSelection(FXObject*, FXSelector, void*) {
    abortConsecutiveLaneSelector();
    return 0;
}


bool
GNECommonNetworkModules::SelectorParentLanes::isLaneSelected(GNELane* lane) const {
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNECommonNetworkModules::SelectorChildEdges - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::SelectorChildEdges::SelectorChildEdges(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Edges"),
    myFrameParent(frameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(getCollapsableFrame(), ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myEdgesSearch = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorChildEdgesModule();
}


GNECommonNetworkModules::SelectorChildEdges::~SelectorChildEdges() {}


std::vector<std::string>
GNECommonNetworkModules::SelectorChildEdges::getEdgeIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        // get Selected edges
        const auto selectedEdges = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
        // Iterate over selectedEdges and getId
        for (const auto& edge : selectedEdges) {
            vectorOfIds.push_back(edge->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return vectorOfIds;
}


void
GNECommonNetworkModules::SelectorChildEdges::showSelectorChildEdgesModule(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // iterate over edges of net
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        // If search criterium is correct, then append ittem
        if (edge.second->getID().find(search) != std::string::npos) {
            myList->appendItem(edge.second->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdgesCheckButton->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void
GNECommonNetworkModules::SelectorChildEdges::hideSelectorChildEdgesModule() {
    FXGroupBoxModule::hide();
}


void
GNECommonNetworkModules::SelectorChildEdges::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedEdges() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNECommonNetworkModules::SelectorChildEdges::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        myEdgesSearch->hide();
        myList->hide();
        myClearEdgesSelection->hide();
        myInvertEdgesSelection->hide();
    } else {
        myEdgesSearch->show();
        myList->show();
        myClearEdgesSelection->show();
        myInvertEdgesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNECommonNetworkModules::SelectorChildEdges::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorChildEdges that contains the searched string
    showSelectorChildEdgesModule(myEdgesSearch->getText().text());
    return 1;
}


long
GNECommonNetworkModules::SelectorChildEdges::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECommonNetworkModules::SelectorChildEdges::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNECommonNetworkModules::SelectorChildEdges::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNECommonNetworkModules::SelectorChildLanes - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::SelectorChildLanes::SelectorChildLanes(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Lanes"),
    myFrameParent(frameParent) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(getCollapsableFrame(), ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myLanesSearch = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorChildLanesModule();
}


GNECommonNetworkModules::SelectorChildLanes::~SelectorChildLanes() {}


std::vector<std::string>
GNECommonNetworkModules::SelectorChildLanes::getLaneIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedLanesCheckButton->getCheck()) {
        // get Selected lanes
        const auto selectedLanes = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedLanes();
        // Iterate over selectedLanes and getId
        for (const auto& lane : selectedLanes) {
            vectorOfIds.push_back(lane->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return vectorOfIds;
}


void
GNECommonNetworkModules::SelectorChildLanes::showSelectorChildLanesModule(std::string search) {
    myList->clearItems();
    // add all network lanes
    for (const auto& lane : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getLanes()) {
        if (lane->getID().find(search) != std::string::npos) {
            myList->appendItem(lane->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanesCheckButton->setCheck(false);
    // Show list
    show();
}


void
GNECommonNetworkModules::SelectorChildLanes::hideSelectorChildLanesModule() {
    FXGroupBoxModule::hide();
}


void
GNECommonNetworkModules::SelectorChildLanes::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedLanes() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNECommonNetworkModules::SelectorChildLanes::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanesCheckButton->getCheck()) {
        myLanesSearch->hide();
        myList->hide();
        clearLanesSelection->hide();
        invertLanesSelection->hide();
    } else {
        myLanesSearch->show();
        myList->show();
        clearLanesSelection->show();
        invertLanesSelection->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNECommonNetworkModules::SelectorChildLanes::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorChildLanes that contains the searched string
    showSelectorChildLanesModule(myLanesSearch->getText().text());
    return 1;
}


long
GNECommonNetworkModules::SelectorChildLanes::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECommonNetworkModules::SelectorChildLanes::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNECommonNetworkModules::SelectorChildLanes::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNECommonNetworkModules::E2MultilaneLaneSelector - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::E2MultilaneLaneSelector::E2MultilaneLaneSelector(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "E2Multilane lane selector"),
    myFrameParent(frameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(getCollapsableFrame(), "No lanes selected", 0, GUIDesignLabelFrameThicked);
    // create button for finish route creation
    myFinishCreationButton = new FXButton(getCollapsableFrame(), "Finish route creation", nullptr, this, MID_GNE_LANEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(getCollapsableFrame(), "Abort route creation", nullptr, this, MID_GNE_LANEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted lane
    myRemoveLastInsertedElement = new FXButton(getCollapsableFrame(), "Remove last inserted lane", nullptr, this, MID_GNE_LANEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create check button
    myShowCandidateLanes = new FXCheckButton(getCollapsableFrame(), "Show candidate lanes", this, MID_GNE_LANEPATH_SHOWCANDIDATES, GUIDesignCheckButton);
    myShowCandidateLanes->setCheck(TRUE);
    // create backspace label (always shown)
    new FXLabel(this,
                "BACKSPACE: undo click",
                0, GUIDesignLabelFrameInformation);
}


GNECommonNetworkModules::E2MultilaneLaneSelector::~E2MultilaneLaneSelector() {}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::showE2MultilaneLaneSelectorModule() {
    // first abort creation
    abortPathCreation();
    // disable buttons
    myFinishCreationButton->disable();
    myAbortCreationButton->disable();
    myRemoveLastInsertedElement->disable();
    // update lane colors
    updateLaneColors();
    // recalc before show (to avoid graphic problems)
    recalc();
    // show modul
    show();
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::hideE2MultilaneLaneSelectorModule() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


const std::vector<std::pair<GNELane*, double> >&
GNECommonNetworkModules::E2MultilaneLaneSelector::getLanePath() const {
    return myLanePath;
}


bool
GNECommonNetworkModules::E2MultilaneLaneSelector::addLane(GNELane* lane) {
    // first check if lane is valid
    if (lane == nullptr) {
        return false;
    }
    // continue depending of number of selected eges
    if ((myLanePath.size() > 0) && (myLanePath.back().first == lane)) {
        // Write warning
        WRITE_WARNING("Double lanes aren't allowed");
        // abort add lane
        return false;
    }
    // check candidate lane
    if ((myShowCandidateLanes->getCheck() == TRUE) && !lane->isPossibleCandidate()) {
        if (lane->isSpecialCandidate() || lane->isConflictedCandidate()) {
            // Write warning
            WRITE_WARNING("Invalid lane");
            // abort add lane
            return false;
        }
    }
    // get mouse position
    const Position mousePos = myFrameParent->getViewNet()->snapToActiveGrid(myFrameParent->getViewNet()->getPositionInformation());
    // calculate lane offset
    const double offset = lane->getLaneShape().nearest_offset_to_point2D(mousePos);
    // All checks ok, then add it in selected elements
    myLanePath.push_back(std::make_pair(lane, offset));
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
    // enable or disable remove last lane button
    if (myLanePath.size() > 1) {
        myRemoveLastInsertedElement->enable();
    } else {
        myRemoveLastInsertedElement->disable();
    }
    // update info route label
    updateInfoRouteLabel();
    // update lane colors
    updateLaneColors();
    return true;
}


bool
GNECommonNetworkModules::E2MultilaneLaneSelector::drawCandidateLanesWithSpecialColor() const {
    return (myShowCandidateLanes->getCheck() == TRUE);
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::updateLaneColors() {
    // reset all flags
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            lane->resetCandidateFlags();
        }
    }
    // set reachability
    if (myLanePath.size() > 0 && (myShowCandidateLanes->getCheck() == TRUE)) {
        // first mark all lanes as invalid
        for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
            for (const auto& lane : edge.second->getLanes()) {
                lane->setConflictedCandidate(true);
            }
        }
        // now mark lane paths as valid
        for (const auto& lane : myLanePath) {
            // disable conflicted candidate
            lane.first->setConflictedCandidate(false);
            if (lane == myLanePath.back()) {
                lane.first->setSourceCandidate(true);
            } else {
                lane.first->setTargetCandidate(true);
            }
        }
        // get parent edge
        const GNEEdge* edge = myLanePath.back().first->getParentEdge();
        // iterate over connections
        for (const auto& connection : edge->getGNEConnections()) {
            // mark possible candidates
            if (connection->getLaneFrom() == myLanePath.back().first) {
                connection->getLaneTo()->setConflictedCandidate(false);
                connection->getLaneTo()->setPossibleCandidate(true);
            }
        }
    }
    // update view net
    myFrameParent->getViewNet()->updateViewNet();
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::drawTemporalE2Multilane(const GUIVisualizationSettings& s) const {
    if (myLanePath.size() > 0) {
        // check if draw start und end
        const bool drawExtremeSymbols = myFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                                        myFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE;
        // get widths
        const double lineWidth = 0.35;
        const double lineWidthin = 0.25;
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, GLO_TEMPORALSHAPE);
        // set first color
        GLHelper::setColor(RGBColor::GREY);
        // iterate over path
        for (int i = 0; i < (int)myLanePath.size(); i++) {
            // get lane
            const GNELane* lane = myLanePath.at(i).first;
            // draw box lines
            GLHelper::drawBoxLines(lane->getLaneShape(), lineWidth);
            // draw connection between lanes
            if ((i + 1) < (int)myLanePath.size()) {
                // get next lane
                const GNELane* nextLane = myLanePath.at(i + 1).first;
                if (lane->getLane2laneConnections().exist(nextLane)) {
                    GLHelper::drawBoxLines(lane->getLane2laneConnections().getLane2laneGeometry(nextLane).getShape(), lineWidth);
                } else {
                    GLHelper::drawBoxLines({lane->getLaneShape().back(), nextLane->getLaneShape().front()}, lineWidth);
                }
            }
        }
        // move to front
        glTranslated(0, 0, 0.1);
        // set color
        GLHelper::setColor(RGBColor::ORANGE);
        // iterate over path again
        for (int i = 0; i < (int)myLanePath.size(); i++) {
            // get lane
            const GNELane* lane = myLanePath.at(i).first;
            // draw box lines
            GLHelper::drawBoxLines(lane->getLaneShape(), lineWidthin);
            // draw connection between lanes
            if ((i + 1) < (int)myLanePath.size()) {
                // get next lane
                const GNELane* nextLane = myLanePath.at(i + 1).first;
                if (lane->getLane2laneConnections().exist(nextLane)) {
                    GLHelper::drawBoxLines(lane->getLane2laneConnections().getLane2laneGeometry(nextLane).getShape(), lineWidthin);
                } else {
                    GLHelper::drawBoxLines({lane->getLaneShape().back(), nextLane->getLaneShape().front()}, lineWidthin);
                }
            }
        }
        // draw points
        const RGBColor pointColor = RGBColor::RED;
        const RGBColor darkerColor = pointColor.changedBrightness(-32);
        // positions
        const Position firstPosition = myLanePath.front().first->getLaneShape().positionAtOffset2D(myLanePath.front().second);
        const Position secondPosition = myLanePath.back().first->getLaneShape().positionAtOffset2D(myLanePath.back().second);
        // draw geometry points
        GUIGeometry::drawGeometryPoints(s, myFrameParent->getViewNet()->getPositionInformation(), {firstPosition, secondPosition},
                                        pointColor, darkerColor, s.neteditSizeSettings.polylineWidth, 1,
                                        myFrameParent->getViewNet()->getNetworkViewOptions().editingElevation(), drawExtremeSymbols);
        // Pop last matrix
        GLHelper::popMatrix();
    }
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::abortPathCreation() {
    // first check that there is elements
    if (myLanePath.size() > 0) {
        // unblock undo/redo
        myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->enableUndoRedo();
        // clear lanes
        clearPath();
        // disable buttons
        myFinishCreationButton->disable();
        myAbortCreationButton->disable();
        myRemoveLastInsertedElement->disable();
        // update info route label
        updateInfoRouteLabel();
        // update reachability
        updateLaneColors();
        // update view (to see the new route)
        myFrameParent->getViewNet()->updateViewNet();
    }
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::removeLastElement() {
    if (myLanePath.size() > 1) {
        // remove special color of last selected lane
        myLanePath.back().first->resetCandidateFlags();
        // remove last lane
        myLanePath.pop_back();
        // change last lane flag
        if ((myLanePath.size() > 0) && myLanePath.back().first->isSourceCandidate()) {
            myLanePath.back().first->setSourceCandidate(false);
            myLanePath.back().first->setTargetCandidate(true);
        }
        // enable or disable remove last lane button
        if (myLanePath.size() > 1) {
            myRemoveLastInsertedElement->enable();
        } else {
            myRemoveLastInsertedElement->disable();
        }
        // update info route label
        updateInfoRouteLabel();
        // update reachability
        updateLaneColors();
        // update view
        myFrameParent->getViewNet()->updateViewNet();
    }
}


long
GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdCreatePath(FXObject*, FXSelector, void*) {
    myFrameParent->createPath();
    return 1;
}


long
GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // just call abort path creation
    abortPathCreation();
    return 1;
}


long
GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    // just call remove last element
    removeLastElement();
    return 1;
}


long
GNECommonNetworkModules::E2MultilaneLaneSelector::onCmdShowCandidateLanes(FXObject*, FXSelector, void*) {
    // recalc frame
    recalc();
    // update lane colors (view will be updated within function)
    updateLaneColors();
    return 1;
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::updateInfoRouteLabel() {
    if (myLanePath.size() > 0) {
        // declare variables for route info
        double length = 0;
        for (const auto& lane : myLanePath) {
            length += lane.first->getParentEdge()->getNBEdge()->getLength();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << "- Selected lanes: " << toString(myLanePath.size()) << "\n"
                << "- Length: " << toString(length);
        // set new label
        myInfoRouteLabel->setText(information.str().c_str());
    } else {
        myInfoRouteLabel->setText("No lanes selected");
    }
}


void
GNECommonNetworkModules::E2MultilaneLaneSelector::clearPath() {
    // reset all flags
    for (const auto& edge : myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            lane->resetCandidateFlags();
        }
    }
    // clear path
    myLanePath.clear();
    // update info route label
    updateInfoRouteLabel();
}

/****************************************************************************/
