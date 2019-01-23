/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
// The Widget for add additional elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>

#include "GNEAdditionalFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame::SelectorLaneParents) ConsecutiveLaneSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_STOPSELECTION,  GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_ABORTSELECTION, GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection),
};

FXDEFMAP(GNEAdditionalFrame::SelectorEdgeChilds) SelectorParentEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge),
};

FXDEFMAP(GNEAdditionalFrame::SelectorLaneChilds) SelectorParentLanesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_USESELECTED,        GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEARSELECTION,     GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTSELECTION,    GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED,  MID_GNE_ADDITIONALFRAME_SEARCH,             GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_SELECT,             GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneParents,        FXGroupBox,         ConsecutiveLaneSelectorMap,     ARRAYNUMBER(ConsecutiveLaneSelectorMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorEdgeChilds,         FXGroupBox,         SelectorParentEdgesMap,         ARRAYNUMBER(SelectorParentEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::SelectorLaneChilds,         FXGroupBox,         SelectorParentLanesMap,         ARRAYNUMBER(SelectorParentLanesMap))


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorLaneParents - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneParents::SelectorLaneParents(GNEAdditionalFrame* additionalFrameParent) :
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


GNEAdditionalFrame::SelectorLaneParents::~SelectorLaneParents() {}


void
GNEAdditionalFrame::SelectorLaneParents::showSelectorLaneParentsModul() {
    // abort current selection before show
    abortConsecutiveLaneSelector();
    // show FXGroupBox
    FXGroupBox::show();
}


void
GNEAdditionalFrame::SelectorLaneParents::hideSelectorLaneParentsModul() {
    // abort current selection before hide
    abortConsecutiveLaneSelector();
    // hide FXGroupBox
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorLaneParents::startConsecutiveLaneSelector(GNELane* lane, const Position& clickedPosition) {
    // Only start selection if SelectorLaneParents modul is shown
    if (shown()) {
        // change buttons
        myStopSelectingButton->enable();
        myAbortSelectingButton->enable();
        // add lane
        addSelectedLane(lane, clickedPosition);
    }
}


bool
GNEAdditionalFrame::SelectorLaneParents::stopConsecutiveLaneSelector() {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = myAdditionalFrameParent->myItemSelector->getCurrentTagProperties();
    // abort if there isn't at least two lanes
    if (mySelectedLanes.size() < 2) {
        WRITE_WARNING(myAdditionalFrameParent->myItemSelector->getCurrentTagProperties().getTagStr() + " requieres at least two lanes.");
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return false;
    }
    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myAdditionalFrameParent->myAdditionalAttributes->getAttributesAndValues();
    // fill valuesOfElement with Netedit attributes from Frame
    myAdditionalFrameParent->myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, nullptr);
    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = myAdditionalFrameParent->generateID(nullptr);
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
    } else if (GNEAdditionalHandler::buildAdditional(myAdditionalFrameParent->myViewNet, true, myAdditionalFrameParent->myItemSelector->getCurrentTagProperties().getTag(), valuesMap)) {
        // abort consecutive lane selector
        abortConsecutiveLaneSelector();
        return true;
    } else {
        return false;
    }
}


void
GNEAdditionalFrame::SelectorLaneParents::abortConsecutiveLaneSelector() {
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
    myAdditionalFrameParent->getViewNet()->update();
}


bool
GNEAdditionalFrame::SelectorLaneParents::addSelectedLane(GNELane* lane, const Position& clickedPosition) {
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
    mySelectedLanes.push_back(std::make_pair(lane, lane->getShape().nearest_offset_to_point2D(clickedPosition) / lane->getLengthGeometryFactor()));
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
    for (auto i : lane->getParentEdge().getGNEConnections()) {
        // check that possible candidate lane isn't already selected
        if ((lane == i->getLaneFrom()) && (!isLaneSelected(i->getLaneTo()))) {
            // set candidate lane
            i->getLaneTo()->setSpecialColor(&myCandidateLaneColor);
            myCandidateLanes.push_back(i->getLaneTo());
        }
    }
    // update view (due colors)
    myAdditionalFrameParent->getViewNet()->update();
    return true;
}


void
GNEAdditionalFrame::SelectorLaneParents::removeLastSelectedLane() {
    if (mySelectedLanes.size() > 1) {
        mySelectedLanes.pop_back();
    } else {
        WRITE_WARNING("First lane cannot be removed");
    }
}


bool
GNEAdditionalFrame::SelectorLaneParents::isSelectingLanes() const {
    return myStopSelectingButton->isEnabled();
}


bool
GNEAdditionalFrame::SelectorLaneParents::isShown() const {
    return shown();
}


