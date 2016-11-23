/****************************************************************************/
/// @file    GNECrossingFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
/// The Widget for add additional elements
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
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEFrameDesigns.h"
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
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_SHOWONLYSELECTEDEDGES, GNECrossingFrame::edgesSelector::onCmdShowOnlySelectedEdges),
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_CLEAREDGESELECTION,    GNECrossingFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_INVERTEDGESELECTION,   GNECrossingFrame::edgesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE, MID_GNE_SELECTEDGE,            GNECrossingFrame::edgesSelector::onCmdSelectEdge),
    FXMAPFUNC(SEL_CHANGED,           MID_GNE_SEARCHEDGE,            GNECrossingFrame::edgesSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_FOCUSIN,           MID_GNE_SEARCHEDGE,            GNECrossingFrame::edgesSelector::onCmdClickEdgeSearchFocusIn),
    FXMAPFUNC(SEL_FOCUSOUT,          MID_GNE_SEARCHEDGE,            GNECrossingFrame::edgesSelector::onCmdClickEdgeSearchFocusOut),
    FXMAPFUNC(SEL_COMMAND,           MID_HELP,                      GNECrossingFrame::edgesSelector::onCmdHelp),
};

FXDEFMAP(GNECrossingFrame::crossingParameters) GNECrossingParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,   GNECrossingFrame::crossingParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                GNECrossingFrame::crossingParameters::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNECrossingFrame,                     FXScrollWindow, GNECrossingMap,           ARRAYNUMBER(GNECrossingMap))
FXIMPLEMENT(GNECrossingFrame::edgesSelector,      FXGroupBox,     GNEEdgesMap,              ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNECrossingFrame::crossingParameters, FXGroupBox,     GNECrossingParametersMap, ARRAYNUMBER(GNECrossingParametersMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECrossingFrame::edgesSelector - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::edgesSelector::edgesSelector(FXComposite* parent, GNECrossingFrame* crossingFrameParent) :
    FXGroupBox(parent, "selection of Edges", GNEDesignGroupBoxFrame),
    myCrossingFrameParent(crossingFrameParent),
    myCurrentJunction(0) {
    // Create CheckBox for selected edges
    myShowOnlySelectedEdges = new FXMenuCheck(this, "Show only selected Edges", this, MID_GNE_SHOWONLYSELECTEDEDGES, GNEDesignCheckButton);

    // Create search box with default text
    myEdgesSearch = new FXTextField(this, GNEDesignTextFieldNCol, this, MID_GNE_SEARCHEDGE, GNEDesignTextField);
    myEdgesSearch->setText("Search edge ID");

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTEDGE, GNEDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GNEDesignHorizontalFrame);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEAREDGESELECTION, GNEDesignButton);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTEDGESELECTION, GNEDesignButton);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonLittle);
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


std::vector<std::string>
GNECrossingFrame::edgesSelector::getEdgeIDSSelected() const {
    std::vector<std::string> IDsSelected;
    // iterate over list
    for(int i = 0; i < myList->getNumItems(); i++) {
        // if item of list is selected
        if(myList->getItem(i)->isSelected()) {
            // save id of edge
            IDsSelected.push_back(myList->getItem(i)->getText().text());
        }
    }
    return IDsSelected;
}


std::vector<GNEEdge*>
GNECrossingFrame::edgesSelector::getGNEEdgesSelected() const {
    // if show only selected edges is checked
    if(myShowOnlySelectedEdges->getCheck()) {
        // Check that myCurrentSelectedEdges isn't empty 
        assert(myCurrentSelectedEdges.size() > 0);
        // return current selected edges
        return myCurrentSelectedEdges;
    } else {
        std::vector<GNEEdge*> GNEEdgesSelected;
        // iterate over list
        for(int i = 0; i < myList->getNumItems(); i++) {
            // if item of list is selected
            if(myList->getItem(i)->isSelected()) {
                // search GNEEdge and save it 
                GNEEdgesSelected.push_back(myCrossingFrameParent->getViewNet()->getNet()->retrieveEdge(myList->getItem(i)->getText().text(), true));
            }
        }
        return GNEEdgesSelected;
    }
}


