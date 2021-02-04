/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEMatchGenericDataAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMatchAttribute.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMatchAttribute) GNEMatchAttributeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNEMatchAttribute::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNEMatchAttribute::onCmdSelMBAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNEMatchAttribute::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNEMatchAttribute::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEMatchAttribute, FXGroupBox, GNEMatchAttributeMap, ARRAYNUMBER(GNEMatchAttributeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEMatchAttribute::GNEMatchAttribute(GNESelectorFrame* selectorFrameParent) :
    FXGroupBox(selectorFrameParent->myContentFrame, "Match Attribute", GUIDesignGroupBoxFrame),
    mySelectorFrameParent(selectorFrameParent),
    myCurrentTag(SUMO_TAG_EDGE),
    myCurrentAttribute(SUMO_ATTR_ID) {
    // Create MatchTagBox for tags
    myMatchTagComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchAttrComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // create button
    myMatchStringButton = new FXButton(this, "Apply selection", nullptr, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignButton);
    // Create help button
    new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
    // Fill list of sub-items (first element will be "edge")
    enableMatchAttribute();
    // Set speed of edge as default attribute
    myMatchAttrComboBox->setText("speed");
    myCurrentAttribute = SUMO_ATTR_SPEED;
    // Set default value for Match string
    myMatchString->setText(">10.0");
}


GNEMatchAttribute::~GNEMatchAttribute() {}


void
GNEMatchAttribute::enableMatchAttribute() {
    // enable comboBox, text field and button
    myMatchTagComboBox->enable();
    myMatchAttrComboBox->enable();
    myMatchString->enable();
    myMatchStringButton->enable();
    // Clear items of myMatchTagComboBox
    myMatchTagComboBox->clearItems();
    // Set items depending of current item set
    std::vector<std::pair<SumoXMLTag, const std::string> > ACTags;
    if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::NETWORKELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::NETWORKELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::ADDITIONALELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::ADDITIONALELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::SHAPE) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::SHAPE, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::TAZELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::TAZELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::DEMANDELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT | GNETagProperties::TagType::STOP, true);
    } else {
        throw ProcessError("Invalid element set");
    }
    // fill combo box
    for (const auto& ACTag : ACTags) {
        myMatchTagComboBox->appendItem(ACTag.second.c_str());
    }
    // set first item as current item
    myMatchTagComboBox->setCurrentItem(0);
    myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
    // Fill attributes with the current element type
    onCmdSelMBTag(nullptr, 0, nullptr);
}


void
GNEMatchAttribute::disableMatchAttribute() {
    // disable comboboxes and text field
    myMatchTagComboBox->disable();
    myMatchAttrComboBox->disable();
    myMatchString->disable();
    myMatchStringButton->disable();
    // change colors to black (even if there are invalid values)
    myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchString->setTextColor(FXRGB(0, 0, 0));
}


void
GNEMatchAttribute::showMatchAttribute() {
    show();
}


void
GNEMatchAttribute::hideMatchAttribute() {
    hide();
}


long
GNEMatchAttribute::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    // First check what type of elementes is being selected
    myCurrentTag = SUMO_TAG_NOTHING;
    // find current element tag
    std::vector<std::pair<SumoXMLTag, const std::string> > ACTags;
    if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::NETWORKELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::NETWORKELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::ADDITIONALELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::ADDITIONALELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::SHAPE) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::SHAPE, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::TAZELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::TAZELEMENT, true);
    } else if (mySelectorFrameParent->myElementSet->getElementSet() == GNESelectorFrame::ElementSet::Type::DEMANDELEMENT) {
        ACTags = GNEAttributeCarrier::getAllowedTagsByCategory(GNETagProperties::TagType::DEMANDELEMENT | GNETagProperties::TagType::STOP, true);
    } else {
        throw ProcessError("Unkown set");
    }
    // fill myMatchTagComboBox
    for (const auto& ACTag : ACTags) {
        if (ACTag.second == myMatchTagComboBox->getText().text()) {
            myCurrentTag = ACTag.first;
        }
    }
    // check that typed-by-user value is correct
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
        // set color and enable items
        myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->enable();
        myMatchString->enable();
        myMatchStringButton->enable();
        myMatchAttrComboBox->clearItems();
        // fill attribute combo box
        for (const auto& attribute : tagValue) {
            myMatchAttrComboBox->appendItem(attribute.getAttrStr().c_str());
        }
        // Add extra attribute "Parameter"
        myMatchAttrComboBox->appendItem(toString(GNE_ATTR_PARAMETERS).c_str());
        // check if item can block movement
        if (tagValue.canBlockMovement()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_BLOCK_MOVEMENT).c_str());
        }
        // check if item can close shape
        if (tagValue.canCloseShape()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_CLOSE_SHAPE).c_str());
        }
        // check if item can have parent
        if (tagValue.isSlave()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_PARENT).c_str());
        }
        // @ToDo: Here can be placed a button to set the default value
        myMatchAttrComboBox->setNumVisible(myMatchAttrComboBox->getNumItems());
        // check if we have to update attribute
        if (tagValue.hasAttribute(myCurrentAttribute)) {
            myMatchAttrComboBox->setText(toString(myCurrentAttribute).c_str());
        } else {
            onCmdSelMBAttribute(nullptr, 0, nullptr);
        }
    } else {
        // change color to red and disable items
        myMatchTagComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchAttrComboBox->disable();
        myMatchString->disable();
        myMatchStringButton->disable();
    }
    update();
    return 1;
}


