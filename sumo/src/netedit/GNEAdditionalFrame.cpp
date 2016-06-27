/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
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
#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Additional.h"
#include "GNEAdditional.h"
#include "GNEAdditionalSet.h"
#include "GNEAdditionalHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAdditionalFrame) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_ITEM, GNEAdditionalFrame::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameterList) GNEAdditionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDROW,    GNEAdditionalFrame::additionalParameterList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVEROW, GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameters) GNEAdditionalParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_HELP, GNEAdditionalFrame::additionalParameters::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::editorParameters) GNEEditorParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GNEAdditionalFrame::editorParameters::onCmdSelectReferencePoint),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                               GNEAdditionalFrame::editorParameters::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::additionalSet) GNEAdditionalSetMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTADDITIONALSET, GNEAdditionalFrame::additionalSet::onCmdSelectAdditionalSet),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNEAdditionalFrame::additionalSet::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::edges) GNEEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEAREDGESELECTION,  GNEAdditionalFrame::edges::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTEDGESELECTION, GNEAdditionalFrame::edges::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHEDGE,          GNEAdditionalFrame::edges::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTEDGE,          GNEAdditionalFrame::edges::onCmdSelectEdge),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNEAdditionalFrame::edges::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::lanes) GNELanesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEARLANESELECTION,  GNEAdditionalFrame::lanes::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTLANESELECTION, GNEAdditionalFrame::lanes::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHLANE,          GNEAdditionalFrame::lanes::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTLANE,          GNEAdditionalFrame::lanes::onCmdSelectLane),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNEAdditionalFrame::lanes::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame,                          FXScrollWindow, GNEAdditionalMap,              ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameterList, FXMatrix,       GNEAdditionalParameterListMap, ARRAYNUMBER(GNEAdditionalParameterListMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameters,    FXGroupBox,     GNEAdditionalParametersMap,    ARRAYNUMBER(GNEAdditionalParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::editorParameters,        FXGroupBox,     GNEEditorParametersMap,        ARRAYNUMBER(GNEEditorParametersMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalSet,           FXGroupBox,     GNEAdditionalSetMap,           ARRAYNUMBER(GNEAdditionalSetMap))
FXIMPLEMENT(GNEAdditionalFrame::edges,                   FXGroupBox,     GNEEdgesMap,                   ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNEAdditionalFrame::lanes,                   FXGroupBox,     GNELanesMap,                   ARRAYNUMBER(GNELanesMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAdditionalFrame::GNEAdditionalFrame(FXComposite* parent, GNEViewNet* viewNet):
    GNEFrame(parent, viewNet, "Additionals"),
    myActualAdditionalType(SUMO_TAG_NOTHING) {

    // Create groupBox for myAdditionalMatchBox
    myGroupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);

    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(myGroupBoxForMyAdditionalMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM,  FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_X);

    // Create additional parameters
    myAdditionalParameters = new GNEAdditionalFrame::additionalParameters(myContentFrame, this);

    // Create editor parameter
    myEditorParameters = new GNEAdditionalFrame::editorParameters(myContentFrame, this);

    // Create create list for additional Set
    myAdditionalSet = new GNEAdditionalFrame::additionalSet(myContentFrame, this, myViewNet);

    /// Create list for edges
    myEdges = new GNEAdditionalFrame::edges(myContentFrame, myViewNet);

    /// Create list for lanes
    myLanes = new GNEAdditionalFrame::lanes(myContentFrame, myViewNet);

    // Add options to myAdditionalMatchBox
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedAdditionalTags();
    for(std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++)
        myAdditionalMatchBox->appendItem(toString(*i).c_str());

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems());

    // If there are additionals
    if(additionalTags.size() > 0) {
        // Set myActualAdditionalType and show
        myActualAdditionalType = additionalTags.front();
        setParametersOfAdditional(myActualAdditionalType);
    }
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    gSelected.remove2Update();
}


