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
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEFrameDesigns.h"
#include "GNEInspectorFrame.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEAttributeCarrier.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNEConnection.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static
// ===========================================================================

const int MAXNUMBEROFATTRCONNECTIONS = 50;

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_COPY_TEMPLATE,  GNEInspectorFrame::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_TEMPLATE,   GNEInspectorFrame::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_COPY_TEMPLATE,  GNEInspectorFrame::onUpdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_BLOCKING,   GNEInspectorFrame::onCmdSetBlocking),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INSPECT_GOBACK, GNEInspectorFrame::onCmdGoBack),
};


FXDEFMAP(GNEInspectorFrame::AttrInput) AttrInputMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE,         GNEInspectorFrame::AttrInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEInspectorFrame::AttrInput::onCmdOpenAttributeEditor)
};

FXDEFMAP(GNEInspectorFrame::AttrEditor) AttrEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_INSPECT_RESET, GNEInspectorFrame::AttrEditor::onCmdReset),
};

FXDEFMAP(GNEInspectorFrame::AttrConnection) AttrConnectionMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOW_CONNECTION, GNEInspectorFrame::AttrConnection::onCmdSetShowConnection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INSPECT_CONNECTION, GNEInspectorFrame::AttrConnection::onCmdInspectConnection),
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame, FXScrollWindow, GNEInspectorFrameMap, ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::AttrInput, FXMatrix, AttrInputMap, ARRAYNUMBER(AttrInputMap))
FXIMPLEMENT(GNEInspectorFrame::AttrEditor, FXDialogBox, AttrEditorMap, ARRAYNUMBER(AttrEditorMap))
FXIMPLEMENT(GNEInspectorFrame::AttrConnection, FXHorizontalFrame, AttrConnectionMap, ARRAYNUMBER(AttrConnectionMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXHorizontalFrame *horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Inspector"),
    myEdgeTemplate(0),
    myAdditional(0),
    myPreviousElement(0) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_NETEDITARROW), this, MID_GNE_INSPECT_GOBACK, GNEDesignButtonHelp);
    myBackButton->hide();

    // Create groupBox for attributes
    myGroupBoxForAttributes = new FXGroupBox(myContentFrame, "attributes", GNEDesignGroupBoxFrame);
    myGroupBoxForAttributes->hide();

    // Create AttrInput
    for (int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        vectorOfAttrInput.push_back(new AttrInput(myGroupBoxForAttributes, this));
    }

    // Create groupBox for templates
    myGroupBoxForTemplates = new FXGroupBox(myContentFrame, "Templates", GNEDesignGroupBoxFrame);
    myGroupBoxForTemplates->hide();

    // Create copy template button
    myCopyTemplateButton = new FXButton(myGroupBoxForTemplates, "", 0, this, MID_GNE_COPY_TEMPLATE, GNEDesignButton, 0, 0, 0, 0, 4, 4, 3, 3);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(myGroupBoxForTemplates, "Set as Template\t\t", 0, this, MID_GNE_SET_TEMPLATE, GNEDesignButton, 0, 0, 0, 0, 4, 4, 3, 3);
    mySetTemplateButton->hide();

    // Create groupBox for editor parameters
    myGroupBoxForEditor = new FXGroupBox(myContentFrame, "editor", GNEDesignGroupBoxFrame);
    myGroupBoxForEditor->hide();

    // Create check blocked button
    myCheckBlocked = new FXCheckButton(myGroupBoxForEditor, "Block movement", this, MID_GNE_SET_BLOCKING, GNEDesignCheckButton);
    myCheckBlocked->hide();

    // Create groupBox for AttrConnection
    myGroupBoxForAttrConnections = new FXGroupBox(myContentFrame, "Connections", GNEDesignGroupBoxFrame);
    myGroupBoxForAttrConnections->hide();

    // Create AttrConnections
    for (int i = 0; i < MAXNUMBEROFATTRCONNECTIONS; i++) {
        myAttrConnections.push_back(new AttrConnection(myGroupBoxForAttrConnections, this));
    }
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
GNEInspectorFrame::inspect(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement) {
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    std::vector<GNEAttributeCarrier*> itemToInspect;
    itemToInspect.push_back(AC);
    inspect(itemToInspect, previousElement);
}


