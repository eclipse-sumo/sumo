/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECrossingFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// The Widget for add Crossing elements
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNECrossingFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Crossing.h"
#include "GNECrossing.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECrossingFrame) GNECrossingMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CROSSINGFRAME_CREATECROSSING,            GNECrossingFrame::onCmdCreateCrossing),
};

FXDEFMAP(GNECrossingFrame::edgesSelector) GNEEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CROSSINGFRAME_USEONLYSELECTEDEDGES,     GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_CLEAREDGESELECTION,     GNECrossingFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALFRAME_INVERTEDGESELECTION,    GNECrossingFrame::edgesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                       GNECrossingFrame::edgesSelector::onCmdHelp),
};

FXDEFMAP(GNECrossingFrame::crossingParameters) GNECrossingParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,                           GNECrossingFrame::crossingParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                                        GNECrossingFrame::crossingParameters::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNECrossingFrame,                     FXVerticalFrame,  GNECrossingMap,           ARRAYNUMBER(GNECrossingMap))
FXIMPLEMENT(GNECrossingFrame::edgesSelector,      FXGroupBox,       GNEEdgesMap,              ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNECrossingFrame::crossingParameters, FXGroupBox,       GNECrossingParametersMap, ARRAYNUMBER(GNECrossingParametersMap))

// ===========================================================================
// static members
// ===========================================================================
RGBColor GNECrossingFrame::crossingParameters::myCandidateColor;
RGBColor GNECrossingFrame::crossingParameters::mySelectedColor;

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECrossingFrame::edgesSelector - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::edgesSelector::edgesSelector(FXComposite* parent, GNECrossingFrame* crossingFrameParent) :
    FXGroupBox(parent, ("selection of " + toString(SUMO_TAG_EDGE) + "s").c_str(), GUIDesignGroupBoxFrame),
    myCrossingFrameParent(crossingFrameParent),
    myCurrentJunction(0) {

    // Create button for selected edges
    myUseSelectedEdges = new FXButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_CROSSINGFRAME_USEONLYSELECTEDEDGES, GUIDesignButton);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(this, ("Clear " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_ADDITIONALFRAME_CLEAREDGESELECTION, GUIDesignButton);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(this, ("Invert " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_ADDITIONALFRAME_INVERTEDGESELECTION, GUIDesignButton);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


GNEJunction*
GNECrossingFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void
GNECrossingFrame::edgesSelector::enableEdgeSelector(GNEJunction* currentJunction) {
    // restore color of all lanes of edge candidates
    restoreEdgeColors();
    // Set current junction
    myCurrentJunction = currentJunction;
    // Update view net to show the new colors
    myCrossingFrameParent->getViewNet()->update();
    // check if use selected eges must be enabled
    myUseSelectedEdges->disable();
    for (auto i : myCurrentJunction->getGNEEdges()) {
        if (gSelected.isSelected(i->getType(), i->getGlID())) {
            myUseSelectedEdges->enable();
        }
    }
    // Enable rest of elements
    helpEdges->enable();
    myClearEdgesSelection->enable();
    myInvertEdgesSelection->enable();
}


void
GNECrossingFrame::edgesSelector::disableEdgeSelector() {
    // disable current junction
    myCurrentJunction = NULL;
    // disable all elements of the edgesSelector
    myUseSelectedEdges->disable();
    helpEdges->disable();
    myClearEdgesSelection->disable();
    myInvertEdgesSelection->disable();
    // Disable crossing parameters
    myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
}


void
GNECrossingFrame::edgesSelector::restoreEdgeColors() {
    if (myCurrentJunction != NULL) {
        // restore color of all lanes of edge candidates
        for (auto i : myCurrentJunction->getGNEEdges()) {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(0);
            }
        }
        // Update view net to show the new colors
        myCrossingFrameParent->getViewNet()->update();
        myCurrentJunction = NULL;
    }
}


long
GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    myCrossingFrameParent->getCrossingParameters()->useSelectedEdges(myCurrentJunction);
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    myCrossingFrameParent->getCrossingParameters()->clearEdges();
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    myCrossingFrameParent->getCrossingParameters()->invertEdges(myCurrentJunction);
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    // @todo finish
    return 0;
}