bool
GNEAdditionalFrame::addAdditional(GNELane *lane, GUISUMOAbstractView* parent) {
    // First check if actual type must be placed over a lane or edge but user did't clicke over a lane
    if((GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANE) == true || GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGE) == true) && lane == NULL)
        return false;
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement = myAdditionalParameters->getAttributes();
    // Generate id of elmement
    valuesOfElement[SUMO_ATTR_ID] = generateID(lane);
    // obtain a new unique id depending if the element needs or not a lane
    if(lane) {
        // Obtain positiono of additional over mouse
        SUMOReal positionOfTheMouseOverLane = lane->getShape().nearest_offset_to_point2D(parent->getPositionInformation());
        // Obtain lane ID
        valuesOfElement[SUMO_ATTR_LANE] = lane->getID();
        // If element has a StartPosition and EndPosition over lane, extract attributes
        if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
            valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(positionOfTheMouseOverLane, myEditorParameters->getLenght()));
            valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(lane->getLaneShapeLenght(), positionOfTheMouseOverLane, myEditorParameters->getLenght()));
        }
        // Extract position of lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
    } else {
        // get position in map
        valuesOfElement[SUMO_ATTR_POSITION] = toString(parent->getPositionInformation());
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.txt
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_FILE) && valuesOfElement[SUMO_ATTR_FILE] == "")
        valuesOfElement[SUMO_ATTR_FILE] = (valuesOfElement[SUMO_ATTR_ID] + ".txt");
    // Save block value
    valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myEditorParameters->isBlockEnabled());
    // If element belongst to an additional Set, get id of parent from myAdditionalSet
    if(GNEAttributeCarrier::hasParent(myActualAdditionalType)) {
        if(myAdditionalSet->getIdSelected() != "")
            valuesOfElement[GNE_ATTR_PARENT] = myAdditionalSet->getIdSelected();
        else {
            WRITE_ERROR("A " + toString(myAdditionalSet->getCurrentlyTag()) + " must be selected before insertion of " + toString(myActualAdditionalType) + ".");
            return false;
        }
    }
    // If element own a list of edges as attribute 
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES)) {
        valuesOfElement[SUMO_ATTR_EDGES] = myEdges->getIdsSelected();
        if(valuesOfElement[SUMO_ATTR_EDGES] == "") {
            WRITE_ERROR("A " + toString(myActualAdditionalType) + " must have at least one edge associated.");
            return false;
        }
    }
    // If element own a list of lanes as attribute 
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANES)) {
        valuesOfElement[SUMO_ATTR_LANES] = myLanes->getIdsSelected();
        if(valuesOfElement[SUMO_ATTR_LANES] == "") {
            WRITE_ERROR("A " + toString(myActualAdditionalType) + " must have at least one lane associated.");
            return false;
        }
    }
    // Create additional
    return GNEAdditionalHandler::buildAdditional(myViewNet, myActualAdditionalType, valuesOfElement);
}

void
GNEAdditionalFrame::removeAdditional(GNEAdditional *additional) {
    myViewNet->getUndoList()->p_begin("delete " + additional->getDescription());
    myViewNet->getUndoList()->add(new GNEChange_Additional(myViewNet->getNet(), additional, false), true);
    myViewNet->getUndoList()->p_end();
}


long
GNEAdditionalFrame::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // set myActualAdditionalType
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedAdditionalTags();
    for(std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++)
        if(toString(*i) == myAdditionalMatchBox->getText().text())
            setParametersOfAdditional(*i);
    return 1;
}


