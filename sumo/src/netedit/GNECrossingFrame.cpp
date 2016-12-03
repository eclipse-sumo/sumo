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
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_USEONLYSELECTEDEDGES, GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_CLEAREDGESELECTION,    GNECrossingFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND,           MID_GNE_INVERTEDGESELECTION,   GNECrossingFrame::edgesSelector::onCmdInvertSelection),
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

    // Create button for selected edges
    myUseSelectedEdges =new FXButton(this, "Use selected Edges", 0, this, MID_GNE_USEONLYSELECTEDEDGES, GNEDesignButton);

    // Create button for clear selection
    myClearEdgesSelection = new FXButton(this, "clear edges", 0, this, MID_GNE_CLEAREDGESELECTION, GNEDesignButton);

    // Create button for invert selection
    myInvertEdgesSelection = new FXButton(this, "invert edges", 0, this, MID_GNE_INVERTEDGESELECTION, GNEDesignButton);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonHelp);
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


void 
GNECrossingFrame::edgesSelector::markEdge(GNEEdge *edge) {
    // Check if edge is already marked
    std::vector<GNEEdge*>::iterator it = std::find(myCurrentSelectedEdges.begin(), myCurrentSelectedEdges.end(), edge);

    if(it == myCurrentSelectedEdges.end()) {
        myCurrentSelectedEdges.push_back(edge);
    } else {
        myCurrentSelectedEdges.erase(it);
    }

    // update edges of crossing parameter
    myCrossingFrameParent->getCrossingParameters()->updateEdges();
}

std::vector<std::string>
GNECrossingFrame::edgesSelector::getEdgeIDSSelected() const {
    std::vector<std::string> IDsSelected;
    // iterate over list and keep the id of every edge
    for(std::vector<GNEEdge*>::const_iterator i = myCurrentSelectedEdges.begin(); i < myCurrentSelectedEdges.end(); i++) {
        IDsSelected.push_back((*i)->getID());
    }
    return IDsSelected;
}


const std::vector<GNEEdge*>&
GNECrossingFrame::edgesSelector::getGNEEdgesSelected() const {
    return myCurrentSelectedEdges;
}


GNEJunction*
GNECrossingFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void 
GNECrossingFrame::edgesSelector::enableEdgeSelector(GNEJunction *currentJunction) {
    // Set current junction
    myCurrentJunction = currentJunction;
    // enable all elements of the edgesSelector
    myUseSelectedEdges->enable();
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


long
GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    // Filter list for show only selected edges
    ///filterListOfEdges(myEdgesSearch->getText().text());
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    // clear selected edges and update edges of crossing parameter
    myCurrentSelectedEdges.clear();
    myCrossingFrameParent->getCrossingParameters()->updateEdges();
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    // Check if crossing parameters has to be enabled
    if(true) {
        myCrossingFrameParent->getCrossingParameters()->disableCrossingParameters();
        myCrossingFrameParent->setCreateCrossingButton(false);
    } else {
        myCrossingFrameParent->getCrossingParameters()->enableCrossingParameters();
        myCrossingFrameParent->setCreateCrossingButton(true);
    }
    // update edges of crossing parameter
    myCrossingFrameParent->getCrossingParameters()->updateEdges();
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

GNECrossingFrame::crossingParameters::crossingParameters(GNECrossingFrame *crossingFrameParent, GNECrossingFrame::edgesSelector *es) :
    FXGroupBox(crossingFrameParent->myContentFrame, "Crossing parameters", GNEDesignGroupBoxFrame),
    myCrossingFrameParent(crossingFrameParent),
    myEdgeSelector(es),
    myCurrentParametersValid(true) {
    // Create a Matrix for parameters
    myAttributesMatrix = new FXMatrix(this, 2, GNEDesignMatrixAttributes);
    // create label for edges
    myCrossingEdgesLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_EDGES).c_str(), 0, GNEDesignLabelAttribute);
    myCrossingEdgesLabel->disable();
    // create string textField for edges
    myCrossingEdges = new FXTextField(myAttributesMatrix, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeStr);
    myCrossingEdges->disable();
    // create label for Priority
    myCrossingPriorityLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GNEDesignLabelAttribute);
    myCrossingPriorityLabel->disable();
    // create CheckBox for Priority
    myCrossingPriority = new FXMenuCheck(myAttributesMatrix, "", this, MID_GNE_SET_ATTRIBUTE, GNEDesignCheckButtonAttribute);
    myCrossingPriority->disable();
    // create label for width
    myCrossingWidthLabel = new FXLabel(myAttributesMatrix, toString(SUMO_ATTR_WIDTH).c_str(), 0, GNEDesignLabelAttribute);
    myCrossingWidthLabel->disable();
    // create extField for width
    myCrossingWidth = new FXTextField(myAttributesMatrix, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeReal);
    myCrossingWidth->disable();
    // Create help button
    myHelpCrossingAttribute = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonHelp);
    myHelpCrossingAttribute->disable();
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
    updateEdges();
    myCrossingPriority->setCheck(GNEAttributeCarrier::getDefaultValue<bool>(SUMO_TAG_CROSSING, SUMO_ATTR_PRIORITY));
    myCrossingWidth->setText(GNEAttributeCarrier::getDefaultValue<std::string>(SUMO_TAG_CROSSING, SUMO_ATTR_WIDTH).c_str()); 
    myCrossingWidth->setTextColor(FXRGB(0, 0, 0));
}