void
GNEInspectorFrame::inspect(const std::vector<GNEAttributeCarrier*>& ACs, GNEAttributeCarrier* previousElement) {
    // Assing ACs to myACs
    myACs = ACs;
    // Show back button if previousElement was defined
    myPreviousElement = previousElement;
    if (myPreviousElement != NULL) {
        myHeaderLeftFrame->show();
        myBackButton->show();
    } else {
        myHeaderLeftFrame->hide();
        myBackButton->hide();
    }
    // Hide all elements
    myGroupBoxForAttributes->hide();
    myGroupBoxForTemplates->hide();
    myCopyTemplateButton->hide();
    mySetTemplateButton->hide();
    myGroupBoxForEditor->hide();
    myGroupBoxForEditor->hide();
    myCheckBlocked->hide();
    myGroupBoxForAttrConnections->hide();
    // If vector of attribute Carriers contain data
    if (myACs.size() > 0) {
        // Set header
        std::string headerString;
        if (myACs.size() > 1) {
            headerString = "Selection: " + toString(myACs.size()) + " " + toString(myACs.front()->getTag()) + "s";
        } else {
            if (dynamic_cast<GNENetElement*>(myACs.front())) {
                headerString = "Net: " + toString(myACs.front()->getTag());
            } else if (dynamic_cast<GNEAdditional*>(myACs.front())) {
                headerString = "Additional: " + toString(myACs.front()->getTag());
            }
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        //Show myGroupBoxForAttributes
        myGroupBoxForAttributes->show();

        // Hide all AttrInput
        for (std::vector<GNEInspectorFrame::AttrInput*>::iterator i = vectorOfAttrInput.begin(); i != vectorOfAttrInput.end(); i++) {
            (*i)->hideAttribute();
        }

        // Hide all AttrConnections
        for (int i = 0; i < MAXNUMBEROFATTRCONNECTIONS; i++) {
            myAttrConnections.at(i)->hideAttrConnection();
        }

        // Gets tag and tattributes of element
        SumoXMLTag tag = myACs.front()->getTag();
        const std::vector<SumoXMLAttr>& attrs = myACs.front()->getAttrs();

        // Declare iterator over AttrImput
        std::vector<GNEInspectorFrame::AttrInput*>::iterator itAttrs = vectorOfAttrInput.begin();

        // Iterate over attributes
        for (std::vector<SumoXMLAttr>::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
            if (myACs.size() > 1 && GNEAttributeCarrier::isUnique(tag, *it)) {
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
            // Obtain connections of edge
            const std::vector<GNEConnection*>& connections = dynamic_cast<GNEEdge*>(myACs.front())->getGNEConnections();
            if (connections.size() > 0) {
                // Check if all connections are editables
                if ((int)connections.size() > MAXNUMBEROFATTRCONNECTIONS) {
                    WRITE_WARNING("Number of connections of " + myACs.front()->getID() + " is greater than the number of editable connections (" + toString(MAXNUMBEROFATTRCONNECTIONS) + ")");
                }
                // Show AttrConnections
                for (int i = 0; i < (int)connections.size() && i < MAXNUMBEROFATTRCONNECTIONS; i++) {
                    myAttrConnections.at(i)->showConnections(connections.at(i));
                }
                myGroupBoxForAttrConnections->show();
            }
        }

        // If attributes correspond to a lane
        if (dynamic_cast<GNELane*>(myACs.front())) {
            // Obtain connections of lane
            std::vector<GNEConnection*> connections = dynamic_cast<GNELane*>(myACs.front())->getGNEOutcomingConnections();
            if (connections.size() > 0) {
                // Check if all connections are editables
                if ((int)connections.size() > MAXNUMBEROFATTRCONNECTIONS) {
                    WRITE_WARNING("Number of connections of " + myACs.front()->getID() + " is greater than the number of editable connections (" + toString(MAXNUMBEROFATTRCONNECTIONS) + ")");
                }
                // Show AttrConnections
                for (int i = 0; i < (int)connections.size() && i < MAXNUMBEROFATTRCONNECTIONS; i++) {
                    myAttrConnections.at(i)->showConnections(connections.at(i));
                }
                myGroupBoxForAttrConnections->show();
            }
        }

        // If attributes correspond to an Additional
        if (dynamic_cast<GNEAdditional*>(myACs.front())) {
            // Get pointer to additional
            myAdditional = dynamic_cast<GNEAdditional*>(myACs.front());
            bool showGroupBoxForEditor = false;

            // Show check blocked if additional is movable
            if (myAdditional->isAdditionalMovable()) {
                myCheckBlocked->setCheck(myAdditional->isAdditionalBlocked());
                myCheckBlocked->show();
                showGroupBoxForEditor = true;
            }
            // Show groupBox for editor Attributes if some of additional attributes are editable
            if (showGroupBoxForEditor == true) {
                myGroupBoxForEditor->show();
            }
        }
    } else {
        getFrameHeaderLabel()->setText("No Object selected");
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
    if (myAdditional) {
        if (myCheckBlocked->getCheck() == 1) {
            myAdditional->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "true", getViewNet()->getUndoList());
        } else {
            myAdditional->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "false", getViewNet()->getUndoList());
        }
    }
    return 1;
}


long
GNEInspectorFrame::onCmdGoBack(FXObject*, FXSelector, void*) {
    // Inspect previous element (if was defined)
    if (myPreviousElement) {
        inspect(myPreviousElement);
    }
    return 1;
}


const std::vector<GNEAttributeCarrier*>&
GNEInspectorFrame::getACs() const {
    return myACs;
}

// ===========================================================================
// AttrInput method definitions
// ===========================================================================

GNEInspectorFrame::AttrInput::AttrInput(FXComposite* parent, GNEInspectorFrame* inspectorFrameParent) :
    FXMatrix(parent, 7, GNEDesignMatrixAttributes),
    myInspectorFrameParent(inspectorFrameParent),
    myTag(SUMO_TAG_NOTHING),
    myAttr(SUMO_ATTR_NOTHING) {
    // Create and hide ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", 0, this, MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEDesignButtonAttribute);
    myButtonCombinableChoices->hide();
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, GNEDesignLabelAttribute);
    myLabel->hide();
    // Create and hide textField int
    myTextFieldInt = new FXTextField(this, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeInt);
    myTextFieldInt->hide();
    // Create and hide textField real
    myTextFieldReal = new FXTextField(this, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeReal);
    myTextFieldReal->hide();
    // Create and hide textField string
    myTextFieldStrings = new FXTextField(this, GNEDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignTextFieldAttributeStr);
    myTextFieldStrings->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, GNEDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GNEDesignComboBoxAttribute);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myCheckBox = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GNEDesignCheckButtonAttribute);
    myCheckBox->hide();
}


