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
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/SUMOSAXAttributesImpl_Cached.h>
#include <netedit/elements/network/GNEEdge.h>
#include <netedit/elements/network/GNELane.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/additional/GNEAdditional.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>

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

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::SelectorParentLanes,        FXGroupBox,         ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorChildEdges,         FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorChildLanes,         FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


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
    // define colors
    myCandidateLaneColor = RGBColor(0, 64, 0, 255);
    mySelectedLaneColor = RGBColor::GREEN;
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
    const auto& tagValues = myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTagProperties();
    // abort if there isn't at least two lanes
    if (mySelectedLanes.size() < 2) {
        WRITE_WARNING(myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTagProperties().getTagStr() + " requires at least two lanes.");
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myAdditionalFrameParent->myAdditionalAttributes->getAttributesAndValues(true);
    // fill valuesOfElement with Netedit attributes from Frame
    myAdditionalFrameParent->myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, nullptr);
    // Check if ID has to be generated
    if (valuesMap.count(SUMO_ATTR_ID) == 0) {
        valuesMap[SUMO_ATTR_ID] = myAdditionalFrameParent->generateID(nullptr);
    }
    // obtain lane IDs
    std::vector<std::string> laneIDs;
    for (auto i : mySelectedLanes) {
        laneIDs.push_back(i.first->getID());
    }
    valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
    // Obtain clicked position over first lane
    valuesMap[SUMO_ATTR_POSITION] = toString(mySelectedLanes.front().second);
    // Obtain clicked position over last lane
    valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectedLanes.back().second);
    // parse common attributes
    if (!myAdditionalFrameParent->buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalFrameParent->myAdditionalAttributes->areValuesValid() == false) {
        myAdditionalFrameParent->myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, myAdditionalFrameParent->getPredefinedTagsMML(), toString(tagValues.getTag()));
        // try to build additional
        if (GNEAdditionalHandler::buildAdditional(myAdditionalFrameParent->myViewNet, true, myAdditionalFrameParent->myAdditionalTagSelector->getCurrentTagProperties().getTag(), SUMOSAXAttrs, nullptr)) {
            // abort consecutive lane selector
            abortConsecutiveLaneSelector();
            // refresh additional attributes
            myAdditionalFrameParent->myAdditionalAttributes->refreshRows();
            return true;
        } else {
            return false;
        }
    }
}