const RGBColor&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLaneColor() const {
    return mySelectedLaneColor;
}


const std::vector<std::pair<GNELane*, double> >&
GNEAdditionalFrame::SelectorLaneParents::getSelectedLanes() const {
    return mySelectedLanes;
}


long
GNEAdditionalFrame::SelectorLaneParents::onCmdStopSelection(FXObject*, FXSelector, void*) {
    stopConsecutiveLaneSelector();
    return 0;
}


long
GNEAdditionalFrame::SelectorLaneParents::onCmdAbortSelection(FXObject*, FXSelector, void*) {
    abortConsecutiveLaneSelector();
    return 0;
}


bool
GNEAdditionalFrame::SelectorLaneParents::isLaneSelected(GNELane* lane) const {
    for (auto i : mySelectedLanes) {
        if (i.first == lane) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorAdditionalParent - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorAdditionalParent::SelectorAdditionalParent(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Parent selector", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent),
    myAdditionalTypeParent(SUMO_TAG_NOTHING) {
    // Create label with the type of SelectorAdditionalParent
    myFirstAdditionalParentsLabel = new FXLabel(this, "No additional selected", nullptr, GUIDesignLabelLeftThick);
    // Create list
    myFirstAdditionalParentsList = new FXList(this, this, MID_GNE_SET_TYPE, GUIDesignListSingleElementFixedHeight);
    // Hide List
    hideSelectorAdditionalParentModul();
}


GNEAdditionalFrame::SelectorAdditionalParent::~SelectorAdditionalParent() {}


std::string
GNEAdditionalFrame::SelectorAdditionalParent::getIdSelected() const {
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->isItemSelected(i)) {
            return myFirstAdditionalParentsList->getItem(i)->getText().text();
        }
    }
    return "";
}


void
GNEAdditionalFrame::SelectorAdditionalParent::setIDSelected(const std::string& id) {
    // first unselect all
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        myFirstAdditionalParentsList->getItem(i)->setSelected(false);
    }
    // select element if correspond to given ID
    for (int i = 0; i < myFirstAdditionalParentsList->getNumItems(); i++) {
        if (myFirstAdditionalParentsList->getItem(i)->getText().text() == id) {
            myFirstAdditionalParentsList->getItem(i)->setSelected(true);
        }
    }
    // recalc myFirstAdditionalParentsList
    myFirstAdditionalParentsList->recalc();
}


bool
GNEAdditionalFrame::SelectorAdditionalParent::showSelectorAdditionalParentModul(SumoXMLTag additionalType) {
    // make sure that we're editing an additional tag
    auto listOfTags = GNEAttributeCarrier::allowedTagsByCategory(GNEAttributeCarrier::TAGProperty::TAGPROPERTY_ADDITIONAL, false);
    for (auto i : listOfTags) {
        if (i == additionalType) {
            myAdditionalTypeParent = additionalType;
            myFirstAdditionalParentsLabel->setText(("Parent type: " + toString(additionalType)).c_str());
            refreshSelectorAdditionalParentModul();
            show();
            return true;
        }
    }
    return false;
}


void
GNEAdditionalFrame::SelectorAdditionalParent::hideSelectorAdditionalParentModul() {
    myAdditionalTypeParent = SUMO_TAG_NOTHING;
    hide();
}


