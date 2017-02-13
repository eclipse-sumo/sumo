/****************************************************************************/
/// @file    GNECrossingFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
/// The Widget for add Crossing elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplCheck.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECrossingFrame) GNECrossingMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CREATE_CROSSING, GNECrossingFrame::onCmdCreateCrossing),
};

FXDEFMAP(GNECrossingFrame::edgesSelector) GNEEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USEONLYSELECTEDEDGES,   GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CLEAREDGESELECTION,    GNECrossingFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INVERTEDGESELECTION,   GNECrossingFrame::edgesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                      GNECrossingFrame::edgesSelector::onCmdHelp),
};

FXDEFMAP(GNECrossingFrame::crossingParameters) GNECrossingParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,   GNECrossingFrame::crossingParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                GNECrossingFrame::crossingParameters::onCmdHelp),
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
    myUseSelectedEdges = new FXButton(this, ("Use selected " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_USEONLYSELECTEDEDGES, GUIDesignButton);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(this, ("clear " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_CLEAREDGESELECTION, GUIDesignButton);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(this, ("invert " + toString(SUMO_TAG_EDGE) + "s").c_str(), 0, this, MID_GNE_INVERTEDGESELECTION, GUIDesignButton);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


GNEJunction*
GNECrossingFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void
GNECrossingFrame::edgesSelector::enableEdgeSelector(GNEJunction* currentJunction) {
    // Set current junction
    myCurrentJunction = currentJunction;
    // Update view net to show the new colors
    myCrossingFrameParent->getViewNet()->update();
    // check if use selected eges must be enabled
    myUseSelectedEdges->disable();
    for (std::vector<GNEEdge*>::const_iterator i = myCurrentJunction->getGNEEdges().begin(); i != myCurrentJunction->getGNEEdges().end(); i++) {
        if (gSelected.isSelected((*i)->getType(), (*i)->getGlID())) {
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
        for (std::vector<GNEEdge*>::const_iterator i = myCurrentJunction->getGNEEdges().begin(); i != myCurrentJunction->getGNEEdges().end(); i++) {
            for (std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                (*j)->setSpecialColor(0);
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
    std::cout << "finish" << std::endl; // @todo finish
    return 0;
}


// ---------------------------------------------------------------------------
// GNECrossingFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNECrossingFrame::crossingParameters::crossingParameters(GNECrossingFrame* crossingFrameParent, GNECrossingFrame::edgesSelector* es) :
    FXGroupBox(crossingFrameParent->myContentFrame, "Crossing parameters", GUIDesignGroupBoxFrame),
    myCrossingFrameParent(crossingFrameParent),
    myEdgeSelector(es),
    myCurrentParametersValid(true) {
    // Create a Matrix for parameters
    myAttributesMatrix = new FXMatrix(this, 2, GUIDesignMatrixAttributes);
    // create label for edges
    myCrossingEdgesLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_EDGES).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingEdgesLabel->disable();
    // create string textField for edges
    myCrossingEdges = new FXTextField(myAttributesMatrix, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldAttributeStr);
    myCrossingEdges->disable();
    // create label for Priority
    myCrossingPriorityLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingPriorityLabel->disable();
    // create CheckBox for Priority
    myCrossingPriority = new FXMenuCheck(myAttributesMatrix, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignMenuCheckAttribute);
    myCrossingPriority->disable();
    // create label for width
    myCrossingWidthLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_WIDTH).c_str(), 0, GUIDesignLabelAttribute);
    myCrossingWidthLabel->disable();
    // create extField for width
    myCrossingWidth = new FXTextField(myAttributesMatrix, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldAttributeReal);
    myCrossingWidth->disable();
    // Create help button
    myHelpCrossingAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonHelp);
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
    myCrossingPriority->enable();
    myCrossingWidthLabel->enable();
    myCrossingWidth->enable();
    myHelpCrossingAttribute->enable();
    // set values of parameters
    onCmdSetAttribute(0, 0, 0);
    myCrossingPriority->setCheck(GNEAttributeCarrier::getDefaultValue<bool>(SUMO_TAG_CROSSING, SUMO_ATTR_PRIORITY));
    myCrossingWidth->setText(GNEAttributeCarrier::getDefaultValue<std::string>(SUMO_TAG_CROSSING, SUMO_ATTR_WIDTH).c_str());
    myCrossingWidth->setTextColor(FXRGB(0, 0, 0));
}


void
GNECrossingFrame::crossingParameters::disableCrossingParameters() {
    // clear all values of parameters
    myCrossingEdges->setText("");
    myCrossingPriority->setCheck(false);
    myCrossingPriority->setText("False");
    myCrossingWidth->setText("");
    // Disable all elements of the crossing frames
    myCrossingEdgesLabel->disable();
    myCrossingEdges->disable();
    myCrossingPriorityLabel->disable();
    myCrossingPriority->disable();
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
            std::vector<std::string> crossingEdges;
            SUMOSAXAttributes::parseStringVector(myCrossingEdges->getText().text(), crossingEdges);
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
    for (std::vector<GNEEdge*>::const_iterator i = parentJunction->getGNEEdges().begin(); i != parentJunction->getGNEEdges().end(); i++) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), (*i)) == myCurrentSelectedEdges.end()) {
            crossingEdges.push_back((*i)->getID());
        }
    }
    myCrossingEdges->setText(joinToString(crossingEdges, " ").c_str());
    // Update colors and attributes
    onCmdSetAttribute(0, 0, 0);
}