void
GNEAdditionalFrame::show() {
    // Show Scroll window
    FXScrollWindow::show();
    // Show Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEAdditionalFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEAdditionalFrame::setParametersOfAdditional(SumoXMLTag actualAdditionalType) {
    // Set new actualAdditionalType
    myActualAdditionalType = actualAdditionalType;
    // Clear default parameters
    myAdditionalParameters->clearAttributes();
    // Hide lenght field and reference point
    myEditorParameters->hideLengthField();
    myEditorParameters->hideReferencePoint();
    // Obtain attributes of actual myActualAdditionalType
    std::vector<std::pair <SumoXMLAttr, std::string> > attrs = GNEAttributeCarrier::allowedAttributes(myActualAdditionalType);
    // Iterate over attributes of myActualAdditionalType
    for(std::vector<std::pair <SumoXMLAttr, std::string> >::iterator i = attrs.begin(); i != attrs.end(); i++) {
        if(!GNEAttributeCarrier::isUnique(i->first)) {
             myAdditionalParameters->addAttribute(myActualAdditionalType, i->first);
        } else if(i->first == SUMO_ATTR_ENDPOS) {
            myEditorParameters->showLengthField();
            myEditorParameters->showReferencePoint();
        }
    }
    // if there are parmeters, show and Recalc groupBox
    if(myAdditionalParameters->getNumberOfAddedAttributes() > 0)
        myAdditionalParameters->showAdditionalParameters();
    else
        myAdditionalParameters->hideAdditionalParameters();
    // Show set parameter if we're adding an additional with parent
    if(GNEAttributeCarrier::hasParent(myActualAdditionalType))
        myAdditionalSet->showList(GNEAttributeCarrier::getParentType(myActualAdditionalType));
    else
        myAdditionalSet->hideList();
    // Show edges if we're adding an additional that own the attribute SUMO_ATTR_EDGES
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_EDGES))
        myEdges->showList();
    else
        myEdges->hideList();
    // Show lanes if we're adding an additional that own the attribute SUMO_ATTR_LANES
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANES))
        myLanes->showList();
    else
        myLanes->hideList();
}


std::string 
GNEAdditionalFrame::generateID(GNELane *lane) const {
    int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myActualAdditionalType);
    if(lane) {
        // generate ID using lane
        while(myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + lane->getID() + "_" + toString(additionalIndex)) != NULL)
            additionalIndex++;
        return toString(myActualAdditionalType) + "_" + lane->getID() + "_" + toString(additionalIndex);
    } else {
        // generate ID without lane
        while(myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + toString(additionalIndex)) != NULL)
            additionalIndex++;
        return toString(myActualAdditionalType) + "_" + toString(additionalIndex);
    }
}


SUMOReal
GNEAdditionalFrame::setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_LEFT :
            return positionOfTheMouseOverLane;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_RIGHT :
            if(positionOfTheMouseOverLane - lenghtOfAdditional >= 0.01)
                 return positionOfTheMouseOverLane - lenghtOfAdditional;
            else if(myEditorParameters->isForcePositionEnabled())
                return 0.01;
            else
                return -1;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_CENTER :
            if(positionOfTheMouseOverLane - lenghtOfAdditional/2 >= 0.01)
                return positionOfTheMouseOverLane - lenghtOfAdditional/2;
            else if(myEditorParameters->isForcePositionEnabled())
                return 0;
            else
                return -1;
        default:
            return -1;
    }
}


SUMOReal
GNEAdditionalFrame::setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameters->getActualReferencePoint()) {
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            if(positionOfTheMouseOverLane + lenghtOfAdditional <= laneLenght - 0.01)
                return positionOfTheMouseOverLane + lenghtOfAdditional;
            else if(myEditorParameters->isForcePositionEnabled())
                return laneLenght - 0.01;
            else
                return -1;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case editorParameters::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            if(positionOfTheMouseOverLane + lenghtOfAdditional/2 <= laneLenght - 0.01)
                return positionOfTheMouseOverLane + lenghtOfAdditional/2;
            else if(myEditorParameters->isForcePositionEnabled())
                return laneLenght - 0.01;
            else
                return -1;
        default:
            return -1;
    }
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalParameter - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParameter::additionalParameter(FXComposite *parent, FXObject* tgt) :
    FXMatrix(parent, 3, MATRIX_BY_COLUMNS | LAYOUT_FILL_X),
    myAttr(SUMO_ATTR_NOTHING) {
    // Create elements
    myLabel = new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0);
    myTextField = new FXTextField(this, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myMenuCheck = new FXMenuCheck(this, "", tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL, LAYOUT_FIX_WIDTH);
    // Set widht of menuCheck manually
    myMenuCheck->setWidth(20);
    // Hide elements
    hideParameter();
}