void
GNEAdditionalFrame::SelectorAdditionalParent::refreshSelectorAdditionalParentModul() {
    myFirstAdditionalParentsList->clearItems();
    if (myAdditionalTypeParent != SUMO_TAG_NOTHING) {
        // fill list with IDs of additionals
        for (auto i : myAdditionalFrameParent->getViewNet()->getNet()->getAdditionalByType(myAdditionalTypeParent)) {
            myFirstAdditionalParentsList->appendItem(i.first.c_str());
        }
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::SelectorEdgeChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorEdgeChilds::SelectorEdgeChilds(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Edges", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create menuCheck for selected edges
    myUseSelectedEdgesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

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
    hideSelectorEdgeChildsModul();
}


GNEAdditionalFrame::SelectorEdgeChilds::~SelectorEdgeChilds() {}


std::string
GNEAdditionalFrame::SelectorEdgeChilds::getEdgeIdsSelected() const {
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
GNEAdditionalFrame::SelectorEdgeChilds::showSelectorEdgeChildsModul(std::string search) {
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
GNEAdditionalFrame::SelectorEdgeChilds::hideSelectorEdgeChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorEdgeChilds::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdgesCheckButton->enable();
    } else {
        myUseSelectedEdgesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorEdgeChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorEdgeChilds that contains the searched string
    showSelectorEdgeChildsModul(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorEdgeChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
// GNEAdditionalFrame::SelectorLaneChilds - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::SelectorLaneChilds::SelectorLaneChilds(GNEAdditionalFrame* additionalFrameParent) :
    FXGroupBox(additionalFrameParent->myContentFrame, "Lanes", GUIDesignGroupBoxFrame),
    myAdditionalFrameParent(additionalFrameParent) {
    // Create CheckBox for selected lanes
    myUseSelectedLanesCheckButton = new FXCheckButton(this, ("Use selected " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_ADDITIONALFRAME_USESELECTED, GUIDesignCheckButtonAttribute);

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
    hideSelectorLaneChildsModul();
}


GNEAdditionalFrame::SelectorLaneChilds::~SelectorLaneChilds() {}


std::string
GNEAdditionalFrame::SelectorLaneChilds::getLaneIdsSelected() const {
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
GNEAdditionalFrame::SelectorLaneChilds::showSelectorLaneChildsModul(std::string search) {
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
GNEAdditionalFrame::SelectorLaneChilds::hideSelectorLaneChildsModul() {
    FXGroupBox::hide();
}


void
GNEAdditionalFrame::SelectorLaneChilds::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myAdditionalFrameParent->getViewNet()->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanesCheckButton->enable();
    } else {
        myUseSelectedLanesCheckButton->disable();
    }
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
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
GNEAdditionalFrame::SelectorLaneChilds::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of SelectorLaneChilds that contains the searched string
    showSelectorLaneChildsModul(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNEAdditionalFrame::SelectorLaneChilds::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
    myItemSelector = new ItemSelector(this, GNEAttributeCarrier::TAGProperty::TAGPROPERTY_ADDITIONAL);

    // Create additional parameters
    myAdditionalAttributes = new ACAttributes(this);

    // Create Netedit parameter
    myNeteditAttributes = new NeteditAttributes(this);

    // Create consecutive Lane Selector
    mySelectorLaneParents = new SelectorLaneParents(this);

    // Create create list for additional Set
    mySelectorAdditionalParent = new SelectorAdditionalParent(this);

    /// Create list for SelectorEdgeChilds
    mySelectorEdgeChilds = new SelectorEdgeChilds(this);

    /// Create list for SelectorLaneChilds
    mySelectorLaneChilds = new SelectorLaneChilds(this);

    // set BusStop as default additional
    myItemSelector->setCurrentTypeTag(SUMO_TAG_BUS_STOP);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {}


void
GNEAdditionalFrame::show() {
    // refresh item selector
    myItemSelector->refreshTagProperties();
    // show frame
    GNEFrame::show();
}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNet::ObjectsUnderCursor& objectsUnderCursor) {
    // first check that current selected additional is valid
    if (myItemSelector->getCurrentTagProperties().getTag() == SUMO_TAG_NOTHING) {
        myViewNet->setStatusBarText("Current selected additional isn't valid.");
        return false;
    }

    // obtain tagproperty (only for improve code legibility)
    const auto& tagValues = myItemSelector->getCurrentTagProperties();

    // Declare map to keep attributes from Frames from Frame
    std::map<SumoXMLAttr, std::string> valuesMap = myAdditionalAttributes->getAttributesAndValues();

    // fill netedit attributes
    if (!myNeteditAttributes->getNeteditAttributesAndValues(valuesMap, objectsUnderCursor.getLaneFront())) {
        return false;
    }

    // If element owns an additional parent, get id of parent from AdditionalParentSelector
    if (tagValues.hasParent() && !buildAdditionalWithParent(valuesMap, objectsUnderCursor.getAdditionalFront(), tagValues)) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if (tagValues.canBePlacedOverEdge()) {
        return buildAdditionalOverEdge(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else if (tagValues.canBePlacedOverLane()) {
        return buildAdditionalOverLane(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else if (tagValues.canBePlacedOverLanes()) {
        return buildAdditionalOverLanes(valuesMap, objectsUnderCursor.getLaneFront(), tagValues);
    } else {
        return buildAdditionalOverView(valuesMap, tagValues);
    }
}


void
GNEAdditionalFrame::showSelectorLaneChildsModul() {
    // Show frame
    GNEFrame::show();
    // Update UseSelectedLane CheckBox
    mySelectorEdgeChilds->updateUseSelectedEdges();
    // Update UseSelectedLane CheckBox
    mySelectorLaneChilds->updateUseSelectedLanes();
}


GNEAdditionalFrame::SelectorLaneParents*
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return mySelectorLaneParents;
}


void
GNEAdditionalFrame::enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties) {
    // show additional attributes modul
    myAdditionalAttributes->showACAttributesModul(tagProperties);
    // show netedit attributes
    myNeteditAttributes->showNeteditAttributesModul(tagProperties);
    // Show myAdditionalFrameParent if we're adding a additional with parent
    if (tagProperties.hasParent()) {
        mySelectorAdditionalParent->showSelectorAdditionalParentModul(tagProperties.getParentTag());
    } else {
        mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
    }
    // Show SelectorEdgeChilds if we're adding an additional that own the attribute SUMO_ATTR_EDGES
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGES)) {
        mySelectorEdgeChilds->showSelectorEdgeChildsModul();
    } else {
        mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
    }
    // Show SelectorLaneChilds or consecutive lane selector if we're adding an additional that own the attribute SUMO_ATTR_LANES
    if (tagProperties.hasAttribute(SUMO_ATTR_LANES)) {
        if (tagProperties.hasParent() && tagProperties.getParentTag() == SUMO_TAG_LANE) {
            // show selector lane parent and hide selector lane child
            mySelectorLaneParents->showSelectorLaneParentsModul();
            mySelectorLaneChilds->hideSelectorLaneChildsModul();
        } else {
            // show selector lane child and hide selector lane parent
            mySelectorLaneChilds->showSelectorLaneChildsModul();
            mySelectorLaneParents->hideSelectorLaneParentsModul();
        }
    } else {
        mySelectorLaneChilds->hideSelectorLaneChildsModul();
        mySelectorLaneParents->hideSelectorLaneParentsModul();
    }
}


void
GNEAdditionalFrame::disableModuls() {
    // hide all moduls if additional isn't valid
    myAdditionalAttributes->hideACAttributesModul();
    myNeteditAttributes->hideNeteditAttributesModul();
    mySelectorAdditionalParent->hideSelectorAdditionalParentModul();
    mySelectorEdgeChilds->hideSelectorEdgeChildsModul();
    mySelectorLaneChilds->hideSelectorLaneChildsModul();
    mySelectorLaneParents->hideSelectorLaneParentsModul();
}


std::string
GNEAdditionalFrame::generateID(GNENetElement* netElement) const {
    // obtain current number of additionals to generate a new index faster
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myItemSelector->getCurrentTagProperties().getTag());
    // obtain tag Properties (only for improve code legilibility
    const auto& tagProperties = myItemSelector->getCurrentTagProperties();
    if (netElement) {
        // generate ID using netElement
        while (myViewNet->getNet()->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + netElement->getID() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return tagProperties.getTagStr() + "_" + netElement->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without netElement
        while (myViewNet->getNet()->retrieveAdditional(tagProperties.getTag(), tagProperties.getTagStr() + "_" + toString(additionalIndex), false) != nullptr) {
            additionalIndex++;
        }
        return tagProperties.getTagStr() + "_" + toString(additionalIndex);
    }
}


bool
GNEAdditionalFrame::buildAdditionalWithParent(std::map<SumoXMLAttr, std::string>& valuesMap, GNEAdditional* additionalParent, const GNEAttributeCarrier::TagProperties& tagValues) {
    // if user click over an additional element parent, mark int in AdditionalParentSelector
    if (additionalParent && (additionalParent->getTagProperty().getTag() == tagValues.getParentTag())) {
        valuesMap[GNE_ATTR_PARENT] = additionalParent->getID();
        mySelectorAdditionalParent->setIDSelected(additionalParent->getID());
    }
    // stop if currently there isn't a valid selected parent
    if (mySelectorAdditionalParent->getIdSelected() != "") {
        valuesMap[GNE_ATTR_PARENT] = mySelectorAdditionalParent->getIdSelected();
    } else {
        myAdditionalAttributes->showWarningMessage("A " + toString(tagValues.getParentTag()) + " must be selected before insertion of " + myItemSelector->getCurrentTagProperties().getTagStr() + ".");
        return false;
    }
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalCommonAttributes(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues) {
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
        if ((myItemSelector->getCurrentTagProperties().getTag() != SUMO_TAG_CALIBRATOR) && (myItemSelector->getCurrentTagProperties().getTag() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            valuesMap[SUMO_ATTR_FILE] = (valuesMap[SUMO_ATTR_ID] + ".xml");
        }
    }
    // If element own a list of SelectorEdgeChilds as attribute
    if (tagValues.hasAttribute(SUMO_ATTR_EDGES) && !tagValues.canBePlacedOverEdges()) {
        // obtain edge IDs
        valuesMap[SUMO_ATTR_EDGES] = mySelectorEdgeChilds->getEdgeIdsSelected();
        // check if attribute has at least one edge
        if (valuesMap[SUMO_ATTR_EDGES] == "") {
            myAdditionalAttributes->showWarningMessage("List of " + toString(SUMO_TAG_EDGE) + "s cannot be empty");
            return false;
        }
    }
    // get values of mySelectorLaneChilds, if tag correspond to an element that has lanes as childs
    if (tagValues.hasAttribute(SUMO_ATTR_LANES) && !tagValues.canBePlacedOverLanes()) {
        // obtain lane IDs
        valuesMap[SUMO_ATTR_LANES] = mySelectorLaneChilds->getLaneIdsSelected();
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
GNEAdditionalFrame::buildAdditionalOverEdge(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues) {
    // check that edge exist
    if (lane) {
        // Get attribute lane's edge
        valuesMap[SUMO_ATTR_EDGE] = lane->getParentEdge().getID();
        // Generate id of element based on the lane's edge
        valuesMap[SUMO_ATTR_ID] = generateID(&lane->getParentEdge());
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
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTagProperties().getTag(), valuesMap) != nullptr) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLane(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues) {
    // check that lane exist
    if (lane != nullptr) {
        // Get attribute lane
        valuesMap[SUMO_ATTR_LANE] = lane->getID();
        // Generate id of element based on the lane
        valuesMap[SUMO_ATTR_ID] = generateID(lane);
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    double mousePositionOverLane = lane->getShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
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
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTagProperties().getTag(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLanes(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane, const GNEAttributeCarrier::TagProperties& tagValues) {
    // stop if lane isn't valid
    if (lane == nullptr) {
        return false;
    }
    if (mySelectorLaneParents->isSelectingLanes()) {
        // select clicked lane, but don't build additional
        mySelectorLaneParents->addSelectedLane(lane, myViewNet->getPositionInformation());
        return false;
    } else if (mySelectorLaneParents->getSelectedLanes().empty()) {
        // if there isn't selected lanes, that means that we will be start selecting lanes
        mySelectorLaneParents->startConsecutiveLaneSelector(lane, myViewNet->getPositionInformation());
        return false;
    } else {
        // Generate id of element based on the first lane
        valuesMap[SUMO_ATTR_ID] = generateID(mySelectorLaneParents->getSelectedLanes().front().first);
        // obtain lane IDs
        std::vector<std::string> laneIDs;
        for (auto i : mySelectorLaneParents->getSelectedLanes()) {
            laneIDs.push_back(i.first->getID());
        }
        valuesMap[SUMO_ATTR_LANES] = joinToString(laneIDs, " ");
        // Check if clicked position over first lane has to be obtained
        if (tagValues.hasAttribute(SUMO_ATTR_POSITION)) {
            valuesMap[SUMO_ATTR_POSITION] = toString(mySelectorLaneParents->getSelectedLanes().front().second);
        }
        // Check if clicked position over last lane has to be obtained
        if (tagValues.hasAttribute(SUMO_ATTR_ENDPOS)) {
            valuesMap[SUMO_ATTR_ENDPOS] = toString(mySelectorLaneParents->getSelectedLanes().back().second);
        }
        // parse common attributes
        if (!buildAdditionalCommonAttributes(valuesMap, tagValues)) {
            return false;
        }
        // show warning dialogbox and stop check if input parameters are valid
        if (myAdditionalAttributes->areValuesValid() == false) {
            myAdditionalAttributes->showWarningMessage();
            return false;
        } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTagProperties().getTag(), valuesMap)) {
            // Refresh additional Parent Selector (For additionals that have a limited number of childs)
            mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
            // abort lane selector
            mySelectorLaneParents->abortConsecutiveLaneSelector();
            return true;
        } else {
            // additional cannot be build
            return false;
        }
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverView(std::map<SumoXMLAttr, std::string>& valuesMap, const GNEAttributeCarrier::TagProperties& tagValues) {
    // Generate id of element
    valuesMap[SUMO_ATTR_ID] = generateID(nullptr);
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
    } else if (GNEAdditionalHandler::buildAdditional(myViewNet, true, myItemSelector->getCurrentTagProperties().getTag(), valuesMap)) {
        // Refresh additional Parent Selector (For additionals that have a limited number of childs)
        mySelectorAdditionalParent->refreshSelectorAdditionalParentModul();
        // clear selected eddges and lanes
        mySelectorEdgeChilds->onCmdClearSelection(nullptr, 0, nullptr);
        mySelectorLaneChilds->onCmdClearSelection(nullptr, 0, nullptr);
        return true;
    } else {
        return false;
    }
}

/****************************************************************************/
