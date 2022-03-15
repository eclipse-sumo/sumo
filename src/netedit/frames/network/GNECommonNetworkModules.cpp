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
/// @date    Mar 2022
///
// Common network modules
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

FXDEFMAP(GNECommonNetworkModules::EdgesSelector) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USESELECTED,        GNECommonNetworkModules::EdgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CLEARSELECTION,     GNECommonNetworkModules::EdgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INVERTSELECTION,    GNECommonNetworkModules::EdgesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_SEARCH,             GNECommonNetworkModules::EdgesSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,             GNECommonNetworkModules::EdgesSelector::onCmdSelectEdge),
};

FXDEFMAP(GNECommonNetworkModules::LanesSelector) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USESELECTED,        GNECommonNetworkModules::LanesSelector::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CLEARSELECTION,     GNECommonNetworkModules::LanesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INVERTSELECTION,    GNECommonNetworkModules::LanesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_SEARCH,             GNECommonNetworkModules::LanesSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,             GNECommonNetworkModules::LanesSelector::onCmdSelectLane),
};

FXDEFMAP(GNECommonNetworkModules::ConsecutiveLaneSelector) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ABORT,          GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_FINISH,         GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVELAST,     GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdRemoveLastElement),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOWCANDIDATES, GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdShowCandidateLanes)
};

// Object implementation
FXIMPLEMENT(GNECommonNetworkModules::EdgesSelector,         FXGroupBoxModule,     SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNECommonNetworkModules::LanesSelector,         FXGroupBoxModule,     SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))
FXIMPLEMENT(GNECommonNetworkModules::ConsecutiveLaneSelector,    FXGroupBoxModule,     ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))


// ---------------------------------------------------------------------------
// GNECommonNetworkModules::EdgesSelector - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::EdgesSelector::EdgesSelector(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Edges"),
    myFrameParent(frameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(getCollapsableFrame(), ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myEdgesSearch = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "Clear", nullptr, this, MID_GNE_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "Invert", nullptr, this, MID_GNE_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideEdgesSelectorModule();
}


GNECommonNetworkModules::EdgesSelector::~EdgesSelector() {}


std::vector<std::string>
GNECommonNetworkModules::EdgesSelector::getEdgeIdsSelected() const {
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
GNECommonNetworkModules::EdgesSelector::showEdgesSelectorModule(std::string search) {
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
GNECommonNetworkModules::EdgesSelector::hideEdgesSelectorModule() {
    FXGroupBoxModule::hide();
}


void
GNECommonNetworkModules::EdgesSelector::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedEdges() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNECommonNetworkModules::EdgesSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
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
GNECommonNetworkModules::EdgesSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of EdgesSelector that contains the searched string
    showEdgesSelectorModule(myEdgesSearch->getText().text());
    return 1;
}


long
GNECommonNetworkModules::EdgesSelector::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECommonNetworkModules::EdgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNECommonNetworkModules::EdgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
// GNECommonNetworkModules::LanesSelector - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::LanesSelector::LanesSelector(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Lanes"),
    myFrameParent(frameParent) {

    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(getCollapsableFrame(), ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_USESELECTED, GUIDesignCheckButton);

    // Create search box
    myLanesSearch = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SEARCH, GUIDesignTextField);

    // Create list
    myList = new FXList(getCollapsableFrame(), this, MID_GNE_SELECT, GUIDesignListFixedHeight, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", nullptr, this, MID_GNE_CLEARSELECTION, GUIDesignButtonRectangular);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", nullptr, this, MID_GNE_INVERTSELECTION, GUIDesignButtonRectangular);

    // Hide List
    hideLanesSelectorModule();
}


GNECommonNetworkModules::LanesSelector::~LanesSelector() {}


std::vector<std::string>
GNECommonNetworkModules::LanesSelector::getLaneIdsSelected() const {
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


bool
GNECommonNetworkModules::LanesSelector::isLaneSelected(const GNELane* lane) const {
    if (myFrameParent->shown() && shown()) {
        // check if id is selected
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i) && (myList->getItem(i)->getText().text() == lane->getID())) {
                return true;
            }
        }
    }
    return false;
}


void
GNECommonNetworkModules::LanesSelector::showLanesSelectorModule(std::string search) {
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
GNECommonNetworkModules::LanesSelector::hideLanesSelectorModule() {
    FXGroupBoxModule::hide();
}


void
GNECommonNetworkModules::LanesSelector::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getNumberOfSelectedLanes() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNECommonNetworkModules::LanesSelector::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
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
GNECommonNetworkModules::LanesSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of LanesSelector that contains the searched string
    showLanesSelectorModule(myLanesSearch->getText().text());
    return 1;
}


long
GNECommonNetworkModules::LanesSelector::onCmdSelectLane(FXObject*, FXSelector, void*) {
    myFrameParent->getViewNet()->update();
    return 1;
}


long
GNECommonNetworkModules::LanesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    myFrameParent->getViewNet()->update();
    return 1;
}


long
GNECommonNetworkModules::LanesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    myFrameParent->getViewNet()->update();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECommonNetworkModules::ConsecutiveLaneSelector - methods
// ---------------------------------------------------------------------------