GNEJunction*
GNECrossingFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void 
GNECrossingFrame::edgesSelector::filterListOfEdges(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of junction
    std::vector<GNEEdge*> vectorOfEdges = myCurrentJunction->getGNEEdges();
    // iterate over edges of junction
    for (std::vector<GNEEdge*>::iterator i = vectorOfEdges.begin(); i != vectorOfEdges.end(); i++) {
        // If search criterium is correct, then append item depending of myShowOnlySelectedEdges
        if ((*i)->getID().find(search) != std::string::npos) {
            if(myShowOnlySelectedEdges->getCheck()) {
                // Check that edge is selected
                if(gSelected.isSelected(GLO_EDGE, (*i)->getGlID())) {
                    myList->appendItem((*i)->getID().c_str());
                }
            } else {
                myList->appendItem((*i)->getID().c_str());
            }
        }
    }
    // Show dialog
    show();
}


void 
GNECrossingFrame::edgesSelector::clearList() {
    // Clear all item of the FXList
    myList->clearItems();
}


bool 
GNECrossingFrame::edgesSelector::markEdge(const std::string &edgeID) {
    // iterate over list
    for(int i = 0; i < myList->getNumItems(); i++) {
        // if item of list is the same as the edge ID, mark it
        if(myList->getItem(i)->getText().text() == edgeID) {
            myList->selectItem(i);
            // Enable crossing parameters
            myCrossingFrameParent->getCrossingParameters()->enableCrossingParameters();
            myCrossingFrameParent->setCreateCrossingButton(true);
            return true;
        }
    }
    return false;
}


void 
GNECrossingFrame::edgesSelector::enableEdgeSelector(GNEJunction *currentJunction) {
    // Set current junction
    myCurrentJunction = currentJunction;

    // Obtain selected edges and filter it
    std::set<GUIGlID> edgesSelected = gSelected.getSelected(GLO_EDGE);
    myCurrentSelectedEdges.clear();
    std::vector<GNEEdge*> currentJunctionEdges = myCurrentJunction->getGNEEdges();
    for(std::vector<GNEEdge*>::iterator i = currentJunctionEdges.begin(); i != currentJunctionEdges.end(); i++) {
        if(edgesSelected.find((*i)->getGlID()) != edgesSelected.end()) {
            myCurrentSelectedEdges.push_back(*i);
        }
    }
    // If current selected edges can be used to create a sidewalk
    if(myCurrentSelectedEdges.size() > 0) {
        myShowOnlySelectedEdges->enable();
    } else {
        myShowOnlySelectedEdges->disable();
    }

    // Enable rest of elements of the edgesSelector
    myList->enable();
    myEdgesSearch->enable();
    helpEdges->enable();
    myClearEdgesSelection->enable();
    myInvertEdgesSelection->enable();
    // show edges in list
    filterListOfEdges();
}


void 
GNECrossingFrame::edgesSelector::disableEdgeSelector() {
    // disable current junction
    myCurrentJunction = NULL;
    // clear list of edges
    clearList();
    // If previously UseSelectedEdges was checked
    if(myShowOnlySelectedEdges->getCheck()) {
        // show all elements
        myShowOnlySelectedEdges->setCheck(false);
        myEdgesSearch->show();
        myList->show();
        myClearEdgesSelection->show();
        myInvertEdgesSelection->show();
        helpEdges->show();
        // Recalc Frame
        recalc();
        // Update Frame
        update();
    }
    // disable all elements of the edgesSelector
    myShowOnlySelectedEdges->disable();
    myList->disable();
    myEdgesSearch->disable();
    helpEdges->disable();
    myClearEdgesSelection->disable();
    myInvertEdgesSelection->disable();
    // Disable crossing parameters
    myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
}



void
GNECrossingFrame::edgesSelector::updateUseSelectedEdges() {
    // Enable or disable show only selected edges
    if (myCrossingFrameParent->getViewNet()->getNet()->retrieveEdges(true).size() > 0) {
        myShowOnlySelectedEdges->enable();
    } else {
        myShowOnlySelectedEdges->disable();
    }
}