GNEAdditionalFrame::additionalParameter::~additionalParameter() {}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, std::string value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(value.c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, int value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(toString(value).c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, SUMOReal value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(toString(value).c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, bool value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myMenuCheck->setCheck(value);
    myMenuCheck->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::hideParameter() {
    myAttr = SUMO_ATTR_NOTHING;
    myLabel->hide();
    myTextField->hide();
    myMenuCheck->hide();
    hide();
}


SumoXMLAttr
GNEAdditionalFrame::additionalParameter::getAttr() const {
    return myAttr;
}


std::string
GNEAdditionalFrame::additionalParameter::getValue() const {
    if(GNEAttributeCarrier::isBool(myAttr))
        return (myMenuCheck->getCheck() == 1)? "true" : "false";
    else
        return myTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalParameterList - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParameterList::additionalParameterList(FXComposite *parent, FXObject* tgt) :
    FXMatrix(parent, 2, MATRIX_BY_COLUMNS | LAYOUT_FILL_X),
    numberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(20),
    myAttr(SUMO_ATTR_NOTHING) {
    // Create elements
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.push_back(new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0));
        myTextFields.push_back(new FXTextField(this, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X));
    }
    // Create label Row
    myLabels.push_back(new FXLabel(this, "Rows", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0));
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    // Create add button
    add = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);
    // Create delete buttons
    remove = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_REMOVEROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);
    // Hide all para meters
    hideParameter();
}


GNEAdditionalFrame::additionalParameterList::~additionalParameterList() {}


void
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<int> value) {
    myAttr = attr;
    std::cout << "FINISH" << std::endl;
}

void
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<SUMOReal> value) {
    myAttr = attr;
    std::cout << "FINISH" << std::endl;
}

void
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<bool> value) {
    myAttr = attr;
    std::cout << "FINISH" << std::endl;
}

void
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<std::string> value) {
    if((int)value.size() < myMaxNumberOfValuesInParameterList) {
        myAttr = attr;
        numberOfVisibleTextfields = (int)value.size();
        if(numberOfVisibleTextfields == 0)
            numberOfVisibleTextfields++;
        for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++)
            myLabels.at(i)->setText((toString(attr) + ": " + toString(i)).c_str());
        for(int i = 0; i < numberOfVisibleTextfields; i++) {
            myLabels.at(i)->show();
            myTextFields.at(i)->show();
        }
        add->show();
        remove->show();
        show();
    }
}


void
GNEAdditionalFrame::additionalParameterList::hideParameter() {
    myAttr = SUMO_ATTR_NOTHING;
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.at(i)->hide();
        myTextFields.at(i)->hide();
        myTextFields.at(i)->setText("");
    }
    add->hide();
    remove->hide();
    hide();
}


SumoXMLAttr
GNEAdditionalFrame::additionalParameterList::getAttr() const {
    return myAttr;
}


std::string
GNEAdditionalFrame::additionalParameterList::getListValues() {
    // Declare, fill and return a string with the list values
    std::string value;
    for(int i = 0; i < numberOfVisibleTextfields; i++)
        if(!myTextFields.at(i)->getText().empty())
            value += (myTextFields.at(i)->getText().text() + std::string(" "));
    return value;
}


long
GNEAdditionalFrame::additionalParameterList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList-1)) {
        myLabels.at(numberOfVisibleTextfields)->show();
        myTextFields.at(numberOfVisibleTextfields)->show();
        numberOfVisibleTextfields++;
        getParent()->recalc();
    }
    return 1;
}