long
GNEMatchAttribute::onCmdSelMBAttribute(FXObject*, FXSelector, void*) {
    // first obtain a copy of item attributes vinculated with current tag
    auto tagPropertiesCopy = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    // obtain tag property (only for improve code legibility)
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    // add an extra AttributeValues to allow select ACs using as criterium "parameters"
    GNEAttributeProperties extraAttrProperty;
    extraAttrProperty = GNEAttributeProperties(GNE_ATTR_PARAMETERS,
                        GNEAttributeProperties::AttrProperty::STRING,
                        "Parameters");
    tagPropertiesCopy.addAttribute(extraAttrProperty);
    // add extra attribute if item can block movement
    if (tagValue.canBlockMovement()) {
        // add an extra AttributeValues to allow select ACs using as criterium "block movement"
        extraAttrProperty = GNEAttributeProperties(GNE_ATTR_BLOCK_MOVEMENT,
                            GNEAttributeProperties::AttrProperty::BOOL | GNEAttributeProperties::AttrProperty::DEFAULTVALUESTATIC,
                            "Block movement",
                            "false");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can close shape
    if (tagValue.canCloseShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "close shape"
        extraAttrProperty = GNEAttributeProperties(GNE_ATTR_CLOSE_SHAPE,
                            GNEAttributeProperties::AttrProperty::BOOL | GNEAttributeProperties::AttrProperty::DEFAULTVALUESTATIC,
                            "Close shape",
                            "true");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can have parent
    if (tagValue.isSlave()) {
        // add an extra AttributeValues to allow select ACs using as criterium "parent"
        extraAttrProperty = GNEAttributeProperties(GNE_ATTR_PARENT,
                            GNEAttributeProperties::AttrProperty::STRING,
                            "Parent element");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // set current selected attribute
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (const auto& attribute : tagPropertiesCopy) {
        if (attribute.getAttrStr() == myMatchAttrComboBox->getText().text()) {
            myCurrentAttribute = attribute.getAttr();
        }
    }
    // check if selected attribute is valid
    if (myCurrentAttribute != SUMO_ATTR_NOTHING) {
        myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchString->enable();
        myMatchStringButton->enable();
    } else {
        myMatchAttrComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchString->disable();
        myMatchStringButton->disable();
    }
    return 1;
}


long
GNEMatchAttribute::onCmdSelMBString(FXObject*, FXSelector, void*) {
    // obtain expresion
    std::string expr(myMatchString->getText().text());
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myCurrentTag);
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr));
    } else if (tagValue.hasAttribute(myCurrentAttribute) && tagValue.getAttributeProperties(myCurrentAttribute).isNumerical()) {
        // The expression must have the form
        //  <val matches if attr < val
        //  >val matches if attr > val
        //  =val matches if attr = val
        //  val matches if attr = val
        char compOp = expr[0];
        if (compOp == '<' || compOp == '>' || compOp == '=') {
            expr = expr.substr(1);
        } else {
            compOp = '=';
        }
        // check if value can be parsed to double
        if (GNEAttributeCarrier::canParse<double>(expr.c_str())) {
            mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr));
        } else {
            valid = false;
        }
    } else {
        // The expression must have the form
        //   =str: matches if <str> is an exact match
        //   !str: matches if <str> is not a substring
        //   ^str: matches if <str> is not an exact match
        //   str: matches if <str> is a substring (sends compOp '@')
        // Alternatively, if the expression is empty it matches all objects
        char compOp = expr[0];
        if (compOp == '=' || compOp == '!' || compOp == '^') {
            expr = expr.substr(1);
        } else {
            compOp = '@';
        }
        mySelectorFrameParent->handleIDs(mySelectorFrameParent->getMatches(myCurrentTag, myCurrentAttribute, compOp, 0, expr));
    }
    if (valid) {
        myMatchString->setTextColor(FXRGB(0, 0, 0));
        myMatchString->killFocus();
        myMatchStringButton->enable();
    } else {
        myMatchString->setTextColor(FXRGB(255, 0, 0));
        myMatchStringButton->disable();
    }
    return 1;
}


long
GNEMatchAttribute::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- The 'Match Attribute' controls allow to specify a set of objects which are then applied to the current selection\n"
            << "  according to the current 'Modification Mode'.\n"
            << "     1. Select an object type from the first input box\n"
            << "     2. Select an attribute from the second input box\n"
            << "     3. Enter a 'match expression' in the third input box and press <return>\n"
            << "\n"
            << "- The empty expression matches all objects\n"
            << "- For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n"
            << "- An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n"
            << "\n"
            << "- For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n"
            << "     '' (no operator) matches if string is a substring of that object'ts attribute.\n"
            << "     '=' matches if string is an exact match.\n"
            << "     '!' matches if string is not a substring.\n"
            << "     '^' matches if string is not an exact match.\n"
            << "\n"
            << "- Examples:\n"
            << "     junction; id; 'foo' -> match all junctions that have 'foo' in their id\n"
            << "     junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n"
            << "     edge; speed; '>10' -> match all edges with a speed above 10\n";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening help dialog of selector frame");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Close help dialog of selector frame");
    return 1;
}

/****************************************************************************/
