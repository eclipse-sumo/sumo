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
/// @file    GNEMatchAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
/****************************************************************************/
#include <config.h>

#include <utils/geom/GeoConvHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMatchAttribute.h"
#include "GNEElementSet.h"

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
FXIMPLEMENT(GNEMatchAttribute, FXGroupBoxModule, GNEMatchAttributeMap, ARRAYNUMBER(GNEMatchAttributeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEMatchAttribute::GNEMatchAttribute(GNEElementSet* elementSet, SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue) :
    FXGroupBoxModule(elementSet->getSelectorFrameParent()->getContentFrame(), "Match Attribute"),
    myElementSet(elementSet),
    myCurrentTag(defaultTag),
    myCurrentAttribute(defaultAttr) {
    // Create MFXIconComboBox for tags
    myMatchTagComboBox = new MFXIconComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create FXComboBox for Attributes
    myMatchAttrComboBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // create button
    myMatchStringButton = new FXButton(getCollapsableFrame(), "Apply selection", nullptr, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignButton);
    // Create help button
    new FXButton(getCollapsableFrame(), "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
    // Set default value for Match string
    myMatchString->setText(defaultValue.c_str());
}


GNEMatchAttribute::~GNEMatchAttribute() {}


void
GNEMatchAttribute::enableMatchAttribute() {
    // enable comboBox, text field and button
    myMatchTagComboBox->enable();
    myMatchAttrComboBox->enable();
    myMatchString->enable();
    myMatchStringButton->enable();
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
GNEMatchAttribute::showMatchAttribute(const GNEElementSet::Type type) {
    // declare flag for proj
    const bool proj = (GeoConvHelper::getFinal().getProjString() != "!");
    // get tags for the given element set
    std::vector<GNETagProperties> tagPropertiesStrings;
    if (type == (GNEElementSet::Type::NETWORK)) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::NETWORKELEMENT);
    } else if (type == GNEElementSet::Type::ADDITIONAL) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::ADDITIONALELEMENT);
    } else if (type == GNEElementSet::Type::SHAPE) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::SHAPE);
    } else if (type == GNEElementSet::Type::TAZ) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::TAZELEMENT);
    } else if (type == GNEElementSet::Type::DEMAND) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::DEMANDELEMENT | GNETagProperties::TagType::STOP);
    } else if (type == GNEElementSet::Type::DATA) {
        tagPropertiesStrings = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
    } else {
        throw ProcessError("Unkown set");
    }
    // now filter to allow only drawables and proj
    myTagPropertiesString.clear();
    for (const auto &tagProperty : tagPropertiesStrings) {
        if (tagProperty.isDrawable() && (!tagProperty.requireProj() || proj)) {
            myTagPropertiesString.push_back(tagProperty);
        }
    }
    // update tag
    updateTag();
    // update attribute
    updateAttribute();
    // show groupbox
    show();
}


void
GNEMatchAttribute::hideMatchAttribute() {
    // hide groupbox
    hide();
}


long
GNEMatchAttribute::onCmdSelMBTag(FXObject*, FXSelector, void*) {
    // reset current tag
    myCurrentTag = SUMO_TAG_NOTHING;
    // set invalid color
    myMatchTagComboBox->setTextColor(FXRGB(255, 0, 0));
    // iterate over tags
    for (const auto& tagString : myTagPropertiesString) {
        if (tagString.getFieldString() == myMatchTagComboBox->getText().text()) {
            // set valid tag
            myCurrentTag = tagString.getTag();
            // set valid color
            myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        }
    }
    // update attribute
    updateAttribute();
    return 1;
}