long
GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields > 1) {
        numberOfVisibleTextfields--;
        myLabels.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->setText("");

        getParent()->recalc();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParameters::additionalParameters(FXComposite *parent, FXObject* tgt) :
    FXGroupBox(parent, "Default parameters", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myIndexParameter(0),
    myIndexParameterList(0),
    maxNumberOfParameters(GNEAttributeCarrier::getHigherNumberOfAttributes()), 
    maxNumberOfListParameters(2) {

    // Create widgets for parameters
    for (int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.push_back(new additionalParameter(this, tgt));

    // Create widgets for parameters
    for (int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfAdditionalParameterList.push_back(new additionalParameterList(this, tgt));

        // Create help button
    helpAdditional = new FXButton(this, "Help", 0, this, MID_HELP);
}


GNEAdditionalFrame::additionalParameters::~additionalParameters() {
}


void
GNEAdditionalFrame::additionalParameters::clearAttributes() {
    // Hidde al fields
    for(int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.at(i)->hideParameter();
    
    // Hidde al list fields
    for(int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfAdditionalParameterList.at(i)->hideParameter();

    // Reset indexs
    myIndexParameterList = 0;
    myIndexParameter = 0;
}


void
GNEAdditionalFrame::additionalParameters::addAttribute(SumoXMLTag additional, SumoXMLAttr attribute) {
    // Set current additional
    myAdditional = additional;
    // If  parameter is of type list
    if(GNEAttributeCarrier::isList(attribute)) {
        // If parameter can be show
        if(myIndexParameterList < maxNumberOfListParameters) {
            // Check type of attribute list
            if(GNEAttributeCarrier::isInt(attribute))
                myVectorOfAdditionalParameterList.at(myIndexParameterList)->showListParameter(attribute, GNEAttributeCarrier::getDefaultValue< std::vector<int> >(additional, attribute));
            else if(GNEAttributeCarrier::isFloat(attribute))
                myVectorOfAdditionalParameterList.at(myIndexParameterList)->showListParameter(attribute, GNEAttributeCarrier::getDefaultValue< std::vector<SUMOReal> >(additional, attribute));
            else if(GNEAttributeCarrier::isBool(attribute))
                myVectorOfAdditionalParameterList.at(myIndexParameterList)->showListParameter(attribute, GNEAttributeCarrier::getDefaultValue< std::vector<bool> >(additional, attribute));
            else if(GNEAttributeCarrier::isString(attribute)) 
                    myVectorOfAdditionalParameterList.at(myIndexParameterList)->showListParameter(attribute, GNEAttributeCarrier::getDefaultValue< std::vector<std::string> >(additional, attribute));
            // Update index
            myIndexParameterList++;
        } else
            WRITE_ERROR("Max number of list attributes reached (" + toString(maxNumberOfListParameters) + ").");
    } else {
        if(myIndexParameter < maxNumberOfParameters) {
            // Check type of attribute list
            if(GNEAttributeCarrier::isInt(attribute))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attribute, GNEAttributeCarrier::getDefaultValue<int>(additional, attribute));
            else if(GNEAttributeCarrier::isFloat(attribute))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attribute, GNEAttributeCarrier::getDefaultValue<SUMOReal>(additional, attribute));
            else if(GNEAttributeCarrier::isBool(attribute))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attribute, GNEAttributeCarrier::getDefaultValue<bool>(additional, attribute));
            else if(GNEAttributeCarrier::isString(attribute))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attribute, GNEAttributeCarrier::getDefaultValue<std::string>(additional, attribute));
            else
                WRITE_WARNING("Attribute '" + toString(attribute) + "' don't have a defined type. Check definition in GNEAttributeCarrier");
            // Update index parameter
            myIndexParameter++;
        } else
            WRITE_ERROR("Max number of attributes reached (" + toString(maxNumberOfParameters) + ").");
    }
}


void
GNEAdditionalFrame::additionalParameters::showAdditionalParameters() {
    recalc();
    show();
}


void
GNEAdditionalFrame::additionalParameters::hideAdditionalParameters() {
    hide();
}


std::map<SumoXMLAttr, std::string>
GNEAdditionalFrame::additionalParameters::getAttributes() const {
    std::map<SumoXMLAttr, std::string> values;
    // get standar Parameters
    for(int i = 0; i < myIndexParameter; i++)
        values[myVectorOfAdditionalParameter.at(i)->getAttr()] = myVectorOfAdditionalParameter.at(i)->getValue();
    // get list parameters
    for(int i = 0; i < myIndexParameterList; i++)
        values[myVectorOfAdditionalParameterList.at(i)->getAttr()] = myVectorOfAdditionalParameterList.at(i)->getListValues();
    return values;
}


int
GNEAdditionalFrame::additionalParameters::getNumberOfAddedAttributes() const {
    return (myIndexParameter + myIndexParameterList);
}


long
GNEAdditionalFrame::additionalParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(myAdditional)).c_str(), DECOR_CLOSE | DECOR_TITLE);
    // Create FXTable
    FXTable *myTable = new FXTable(helpDialog, this, MID_TABLE, TABLE_READONLY);
    myTable->setVisibleRows((FXint)(myIndexParameter + myIndexParameterList));
    myTable->setVisibleColumns(3);
    myTable->setTableSize((FXint)(myIndexParameter + myIndexParameterList), 3);
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
    for(int i = 0; i < myIndexParameter; i++) {
        SumoXMLAttr attr = myVectorOfAdditionalParameter.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem *type = new FXTableItem("");
        if(GNEAttributeCarrier::isInt(attr))
            type->setText("int");
        else if(GNEAttributeCarrier::isFloat(attr))
            type->setText("float");
        else if(GNEAttributeCarrier::isBool(attr))
            type->setText("bool");
        else if(GNEAttributeCarrier::isString(attr)) 
            type->setText("string");
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem *definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditional, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if((int)GNEAttributeCarrier::getDefinition(myAdditional, attr).size() > maxSizeColumnDefinitions)
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditional, attr).size());
    }
    // Iterate over vector of additional parameters list
    for(int i = 0; i < myIndexParameterList; i++) {
        SumoXMLAttr attr = myVectorOfAdditionalParameterList.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem *type = new FXTableItem("");
        if(GNEAttributeCarrier::isInt(attr))
            type->setText("list of int");
        else if(GNEAttributeCarrier::isFloat(attr))
            type->setText("list of float");
        else if(GNEAttributeCarrier::isBool(attr))
            type->setText("list of bool");
        else if(GNEAttributeCarrier::isString(attr)) 
            type->setText("list of string");
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem *definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditional, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if((int)GNEAttributeCarrier::getDefinition(myAdditional, attr).size() > maxSizeColumnDefinitions)
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditional, attr).size());
    }
    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions*6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", 0, helpDialog, FXDialogBox::ID_ACCEPT, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::editorParameters::editorParameters(FXComposite *parent, FXObject* tgt) :
    FXGroupBox(parent, "editor parameters", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT) {
    // Create FXListBox for the reference points
    myReferencePointMatchBox = new FXComboBox(this, 12, this, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
                                              FRAME_SUNKEN | LAYOUT_LEFT  | COMBOBOX_STATIC | LAYOUT_FILL_X);

    // Create Frame for Label and TextField
    FXHorizontalFrame *lengthFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X | LAYOUT_LEFT );

    // Create length label
    myLengthLabel = new FXLabel(lengthFrame, "Length:", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);

    // Create length text field
    myLengthTextField = new FXTextField(lengthFrame, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);

    // Set default value of length
    myLengthTextField->setText("10");

    // Create FXMenuCheck for the force option
    myCheckForcePosition = new FXMenuCheck(this, "Force position", this, MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
                                           LAYOUT_LEFT | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckBlock = new FXMenuCheck(this, "Block movement", this, MID_GNE_SET_BLOCKING,
                                   LAYOUT_LEFT | LAYOUT_FILL_X);

    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP);

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");

    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::editorParameters::~editorParameters() {}