// ---------------------------------------------------------------------------
// GNECrossingFrame::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNECrossingFrame::crossingParameters::crossingParameters(GNECrossingFrame* crossingFrameParent, GNECrossingFrame::edgesSelector* es) :
    FXGroupBox(crossingFrameParent->myContentFrame, "Crossing parameters", GUIDesignGroupBoxFrame),
    myCrossingFrameParent(crossingFrameParent),
    myEdgeSelector(es),
    myCurrentParametersValid(true) {
    FXHorizontalFrame* crossingParameter = NULL;
    // create label and string textField for edges
    crossingParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCrossingEdgesLabel = new FXLabel(crossingParameter, toString(SUMO_ATTR_EDGES).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingEdges = new FXTextField(crossingParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myCrossingEdgesLabel->disable();
    myCrossingEdges->disable();
    // create label and checkbox for Priority
    crossingParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCrossingPriorityLabel = new FXLabel(crossingParameter, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingPriorityCheckButton = new FXCheckButton(crossingParameter, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myCrossingPriorityLabel->disable();
    myCrossingPriorityCheckButton->disable();
    // create label and textfield for width
    crossingParameter = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myCrossingWidthLabel = new FXLabel(crossingParameter, toString(SUMO_ATTR_WIDTH).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingWidth = new FXTextField(crossingParameter, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myCrossingWidthLabel->disable();
    myCrossingWidth->disable();
    // Create help button
    myHelpCrossingAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    myHelpCrossingAttribute->disable();
    // set colors
    myCandidateColor = RGBColor(0, 64, 0, 255);
    mySelectedColor = RGBColor::GREEN;
}


GNECrossingFrame::crossingParameters::~crossingParameters() {}


void
GNECrossingFrame::crossingParameters::enableCrossingParameters() {
    // Enable all elements of the crossing frames
    myCrossingEdgesLabel->enable();
    myCrossingEdges->enable();
    myCrossingPriorityLabel->enable();
    myCrossingPriorityCheckButton->enable();
    myCrossingWidthLabel->enable();
    myCrossingWidth->enable();
    myHelpCrossingAttribute->enable();
    // set values of parameters
    onCmdSetAttribute(0, 0, 0);
    myCrossingPriorityCheckButton->setCheck(GNEAttributeCarrier::getDefaultValue<bool>(SUMO_TAG_CROSSING, SUMO_ATTR_PRIORITY));
    myCrossingWidth->setText(GNEAttributeCarrier::getDefaultValue<std::string>(SUMO_TAG_CROSSING, SUMO_ATTR_WIDTH).c_str());
    myCrossingWidth->setTextColor(FXRGB(0, 0, 0));
}


void
GNECrossingFrame::crossingParameters::disableCrossingParameters() {
    // clear all values of parameters
    myCrossingEdges->setText("");
    myCrossingPriorityCheckButton->setCheck(false);
    myCrossingPriorityCheckButton->setText("false");
    myCrossingWidth->setText("");
    // Disable all elements of the crossing frames
    myCrossingEdgesLabel->disable();
    myCrossingEdges->disable();
    myCrossingPriorityLabel->disable();
    myCrossingPriorityCheckButton->disable();
    myCrossingWidthLabel->disable();
    myCrossingWidth->disable();
    myHelpCrossingAttribute->disable();
    myCrossingFrameParent->setCreateCrossingButton(false);
}


bool
GNECrossingFrame::crossingParameters::isCrossingParametersEnabled() const {
    return myCrossingEdgesLabel->isEnabled();
}


void
GNECrossingFrame::crossingParameters::markEdge(GNEEdge* edge) {
    GNEJunction* currentJunction = myCrossingFrameParent->getEdgeSelector()->getCurrentJunction();
    if (currentJunction != NULL) {
        // Check if edge belongs to junction's edge
        if (std::find(currentJunction->getGNEEdges().begin(), currentJunction->getGNEEdges().end(), edge) != currentJunction->getGNEEdges().end()) {
            // Update text field with the new edge
            std::vector<std::string> crossingEdges = GNEAttributeCarrier::parse<std::vector<std::string> > (myCrossingEdges->getText().text());
            // Check if new edge must be added or removed
            std::vector<std::string>::iterator itFinder = std::find(crossingEdges.begin(), crossingEdges.end(), edge->getID());
            if (itFinder == crossingEdges.end()) {
                crossingEdges.push_back(edge->getID());
            } else {
                crossingEdges.erase(itFinder);
            }
            myCrossingEdges->setText(joinToString(crossingEdges, " ").c_str());
        }
        // Update colors and attributes
        onCmdSetAttribute(0, 0, 0);
    }
}


void
GNECrossingFrame::crossingParameters::clearEdges() {
    myCrossingEdges->setText("");
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


void
GNECrossingFrame::crossingParameters::invertEdges(GNEJunction* parentJunction) {
    std::vector<std::string> crossingEdges;
    for (auto i : parentJunction->getGNEEdges()) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), i) == myCurrentSelectedEdges.end()) {
            crossingEdges.push_back(i->getID());
        }
    }
    myCrossingEdges->setText(joinToString(crossingEdges, " ").c_str());
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


void
GNECrossingFrame::crossingParameters::useSelectedEdges(GNEJunction* parentJunction) {
    std::vector<std::string> crossingEdges;
    for (auto i : parentJunction->getGNEEdges()) {
        if (gSelected.isSelected(i->getType(), i->getGlID())) {
            crossingEdges.push_back(i->getID());
        }
    }
    myCrossingEdges->setText(joinToString(crossingEdges, " ").c_str());
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


std::vector<NBEdge*>
GNECrossingFrame::crossingParameters::getCrossingEdges() const {
    std::vector<NBEdge*> NBEdgeVector;
    // Iterate over myCurrentSelectedEdges
    for (auto i : myCurrentSelectedEdges) {
        NBEdgeVector.push_back(i->getNBEdge());
    }
    return NBEdgeVector;
}


bool
GNECrossingFrame::crossingParameters::getCrossingPriority() const {
    if (myCrossingPriorityCheckButton->getCheck()) {
        return true;
    } else {
        return false;
    }
}


bool
GNECrossingFrame::crossingParameters::isCurrentParametersValid() const {
    return myCurrentParametersValid;
}


double
GNECrossingFrame::crossingParameters::getCrossingWidth() const {
    return GNEAttributeCarrier::parse<double>(myCrossingWidth->getText().text());
}


const RGBColor&
GNECrossingFrame::crossingParameters::getCandidateColor() const {
    return myCandidateColor;
}


const RGBColor&
GNECrossingFrame::crossingParameters::getSelectedColor() const {
    return mySelectedColor;
}


long
GNECrossingFrame::crossingParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    myCurrentParametersValid = true;
    // get string vector with the edges
    std::vector<std::string> crossingEdges = GNEAttributeCarrier::parse<std::vector<std::string> > (myCrossingEdges->getText().text());
    // Clear selected edges
    myCurrentSelectedEdges.clear();
    // iterate over vector of edge IDs
    for (auto i : crossingEdges) {
        GNEEdge* edge = myCrossingFrameParent->getViewNet()->getNet()->retrieveEdge(i, false);
        GNEJunction* currentJunction = myCrossingFrameParent->getEdgeSelector()->getCurrentJunction();
        // Check that edge exists and belongs to Junction
        if (edge == 0) {
            myCurrentParametersValid = false;
        } else if (std::find(currentJunction->getGNEEdges().begin(), currentJunction->getGNEEdges().end(), edge) == currentJunction->getGNEEdges().end()) {
            myCurrentParametersValid = false;
        } else {
            // select or unselected edge
            auto itFinder = std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), edge);
            if (itFinder == myCurrentSelectedEdges.end()) {
                myCurrentSelectedEdges.push_back(edge);
            } else {
                myCurrentSelectedEdges.erase(itFinder);
            }
        }
    }

    // change color of textfield dependig of myCurrentParametersValid
    if (myCurrentParametersValid) {
        myCrossingEdges->setTextColor(FXRGB(0, 0, 0));
        myCrossingEdges->killFocus();
    } else {
        myCrossingEdges->setTextColor(FXRGB(255, 0, 0));
        myCurrentParametersValid = false;
    }

    // Update colors of edges
    for (auto i : myEdgeSelector->getCurrentJunction()->getGNEEdges()) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), i) != myCurrentSelectedEdges.end()) {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(&mySelectedColor);
            }
        } else {
            for (auto j : i->getLanes()) {
                j->setSpecialColor(&myCandidateColor);
            }
        }
    }
    // Update view net
    myCrossingFrameParent->getViewNet()->update();

    // Check that at least there are a selected edge
    if (crossingEdges.empty()) {
        myCurrentParametersValid = false;
    }

    // change label of crossing priority
    if (myCrossingPriorityCheckButton->getCheck()) {
        myCrossingPriorityCheckButton->setText("true");
    } else {
        myCrossingPriorityCheckButton->setText("false");
    }

    // Check width
    if (GNEAttributeCarrier::canParse<double>(myCrossingWidth->getText().text()) &&
            GNEAttributeCarrier::parse<double>(myCrossingWidth->getText().text()) > 0) {
        myCrossingWidth->setTextColor(FXRGB(0, 0, 0));
        myCrossingWidth->killFocus();
    } else {
        myCrossingWidth->setTextColor(FXRGB(255, 0, 0));
        myCurrentParametersValid = false;
    }

    // Enable or disable create crossing button depending of the current parameters
    myCrossingFrameParent->setCreateCrossingButton(myCurrentParametersValid);
    return 0;
}


