/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEAdditionalFrame.cpp
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

#include "GNEAdditionalFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame::SelectorParentLanes) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_STOPSELECTION,  GNEAdditionalFrame::SelectorParentLanes::onCmdStopSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GNEAdditionalFrame::SelectorParentLanes::onCmdAbortSelection),
};

FXDEFMAP(GNEAdditionalFrame::SelectorChildEdges) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorChildEdges::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorChildEdges::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorChildEdges::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorChildEdges::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorChildEdges::onCmdSelectEdge),
};

FXDEFMAP(GNEAdditionalFrame::SelectorChildLanes) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorChildLanes::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorChildLanes::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorChildLanes::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorChildLanes::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorChildLanes::onCmdSelectLane),
};

FXDEFMAP(GNEAdditionalFrame::E2MultilaneLaneSelector) E2MultilaneLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_ABORT,          GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_FINISH,         GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_REMOVELAST,     GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdRemoveLastElement),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANEPATH_SHOWCANDIDATES, GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdShowCandidateLanes)
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentLanes,        FXGroupBox,     ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorChildEdges,         FXGroupBox,     SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorChildLanes,         FXGroupBox,     SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))
FXIMPLEMENT(GNEAdditionalFrame::E2MultilaneLaneSelector,    FXGroupBox,     E2MultilaneLaneSelectorMap,     ARRAYNUMBER(E2MultilaneLaneSelectorMap))


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorParentLanes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorParentLanes::SelectorParentLanes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lane Selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // create start and stop buttons
    myStopSelectingButton = new FXButton(this, "Stop selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_STOPSELECTION, GUIDesignButton);
    myAbortSelectingButton = new FXButton(this, "Abort selecting", nullptr, this, MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GUIDesignButton);
    // disable stop and abort functions as init
    myStopSelectingButton->disable();
    myAbortSelectingButton->disable();
}


GNEAdditionalFrame::SelectorParentLanes::~SelectorParentLanes() {}


void
GNEAdditionalFrame::SelectorParentLanes::showSelectorParentLanesModul() {
    // abort current selection before show
    abortConsecutiveLaneSelector();
    // show FXGroupBox
    FXGroupBox::show();
}


void
GNEAdditionalFrame::SelectorParentLanes::hideSelectorParentLanesModul() {
    // abort current selection before hide
    abortConsecutiveLaneSelector();
    // hide FXGroupBox
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorParentLanes::startConsecutiveLaneSelector(GNELane* lane, const Position& clickedPosition) {
    // Only start selection if SelectorParentLanes modul is shown
    if (shown()) {
        // change buttons
        myStopSelectingButton->enable();
        myAbortSelectingButton->enable();
        // add lane
        addSelectedLane(lane, clickedPosition);
    }
}


bool
GNEAdditionalFrame::SelectorParentLanes::stopConsecutiveLaneSelector() {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // abort if there isn't at least two lanes
    if (mySelectedLanes.size() < 2) {
        WRITE_WARNING(myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + " requires at least two lanes.");
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return false;
    }
    // create base additional
    if (!myAdditionalFrameParent->createBaseAdditionalObject(tagProperties)) {
        return false;
    }
    // get attributes and values
    myAdditionalFrameParent->myAdditionalAttributes->getAttributesAndValues(myAdditionalFrameParent->myBaseAdditional, true);
    // fill valuesOfElement with Netedit attributes from Frame
    myAdditionalFrameParent->myNeteditAttributes->getNeteditAttributesAndValues(myAdditionalFrameParent->myBaseAdditional, nullptr);
    // Check if ID has to be generated
    if (!myAdditionalFrameParent->myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
        myAdditionalFrameParent->myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myAdditionalFrameParent->generateID(nullptr));
    }
    // obtain lane IDs
    std::vector<std::string> laneIDs;
    for (auto i : mySelectedLanes) {
        laneIDs.push_back(i.first->getID());
    }
    myAdditionalFrameParent->myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, laneIDs);
    // Obtain clicked position over first lane
    myAdditionalFrameParent->myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, mySelectedLanes.front().second);
    // Obtain clicked position over last lane
    myAdditionalFrameParent->myBaseAdditional->addDoubleAttribute(SUMO_ATTR_ENDPOS, mySelectedLanes.back().second);
    // parse common attributes
    if (!myAdditionalFrameParent->buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalFrameParent->myAdditionalAttributes->areValuesValid() == false) {
        myAdditionalFrameParent->myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myAdditionalFrameParent->getViewNet()->getNet(), true);
        // build additional
        additionalHandler.parseSumoBaseObject(myAdditionalFrameParent->myBaseAdditional);
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        // refresh additional attributes
        myAdditionalFrameParent->myAdditionalAttributes->refreshRows();
        return true;
    }
}