GNECommonNetworkModules::ConsecutiveLaneSelector::ConsecutiveLaneSelector(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->getContentFrame(), "Consecutive lane selector"),
    myFrameParent(frameParent) {
    // create label for route info
    myInfoRouteLabel = new FXLabel(getCollapsableFrame(), "No lanes selected", 0, GUIDesignLabelFrameThicked);
    // create button for finish route creation
    myFinishCreationButton = new FXButton(getCollapsableFrame(), "Finish route creation", nullptr, this, MID_GNE_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(getCollapsableFrame(), "Abort route creation", nullptr, this, MID_GNE_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted lane
    myRemoveLastInsertedElement = new FXButton(getCollapsableFrame(), "Remove last inserted lane", nullptr, this, MID_GNE_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create check button
    myShowCandidateLanes = new FXCheckButton(getCollapsableFrame(), "Show candidate lanes", this, MID_GNE_SHOWCANDIDATES, GUIDesignCheckButton);
    myShowCandidateLanes->setCheck(TRUE);
    // create backspace label (always shown)
    new FXLabel(this,
                "BACKSPACE: undo click",
                0, GUIDesignLabelFrameInformation);
}


GNECommonNetworkModules::ConsecutiveLaneSelector::~ConsecutiveLaneSelector() {}


void
GNECommonNetworkModules::ConsecutiveLaneSelector::showConsecutiveLaneSelectorModule() {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::hideConsecutiveLaneSelectorModule() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


const std::vector<std::pair<GNELane*, double> >&
GNECommonNetworkModules::ConsecutiveLaneSelector::getLanePath() const {
    return myLanePath;
}


const std::vector<std::string> 
GNECommonNetworkModules::ConsecutiveLaneSelector::getLaneIDPath() const {
    std::vector<std::string> laneIDs;
    for (const auto& lane : myLanePath) {
        laneIDs.push_back(lane.first->getID());
    }
    return laneIDs;
}


bool
GNECommonNetworkModules::ConsecutiveLaneSelector::addLane(GNELane* lane) {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::drawCandidateLanesWithSpecialColor() const {
    return (myShowCandidateLanes->getCheck() == TRUE);
}


void
GNECommonNetworkModules::ConsecutiveLaneSelector::updateLaneColors() {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::drawTemporalConsecutiveLanePath(const GUIVisualizationSettings& s) const {
    // Only draw if there is at least one lane
    if (myLanePath.size() > 0) {
        // get widths
        const double lineWidth = 0.35;
        const double lineWidthin = 0.25;
        // declare vector with shapes
        std::vector<PositionVector> shapes;
        // iterate over lanes (only if there is more than one)
        if (myLanePath.size() > 1) {
            // get shapes
            for (int i = 0; i < (int)myLanePath.size(); i++) {
                // get lane
                const GNELane* lane = myLanePath.at(i).first;
                // add lane shape
                shapes.push_back(lane->getLaneShape());
                // draw connection between lanes
                if ((i + 1) < (int)myLanePath.size()) {
                    // get next lane
                    const GNELane* nextLane = myLanePath.at(i + 1).first;
                    if (lane->getLane2laneConnections().exist(nextLane)) {
                        shapes.push_back(lane->getLane2laneConnections().getLane2laneGeometry(nextLane).getShape());
                    } else {
                        shapes.push_back({lane->getLaneShape().back(), nextLane->getLaneShape().front()});
                    }
                }
            }
            // adjust first and last shape
            shapes.front() = shapes.front().splitAt(myLanePath.front().second).second;
            shapes.back() = shapes.back().splitAt(myLanePath.back().second).first;
        }
        // Add a draw matrix
        GLHelper::pushMatrix();
        // move to temporal shape
        glTranslated(0, 0, GLO_TEMPORALSHAPE);
        // iterate over shapes
        for (const auto &shape : shapes) {
            // set extern
            GLHelper::setColor(RGBColor::GREY);
            // draw extern shape
            GLHelper::drawBoxLines(shape, lineWidth);
            // push matrix
            GLHelper::pushMatrix();
            // move to front
            glTranslated(0, 0, 0.1);
            // set orange color
            GLHelper::setColor(RGBColor::ORANGE);
            // draw intern shape
            GLHelper::drawBoxLines(shape, lineWidthin);
            // Pop matrix
            GLHelper::popMatrix();
        }
        // draw points
        const RGBColor pointColor = RGBColor::RED;
        // positions
        const Position firstPosition = myLanePath.front().first->getLaneShape().positionAtOffset2D(myLanePath.front().second);
        const Position secondPosition = myLanePath.back().first->getLaneShape().positionAtOffset2D(myLanePath.back().second);
        // draw geometry points
        GUIGeometry::drawGeometryPoints(s, myFrameParent->getViewNet()->getPositionInformation(), {firstPosition, secondPosition},
                                        pointColor, RGBColor::WHITE, s.neteditSizeSettings.polylineWidth, 1, false, true);
        // Pop last matrix
        GLHelper::popMatrix();
    }
}


void
GNECommonNetworkModules::ConsecutiveLaneSelector::abortPathCreation() {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::removeLastElement() {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdCreatePath(FXObject*, FXSelector, void*) {
    myFrameParent->createPath();
    return 1;
}


long
GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // just call abort path creation
    abortPathCreation();
    return 1;
}


long
GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    // just call remove last element
    removeLastElement();
    return 1;
}


long
GNECommonNetworkModules::ConsecutiveLaneSelector::onCmdShowCandidateLanes(FXObject*, FXSelector, void*) {
    // recalc frame
    recalc();
    // update lane colors (view will be updated within function)
    updateLaneColors();
    return 1;
}


void
GNECommonNetworkModules::ConsecutiveLaneSelector::updateInfoRouteLabel() {
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
GNECommonNetworkModules::ConsecutiveLaneSelector::clearPath() {
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