long
GNECrossingFrame::crossingParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(SUMO_TAG_CROSSING)).c_str(), GUIDesignDialogBox);
    // Create FXTable
    FXTable* myTable = new FXTable(helpDialog, this, MID_TABLE, TABLE_READONLY);
    myTable->setVisibleRows((FXint)(GNEAttributeCarrier::allowedAttributes(SUMO_TAG_CROSSING).size()));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(GNEAttributeCarrier::allowedAttributes(SUMO_TAG_CROSSING).size()), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 80);
    int maxSizeColumnDefinitions = 0;
    // Iterate over vector of additional parameters
    for (int i = 0; i < (int)GNEAttributeCarrier::allowedAttributes(SUMO_TAG_CROSSING).size(); i++) {
        SumoXMLAttr attr = GNEAttributeCarrier::allowedAttributes(SUMO_TAG_CROSSING).at(i).first;
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(SUMO_TAG_CROSSING, attr)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(SUMO_TAG_CROSSING, attr)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isTime(SUMO_TAG_CROSSING, attr)) {
            type->setText("time");
        } else if (GNEAttributeCarrier::isBool(SUMO_TAG_CROSSING, attr)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isString(SUMO_TAG_CROSSING, attr)) {
            type->setText("string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(SUMO_TAG_CROSSING, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(SUMO_TAG_CROSSING, attr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(SUMO_TAG_CROSSING, attr).size());
        }
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECrossingFrame - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::GNECrossingFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Crossings") {
    // Create Groupbox for labels
    myGroupBoxLabel = new FXGroupBox(myContentFrame, "Junction", GUIDesignGroupBoxFrame);
    myCurrentJunctionLabel = new FXLabel(myGroupBoxLabel, "No junction selected", 0, GUIDesignLabelLeft);

    // Create edge Selector
    myEdgeSelector = new edgesSelector(myContentFrame, this);

    // Create crossingParameters
    myCrossingParameters = new crossingParameters(this, myEdgeSelector);

    // Create groupbox for create crossings
    myGroupBoxButtons = new FXGroupBox(myContentFrame, "Create", GUIDesignGroupBoxFrame);
    myCreateCrossingButton = new FXButton(myGroupBoxButtons, "Create crossing", 0, this, MID_GNE_CROSSINGFRAME_CREATECROSSING, GUIDesignButton);
    myCreateCrossingButton->disable();

    // Create groupbox and labels for legends
    myGroupBoxLegend = new FXGroupBox(myContentFrame, "Legend", GUIDesignGroupBoxFrame);
    myColorCandidateLabel = new FXLabel(myGroupBoxLegend, "Candidate", 0, GUIDesignLabelLeft);
    myColorCandidateLabel->setBackColor(MFXUtils::getFXColor(myCrossingParameters->getCandidateColor()));
    myColorSelectedLabel = new FXLabel(myGroupBoxLegend, "Selected", 0, GUIDesignLabelLeft);
    myColorSelectedLabel->setBackColor(MFXUtils::getFXColor(myCrossingParameters->getSelectedColor()));

    // disable edge selector
    myEdgeSelector->disableEdgeSelector();
}