void
GNEAdditionalFrame::SelectorParentLanes::abortConsecutiveLaneSelector() {
    // reset color of all candidate lanes
    for (auto i : myCandidateLanes) {
        i->setSpecialColor(nullptr);
    }
    // clear candidate colors
    myCandidateLanes.clear();
    // reset color of all selected lanes
    for (auto i : mySelectedLanes) {
        i.first->setSpecialColor(nullptr);
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
    lane->setSpecialColor(&mySelectedLaneColor);
    // restore original color of candidates (except already selected)
    for (auto i : myCandidateLanes) {
        if (!isLaneSelected(i)) {
            i->setSpecialColor(nullptr);
        }
    }
    // clear candidate lanes
    myCandidateLanes.clear();
    // fill candidate lanes
    for (auto i : lane->getParentEdge()->getGNEConnections()) {
        // check that possible candidate lane isn't already selected
        if ((lane == i->getLaneFrom()) && (!isLaneSelected(i->getLaneTo()))) {
            // set candidate lane
            i->getLaneTo()->setSpecialColor(&myCandidateLaneColor);
            myCandidateLanes.push_back(i->getLaneTo());
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


const RGBColor&
GNEAdditionalFrame::SelectorParentLanes::getSelectedLaneColor() const {
    return mySelectedLaneColor;
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


std::string
GNEAdditionalFrame::SelectorChildEdges::getEdgeIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedEdgesCheckButton->getCheck()) {
        // get Selected edges
        std::vector<GNEEdge*> selectedEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true);
        // Iterate over selectedEdges and getId
        for (auto i : selectedEdges) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorChildEdges::showSelectorChildEdgesModul(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of net
    /// @todo this function must be improved.
    std::vector<GNEEdge*> vectorOfEdges = myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(false);
    // iterate over edges of net
    for (auto i : vectorOfEdges) {
        // If search criterium is correct, then append ittem
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
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
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
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


std::string
GNEAdditionalFrame::SelectorChildLanes::getLaneIdsSelected() const {
    std::vector<std::string> vectorOfIds;
    if (myUseSelectedLanesCheckButton->getCheck()) {
        // get Selected lanes
        std::vector<GNELane*> selectedLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true);
        // Iterate over selectedLanes and getId
        for (auto i : selectedLanes) {
            vectorOfIds.push_back(i->getID());
        }
    } else {
        // Obtain Id's of list
        for (int i = 0; i < myList->getNumItems(); i++) {
            if (myList->isItemSelected(i)) {
                vectorOfIds.push_back(myList->getItem(i)->getText().text());
            }
        }
    }
    return joinToString(vectorOfIds, " ");
}


void
GNEAdditionalFrame::SelectorChildLanes::showSelectorChildLanesModul(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(false);
    for (auto i : vectorOfLanes) {
        if (i->getID().find(search) != std::string::npos) {
            myList->appendItem(i->getID().c_str());
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
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true).size() > 0) {
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

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Additionals") {

    // create item Selector modul for additionals
    myAdditionalTagSelector = new GNEFrameModuls::TagSelector(this, GNETagProperties::TagType::ADDITIONALELEMENT);

    // Create additional parameters
    myAdditionalAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);

    // Create Netedit parameter
    myNeteditAttributes = new GNEFrameAttributesModuls::NeteditAttributes(this);

    // Create consecutive Lane Selector
    mySelectorParentLanes = new SelectorParentLanes(this);

    // Create selector parent
    myParentAdditional = new GNEFrameModuls::SelectorParent(this);

    /// Create list for SelectorChildEdges
    mySelectorChildEdges = new SelectorChildEdges(this);

    /// Create list for SelectorChildLanes
    mySelectorChildLanes = new SelectorChildLanes(this);

    // set BusStop as default additional
    myAdditionalTagSelector->setCurrentTag(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {}


void
GNEAdditionalFrame::show() {
    // refresh item selector
    myAdditionalTagSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that current selected additional is valid
    if (myAdditionalTagSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return false;
    }
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = myAdditionalTagSelector->getCurrentTagProperties();
    // Declare map to keep attributes obtained in frame
    std::map<SumoXMLAttr, std::string> valuesMap = myAdditionalAttributes->getAttributesAndValues(true);
    // fill netedit attributes
    if (!myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, objectsUnderCursor.getLaneFront())) {
        return false;
    }
    // If element owns an parent additional, get id of parent from ParentAdditionalSelector
    if (tagValues.hasParent() && !buildAdditionalWithParent(valuesMap, objectsUnderCursor.getAdditionalFront(), tagValues)) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if (tagValues.hasAttribute(SUMO_ATTR_EDGE) || (tagValues.getTag() == SUMO_TAG_VAPORIZER)) {
        return buildAdditionalOverEdge(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else if (tagValues.hasAttribute(SUMO_ATTR_LANE)) {
        return buildAdditionalOverLane(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else if (tagValues.getTag() == SUMO_TAG_E2DETECTOR_MULTILANE) {
        return buildAdditionalOverLanes(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else {
        return buildAdditionalOverView(valuesMap, tagValues);
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
    return mySelectorParentLanes;
}


void
GNEAdditionalFrame::tagSelected() {
    if (myAdditionalTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_NOTHING) {
        // show additional attributes modul
        myAdditionalAttributes->showAttributesCreatorModul(myAdditionalTagSelector->getCurrentTagProperties(), {});
        // show netedit attributes
        myNeteditAttributes->showNeteditAttributesModul(myAdditionalTagSelector->getCurrentTagProperties());
        // Show myAdditionalFrameParent if we're adding a additional with parent
        if (myAdditionalTagSelector->getCurrentTagProperties().hasParent()) {
            myParentAdditional->showSelectorParentModul(myAdditionalTagSelector->getCurrentTagProperties().getParentTag());
        } else {
            myParentAdditional->hideSelectorParentModul();
        }
        // Show SelectorChildEdges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (myAdditionalTagSelector->getCurrentTagProperties().hasAttribute(SUMO_ATTR_EDGES)) {
            mySelectorChildEdges->showSelectorChildEdgesModul();
        } else {
            mySelectorChildEdges->hideSelectorChildEdgesModul();
        }
        // Show SelectorChildLanes or consecutive lane selector if we're adding an additional that own the attribute SUMO_ATTR_LANES
        if (myAdditionalTagSelector->getCurrentTagProperties().hasAttribute(SUMO_ATTR_LANES)) {
            if (myAdditionalTagSelector->getCurrentTagProperties().hasParent() &&
                    (myAdditionalTagSelector->getCurrentTagProperties().getParentTag() == SUMO_TAG_LANE)) {
                // show selector parent lane and hide selector child lane
                mySelectorParentLanes->showSelectorParentLanesModul();
                mySelectorChildLanes->hideSelectorChildLanesModul();
            } else {
                // show selector child lane and hide selector parent lane
                mySelectorChildLanes->showSelectorChildLanesModul();
                mySelectorParentLanes->hideSelectorParentLanesModul();
            }
        } else {
            mySelectorChildLanes->hideSelectorChildLanesModul();
            mySelectorParentLanes->hideSelectorParentLanesModul();
        }
    } else {
        // hide all moduls if additional isn't valid
        myAdditionalAttributes->hideAttributesCreatorModul();
        myNeteditAttributes->hideNeteditAttributesModul();
        myParentAdditional->hideSelectorParentModul();
        mySelectorChildEdges->hideSelectorChildEdgesModul();
        mySelectorChildLanes->hideSelectorChildLanesModul();
        mySelectorParentLanes->hideSelectorParentLanesModul();
    }
}


std::string
GNEAdditionalFrame::generateID(GNENetworkElement* networkElement) const {
    // obtain current number of additionals to generate a new index faster
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myAdditionalTagSelector->getCurrentTagProperties().getTag());
    // obtain tag Properties (only for improve code legilibility
    const auto& tagProperties = myAdditionalTagSelector->getCurrentTagProperties();
    if (networkElement) {
        // special case for vaporizers
        if (tagProperties.getTag() == SUMO_TAG_VAPORIZER) {
            return networkElement->getID();
        } else {
            // generate ID using networkElement
            while (myViewNet->getNet()->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + networkElement->getID() + "_" + toString(additionalIndex), false) != nullptr) {
                additionalIndex++;
            }
            return tagProperties.getTagStr() + "_" + networkElement->getID() + "_" + toString(additionalIndex);
        }
    } else {
        // generate ID without networkElement
        while (myViewNet->getNet()->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return tagProperties.getTagStr() + "_" + toString(additionalIndex);
    }
}


bool
GNEAdditionalFrame::buildAdditionalWithParent(std::map<SumoXMLAttr, std::string>& valuesMap, GNEAdditional* additionalParent, const GNETagProperties& tagValues) {
    // if user click over an additional element parent, mark int in ParentAdditionalSelector
    if (additionalParent && (additionalParent->getTagProperty().getTag() == tagValues.getParentTag())) {
        valuesMap[GNE_ATTR_PARENT] = additionalParent->getID();
        myParentAdditional->setIDSelected(additionalParent->getID());
    }
    // stop if currently there isn't a valid selected parent
    if (myParentAdditional->getIdSelected() != "") {
        valuesMap[GNE_ATTR_PARENT] = myParentAdditional->getIdSelected();
    } else {
        myAdditionalAttributes->showWarningMessage("A " + toString(tagValues.getParentTag()) + " must be selected before insertion of " + myAdditionalTagSelector->getCurrentTagProperties().getTagStr() + ".");
        return false;
    }
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string>& valuesMap, const GNETagProperties& tagValues) {
    // If additional has a interval defined by a begin or end, check that is valid
    if (tagValues.hasAttribute(SUMO_ATTR_STARTTIME) && tagValues.hasAttribute(SUMO_ATTR_END)) {
        double begin = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_STARTTIME]);
        double end = GNEAttributeCarrier::parse<double>(valuesMap[SUMO_ATTR_END]);
        if (begin > end) {
            myAdditionalAttributes->showWarningMessage("Attribute '" + toString(SUMO_ATTR_STARTTIME) + "' cannot be greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
            return false;
        }
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (tagValues.hasAttribute(SUMO_ATTR_FILE) && valuesMap[SUMO_ATTR_FILE] == "") {
        if ((myAdditionalTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_CALIBRATOR) && (myAdditionalTagSelector->getCurrentTagProperties().getTag() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            valuesMap[SUMO_ATTR_FILE] = (valuesMap[SUMO_ATTR_ID] + ".xml");
        }
    }
    // If element own a list of SelectorChildEdges as attribute
    if (tagValues.hasAttribute(SUMO_ATTR_EDGES) && valuesMap[SUMO_ATTR_EDGES].empty()) {
        // obtain edge IDs
        valuesMap[SUMO_ATTR_EDGES] = mySelectorChildEdges->getEdgeIdsSelected();
        // check if attribute has at least one edge
        if (valuesMap[SUMO_ATTR_EDGES] == "") {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return false;
        }
    }
    // get values of mySelectorChildLanes, if tag correspond to an element that has lanes as children
    if (tagValues.hasAttribute(SUMO_ATTR_LANES) && valuesMap[SUMO_ATTR_LANES].empty()) {
        // obtain lane IDs
        valuesMap[SUMO_ATTR_LANES] = mySelectorChildLanes->getLaneIdsSelected();
        // check if attribute has at least a lane
        if (valuesMap[SUMO_ATTR_LANES] == "") {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_LANE) + "s cannot be empty");
            return false;
        }
    }
    // all ok, continue building additionals
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNETagProperties& tagValues) {
    // check that edge exist
    if (lane) {
        // Get attribute lane's edge
        valuesMap[SUMO_ATTR_EDGE] = lane->getParentEdge()->getID();
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            valuesMap[SUMO_ATTR_ID] = generateID(lane->getParentEdge());
        }
    } else {
        return false;
    }
    // parse common attributes
    if (!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(tagValues.getTag()));
        // try to build additional
        if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalTagSelector->getCurrentTagProperties().getTag(), SUMOSAXAttrs, nullptr)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of children)
            myParentAdditional->refreshSelectorParentModul();
            // clear selected eddges and lanes
            mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
            mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
            // refresh additional attributes
            myAdditionalAttributes->refreshRows();
            return true;
        } else {
            return false;
        }
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLane(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNETagProperties& tagValues) {
    // check that lane exist
    if (lane != nullptr) {
        // Get attribute lane
        valuesMap[SUMO_ATTR_LANE] = lane->getID();
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            valuesMap[SUMO_ATTR_ID] = generateID(lane);
        }
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // set attribute position as mouse position over lane
    valuesMap[SUMO_ATTR_POSITION] = toString(mousePositionOverLane);
    // parse common attributes
    if (!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(tagValues.getTag()));
        // try to build additional
        if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalTagSelector->getCurrentTagProperties().getTag(), SUMOSAXAttrs, nullptr)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of children)
            myParentAdditional->refreshSelectorParentModul();
            // clear selected eddges and lanes
            mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
            mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
            // refresh additional attributes
            myAdditionalAttributes->refreshRows();
            return true;
        } else {
            return false;
        }
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNETagProperties& tagValues) {
    // stop if lane isn't valid
    if (lane == nullptr) {
        return false;
    }
    if (mySelectorParentLanes->isSelectingLanes()) {
        // select clicked lane, but don't build additional
        mySelectorParentLanes->addSelectedLane(lane, myViewNet->getPositionInformation());
        return false;
    } else if (mySelectorParentLanes->getSelectedLanes().empty()) {
        // if there isn't selected lanes, that means that we will be start selecting lanes
        mySelectorParentLanes->startConsecutiveLaneSelector(lane, myViewNet->getPositionInformation());
        return false;
    } else {
        // Check if ID has to be generated
        if (valuesMap.count(SUMO_ATTR_ID) == 0) {
            valuesMap[SUMO_ATTR_ID] = generateID(mySelectorParentLanes->getSelectedLanes().front().first);
        }
        // obtain lane IDs
        std::vector<std::string> laneIDs;
        for (auto i : mySelectorParentLanes->getSelectedLanes()) {
            laneIDs.push_back(i.first->getID());
        }
        valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
        // Check if clicked position over first lane has to be obtained
        if (tagValues.hasAttribute(SUMO_ATTR_POSITION)) {
            valuesMap[SUMO_ATTR_POSITION] = toString(mySelectorParentLanes->getSelectedLanes().front().second);
        }
        // Check if clicked position over last lane has to be obtained
        if (tagValues.hasAttribute(SUMO_ATTR_ENDPOS)) {
            valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectorParentLanes->getSelectedLanes().back().second);
        }
        // parse common attributes
        if (!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
            return false;
        }
        // show warning dialogbox and stop check if input parameters are valid
        if (myAdditionalAttributes->areValuesValid() == false) {
            myAdditionalAttributes->showWarningMessage();
            return false;
        } else {
            // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
            SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(tagValues.getTag()));
            // try to build additional
            if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalTagSelector->getCurrentTagProperties().getTag(), SUMOSAXAttrs, nullptr)) {
                // Refresh additional Parent Selector (For additionals that have a limited number of children)
                myParentAdditional->refreshSelectorParentModul();
                // abort lane selector
                mySelectorParentLanes->abortConsecutiveLaneSelector();
                // refresh additional attributes
                myAdditionalAttributes->refreshRows();
                return true;
            } else {
                // additional cannot be build
                return false;
            }
        }
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverView(std::map<SumoXMLAttr, std::string>& valuesMap, const GNETagProperties& tagValues) {
    // Check if ID has to be generated
    if (valuesMap.count(SUMO_ATTR_ID) == 0) {
        valuesMap[SUMO_ATTR_ID] = generateID(nullptr);
    }
    // Obtain position as the clicked position over view
    valuesMap[SUMO_ATTR_POSITION] = toString(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation()));
    // parse common attributes
    if (!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (myAdditionalAttributes->areValuesValid() == false) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare SUMOSAXAttributesImpl_Cached to convert valuesMap into SUMOSAXAttributes
        SUMOSAXAttributesImpl_Cached SUMOSAXAttrs(valuesMap, getPredefinedTagsMML(), toString(tagValues.getTag()));
        // try to build additional
        if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myAdditionalTagSelector->getCurrentTagProperties().getTag(), SUMOSAXAttrs, nullptr)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of children)
            myParentAdditional->refreshSelectorParentModul();
            // clear selected eddges and lanes
            mySelectorChildEdges->onCmdClearSelection(nullptr, 0, nullptr);
            mySelectorChildLanes->onCmdClearSelection(nullptr, 0, nullptr);
            // refresh additional attributes
            myAdditionalAttributes->refreshRows();
            return true;
        } else {
            return false;
        }
    }
}


/****************************************************************************/