void
GNECrossingFrame::crossingParameters::useSelectedEdges(GNEJunction* parentJunction) {
    std::vector<std::string> crossingEdges;
    for (std::vector<GNEEdge*>::const_iterator i = parentJunction->getGNEEdges().begin(); i != parentJunction->getGNEEdges().end(); i++) {
        if (gSelected.isSelected((*i)->getType(), (*i)->getGlID())) {
            crossingEdges.push_back((*i)->getID());
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
    for (std::vector<GNEEdge*>::const_iterator i = myCurrentSelectedEdges.begin(); i != myCurrentSelectedEdges.end(); i++) {
        NBEdgeVector.push_back((*i)->getNBEdge());
    }
    return NBEdgeVector;
}


bool
GNECrossingFrame::crossingParameters::getCrossingPriority() const {
    if (myCrossingPriority->getCheck()) {
        return true;
    } else {
        return false;
    }
}


bool
GNECrossingFrame::crossingParameters::isCurrentParametersValid() const {
    return myCurrentParametersValid;
}


SUMOReal
GNECrossingFrame::crossingParameters::getCrossingWidth() const {
    return GNEAttributeCarrier::parse<SUMOReal>(myCrossingWidth->getText().text());
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
    std::vector<std::string> crossingEdges;
    SUMOSAXAttributes::parseStringVector(myCrossingEdges->getText().text(), crossingEdges);

    // Clear selected edges
    myCurrentSelectedEdges.clear();
    // iterate over vector of edge IDs
    for (std::vector<std::string>::iterator i = crossingEdges.begin(); i != crossingEdges.end(); i++) {
        GNEEdge* edge = myCrossingFrameParent->getViewNet()->getNet()->retrieveEdge((*i), false);
        GNEJunction* currentJunction = myCrossingFrameParent->getEdgeSelector()->getCurrentJunction();
        // Check that edge exists and belongs to Junction
        if (edge == 0) {
            myCurrentParametersValid = false;
        } else if (std::find(currentJunction->getGNEEdges().begin(), currentJunction->getGNEEdges().end(), edge) == currentJunction->getGNEEdges().end()) {
            myCurrentParametersValid = false;
        } else {
            // select or unselected edge
            std::vector<GNEEdge*>::iterator itFinder = std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), edge);
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
    for (std::vector<GNEEdge*>::const_iterator i = myEdgeSelector->getCurrentJunction()->getGNEEdges().begin(); i != myEdgeSelector->getCurrentJunction()->getGNEEdges().end(); i++) {
        if (std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), *i) != myCurrentSelectedEdges.end()) {
            for (std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                (*j)->setSpecialColor(&mySelectedColor);
            }
        } else {
            for (std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                (*j)->setSpecialColor(&myCandidateColor);
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
    if (myCrossingPriority->getCheck()) {
        myCrossingPriority->setText("True");
    } else {
        myCrossingPriority->setText("False");
    }

    // Check width
    if (TplCheck::_str2SUMOReal(myCrossingWidth->getText().text()) &&
            TplConvert::_str2SUMOReal(myCrossingWidth->getText().text()) > 0) {
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
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
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
    myCreateCrossingButton = new FXButton(myGroupBoxButtons, "Create crossing", 0, this, MID_GNE_CREATE_CROSSING, GUIDesignButton);
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
    // Set default colors of edges (if a junction is yet selected)
    if (myEdgeSelector->getCurrentJunction() != NULL) {
        // remove color of edges
        for (std::vector<GNEEdge*>::const_iterator i = myEdgeSelector->getCurrentJunction()->getGNEEdges().begin(); i != myEdgeSelector->getCurrentJunction()->getGNEEdges().end(); i++) {
            for (std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                (*j)->setSpecialColor(0);
            }
        }
    }

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
                                          true), true);
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