bool
GNECrossingFrame::edgesSelector::isUseSelectedEdgesEnable() const {
    if (myShowOnlySelectedEdges->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNECrossingFrame::edgesSelector::onCmdShowOnlySelectedEdges(FXObject*, FXSelector, void*) {
    // Filter list for show only selected edges
    filterListOfEdges(myEdgesSearch->getText().text());
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of edgesSelector that contains the searched string
    if(myEdgesSearch->getText() == "Search edge ID") {
        filterListOfEdges("");
    } else {
        filterListOfEdges(myEdgesSearch->getText().text());
    }
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    // iterate over list items
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            // Enable crossing parameters
            myCrossingFrameParent->getCrossingParameters()->enableCrossingParameters();
            myCrossingFrameParent->setCreateCrossingButton(true);
            return 1;
        }
    }
    // disable crossing parameters and create crossing
    myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
    myCrossingFrameParent->setCreateCrossingButton(false);
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    // iterate over list items
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            // Desselect items
            myList->deselectItem(i);
        }
    }
    // disable crossing parameters and create crossing
    myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
    myCrossingFrameParent->setCreateCrossingButton(false);
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    // by default crossing parameters will be disabled
    bool disableCrossingParameter = true;
    // Iterate over list items
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
            // At least there is a selected item
            disableCrossingParameter = false;
        }
    }
    // Check if crossing parameters has to be enabled
    if(disableCrossingParameter) {
        myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
        myCrossingFrameParent->setCreateCrossingButton(false);
    } else {
        myCrossingFrameParent->getCrossingParameters()->enableCrossingParameters();
        myCrossingFrameParent->setCreateCrossingButton(true);
    }
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClickEdgeSearchFocusIn(FXObject*, FXSelector, void*) {
    // if search edge is the default value, clear text
    if(myEdgesSearch->getText() == "Search edge ID") {
        myEdgesSearch->setText("");
    }
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClickEdgeSearchFocusOut(FXObject*, FXSelector, void*) {
    // if search edge is empty, set default value
    if(myEdgesSearch->getText().empty()) {
        myEdgesSearch->setText("Search edge ID");
    }
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

GNECrossingFrame::crossingParameters::crossingParameters(FXComposite* parent, GNECrossingFrame::edgesSelector *es) :
    FXGroupBox(parent, "Crossing parameters", GNEDesignGroupBoxFrame),
    myEdgeSelector(es) {
    // Create a Matrix for parameters
    attributesMatrix = new FXMatrix(this, 2, GNEDesignMatrix);
    // create label for edges
    crossingEdgesLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_EDGES).c_str(), 0, GNEDesignLabelAttribute);
    // create string textField for edges
    crossingEdges = new FXTextField(attributesMatrix, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeStr);
    // create label for Priority
    crossingPriorityLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GNEDesignLabelAttribute);
    // create CheckBox for Priority
    crossingPriority = new FXMenuCheck(attributesMatrix, "", this, MID_GNE_SET_ATTRIBUTE, GNEDesignCheckButtonAttribute);
    // create label for width
    crossingWidthLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_WIDTH).c_str(), 0, GNEDesignLabelAttribute);
    // create extField for width
    crossingWidth = new FXTextField(attributesMatrix, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeReal);
    // Create help button
    helpAdditional = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonLittle);
    // At start crossing parameters is disabled
    disableCrossingParameters();
}


GNECrossingFrame::crossingParameters::~crossingParameters() {
}


void
GNECrossingFrame::crossingParameters::enableCrossingParameters() {
    // Enable all elements of the crossing frames
    crossingEdgesLabel->enable();
    crossingEdges->enable();
    crossingPriorityLabel->enable();
    crossingPriority->enable();
    crossingWidthLabel->enable();
    crossingWidth->enable();
    helpAdditional->enable();
    // set values of parameters
    crossingEdges->setText(joinToString(myEdgeSelector->getEdgeIDSSelected(), " ").c_str());
    crossingPriority->setCheck(GNEAttributeCarrier::getDefaultValue<bool>(SUMO_TAG_CROSSING, SUMO_ATTR_PRIORITY));
    crossingWidth->setText(GNEAttributeCarrier::getDefaultValue<std::string>(SUMO_TAG_CROSSING, SUMO_ATTR_WIDTH).c_str());    
}


void
GNECrossingFrame::crossingParameters::disableCrossingParameters() {
    // clear all values of parameters
    crossingEdges->setText("");
    crossingPriority->setCheck(false);
    crossingWidth->setText("");    
    // Disable all elements of the crossing frames
    crossingEdgesLabel->disable();
    crossingEdges->disable();
    crossingPriorityLabel->disable();
    crossingPriority->disable();
    crossingWidthLabel->disable();
    crossingWidth->disable();
    helpAdditional->disable();
}


std::vector<NBEdge*> 
GNECrossingFrame::crossingParameters::getCrossingEdges() const {
    std::vector<NBEdge*> NBEdgeVector;
    std::vector<GNEEdge*> GNEEdgesVector = myEdgeSelector->getGNEEdgesSelected();
    // Iterate over GNEEdges
    for(std::vector<GNEEdge*>::iterator i = GNEEdgesVector.begin(); i != GNEEdgesVector.end(); i++) {
        NBEdgeVector.push_back((*i)->getNBEdge());
    }
    return NBEdgeVector;
}


