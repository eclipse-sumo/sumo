/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEConsecutiveSelector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Consecutive lane selector module
/****************************************************************************/
#include <config.h>

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEConsecutiveSelector.h"
#include "GNEFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEConsecutiveSelector) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ABORT,          GNEConsecutiveSelector::onCmdAbortPathCreation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_FINISH,         GNEConsecutiveSelector::onCmdCreatePath),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVELAST,     GNEConsecutiveSelector::onCmdRemoveLastElement),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOWCANDIDATES, GNEConsecutiveSelector::onCmdShowCandidateLanes)
};

// Object implementation
FXIMPLEMENT(GNEConsecutiveSelector, MFXGroupBoxModule, ConsecutiveLaneSelectorMap, ARRAYNUMBER(ConsecutiveLaneSelectorMap))

// ---------------------------------------------------------------------------
// GNEConsecutiveSelector - methods
// ---------------------------------------------------------------------------

GNEConsecutiveSelector::GNEConsecutiveSelector(GNEFrame* frameParent, const bool allowOneLane) :
    MFXGroupBoxModule(frameParent, TL("Consecutive lane selector")),
    myFrameParent(frameParent),
    myAllowOneLane(allowOneLane) {
    // create label for route info
    myInfoPathLabel = new FXLabel(getCollapsableFrame(), TL("No lanes selected"), 0, GUIDesignLabelThick(JUSTIFY_LEFT));
    // create button for finish route creation
    myFinishCreationButton = new FXButton(getCollapsableFrame(), TL("Finish path creation"), nullptr, this, MID_GNE_FINISH, GUIDesignButton);
    myFinishCreationButton->disable();
    // create button for abort route creation
    myAbortCreationButton = new FXButton(getCollapsableFrame(), TL("Abort path creation"), nullptr, this, MID_GNE_ABORT, GUIDesignButton);
    myAbortCreationButton->disable();
    // create button for remove last inserted lane
    myRemoveLastInsertedElement = new FXButton(getCollapsableFrame(), TL("Remove last lane"), nullptr, this, MID_GNE_REMOVELAST, GUIDesignButton);
    myRemoveLastInsertedElement->disable();
    // create check button
    myShowCandidateLanes = new FXCheckButton(getCollapsableFrame(), TL("Show candidate lanes"), this, MID_GNE_SHOWCANDIDATES, GUIDesignCheckButton);
    myShowCandidateLanes->setCheck(TRUE);
    // create information label
    new FXLabel(this, (TL("-BACKSPACE: undo click") + std::string("\n") + TL("-ESC: Abort path creation")).c_str(), 0, GUIDesignLabelFrameInformation);
}


GNEConsecutiveSelector::~GNEConsecutiveSelector() {}


