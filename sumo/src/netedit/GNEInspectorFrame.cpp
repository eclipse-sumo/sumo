/****************************************************************************/
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cmath>
#include <cassert>
#include <iostream>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEInspectorFrame.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNEAttributeCarrier.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY_TEMPLATE,   GNEInspectorFrame::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TEMPLATE,    GNEInspectorFrame::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_COPY_TEMPLATE,   GNEInspectorFrame::onUpdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_BLOCKING,    GNEInspectorFrame::onCmdSetBlocking),
};


FXDEFMAP(GNEInspectorFrame::AttrInput) AttrInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,         GNEInspectorFrame::AttrInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEInspectorFrame::AttrInput::onCmdOpenAttributeEditor)
};

FXDEFMAP(GNEInspectorFrame::AttrEditor) AttrEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_INSPECT_RESET,  GNEInspectorFrame::AttrEditor::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame, FXScrollWindow, GNEInspectorFrameMap, ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::AttrInput, FXMatrix, AttrInputMap, ARRAYNUMBER(AttrInputMap))
FXIMPLEMENT(GNEInspectorFrame::AttrEditor, FXDialogBox, AttrEditorMap, ARRAYNUMBER(AttrEditorMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXComposite* parent, GNEViewNet* viewNet):
    GNEFrame(parent, viewNet, "Inspector"),
    myEdgeTemplate(0) {

    // Create groupBox for attributes
    myGroupBoxForAttributes = new FXGroupBox(myContentFrame, "attributes", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForAttributes->hide();

    // Create AttrInput
    for(int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        vectorOfAttrInput.push_back(new AttrInput(myGroupBoxForAttributes, this));
    }

    // Create groupBox for templates
    myGroupBoxForTemplates = new FXGroupBox(myContentFrame, "templates", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForTemplates->hide();

    // Create copy template button
    myCopyTemplateButton = new FXButton(myGroupBoxForTemplates, "", 0, this, MID_GNE_COPY_TEMPLATE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(myGroupBoxForTemplates, "Set as Template\t\t", 0, this, MID_GNE_SET_TEMPLATE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    mySetTemplateButton->hide();

    // Create groupBox for editor parameters
    myGroupBoxForEditor = new FXGroupBox(myContentFrame, "editor", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForEditor->hide();

    // Create check blocked button
    myCheckBlocked = new FXCheckButton(myGroupBoxForEditor, "Block movement", this, MID_GNE_SET_BLOCKING);
    myCheckBlocked->hide();
}

GNEInspectorFrame::~GNEInspectorFrame() {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::~GNEInspectorFrame");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspectorFrame::show() {
    // Show Scroll window
    FXScrollWindow::show();
    // Show and update Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEInspectorFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEInspectorFrame::inspect(const std::vector<GNEAttributeCarrier*>& ACs) {
    // Assing ACS to myACs
    myACs = ACs;

    // If vector of attribute Carriers contain data
    if (myACs.size() > 0) {
        // Set header
        std::string headerString = toString(myACs.front()->getTag());
        if (myACs.size() > 1) {
            headerString = toString(myACs.size()) + " " + headerString + "s";
        }
        getFrameHeaderLabel()->setText(headerString.c_str());
        
        //Show myGroupBoxForAttributes
        myGroupBoxForAttributes->show();

        // Hide all AttrInput
        for(std::vector<GNEInspectorFrame::AttrInput*>::iterator i = vectorOfAttrInput.begin(); i != vectorOfAttrInput.end(); i++) {
           (*i)->hiddeAttribute();
        }
        
        // Gets attributes of element
        const std::vector<SumoXMLAttr>& attrs = myACs.front()->getAttrs();
        
        // Declare iterator over AttrImput
        std::vector<GNEInspectorFrame::AttrInput*>::iterator itAttrs = vectorOfAttrInput.begin();

        // Iterate over attributes
        for (std::vector<SumoXMLAttr>::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
            if (myACs.size() > 1 && GNEAttributeCarrier::isUnique(*it)) {
                // disable editing for some attributes in case of multi-selection
                // even displaying is problematic because of string rendering restrictions
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = myACs.begin(); it_ac != myACs.end(); it_ac++) {
                occuringValues.insert((*it_ac)->getAttribute(*it));
            }
            // get current value
            std::ostringstream oss;
            for (std::set<std::string>::iterator it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // Show attribute
            (*itAttrs)->showAttribute(myACs.front()->getTag(), *it, oss.str());
            itAttrs++;
        } 
            
        // If attributes correspond to an Edge
        if (dynamic_cast<GNEEdge*>(myACs.front())) {
            // show groupBox for templates
            myGroupBoxForTemplates->show();
            // show "Copy Template" (caption supplied via onUpdate)
            myCopyTemplateButton->show();
            // show "Set As Template"
            if (myACs.size() == 1) {
                mySetTemplateButton->show();
            }
        } else {
            // Hidde all template elements
            myGroupBoxForTemplates->hide();
            myCopyTemplateButton->hide();
            mySetTemplateButton->hide();
        }

        // If attributes correspond to an Additional
        if(dynamic_cast<GNEAdditional*>(myACs.front())) {
            // Get pointer to additional
            myAdditional = dynamic_cast<GNEAdditional*>(myACs.front());
            // Show groupBox for editor Attributes
            myGroupBoxForEditor->show();
            // Show check blocked
            myCheckBlocked->setCheck(myAdditional->isBlocked());
            myCheckBlocked->show();
        } else {
            // Hide all additional elements
            myGroupBoxForEditor->hide();
            myGroupBoxForEditor->hide();
            myCheckBlocked->hide();
        }
    } else {
        getFrameHeaderLabel()->setText("No Object selected");
        // Hide all elements
        myGroupBoxForAttributes->hide();
        myGroupBoxForTemplates->hide();
        myCopyTemplateButton->hide();
        mySetTemplateButton->hide();
        myGroupBoxForEditor->hide();
        myGroupBoxForEditor->hide();
        myCheckBlocked->hide();
    }
}

GNEEdge*
GNEInspectorFrame::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspectorFrame::setEdgeTemplate");
}


long
GNEInspectorFrame::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (std::vector<GNEAttributeCarrier*>::iterator it = myACs.begin(); it != myACs.end(); it++) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(*it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myViewNet->getUndoList());
        inspect(myACs);
    }
    return 1;
}


long
GNEInspectorFrame::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myACs.size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myACs.front());
    assert(edge);
    setEdgeTemplate(edge);
    return 1;
}


long
GNEInspectorFrame::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    FXString caption;
    if (myEdgeTemplate) {
        caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "'").c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    } else {
        caption = "No Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}


long
GNEInspectorFrame::onCmdSetBlocking(FXObject*, FXSelector, void*) {
    if(myAdditional) {
        myAdditional->setBlocked(myCheckBlocked->getCheck() == 1? true : false);
        myViewNet->update();
    }
    return 1;
}
 

const std::vector<GNEAttributeCarrier*> &
GNEInspectorFrame::getACs() const {
    return myACs;
}

// ===========================================================================
// AttrInput method definitions
// ===========================================================================

GNEInspectorFrame::AttrInput::AttrInput(FXComposite* parent, GNEInspectorFrame *inspectorFrameParent) :
    FXMatrix(parent, 8, MATRIX_BY_COLUMNS | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH),
    myInspectorFrameParent(inspectorFrameParent), 
    myTag(SUMO_TAG_NOTHING), 
    myAttr(SUMO_ATTR_NOTHING) {
    // Create and hidde ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", 0, this, MID_GNE_OPEN_ATTRIBUTE_EDITOR, ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED);
    myButtonCombinableChoices->hide();
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myLabel->hide();
    // Create and hide textField int
    myTextFieldInt = new FXTextField(this, 1, this, MID_GNE_SET_ATTRIBUTE, FRAME_THICK | TEXTFIELD_INTEGER | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myTextFieldInt->hide();
    // Create and hide textField real
    myTextFieldReal = new FXTextField(this, 1, this, MID_GNE_SET_ATTRIBUTE, FRAME_THICK | TEXTFIELD_REAL | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myTextFieldReal->hide();
    // Create and hide textField string
    myTextFieldStrings = new FXTextField(this, 1, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myTextFieldStrings->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, 1, this, MID_GNE_SET_ATTRIBUTE, FRAME_THICK | LAYOUT_CENTER_Y | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myCheckBox = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myCheckBox->hide();
}

    
void 
GNEInspectorFrame::AttrInput::showAttribute(SumoXMLTag tag, SumoXMLAttr attr, const std::string &value) {
    // Set actual Tag and attribute
    myTag = tag;
    myAttr = attr;
    // ShowLabel
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    // Set field depending of the type of value
    if(GNEAttributeCarrier::isBool(myAttr)) {
        // set value of checkbox
        if(value == "true") {
            myCheckBox->setCheck(true);
        } else {
            myCheckBox->setCheck(false);
        }
        myCheckBox->show();
    } else if(GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if(choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // hide label
            myLabel->hide();
            // Show button combinable choices
            myButtonCombinableChoices->setText(toString(myAttr).c_str());
            myButtonCombinableChoices->show();
            // Show string with the values
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
        } else {
            // fill comboBox
            myChoicesCombo->clearItems();
            for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
                myChoicesCombo->appendItem(it->c_str());
            }
            myChoicesCombo->setNumVisible((int)choices.size());
            myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(value.c_str()));
            myChoicesCombo->show();
        }
    } else if(GNEAttributeCarrier::isFloat(myAttr)) {
        // show TextField for real values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
    } else if(GNEAttributeCarrier::isInt(myAttr)) {
        // Show textField for int attributes
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->show();
    } else {
        // In any other case (String, list, etc.), show value as String
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->show();
    }
    // Show AttrInput
    show();
}


void 
GNEInspectorFrame::AttrInput::hiddeAttribute() {
    // Hide all elements
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myChoicesCombo->hide();
    myCheckBox->hide();
    myButtonCombinableChoices->hide();
    // hide AttrInput
    hide();
}


SumoXMLTag 
GNEInspectorFrame::AttrInput::getTag() const {
    return myTag;
}


SumoXMLAttr 
GNEInspectorFrame::AttrInput::getAttr() const {
    return myAttr;
}


long
GNEInspectorFrame::AttrInput::onCmdOpenAttributeEditor(FXObject*, FXSelector, void*) {
    // Open AttrEditor
    AttrEditor(this, myTextFieldStrings);
    return 1;
}


long
GNEInspectorFrame::AttrInput::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // First, obtain the string value of the new attribute depending of their type
    if(GNEAttributeCarrier::isBool(myAttr)) {
        // Set true o false depending of the checBox
        if(myCheckBox->getCheck()) {
            newVal = "true";
        } else {
            newVal = "false";
        }
    } else if(GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if(choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // Get value obtained using AttrEditor
            newVal = myTextFieldStrings->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        }
    } else if(GNEAttributeCarrier::isFloat(myAttr)) {
        // obtain value of myTextFieldReal
        newVal = myTextFieldReal->getText().text();
    } else if(GNEAttributeCarrier::isInt(myAttr)) {
        // obtain value of myTextFieldInt
        newVal = myTextFieldInt->getText().text();
    } else if(GNEAttributeCarrier::isString(myAttr)) {
        // obtain value of myTextFieldStrings
        newVal = myTextFieldStrings->getText().text();
    }

    // Check if newvalue is valid 
    if (myInspectorFrameParent->getACs().front()->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myInspectorFrameParent->getACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        // Set all attributes
        for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = myInspectorFrameParent->getACs().begin(); it_ac != myInspectorFrameParent->getACs().end(); it_ac++) {
            (*it_ac)->setAttribute(myAttr, newVal, myInspectorFrameParent->getViewNet()->getUndoList());
        }
        if (myInspectorFrameParent->getACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // If previously value of TextField was red, change color to black
        if(GNEAttributeCarrier::isFloat(myAttr) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if(GNEAttributeCarrier::isInt(myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (GNEAttributeCarrier::isString(myAttr) && myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
    } else {
        // IF value of TextField isn't valid, change color to Red depending of type
        if(GNEAttributeCarrier::isFloat(myAttr) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if(GNEAttributeCarrier::isInt(myAttr) && myTextFieldStrings != 0) { 
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isString(myAttr) && myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }
    }
    // Update view net
    myInspectorFrameParent->getViewNet()->update();
    return 1;
}


void 
GNEInspectorFrame::AttrInput::show() {
    FXMatrix::show();
}


void 
GNEInspectorFrame::AttrInput::hide() {
    FXMatrix::hide();
}

// ===========================================================================
// AttrEditor method definitions
// ===========================================================================

GNEInspectorFrame::AttrEditor::AttrEditor(AttrInput *attrInputParent, FXTextField *textFieldAttr) :
    FXDialogBox(attrInputParent->getApp(), ("Editing attribute '" + toString(attrInputParent->getAttr()) + "'").c_str(), DECOR_CLOSE | DECOR_TITLE),
    myAttrInputParent(attrInputParent),
    myTextFieldAttr(textFieldAttr) {
    // Create matrix
    myCheckBoxMatrix = new FXMatrix(this, 2, MATRIX_BY_COLUMNS);

    // Obtain vector with the choices
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myAttrInputParent->getTag(), myAttrInputParent->getAttr());
    
    // Get old value
    const std::string oldValue = myTextFieldAttr->getText().text();

    // Resize myVectorOfCheckBox
    myVectorOfCheckBox.resize(choices.size(), NULL);

    // Iterate over choices
    for (int i = 0; i < (int)choices.size(); i++) {
        // Create checkBox
        myVectorOfCheckBox.at(i) = new FXCheckButton(myCheckBoxMatrix, choices.at(i).c_str());
        // Set initial value
        if (oldValue.find(choices.at(i)) != std::string::npos) {
            myVectorOfCheckBox.at(i)->setCheck(true);
        }
    }

    // Add separator
    new FXHorizontalSeparator(this, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);

    // Create frame for buttons
    frameButtons = new FXHorizontalFrame(this, LAYOUT_FILL_X);

    // Create accept button
    myAcceptButton = new FXButton(frameButtons, "Accept", 0, this, FXDialogBox::ID_ACCEPT, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED);

    // Create cancel button
    myCancelButton = new FXButton(frameButtons, "Cancel", 0, this, FXDialogBox::ID_CANCEL, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED);

    // Create reset button
    myResetButton = new FXButton(frameButtons, "Reset", 0, this, MID_GNE_MODE_INSPECT_RESET, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED);

    // Execute dialog to make it modal, and if user press button "accept", save attribute
    if (execute()) {
        std::vector<std::string> attrSolution;
        // Iterate  over myVectorOfCheckBox
        for (int i = 0; i < (int)myVectorOfCheckBox.size(); i++) {
            // If checkBox is cheked, save attribute
            if (myVectorOfCheckBox.at(i)->getCheck()) {
                attrSolution.push_back(std::string(myVectorOfCheckBox.at(i)->getText().text()));
            }
        }
        // join to string
        myTextFieldAttr->setText(joinToString(attrSolution, " ").c_str());
        // Set attribute
        myAttrInputParent->onCmdSetAttribute(0, 0, 0);
    }
}


GNEInspectorFrame::AttrEditor::~AttrEditor() {}

long
GNEInspectorFrame::AttrEditor::onCmdReset(FXObject*, FXSelector, void*) {
    // Obtain vector with the choices
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myAttrInputParent->getTag(), myAttrInputParent->getAttr());
    // Get old value
    const std::string oldValue = myTextFieldAttr->getText().text();
    // Reset values
    for (int i = 0; i < (int)choices.size(); i++) {
        if (oldValue.find(choices.at(i)) != std::string::npos) {
            myVectorOfCheckBox.at(i)->setCheck(true);
        } else {
            myVectorOfCheckBox.at(i)->setCheck(false);
        }
    }
    return 1;
}

/****************************************************************************/