void
GNEAdditionalFrame::SelectorParentLanes::abortConsecutiveLaneSelector() {
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
    myAdditionalFrameParent->getViewNet()->updateViewNet();
}


bool
GNEAdditionalFrame::SelectorParentLanes::addSelectedLane(GNELane* lane, const Position& clickedPosition) {
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
    myAdditionalFrameParent->getViewNet()->updateViewNet();
    return true;
}


void
GNEAdditionalFrame::SelectorParentLanes::removeLastSelectedLane() {
    if (mySelectedLanes.size() > 1) {
        mySelectedLanes.pop_back();
    } else {
        WRITE_WARNING("First lane cannot be removed");
    }
}


bool
GNEAdditionalFrame::SelectorParentLanes::isSelectingLanes() const {
    return myStopSelectingButton->isEnabled();
}


bool
GNEAdditionalFrame::SelectorParentLanes::isShown() const {
    return shown();
}


const std::vector<std::pair<GNELane*, double> >&
GNEAdditionalFrame::SelectorParentLanes::getSelectedLanes() const {
    return mySelectedLanes;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdStopSelection(FXObject*, FXSelector, void*) {
    stopConsecutiveLaneSelector();
    return 0;
}


long
GNEAdditionalFrame::SelectorParentLanes::onCmdAbortSelection(FXObject*, FXSelector, void*) {
    abortConsecutiveLaneSelector();
    return 0;
}


bool
GNEAdditionalFrame::SelectorParentLanes::isLaneSelected(GNELane* lane) const {
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorChildEdges - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorChildEdges::SelectorChildEdges(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Edges", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myEdgesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorChildEdgesModul();
}


GNEAdditionalFrame::SelectorChildEdges::~SelectorChildEdges() {}


std::vector<std::string>
GNEAdditionalFrame::SelectorChildEdges::getEdgeIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        // get Selected edges
        const auto selectedEdges = myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
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
GNEAdditionalFrame::SelectorChildEdges::showSelectorChildEdgesModul(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // iterate over edges of net
    for (const auto& edge : myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
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
GNEAdditionalFrame::SelectorChildEdges::hideSelectorChildEdgesModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorChildEdges::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedEdges() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorChildEdges::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorChildEdges::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorChildEdges that contains the searched string
    showSelectorChildEdgesModul(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorChildEdges::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorChildEdges::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorChildEdges::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
// GNEAdditionalFrame::SelectorChildLanes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorChildLanes::SelectorChildLanes(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lanes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myLanesSearch = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_ADDITIONALFRAME_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_ADDITIONALFRAME_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", nullptr, this, MID_GNE_ADDITIONALFRAME_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", nullptr, this, MID_GNE_ADDITIONALFRAME_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideSelectorChildLanesModul();
}


GNEAdditionalFrame::SelectorChildLanes::~SelectorChildLanes() {}


std::vector<std::string>
GNEAdditionalFrame::SelectorChildLanes::getLaneIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedLanesCheckButton->getCheck()) {
        // get Selected lanes
        const auto selectedLanes = myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getSelectedLanes();
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
GNEAdditionalFrame::SelectorChildLanes::showSelectorChildLanesModul(std::string search) {
    myList->clearItems();
    // add all network lanes
    for (const auto& lane : myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getLanes()) {
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
GNEAdditionalFrame::SelectorChildLanes::hideSelectorChildLanesModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorChildLanes::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedLanes() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorChildLanes::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorChildLanes::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorChildLanes that contains the searched string
    showSelectorChildLanesModul(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorChildLanes::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorChildLanes::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorChildLanes::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
// GNEAdditionalFrame::E2MultilaneLaneSelector - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::E2MultilaneLaneSelector::E2MultilaneLaneSelector(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "E2Multilane lane selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(this, "No lanes selected", 0, GUIDesignLabelFrameThicked);
    // create button for finish route creation
    myFinishCreationButton = new FXButton(this, "Finish route creation", nullptr, this, MID_GNE_LANEPATH_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(this, "Abort route creation", nullptr, this, MID_GNE_LANEPATH_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted lane
    myRemoveLastInsertedElement = new FXButton(this, "Remove last inserted lane", nullptr, this, MID_GNE_LANEPATH_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create check button
    myShowCandidateLanes = new FXCheckButton(this, "Show candidate lanes", this, MID_GNE_LANEPATH_SHOWCANDIDATES, GUIDesignCheckButton);
    myShowCandidateLanes->setCheck(TRUE);
    // create backspace label (always shown)
    new FXLabel(this,
                "BACKSPACE: undo click",
                0, GUIDesignLabelFrameInformation);
}


GNEAdditionalFrame::E2MultilaneLaneSelector::~E2MultilaneLaneSelector() {}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::showE2MultilaneLaneSelectorModul() {
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
GNEAdditionalFrame::E2MultilaneLaneSelector::hideE2MultilaneLaneSelectorModul() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


bool
GNEAdditionalFrame::E2MultilaneLaneSelector::addLane(GNELane* lane) {
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
    const Position mousePos = myAdditionalFrameParent->getViewNet()->snapToActiveGrid(myAdditionalFrameParent->getViewNet()->getPositionInformation());
    // calculate lane offset
    const double offset = lane->getLaneShape().nearest_offset_to_point2D(mousePos);
    // All checks ok, then add it in selected elements
    myLanePath.push_back(std::make_pair(lane, offset));
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myAdditionalFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo("route creation");
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
GNEAdditionalFrame::E2MultilaneLaneSelector::drawCandidateLanesWithSpecialColor() const {
    return (myShowCandidateLanes->getCheck() == TRUE);
}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::updateLaneColors() {
    // reset all flags
    for (const auto& edge : myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            lane->resetCandidateFlags();
        }
    }
    // set reachability
    if (myLanePath.size() > 0 && (myShowCandidateLanes->getCheck() == TRUE)) {
        // first mark all lanes as invalid
        for (const auto& edge : myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
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
    myAdditionalFrameParent->getViewNet()->updateViewNet();
}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::drawTemporalE2Multilane(const GUIVisualizationSettings& s) const {
    if (myLanePath.size() > 0) {
        // check if draw start und end
        const bool drawExtremeSymbols = myAdditionalFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                                        myAdditionalFrameParent->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE;
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
        GUIGeometry::drawGeometryPoints(s, myAdditionalFrameParent->getViewNet()->getPositionInformation(), {firstPosition, secondPosition},
                                        pointColor, darkerColor, s.neteditSizeSettings.polylineWidth, 1,
                                        myAdditionalFrameParent->getViewNet()->getNetworkViewOptions().editingElevation(), drawExtremeSymbols);
        // Pop last matrix
        GLHelper::popMatrix();
    }
}


bool
GNEAdditionalFrame::E2MultilaneLaneSelector::createPath() {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // first check that current tag is valid
    if (tagProperties.getTag() != GNE_TAG_E2DETECTOR_MULTILANE) {
        return false;
    }
    // now check number of lanes
    if (myLanePath.size() < 2) {
        WRITE_WARNING("E2 multilane detectors need at least two consecutive lanes");
        return false;
    }
    // create base additional
    if (!myAdditionalFrameParent->createBaseAdditionalObject(tagProperties)) {
        return false;
    }
    // get attributes and values
    myAdditionalFrameParent->myAdditionalAttributes->getAttributesAndValues(myAdditionalFrameParent->myBaseAdditional, true);
    // fill netedit attributes
    if (!myAdditionalFrameParent->myNeteditAttributes->getNeteditAttributesAndValues(myAdditionalFrameParent->myBaseAdditional, nullptr)) {
        return false;
    }
    // Check if ID has to be generated
    if (!myAdditionalFrameParent->myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
        myAdditionalFrameParent->myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myAdditionalFrameParent->myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(GNE_TAG_E2DETECTOR_MULTILANE));
    }
    // obtain lane IDs
    std::vector<std::string> laneIDs;
    for (const auto& lane : myLanePath) {
        laneIDs.push_back(lane.first->getID());
    }
    myAdditionalFrameParent->myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, laneIDs);
    // set positions
    myAdditionalFrameParent->myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, myLanePath.front().second);
    myAdditionalFrameParent->myBaseAdditional->addDoubleAttribute(SUMO_ATTR_ENDPOS, myLanePath.back().second);
    // parse common attributes
    if (!myAdditionalFrameParent->buildAdditionalCommonAttributes(myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty())) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalFrameParent->myAdditionalAttributes->areValuesValid() == false) {
        myAdditionalFrameParent->myAdditionalAttributes->showWarningMessage();
        return false;
    }
    // declare additional handler
    GNEAdditionalHandler additionalHandler(myAdditionalFrameParent->getViewNet()->getNet(), true);
    // build additional
    additionalHandler.parseSumoBaseObject(myAdditionalFrameParent->myBaseAdditional);
    // Refresh additional Parent Selector (For additionals that have a limited number of children)
    myAdditionalFrameParent->mySelectorAdditionalParent->refreshSelectorParentModul();
    // abort E2 creation
    abortPathCreation();
    // refresh additional attributes
    myAdditionalFrameParent->myAdditionalAttributes->refreshRows();
    return true;
}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::abortPathCreation() {
    // first check that there is elements
    if (myLanePath.size() > 0) {
        // unblock undo/redo
        myAdditionalFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->enableUndoRedo();
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
        myAdditionalFrameParent->getViewNet()->updateViewNet();
    }
}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::removeLastElement() {
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
        myAdditionalFrameParent->getViewNet()->updateViewNet();
    }
}


long
GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdCreatePath(FXObject*, FXSelector, void*) {
    // just call create path
    createPath();
    return 1;
}


long
GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // just call abort path creation
    abortPathCreation();
    return 1;
}


long
GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    // just call remove last element
    removeLastElement();
    return 1;
}


long
GNEAdditionalFrame::E2MultilaneLaneSelector::onCmdShowCandidateLanes(FXObject*, FXSelector, void*) {
    // recalc frame
    recalc();
    // update lane colors (view will be updated within function)
    updateLaneColors();
    return 1;
}


void
GNEAdditionalFrame::E2MultilaneLaneSelector::updateInfoRouteLabel() {
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
GNEAdditionalFrame::E2MultilaneLaneSelector::clearPath() {
    // reset all flags
    for (const auto& edge : myAdditionalFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdges()) {
        for (const auto& lane : edge.second->getLanes()) {
            lane->resetCandidateFlags();
        }
    }
    // clear path
    myLanePath.clear();
    // update info route label
    updateInfoRouteLabel();
}

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Additionals"),
    myBaseAdditional(nullptr) {

    // create item Selector modul for additionals
    myAdditionalTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::ADDITIONALELEMENT, SUMO_TAG_BUS_STOP);

    // Create additional parameters
    myAdditionalAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributesModuls::NeteditAttributes(this);

    // Create consecutive Lane Selector
    mySelectorLaneParents = new SelectorParentLanes(this);

    // Create selector parent
    mySelectorAdditionalParent = new GNEFrameModuls::SelectorParent(this);

    // Create selector child edges
    mySelectorChildEdges = new SelectorChildEdges(this);

    // Create selector child lanes
    mySelectorChildLanes = new SelectorChildLanes(this);

    // Create list for E2Multilane lane selector
    myE2MultilaneLaneSelector = new E2MultilaneLaneSelector(this);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    // check if we have to delete base additional object
    if (myBaseAdditional) {
        delete myBaseAdditional;
    }
}


void
GNEAdditionalFrame::show() {
    // refresh tag selector
    myAdditionalTagSelector->refreshTagSelector();
    // show frame
    GNEFrame::show();
}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that current selected additional is valid
    if (myAdditionalTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    }
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // create base additional
    if (!createBaseAdditionalObject(tagProperties)) {
        return false;
    }
    // obtain attributes and values
    myAdditionalAttributes->getAttributesAndValues(myBaseAdditional, true);
    // fill netedit attributes
    if (!myNeteditAttributes->getNeteditAttributesAndValues(myBaseAdditional, objectsUnderCursor.getLaneFront())) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGE) || (tagProperties.getTag() == SUMO_TAG_VAPORIZER)) {
        return buildAdditionalOverEdge(objectsUnderCursor.getLaneFront(), tagProperties);
    } else if (tagProperties.hasAttribute(SUMO_ATTR_LANE)) {
        return buildAdditionalOverLane(objectsUnderCursor.getLaneFront(), tagProperties);
    } else if (tagProperties.getTag() == GNE_TAG_E2DETECTOR_MULTILANE) {
        return myE2MultilaneLaneSelector->addLane(objectsUnderCursor.getLaneFront());
    } else {
        return buildAdditionalOverView(tagProperties);
    }
}


void
GNEAdditionalFrame::showSelectorChildLanesModul() {
    // Show frame
    GNEFrame::show();
    // Update UseSelectedLane CheckBox
    mySelectorChildEdges->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    mySelectorChildLanes->updateUseSelectedLanes();
}


GNEAdditionalFrame::SelectorParentLanes*
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return mySelectorLaneParents;
}


GNEAdditionalFrame::E2MultilaneLaneSelector*
GNEAdditionalFrame::getE2MultilaneLaneSelector() const {
    return myE2MultilaneLaneSelector;
}


void
GNEAdditionalFrame::tagSelected() {
    if (myAdditionalTagSelector->getCurrentTemplateAC()) {
        // show additional attributes modul
        myAdditionalAttributes->showAttributesCreatorModul(myAdditionalTagSelector->getCurrentTemplateAC(), {});
        // show netedit attributes
        myNeteditAttributes->showNeteditAttributesModul(myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty());
        // Show myAdditionalFrameParent if we're adding an slave element
        if (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().isChild()) {
            mySelectorAdditionalParent->showSelectorParentModul(myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getParentTags());
        } else {
            mySelectorAdditionalParent->hideSelectorParentModul();
        }
        // Show SelectorChildEdges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
            mySelectorChildEdges->showSelectorChildEdgesModul();
        } else {
            mySelectorChildEdges->hideSelectorChildEdgesModul();
        }
        // check if we must show E2 multilane lane selector
        if (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() == GNE_TAG_E2DETECTOR_MULTILANE) {
            myE2MultilaneLaneSelector->showE2MultilaneLaneSelectorModul();
        } else if (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().hasAttribute(SUMO_ATTR_LANES)) {
            myE2MultilaneLaneSelector->hideE2MultilaneLaneSelectorModul();
            // Show SelectorChildLanes or consecutive lane selector if we're adding an additional that own the attribute SUMO_ATTR_LANES
            if (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().isChild() &&
                    (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getParentTags().front() == SUMO_TAG_LANE)) {
                // show selector parent lane and hide selector child lane
                mySelectorLaneParents->showSelectorParentLanesModul();
                mySelectorChildLanes->hideSelectorChildLanesModul();
            } else {
                // show selector child lane and hide selector parent lane
                mySelectorChildLanes->showSelectorChildLanesModul();
                mySelectorLaneParents->hideSelectorParentLanesModul();
            }
        } else {
            myE2MultilaneLaneSelector->hideE2MultilaneLaneSelectorModul();
            mySelectorChildLanes->hideSelectorChildLanesModul();
            mySelectorLaneParents->hideSelectorParentLanesModul();
        }
    } else {
        // hide all moduls if additional isn't valid
        myAdditionalAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        mySelectorAdditionalParent->hideSelectorParentModul();
        mySelectorChildEdges->hideSelectorChildEdgesModul();
        mySelectorChildLanes->hideSelectorChildLanesModul();
        mySelectorLaneParents->hideSelectorParentLanesModul();
        myE2MultilaneLaneSelector->hideE2MultilaneLaneSelectorModul();
    }
}