GNECrossingFrame::~GNECrossingFrame() {
    gSelected.remove2Update();
}


void
GNECrossingFrame::hide() {
    // restore color of all lanes of edge candidates
    myEdgeSelector->restoreEdgeColors();
    // hide frame
    GNEFrame::hide();
}


bool
GNECrossingFrame::addCrossing(GNENetElement* netElement) {
    // cast netElement
    GNEJunction* currentJunction = dynamic_cast<GNEJunction*>(netElement);
    GNEEdge* selectedEdge = dynamic_cast<GNEEdge*>(netElement);
    GNELane* selectedLane = dynamic_cast<GNELane*>(netElement);

    // If current element is a junction
    if (currentJunction != NULL) {
        // change label
        myCurrentJunctionLabel->setText((std::string("Current Junction: ") + currentJunction->getID()).c_str());
        // Enable edge selector and crossing parameters
        myEdgeSelector->enableEdgeSelector(currentJunction);
        myCrossingParameters->enableCrossingParameters();
        // clears selected edges
        myCrossingParameters->clearEdges();
    } else if (selectedEdge != NULL) {
        myCrossingParameters->markEdge(selectedEdge);
    } else if (selectedLane != NULL) {
        myCrossingParameters->markEdge(&selectedLane->getParentEdge());
    } else {
        // set default label
        myCurrentJunctionLabel->setText("No junction selected");
        // restore  color of all lanes of edge candidates
        myEdgeSelector->restoreEdgeColors();
        // Disable edge selector
        myEdgeSelector->disableEdgeSelector();
    }
    return false;
}