void
GNEAdditionalFrame::editorParameters::showLengthField() {
    myLengthLabel->show();
    myLengthTextField->show();
}


void
GNEAdditionalFrame::editorParameters::hideLengthField() {
    myLengthLabel->hide();
    myLengthTextField->hide();
}


void
GNEAdditionalFrame::editorParameters::showReferencePoint() {
    myReferencePointMatchBox->show();
    myCheckForcePosition->show();
}


void
GNEAdditionalFrame::editorParameters::hideReferencePoint() {
    myReferencePointMatchBox->hide();
    myCheckForcePosition->hide();
}


GNEAdditionalFrame::editorParameters::additionalReferencePoint
GNEAdditionalFrame::editorParameters::getActualReferencePoint() {
    return myActualAdditionalReferencePoint;
}


SUMOReal
GNEAdditionalFrame::editorParameters::getLenght() {
    return GNEAttributeCarrier::parse<SUMOReal>(myLengthTextField->getText().text());
}


bool
GNEAdditionalFrame::editorParameters::isBlockEnabled() {
    return myCheckBlock->getCheck() == 1? true : false;
}


bool
GNEAdditionalFrame::editorParameters::isForcePositionEnabled() {
    return myCheckForcePosition->getCheck() == 1? true : false;
}


long
GNEAdditionalFrame::editorParameters::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    // Cast actual reference point type
    myActualAdditionalReferencePoint = static_cast<additionalReferencePoint>(myReferencePointMatchBox->getCurrentItem());
    return 1;
}