void
GNEConsecutiveSelector::showConsecutiveLaneSelectorModule() {
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
GNEConsecutiveSelector::hideConsecutiveLaneSelectorModule() {
    // clear path
    clearPath();
    // hide modul
    hide();
}


const std::vector<std::pair<GNELane*, double> >&
GNEConsecutiveSelector::getLanePath() const {
    return myLanePath;
}


const std::vector<std::string>
GNEConsecutiveSelector::getLaneIDPath() const {
    std::vector<std::string> laneIDs;
    for (const auto& lane : myLanePath) {
        if (laneIDs.empty() || (laneIDs.back() != lane.first->getID())) {
            laneIDs.push_back(lane.first->getID());
        }
    }
    return laneIDs;
}


bool
GNEConsecutiveSelector::addLane(GNELane* lane) {
    // first check if lane is valid
    if (lane == nullptr) {
        return false;
    }
    // check candidate lane
    if ((myShowCandidateLanes->getCheck() == TRUE) && !lane->isPossibleCandidate()) {
        if (lane->isSpecialCandidate() || lane->isConflictedCandidate()) {
            // Write warning
            WRITE_WARNING(TL("Invalid lane"));
            // abort add lane
            return false;
        }
    }
    // get mouse position
    const Position mousePos = myFrameParent->getViewNet()->snapToActiveGrid(myFrameParent->getViewNet()->getPositionInformation());
    // calculate lane offset
    const double posOverLane = lane->getLaneShape().nearest_offset_to_point2D(mousePos);
    // All checks ok, then add it in selected elements
    if (myLanePath.empty()) {
        myLanePath.push_back(std::make_pair(lane, posOverLane));
    } else if ((myLanePath.size() == 1) && (myLanePath.front().first == lane)) {
        if (myAllowOneLane) {
            myLanePath.push_back(std::make_pair(lane, posOverLane));
        } else {
            // Write warning
            WRITE_WARNING(TL("Lane path needs at least two lanes"));
            // abort add lane
            return false;
        }
    } else if (myLanePath.back().first == lane) {
        // only change last position
        myLanePath.back().second = posOverLane;
    } else {
        myLanePath.push_back(std::make_pair(lane, posOverLane));
        // special case if we clicked over a new lane after a previous double lane
        if ((myLanePath.size() == 3) && (myLanePath.at(0).first == myLanePath.at(1).first)) {
            // remove second lane
            myLanePath.erase(myLanePath.begin() + 1);
        }
    }
    // enable abort route button
    myAbortCreationButton->enable();
    // enable finish button
    myFinishCreationButton->enable();
    // disable undo/redo
    myFrameParent->getViewNet()->getViewParent()->getGNEAppWindows()->disableUndoRedo(TL("route creation"));
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
GNEConsecutiveSelector::drawCandidateLanesWithSpecialColor() const {
    return (myShowCandidateLanes->getCheck() == TRUE);
}


void
GNEConsecutiveSelector::updateLaneColors() {
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
GNEConsecutiveSelector::drawTemporalConsecutiveLanePath(const GUIVisualizationSettings& s) const {
    // Only draw if there is at least one lane
    if (myLanePath.size() > 0) {
        // get widths
        const double lineWidth = 0.35;
        const double lineWidthin = 0.25;
        // declare vector with shapes
        std::vector<PositionVector> shapes;
        // iterate over lanes (only if there is more than one)
        if ((myLanePath.size() == 2) && (myLanePath.front().first == myLanePath.back().first)) {
            // only add first lane shape
            shapes.push_back(myLanePath.front().first->getLaneShape());
            // adjust shape
            shapes.front() = shapes.front().getSubpart(myLanePath.front().second, myLanePath.back().second);
        } else if (myLanePath.size() > 1) {
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
        for (const auto& shape : shapes) {
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
GNEConsecutiveSelector::abortPathCreation() {
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
GNEConsecutiveSelector::removeLastElement() {
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
GNEConsecutiveSelector::onCmdCreatePath(FXObject*, FXSelector, void*) {
    myFrameParent->createPath(false);
    return 1;
}


long
GNEConsecutiveSelector::onCmdAbortPathCreation(FXObject*, FXSelector, void*) {
    // just call abort path creation
    abortPathCreation();
    return 1;
}


long
GNEConsecutiveSelector::onCmdRemoveLastElement(FXObject*, FXSelector, void*) {
    // just call remove last element
    removeLastElement();
    return 1;
}


long
GNEConsecutiveSelector::onCmdShowCandidateLanes(FXObject*, FXSelector, void*) {
    // recalc frame
    recalc();
    // update lane colors (view will be updated within function)
    updateLaneColors();
    return 1;
}


GNEConsecutiveSelector::GNEConsecutiveSelector() :
    myFrameParent(nullptr),
    myAllowOneLane(false) {
}


void
GNEConsecutiveSelector::updateInfoRouteLabel() {
    if (myLanePath.size() > 0) {
        // declare variables for route info
        double length = 0;
        for (const auto& lane : myLanePath) {
            length += lane.first->getParentEdge()->getNBEdge()->getLength();
        }
        // declare ostringstream for label and fill it
        std::ostringstream information;
        information
                << TL("- Selected lanes: ") << toString(myLanePath.size()) << "\n"
                << TL("- Length: ") << toString(length);
        // set new label
        myInfoPathLabel->setText(information.str().c_str());
    } else {
        myInfoPathLabel->setText(TL("No lanes selected"));
    }
}


void
GNEConsecutiveSelector::clearPath() {
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