bool
GNEAdditionalFrame::createBaseAdditionalObject(const GNETagProperties& tagProperty) {
    // check if baseAdditional exist, and if yes, delete it
    if (myBaseAdditional) {
        // go to base additional root
        while (myBaseAdditional->getParentSumoBaseObject()) {
            myBaseAdditional = myBaseAdditional->getParentSumoBaseObject();
        }
        // delete baseAdditional (and all children)
        delete myBaseAdditional;
        // reset baseAdditional
        myBaseAdditional = nullptr;
    }
    // declare tag for base additional
    SumoXMLTag baseAdditionalTag = tagProperty.getTag();
    // check if baseAdditionalTag has to be updated
    if (baseAdditionalTag == GNE_TAG_E2DETECTOR_MULTILANE) {
        baseAdditionalTag = SUMO_TAG_E2DETECTOR;
    } else if (baseAdditionalTag == GNE_TAG_FLOW_CALIBRATOR) {
        baseAdditionalTag = SUMO_TAG_FLOW;
    }
    // check if additional is child
    if (tagProperty.isChild()) {
        // get additional under cursor
        const GNEAdditional* additionalUnderCursor = myViewNet->getObjectsUnderCursor().getAdditionalFront();
        // if user click over an additional element parent, mark int in ParentAdditionalSelector
        if (additionalUnderCursor && (additionalUnderCursor->getTagProperty().getTag() == tagProperty.getParentTags().front())) {
            // update parent additional selected
            mySelectorAdditionalParent->setIDSelected(additionalUnderCursor->getID());
        }
        // stop if currently there isn't a valid selected parent
        if (mySelectorAdditionalParent->getIdSelected().empty()) {
            myAdditionalAttributes->showWarningMessage("A " + toString(tagProperty.getParentTags().front()) + " must be selected before insertion of " + myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + ".");
            return false;
        } else {
            // create baseAdditional parent
            myBaseAdditional = new CommonXMLStructure::SumoBaseObject(nullptr);
            // set parent tag
            myBaseAdditional->setTag(tagProperty.getParentTags().front());
            // add ID
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, mySelectorAdditionalParent->getIdSelected());
            // create baseAdditional again as child of current myBaseAdditional
            myBaseAdditional = new CommonXMLStructure::SumoBaseObject(myBaseAdditional);
        }
    } else {
        // just create a base additional
        myBaseAdditional = new CommonXMLStructure::SumoBaseObject(nullptr);
    }
    // set baseAdditionalTag
    myBaseAdditional->setTag(baseAdditionalTag);
    // BaseAdditional created, then return true
    return true;
}