long
GNEAdditionalFrame::editorParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "Parameter editor Help", DECOR_CLOSE | DECOR_TITLE);
    std::ostringstream help;
    help
            << "Referece point: Mark the initial position of the additional element.\n"
            << "Example: If you want to create a busStop with a lenght of 30 in the point 100 of the lane:\n"
            << "- Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "- Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "- Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "Force position: if is enabled, will create the additional adapting size of additional element to lane.\n"
            << "Example: If you have a lane with lenght = 100, but you try to create a busStop with size = 50\n"
            << "in the position 80 of the lane, a busStop with startPos = 80 and endPos = 100 will be created\n"
            << "instead of a busStop with startPos = 80 and endPos = 130.\n"
            << "\n"
            << "Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "the mouse. This option can be modified with the Inspector.";
    new FXLabel(helpDialog, help.str().c_str(), 0, JUSTIFY_LEFT);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", 0, helpDialog, FXDialogBox::ID_ACCEPT,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}


std::string
GNEAdditionalFrame::getIdsSelected(const FXList* list) {
    // Obtain Id's of list
    std::string vectorOfIds;
    for (int i = 0; i < list->getNumItems(); i++) {
        if (list->isItemSelected(i)) {
            if (vectorOfIds.size() > 0) {
                vectorOfIds += " ";
            }
            vectorOfIds += (list->getItem(i)->getText()).text();
        }
    }
    return vectorOfIds;
}



// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalSet - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalSet::additionalSet(FXComposite *parent, FXObject* tgt, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Additional Set", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myViewNet(viewNet),
    myType(SUMO_TAG_NOTHING) {

    // Create label with the type of additionalSet
    mySetLabel = new FXLabel(this, "Set Type:", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, tgt, MID_GNE_SELECTADDITIONALSET, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create help button
    helpAdditionalSet = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNEAdditionalFrame::additionalSet::~additionalSet() {}


std::string
GNEAdditionalFrame::additionalSet::getIdSelected() const {
    for(int i = 0; i < myList->getNumItems(); i++)
        if(myList->isItemSelected(i))
            return myList->getItem(i)->getText().text();
    return "";
}


SumoXMLTag
GNEAdditionalFrame::additionalSet::getCurrentlyTag() const {
    return myType;
}


void
GNEAdditionalFrame::additionalSet::showList(SumoXMLTag type) {
    myType = type;
    mySetLabel->setText(("Type of set: " + toString(myType)).c_str());
    myList->clearItems();
    const std::vector<GNEAdditional*> &vectorOfAdditionalSets = myViewNet->getNet()->getAdditionals(myType);
    for(std::vector<GNEAdditional*>::const_iterator i = vectorOfAdditionalSets.begin(); i != vectorOfAdditionalSets.end(); i++)
        myList->appendItem((*i)->getID().c_str());
    show();
}


void
GNEAdditionalFrame::additionalSet::hideList() {
    myType = SUMO_TAG_NOTHING;
    hide();
}


long
GNEAdditionalFrame::additionalSet::onCmdSelectAdditionalSet(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::additionalSet::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}


// ---------------------------------------------------------------------------
// GNEAdditionalFrame::edges - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::edges::edges(FXComposite *parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Edges", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myViewNet(viewNet) {

    // Create search box
    myEdgesSearch = new FXTextField(this, 10, this, MID_GNE_SEARCHEDGE, LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTEDGE, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X);

    // Create button for clear selection
    clearEdgesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEAREDGESELECTION);

    // Create button for invert selection
    invertEdgesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTEDGESELECTION);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNEAdditionalFrame::edges::~edges() {}

std::string
GNEAdditionalFrame::edges::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}

void
GNEAdditionalFrame::edges::showList(std::string search) {
    myList->clearItems();
    std::vector<GNEEdge*> vectorOfEdges = myViewNet->getNet()->retrieveEdges(false);
    for(std::vector<GNEEdge*>::iterator i = vectorOfEdges.begin(); i != vectorOfEdges.end(); i++)
        if((*i)->getID().find(search) != std::string::npos)
            myList->appendItem((*i)->getID().c_str());
    show();
}


void
GNEAdditionalFrame::edges::hideList() {
    FXGroupBox::hide();
}


long 
GNEAdditionalFrame::edges::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of edges that contains the searched string
    showList(myEdgesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::edges::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::edges::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for(int i = 0; i < myList->getNumItems(); i++)
        if(myList->getItem(i)->isSelected())
            myList->deselectItem(i);
    return 1;
}


long
GNEAdditionalFrame::edges::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for(int i = 0; i < myList->getNumItems(); i++)
        if(myList->getItem(i)->isSelected())
            myList->deselectItem(i);
        else
            myList->selectItem(i);
    return 1;
}


long
GNEAdditionalFrame::edges::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::lanes - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::lanes::lanes(FXComposite *parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "lanes", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myViewNet(viewNet) {

    // Create search box
    myLanesSearch = new FXTextField(this, 10, this, MID_GNE_SEARCHLANE, LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTLANE, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEARLANESELECTION);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTLANESELECTION);

    // Create help button
    helplanes = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNEAdditionalFrame::lanes::~lanes() {}


std::string
GNEAdditionalFrame::lanes::getIdsSelected() const {
    return GNEAdditionalFrame::getIdsSelected(myList);
}


void
GNEAdditionalFrame::lanes::showList(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myViewNet->getNet()->retrieveLanes(false);
    for(std::vector<GNELane*>::iterator i = vectorOfLanes.begin(); i != vectorOfLanes.end(); i++)
        if((*i)->getID().find(search) != std::string::npos)
            myList->appendItem((*i)->getID().c_str());
    show();
}


void
GNEAdditionalFrame::lanes::hideList() {
    hide();
}


long 
GNEAdditionalFrame::lanes::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of lanes that contains the searched string
    showList(myLanesSearch->getText().text());
    return 1;
}


long
GNEAdditionalFrame::lanes::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAdditionalFrame::lanes::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for(int i = 0; i < myList->getNumItems(); i++)
        if(myList->getItem(i)->isSelected())
            myList->deselectItem(i);
    return 1;
}


long
GNEAdditionalFrame::lanes::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for(int i = 0; i < myList->getNumItems(); i++)
        if(myList->getItem(i)->isSelected())
            myList->deselectItem(i);
        else
            myList->selectItem(i);
    return 1;
}


long
GNEAdditionalFrame::lanes::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