long
GNEMatchAttribute::onCmdSelMBAttribute(FXObject*, FXSelector, void*) {
    // first obtain a copy of item attributes vinculated with current tag
    auto tagPropertiesCopy = GNEAttributeCarrier::getTagProperty(myCurrentTag);
    // obtain tag property (only for improve code legibility)
    const auto& tagValue = GNEAttributeCarrier::getTagProperty(myCurrentTag);
    // add an extra AttributeValues to allow select ACs using as criterium "parameters"
    GNEAttributeProperties extraAttrProperty;
    extraAttrProperty = GNEAttributeProperties(GNE_ATTR_PARAMETERS,
                        GNEAttributeProperties::AttrProperty::STRING,
                        "Parameters");
    tagPropertiesCopy.addAttribute(extraAttrProperty);
    // add extra attribute if item can close shape
    if (tagValue.canCloseShape()) {
        // add an extra AttributeValues to allow select ACs using as criterium "close shape"
        extraAttrProperty = GNEAttributeProperties(GNE_ATTR_CLOSE_SHAPE,
                            GNEAttributeProperties::AttrProperty::BOOL | GNEAttributeProperties::AttrProperty::DEFAULTVALUE,
                            "Close shape",
                            "true");
        tagPropertiesCopy.addAttribute(extraAttrProperty);
    }
    // add extra attribute if item can have parent
    if (tagValue.isChild()) {
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
    const auto& tagValue = GNEAttributeCarrier::getTagProperty(myCurrentTag);
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr));
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
            myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr));
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
        myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, compOp, 0, expr));
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
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), "Netedit Parameters Help", GUIDesignDialogBox);
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


void
GNEMatchAttribute::updateTag() {
    // declare tag index
    int tagIndex = -1;
    // fill combo box tags
    myMatchTagComboBox->clearItems();
    myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
    // itreate over myTagPropertiesString
    for (int i = 0; i < (int)myTagPropertiesString.size(); i++) {
        // add tag in combo Box
        myMatchTagComboBox->appendIconItem(myTagPropertiesString.at(i).getFieldString().c_str(), GUIIconSubSys::getIcon(myTagPropertiesString.at(i).getGUIIcon()));
        // check tag index
        if (myTagPropertiesString.at(i).getTag() == myCurrentTag) {
            tagIndex = i;
        }
    }
    // set num visible items
    myMatchTagComboBox->setNumVisible(myMatchTagComboBox->getNumItems());
    // check tagIndex
    if (tagIndex == -1) {
        myMatchTagComboBox->setCurrentItem(0);
        myCurrentTag = myTagPropertiesString.front().getTag();
    } else {
        myMatchTagComboBox->setCurrentItem(tagIndex);
    }
}


void
GNEMatchAttribute::updateAttribute() {
    // first check if tag is valid
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // now continue with attributes
        const auto& tagProperty = GNEAttributeCarrier::getTagProperty(myCurrentTag);
        // set color and enable items
        myMatchAttrComboBox->enable();
        myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->clearItems();
        // declare attr index
        int attrIndex = -1;
        // fill attribute combo box
        for (int i = 0; i < (int)tagProperty.getNumberOfAttributes(); i++) {
            myMatchAttrComboBox->appendItem(tagProperty.at(i).getAttrStr().c_str());
            // check attr index
            if (tagProperty.at(i).getAttr() == myCurrentAttribute) {
                attrIndex = i;
            }
        }
        // Add extra attribute "Parameter"
        myMatchAttrComboBox->appendItem(toString(GNE_ATTR_PARAMETERS).c_str());
        if (myCurrentAttribute == GNE_ATTR_PARAMETERS) {
            attrIndex = (myMatchAttrComboBox->getNumItems() - 1);
        }
        // check if item can close shape
        if (tagProperty.canCloseShape()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_CLOSE_SHAPE).c_str());
            if (myCurrentAttribute == GNE_ATTR_CLOSE_SHAPE) {
                attrIndex = (myMatchAttrComboBox->getNumItems() - 1);
            }
        }
        // check if item can have parent
        if (tagProperty.isChild()) {
            myMatchAttrComboBox->appendItem(toString(GNE_ATTR_PARENT).c_str());
            if (myCurrentAttribute == GNE_ATTR_PARENT) {
                attrIndex = (myMatchAttrComboBox->getNumItems() - 1);
            }
        }
        // set num visible items
        myMatchAttrComboBox->setNumVisible(myMatchAttrComboBox->getNumItems());
        // check attrIndex
        if (attrIndex == -1) {
            myMatchAttrComboBox->setCurrentItem(0);
            myCurrentAttribute = tagProperty.begin()->getAttr();
        } else {
            myMatchAttrComboBox->setCurrentItem(attrIndex);
        }
        // enable mach string
        myMatchString->enable();
        myMatchStringButton->enable();
    } else {
        // disable myMatchAttrComboBox
        myMatchAttrComboBox->disable();
        // disable mach string
        myMatchString->disable();
        myMatchStringButton->disable();
    }
}

/****************************************************************************/