std::string
GNEAdditionalFrame::generateID(GNENetworkElement* networkElement) const {
    // obtain current number of additionals to generate a new index faster
    int additionalIndex = (int)myViewNet->getNet()->getAttributeCarriers()->getAdditionals().at(myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag()).size();
    // obtain tag Properties (only for improve code legilibility
    const auto& tagProperties = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // get attribute carriers
    const auto& attributeCarriers = myViewNet->getNet()->getAttributeCarriers();
    if (networkElement) {
        // special case for vaporizers
        if (tagProperties.getTag() == SUMO_TAG_VAPORIZER) {
            return networkElement->getID();
        } else {
            // generate ID using networkElement
            while (attributeCarriers->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + networkElement->getID() + "_" + toString(additionalIndex), false) != nullptr) {
                additionalIndex++;
            }
            return tagProperties.getTagStr() + "_" + networkElement->getID() + "_" + toString(additionalIndex);
        }
    } else {
        // generate ID without networkElement
        while (attributeCarriers->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return tagProperties.getTagStr() + "_" + toString(additionalIndex);
    }
}



bool
GNEAdditionalFrame::buildAdditionalCommonAttributes(const GNETagProperties& tagProperties) {
    // If additional has a interval defined by a begin or end, check that is valid
    if (tagProperties.hasAttribute(SUMO_ATTR_STARTTIME) && tagProperties.hasAttribute(SUMO_ATTR_END)) {
        const double begin = myBaseAdditional->getDoubleAttribute(SUMO_ATTR_STARTTIME);
        const double end = myBaseAdditional->getDoubleAttribute(SUMO_ATTR_END);
        if (begin > end) {
            myAdditionalAttributes->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return false;
        }
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (tagProperties.hasAttribute(SUMO_ATTR_FILE) && myBaseAdditional->getStringAttribute(SUMO_ATTR_FILE).empty()) {
        if ((myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_CALIBRATOR) && (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            myBaseAdditional->addStringAttribute(SUMO_ATTR_FILE, myBaseAdditional->getStringAttribute(SUMO_ATTR_ID) + ".xml");
        }
    }
    // check edge children
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGES) && (!myBaseAdditional->hasStringListAttribute(SUMO_ATTR_EDGES) || myBaseAdditional->getStringListAttribute(SUMO_ATTR_EDGES).empty())) {
        // obtain edge IDs
        myBaseAdditional->addStringListAttribute(SUMO_ATTR_EDGES, mySelectorChildEdges->getEdgeIdsSelected());
        // check if attribute has at least one edge
        if (myBaseAdditional->getStringListAttribute(SUMO_ATTR_EDGES).empty()) {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return false;
        }
    }
    // check lane children
    if (tagProperties.hasAttribute(SUMO_ATTR_LANES) && (!myBaseAdditional->hasStringListAttribute(SUMO_ATTR_LANES) || myBaseAdditional->getStringListAttribute(SUMO_ATTR_LANES).empty())) {
        // obtain lane IDs
        myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, mySelectorChildLanes->getLaneIdsSelected());
        // check if attribute has at least one lane
        if (myBaseAdditional->getStringListAttribute(SUMO_ATTR_LANES).empty()) {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return false;
        }
    }
    // all ok, continue building additional
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalOverEdge(GNELane* lane, const GNETagProperties& tagProperties) {
    // check that lane exist
    if (lane) {
        // Get attribute lane's edge
        myBaseAdditional->addStringAttribute(SUMO_ATTR_EDGE, lane->getParentEdge()->getID());
        // Check if ID has to be generated
        if (!myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, generateID(lane->getParentEdge()));
        }
    } else {
        return false;
    }
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModul();
        // clear selected eddges and lanes
        mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshRows();
        return true;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLane(GNELane* lane, const GNETagProperties& tagProperties) {
    // check that lane exist
    if (lane != nullptr) {
        // Get attribute lane
        myBaseAdditional->addStringAttribute(SUMO_ATTR_LANE, lane->getID());
        // Check if ID has to be generated
        if (!myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, generateID(lane));
        }
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    const double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // set attribute position as mouse position over lane
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, mousePositionOverLane);
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModul();
        // clear selected eddges and lanes
        mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshRows();
        return true;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverView(const GNETagProperties& tagProperties) {
    // Check if ID has to be generated
    if (!myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
        myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, generateID(nullptr));
    }
    // Obtain position as the clicked position over view
    const Position viewPos = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());
    // add position and X-Y-Z attributes
    myBaseAdditional->addPositionAttribute(SUMO_ATTR_POSITION, viewPos);
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_X, viewPos.x());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Y, viewPos.y());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Z, viewPos.z());
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // special case for VSS Steps
    if (myBaseAdditional->getTag() == SUMO_TAG_STEP) {
        // get VSS parent
        const auto VSSParent = myViewNet->getNet()->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_VSS,
                               myBaseAdditional->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
        // get last step
        GNEAdditional* step = nullptr;
        for (const auto& additionalChild : VSSParent->getChildAdditionals()) {
            if (!additionalChild->getTagProperty().isSymbol()) {
                step = additionalChild;
            }
        }
        // set time
        if (step) {
            myBaseAdditional->addTimeAttribute(SUMO_ATTR_TIME, string2time(step->getAttribute(SUMO_ATTR_TIME)) + TIME2STEPS(900));
        } else {
            myBaseAdditional->addTimeAttribute(SUMO_ATTR_TIME, 0);
        }
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalAttributes->areValuesValid() == false) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModul();
        // clear selected eddges and lanes
        mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshRows();
        return true;
    }
}

/****************************************************************************/
