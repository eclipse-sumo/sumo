/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNETagPropertiesDatabase.h>
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
FXIMPLEMENT(GNEMatchAttribute, MFXGroupBoxModule, GNEMatchAttributeMap, ARRAYNUMBER(GNEMatchAttributeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEMatchAttribute::GNEMatchAttribute(GNEElementSet* elementSet, SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue) :
    MFXGroupBoxModule(elementSet->getSelectorFrameParent(), TL("Match Attribute")),
    myElementSet(elementSet),
    myCurrentTag(defaultTag),
    myCurrentAttribute(defaultAttr) {
    // Create MFXComboBoxIcon for tags
    myMatchTagComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create MFXComboBoxIcon for Attributes
    myMatchAttrComboBox = new MFXComboBoxIcon(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // create button
    myMatchStringButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Apply selection"), "", "", nullptr, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignButton);
    // Create help button
    GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
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
    const auto tagPropertiesDatabase = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase();
    // declare flag for proj
    const bool proj = (GeoConvHelper::getFinal().getProjString() != "!");
    // get tags for the given element set
    std::vector<const GNETagProperties*> tagPropertiesByType;
    if (type == (GNEElementSet::Type::NETWORK)) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::NETWORKELEMENT);
    } else if (type == GNEElementSet::Type::ADDITIONAL) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::ADDITIONALELEMENT);
    } else if (type == GNEElementSet::Type::SHAPE) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::SHAPE);
    } else if (type == GNEElementSet::Type::TAZ) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::TAZELEMENT);
    } else if (type == GNEElementSet::Type::DEMAND) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::DEMANDELEMENT);
    } else if (type == GNEElementSet::Type::GENERICDATA) {
        tagPropertiesByType = tagPropertiesDatabase->getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
    } else {
        throw ProcessError(TL("Unknown set"));
    }
    // now filter to allow only drawables and proj
    myTagProperties.clear();
    for (const auto tagProperty : tagPropertiesByType) {
        if (tagProperty->isDrawable() && (!tagProperty->requireProj() || proj)) {
            myTagProperties.push_back(tagProperty);
        }
    }
    // update tag
    updateTag();
    // update attributeProperty
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
    for (const auto& tagProperty : myTagProperties) {
        if (tagProperty->getSelectorText() == myMatchTagComboBox->getText().text()) {
            // set valid tag
            myCurrentTag = tagProperty->getTag();
            // set valid color
            myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
        }
    }
    // update attributeProperty
    updateAttribute();
    return 1;
}


long
GNEMatchAttribute::onCmdSelMBAttribute(FXObject*, FXSelector, void*) {
    // set current selected attributeProperty
    myCurrentAttribute = SUMO_ATTR_NOTHING;
    for (const auto attributeProperty : myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagProperty(myCurrentTag)->getAttributeProperties()) {
        if (attributeProperty->getAttrStr() == myMatchAttrComboBox->getText().text()) {
            myCurrentAttribute = attributeProperty->getAttr();
        }
    }
    // check if selected attributeProperty is valid
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
    const auto tagProperty = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagProperty(myCurrentTag);
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr));
    } else if (tagProperty->hasAttribute(myCurrentAttribute) && tagProperty->getAttributeProperties(myCurrentAttribute)->isNumerical()) {
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
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), TL("Netedit Parameters Help"), GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << TL("- The 'Match Attribute' controls allow to specify a set of objects which are then applied to the current selection\n")
            << TL("  according to the current 'Modification Mode'.\n")
            << TL("     1. Select an object type from the first input box\n")
            << TL("     2. Select an attribute from the second input box\n")
            << TL("     3. Enter a 'match expression' in the third input box and press <return>\n")
            << "\n"
            << TL("- The empty expression matches all objects\n")
            << TL("- For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n")
            << TL("- An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n")
            << "\n"
            << TL("- For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n")
            << TL("     '' (no operator) matches if string is a substring of that object's attribute.\n")
            << TL("     '=' matches if string is an exact match.\n")
            << TL("     '!' matches if string is not a substring.\n")
            << TL("     '^' matches if string is not an exact match.\n")
            << "\n"
            << TL("- Examples:\n")
            << TL("     junction; id; 'foo' -> match all junctions that have 'foo' in their id\n")
            << TL("     junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n")
            << TL("     edge; speed; '>10' -> match all edges with a speed above 10\n");
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(myHorizontalFrameOKButton, TL("OK"), "", TL("close"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    return 1;
}


void
GNEMatchAttribute::updateTag() {
    // declare tag index
    int tagIndex = -1;
    // fill combo box tags
    myMatchTagComboBox->clearItems();
    myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
    // itreate over myTagProperties
    for (int i = 0; i < (int)myTagProperties.size(); i++) {
        // add tag in combo Box
        myMatchTagComboBox->appendIconItem(myTagProperties.at(i)->getSelectorText().c_str(), GUIIconSubSys::getIcon(myTagProperties.at(i)->getGUIIcon()));
        // check tag index
        if (myTagProperties.at(i)->getTag() == myCurrentTag) {
            tagIndex = i;
        }
    }
    // check tagIndex
    if (tagIndex == -1) {
        myMatchTagComboBox->setCurrentItem(0);
        myCurrentTag = myTagProperties.front()->getTag();
    } else {
        myMatchTagComboBox->setCurrentItem(tagIndex);
    }
}


void
GNEMatchAttribute::updateAttribute() {
    // first check if tag is valid
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // now continue with attributes
        const auto tagProperty = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagProperty(myCurrentTag);
        // set color and enable items
        myMatchAttrComboBox->enable();
        myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchAttrComboBox->clearItems();
        // declare attr index
        int attrIndex = -1;
        // fill attributeProperty combo box
        for (int i = 0; i < (int)tagProperty->getNumberOfAttributes(); i++) {
            myMatchAttrComboBox->appendIconItem(tagProperty->at(i)->getAttrStr().c_str());
            // check attr index
            if (tagProperty->at(i)->getAttr() == myCurrentAttribute) {
                attrIndex = i;
            }
        }
        // check attrIndex
        if (attrIndex == -1) {
            myMatchAttrComboBox->setCurrentItem(0);
            if (tagProperty->getNumberOfAttributes() > 0) {
                myCurrentAttribute = tagProperty->getAttributeProperties().front()->getAttr();
            }
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