void 
GNECrossingFrame::crossingParameters::updateEdges() {
    myCrossingEdges->setText(joinToString(myEdgeSelector->getEdgeIDSSelected(), " ").c_str());
    onCmdSetAttribute(0,0,0);
}

void
GNECrossingFrame::crossingParameters::disableCrossingParameters() {
    // clear all values of parameters
    myCrossingEdges->setText("");
    myCrossingPriority->setCheck(false);
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


long
GNECrossingFrame::crossingParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    myCurrentParametersValid = true;
    // obtain crossing edges and check if at least there are two
    std::vector<std::string> crossingEdges;
    SUMOSAXAttributes::parseStringVector(myCrossingEdges->getText().text(), crossingEdges);
    if(crossingEdges.size() > 0) {
        // Check that all edges exist
        for(std::vector<std::string>::iterator i = crossingEdges.begin(); i != crossingEdges.end(); i++) {
            if(myCrossingFrameParent->getViewNet()->getNet()->retrieveEdge((*i), false) == 0) {
                myCurrentParametersValid = false;
            }
        }
        // change color of textfield dependig of myCurrentParametersValid
        if(myCurrentParametersValid) {
            myCrossingEdges->setTextColor(FXRGB(0, 0, 0));
            myCrossingEdges->killFocus();
        } else {
            myCrossingEdges->setTextColor(FXRGB(255, 0, 0));
            myCurrentParametersValid = false;
        }
    } else {
        myCurrentParametersValid = false;
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
    // Create Groupbox for labels
    myGroupBoxLabel = new FXGroupBox(myContentFrame, "Junction", GNEDesignGroupBoxFrame);
    myCurrentJunctionLabel = new FXLabel(myGroupBoxLabel, "No junction selected", 0, GNEDesignLabel);

    // Create edge Selector
    myEdgeSelector = new edgesSelector(myContentFrame, this);
    
    // Create crossingParameters
    myCrossingParameters = new crossingParameters(this, myEdgeSelector);
    
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
        // Enable edge selector and crossing parameters
        myEdgeSelector->enableEdgeSelector(currentJunction);
        myCrossingParameters->enableCrossingParameters();
    } else if(selectedEdge != NULL) {
        myEdgeSelector->markEdge(selectedEdge);
    } else if(selectedLane != NULL) {
        myEdgeSelector->markEdge(&selectedLane->getParentEdge());
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
    // First check that current parameters are valid
    if(myCrossingParameters->isCurrentParametersValid()) {
        // create new crossing
        myViewNet->getUndoList()->add(new GNEChange_Crossing(myEdgeSelector->getCurrentJunction(), 
                                                             myCrossingParameters->getCrossingEdges(), 
                                                             myCrossingParameters->getCrossingWidth(), 
                                                             myCrossingParameters->getCrossingPriority(), 
                                                             true), true);
        // clear selected edges
        myEdgeSelector->onCmdClearSelection(0, 0, 0);
    }
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