void
GNEInspectorFrame::AttrInput::showAttribute(SumoXMLTag tag, SumoXMLAttr attr, const std::string& value) {
    // Set actual Tag and attribute
    myTag = tag;
    myAttr = attr;
    // ShowLabel
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    // Set field depending of the type of value
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
        // set value of checkbox
        myCheckBox->setCheck(GNEAttributeCarrier::parse<bool>(value));
        myCheckBox->show();
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
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
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr)) {
        // show TextField for real values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
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
GNEInspectorFrame::AttrInput::hideAttribute() {
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
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
        // Set true o false depending of the checBox
        if (myCheckBox->getCheck()) {
            newVal = "true";
        } else {
            newVal = "false";
        }
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // Get value obtained using AttrEditor
            newVal = myTextFieldStrings->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        }
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr)) {
        // obtain value of myTextFieldReal
        newVal = myTextFieldReal->getText().text();
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
        // obtain value of myTextFieldInt
        newVal = myTextFieldInt->getText().text();
    } else if (GNEAttributeCarrier::isString(myTag, myAttr)) {
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
        if (GNEAttributeCarrier::isFloat(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (GNEAttributeCarrier::isString(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
    } else {
        // IF value of TextField isn't valid, change color to Red depending of type
        if (GNEAttributeCarrier::isFloat(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isString(myTag, myAttr) && myTextFieldStrings != 0) {
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

GNEInspectorFrame::AttrEditor::AttrEditor(AttrInput* attrInputParent, FXTextField* textFieldAttr) :
    FXDialogBox(attrInputParent->getApp(), ("Editing attribute '" + toString(attrInputParent->getAttr()) + "'").c_str(), DECOR_CLOSE | DECOR_TITLE),
    myAttrInputParent(attrInputParent),
    myTextFieldAttr(textFieldAttr) {
    // Create matrix
    myCheckBoxMatrix = new FXMatrix(this, 2, GNEDesignMatrixAttributes);

    // Obtain vector with the choices
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myAttrInputParent->getTag(), myAttrInputParent->getAttr());

    // Get old value
    const std::string oldValue = myTextFieldAttr->getText().text();

    // Resize myVectorOfCheckBox
    myVectorOfCheckBox.resize(choices.size(), NULL);

    // Iterate over choices
    for (int i = 0; i < (int)choices.size(); i++) {
        // Create checkBox
        myVectorOfCheckBox.at(i) = new FXCheckButton(myCheckBoxMatrix, choices.at(i).c_str(),NULL, 0, GNEDesignCheckButtonAttribute);
        // Set initial value
        if (oldValue.find(choices.at(i)) != std::string::npos) {
            myVectorOfCheckBox.at(i)->setCheck(true);
        }
    }

    // Add separator
    new FXHorizontalSeparator(this, GNEDesignHorizontalSeparator);

    // Create frame for buttons
    frameButtons = new FXHorizontalFrame(this, GNEDesignHorizontalFrame);

    // Create accept button
    myAcceptButton = new FXButton(frameButtons, "Accept", 0, this, FXDialogBox::ID_ACCEPT, GNEDesignButtonDialog);

    // Create cancel button
    myCancelButton = new FXButton(frameButtons, "Cancel", 0, this, FXDialogBox::ID_CANCEL, GNEDesignButtonDialog);

    // Create reset button
    myResetButton = new FXButton(frameButtons, "Reset", 0, this, MID_GNE_MODE_INSPECT_RESET, GNEDesignButtonDialog);

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



GNEInspectorFrame::AttrConnection::AttrConnection(FXComposite* parent, GNEInspectorFrame* inspectorFrameParent) :
    FXHorizontalFrame(parent, GNEDesignHorizontalFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myConnection(NULL) {
    // Create label for connection
    myConnectionInfoLabel = new FXLabel(this, "", NULL, GNEDesignLabelThick);
    // Create checkButton for show connection
    myShowConnection = new FXCheckButton(this,"Show", this, MID_GNE_SHOW_CONNECTION, GNEDesignCheckButton);
    // Create FXButton for inspectConnection
    myInspectConnection = new FXButton(this, "inspect", 0, this, MID_GNE_INSPECT_CONNECTION, GNEDesignButton);
}


GNEInspectorFrame::AttrConnection::~AttrConnection() {}


void
GNEInspectorFrame::AttrConnection::showConnections(GNEConnection* connection) {
    // Set pointer to current connection
    myConnection = connection;
    // set Label
    const NBEdge::Connection& con = myConnection->getNBEdgeConnection();
    myConnectionInfoLabel->setText(std::string(
                                       myConnection->getEdgeFrom()->getNBEdge()->getLaneID(con.toLane) +
                                       "->" + con.toEdge->getLaneID(con.toLane)).c_str());
    // Show Label
    myConnectionInfoLabel->show();
    // set show Connection
    myShowConnection->setCheck(myConnection->getDrawConnection());
    // show Show Connection
    myShowConnection->show();
    // Show AttrConnection
    show();
}


void
GNEInspectorFrame::AttrConnection::hideAttrConnection() {
    // hide all elements
    myConnectionInfoLabel->hide();
    myShowConnection->hide();
    hide();
}


long
GNEInspectorFrame::AttrConnection::onCmdSetShowConnection(FXObject*, FXSelector, void*) {
    if (myShowConnection->getCheck()) {
        myConnection->setDrawConnection(true);
    } else {
        myConnection->setDrawConnection(false);
    }
    // Update view net
    myInspectorFrameParent->getViewNet()->update();
    return 1;
}


long
GNEInspectorFrame::AttrConnection::onCmdInspectConnection(FXObject*, FXSelector, void*) {
    // Inspect connection depending of the checkBox "selectEdges"
    if (myInspectorFrameParent->getViewNet()->selectEdges()) {
        myInspectorFrameParent->inspect(myConnection, myConnection->getEdgeFrom());
    } else {
        myInspectorFrameParent->inspect(myConnection, myConnection->getLaneFrom());
    }
    return 1;
}


void
GNEInspectorFrame::AttrConnection::show() {
    FXHorizontalFrame::show();
}


void
GNEInspectorFrame::AttrConnection::hide() {
    FXHorizontalFrame::hide();
}

/****************************************************************************/