long
GNECrossingFrame::onCmdCreateCrossing(FXObject*, FXSelector, void*) {
    // First check that current parameters are valid
    if (myCrossingParameters->isCurrentParametersValid()) {
        // iterate over junction's crossing to find duplicated crossings
        if (myEdgeSelector->getCurrentJunction()->getNBNode()->checkCrossingDuplicated(myCrossingParameters->getCrossingEdges()) == false) {
            // create new crossing
            myViewNet->getUndoList()->add(new GNEChange_Crossing(myEdgeSelector->getCurrentJunction(),
                                          myCrossingParameters->getCrossingEdges(),
                                          myCrossingParameters->getCrossingWidth(),
                                          myCrossingParameters->getCrossingPriority(),
                                          -1,
                                          PositionVector::EMPTY,
                                          false, true), true);
            // clear selected edges
            myEdgeSelector->onCmdClearSelection(0, 0, 0);
        } else {
            WRITE_WARNING("There is already another crossing with the same edges in the junction; Duplicated crossing aren't allowed.");
        }
    }
    return 1;
}


void
GNECrossingFrame::setCreateCrossingButton(bool value) {
    if (value) {
        myCreateCrossingButton->enable();
    } else {
        myCreateCrossingButton->disable();
    }
}


GNECrossingFrame::edgesSelector*
GNECrossingFrame::getEdgeSelector() const {
    return myEdgeSelector;
}


GNECrossingFrame::crossingParameters*
GNECrossingFrame::getCrossingParameters() const {
    return myCrossingParameters;
}

/****************************************************************************/