bool 
GNECrossingFrame::crossingParameters::getCrossingPriority() const {
    if (crossingPriority->getCheck()) {
        return true;
    } else {
        return false;
    }
}

SUMOReal 
GNECrossingFrame::crossingParameters::getCrossingWidth() const {
    return GNEAttributeCarrier::parse<SUMOReal>(crossingWidth->getText().text());
}


long
GNECrossingFrame::crossingParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    /***************************/
    return 0;
}


long
GNECrossingFrame::crossingParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(SUMO_TAG_CROSSING)).c_str(), GNEDesignDialogBox);
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
        // Attribute ID cannot be edited in crossings
        if (attr != SUMO_ATTR_ID) {
            // Set name of attribute
            myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
            // Set type
            FXTableItem* type = new FXTableItem("");
            if (GNEAttributeCarrier::isInt(SUMO_TAG_CROSSING, attr)) {
                type->setText("int");
            } else if (GNEAttributeCarrier::isFloat(SUMO_TAG_CROSSING, attr)) {
                type->setText("float");
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
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", 0, helpDialog, FXDialogBox::ID_ACCEPT, GNEDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECrossingFrame - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::GNECrossingFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Crossings") {
    // Create Groupbox for laelLabel
    myGroupBoxLabel = new FXGroupBox(myContentFrame, "Junction", GNEDesignGroupBoxFrame);
    myCurrentJunctionLabel = new FXLabel(myGroupBoxLabel, "No junction selected", 0, GNEDesignLabel);

    // Create edge Selector
    myEdgeSelector = new edgesSelector(myContentFrame, this);
    
    // Create crossingParameters
    myCrossingParameters = new crossingParameters(myContentFrame, myEdgeSelector);
    
    /// Create groupbox for create crossings 
    myGroupBoxButtons = new FXGroupBox(myContentFrame, "Create", GNEDesignGroupBoxFrame);
    myCreateCrossingButton = new FXButton(myGroupBoxButtons, "Create crossing", 0, this, MID_GNE_CREATE_CROSSING, GNEDesignButton);
    myCreateCrossingButton->disable();

    // disable edge selector
    myEdgeSelector->disableEdgeSelector();
}


GNECrossingFrame::~GNECrossingFrame() {
    gSelected.remove2Update();
}


bool
GNECrossingFrame::addCrossing(GNENetElement* netElement) {
    // cast netelement
    GNEJunction *currentJunction = dynamic_cast<GNEJunction*>(netElement);
    GNEEdge *selectedEdge = dynamic_cast<GNEEdge*>(netElement);
    GNELane *selectedLane = dynamic_cast<GNELane*>(netElement);
    
    // If current element is a junction 
    if(currentJunction != NULL) {
        // change label
        myCurrentJunctionLabel->setText((std::string("Current Junction: ") + currentJunction->getID()).c_str());
        // Enable edge selector
        myEdgeSelector->enableEdgeSelector(currentJunction);
    } else if(selectedEdge != NULL) {
        myEdgeSelector->markEdge(selectedEdge->getID());
    } else if(selectedLane != NULL) {
        myEdgeSelector->markEdge(selectedLane->getParentEdge().getID());
    } else {
        // set default label
        myCurrentJunctionLabel->setText("No junction selected");
        // Disable edge selector
        myEdgeSelector->disableEdgeSelector();
    }
    return false;
}


long
GNECrossingFrame::onCmdCreateCrossing(FXObject*, FXSelector, void*) {
    // Add crossing to NBEdge
    myEdgeSelector->getCurrentJunction()->getNBNode()->addCrossing(myCrossingParameters->getCrossingEdges(), myCrossingParameters->getCrossingWidth(), myCrossingParameters->getCrossingPriority());
    // rebuild crossings of junction
    myEdgeSelector->getCurrentJunction()->rebuildCrossings(false);
    // Update view
    myViewNet->update();
    // clear selected edges
    myEdgeSelector->onCmdClearSelection(0, 0, 0);
    return 1;
}


void
GNECrossingFrame::setCreateCrossingButton(bool value) {
    if(value) {
        myCreateCrossingButton->enable();
    } else {
        myCreateCrossingButton->disable();
    }
}


GNECrossingFrame::crossingParameters*
GNECrossingFrame::getCrossingParameters() const {
    return myCrossingParameters;
}

/****************************************************************************/
