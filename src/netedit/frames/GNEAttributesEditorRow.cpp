/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributesEditorRow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes in GNEAttributesEditor
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/POIIcons.h>
#include <utils/gui/images/VClassIcons.h>

#include "GNEAttributesEditorRow.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAttributesEditorRow) GNEAttributeRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE,           GNEAttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_TOGGLEENABLEATTRIBUTE,  GNEAttributesEditorRow::onCmdToggleEnableAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENCOLORDIALOG,        GNEAttributesEditorRow::onCmdOpenColorDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENALLOWDIALLOG,       GNEAttributesEditorRow::onCmdOpenAllowDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_REPARENT,               GNEAttributesEditorRow::onCmdReparent),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_INSPECTPARENT,          GNEAttributesEditorRow::onCmdInspectParent),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_MOVELANEUP,             GNEAttributesEditorRow::onCmdMoveLaneUp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_MOVELANEDOWN,           GNEAttributesEditorRow::onCmdMoveLaneDown)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditorRow, FXHorizontalFrame, GNEAttributeRowMap, ARRAYNUMBER(GNEAttributeRowMap))

// ===========================================================================
// defines
// ===========================================================================

#define TEXTCOLOR_BLACK FXRGB(0, 0, 0)
#define TEXTCOLOR_BLUE FXRGB(0, 0, 255)
#define TEXTCOLOR_RED FXRGB(255, 0, 0)
#define TEXTCOLOR_BACKGROUND_RED FXRGBA(255, 213, 213, 255)
#define TEXTCOLOR_BACKGROUND_WHITE FXRGB(255, 255, 255)

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditorRow::GNEAttributesEditorRow(GNEAttributesEditor* attributeTable) :
    FXHorizontalFrame(attributeTable->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributeTable(attributeTable) {
    // get static tooltip menu
    const auto tooltipMenu = attributeTable->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create left label
    myAttributeLabel = new MFXLabelTooltip(this, tooltipMenu, "Label", nullptr, GUIDesignLabelThickedFixed(100));
    myAttributeLabel->hide();
    // create lef boolean checkBox for enable/disable attributes
    myAttributeToggleEnableCheckButton = new FXCheckButton(this, "Enable/Disable attribute checkBox", this,
            MID_GNE_ATTRIBUTESEDITORROW_TOGGLEENABLEATTRIBUTE, GUIDesignCheckButtonAttribute);
    myAttributeToggleEnableCheckButton->hide();
    // create left button for reparent
    myAttributeReparentButton = new MFXButtonTooltip(this, tooltipMenu, "Reparent", nullptr, this,
            MID_GNE_ATTRIBUTESEDITORROW_REPARENT, GUIDesignButtonAttribute);
    myAttributeReparentButton->setHelpText(TL("Change parent of this element"));
    myAttributeReparentButton->hide();
    // create left button for inspect parent
    myAttributeInspectParentButton = new MFXButtonTooltip(this, tooltipMenu, "Inspect parent button", nullptr, this,
            MID_GNE_ATTRIBUTESEDITORROW_INSPECTPARENT, GUIDesignButtonAttribute);
    myAttributeInspectParentButton->hide();
    // create lef button for edit allow/disallow vClasses
    myAttributeVClassButton = new MFXButtonTooltip(this, tooltipMenu, "Edit vClass button", nullptr, this,
            MID_GNE_ATTRIBUTESEDITORROW_OPENALLOWDIALLOG, GUIDesignButtonAttribute);
    myAttributeVClassButton->hide();
    // set tip text for edit vClasses button
    myAttributeVClassButton->setTipText(TL("Open dialog for editing vClasses"));
    myAttributeVClassButton->setHelpText(TL("Open dialog for editing vClasses"));
    // create lef attribute for edit color
    myAttributeColorButton = new MFXButtonTooltip(this, tooltipMenu, "color button", GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), this,
            MID_GNE_ATTRIBUTESEDITORROW_OPENCOLORDIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // set tip text for color button
    myAttributeColorButton->setTipText(TL("Open dialog for editing color"));
    myAttributeColorButton->setHelpText(TL("Open dialog for editing color"));
    // create right text field for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, tooltipMenu, GUIDesignTextFieldNCol, this,
            MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    // create right combo box for discrete attributes
    myValueComboBox = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems, this,
                                          MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBox->hide();
    // Create right check button
    myValueCheckButton = new FXCheckButton(this, "check button", this, MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // create right move lane up button
    myValueLaneUpButton = new MFXButtonTooltip(this, tooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this,
            MID_GNE_ATTRIBUTESEDITORROW_MOVELANEUP, GUIDesignButtonIcon);
    myValueLaneUpButton->hide();
    // set tip texts
    myValueLaneUpButton->setTipText(TL("Move element up one lane"));
    myValueLaneUpButton->setHelpText(TL("Move element up one lane"));
    // create right move lane down button
    myValueLaneDownButton = new MFXButtonTooltip(this, tooltipMenu, "", GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this,
            MID_GNE_ATTRIBUTESEDITORROW_MOVELANEDOWN, GUIDesignButtonIcon);
    myValueLaneDownButton->hide();
    // set tip texts
    myValueLaneDownButton->setTipText(TL("Move element down one lane"));
    myValueLaneDownButton->setHelpText(TL("Move element down one lane"));
}


bool
GNEAttributesEditorRow::showAttributeRow(const GNEAttributeProperties& attrProperty, const bool forceDisable) {
    if (myAttributeTable->myEditedACs.empty()) {
        return false;
    }
    myAttribute = attrProperty.getAttr();
    const auto& tagProperty = attrProperty.getTagPropertyParent();
    const auto firstEditedAC = myAttributeTable->myEditedACs.front();
    // check if we're editing multiple ACs
    const auto multipleEditedACs = (myAttributeTable->myEditedACs.size() > 1);
    // declare flag for show attribute enabled
    const bool attributeEnabled = attrProperty.isAlwaysEnabled() ? true : firstEditedAC->isAttributeEnabled(myAttribute);
    // check if this attribute is computed
    const bool computedAttribute = multipleEditedACs ? false : firstEditedAC->isAttributeComputed(myAttribute);
    // get string value depending if attribute is enabled
    const std::string value = getAttributeValue(attributeEnabled);
    // get parent if we're editing single vTypes
    GNEAttributeCarrier* ACParent = nullptr;
    if (!multipleEditedACs && attrProperty.isVType()) {
        const auto& ACs = myAttributeTable->myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
        // parent can be either type or distribution
        if (myAttribute == SUMO_ATTR_TYPE) {
            ACParent = ACs->retrieveDemandElement(SUMO_TAG_VTYPE, firstEditedAC->getAttribute(SUMO_ATTR_TYPE), false);
        }
        if (ACParent == nullptr) {
            ACParent = ACs->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, firstEditedAC->getAttribute(SUMO_ATTR_TYPE), false);
        }
    }
    // hide editing for unique attributes in case of multi-selection
    if (multipleEditedACs && attrProperty.isUnique()) {
        return hideAttributeRow();
    }
    // front element has their own button, and doesn't use the UndoList
    if ((myAttribute == GNE_ATTR_FRONTELEMENT) || (myAttribute == GNE_ATTR_SELECTED)) {
        return hideAttributeRow();
    }
    // if we have a disabled flow attribute, don't show row
    if (attrProperty.isFlow() && !attributeEnabled) {
        return hideAttributeRow();
    }
    // expected and joins depend of triggered
    if (tagProperty.isVehicleStop() && !attributeEnabled) {
        if (myAttribute == SUMO_ATTR_EXPECTED) {
            return hideAttributeRow();
        } else if (myAttribute == SUMO_ATTR_EXPECTED_CONTAINERS) {
            return hideAttributeRow();
        } else if (myAttribute == SUMO_ATTR_JOIN) {
            return hideAttributeRow();
        }
    }
    // don't show stop offset exception if stopOffset is zero
    if ((myAttribute == GNE_ATTR_STOPOEXCEPTION) &&
            (GNEAttributeCarrier::parse<double>(firstEditedAC->getAttribute(GNE_ATTR_STOPOFFSET)) == 0)) {
        return hideAttributeRow();
    }
    // show elements depending of attribute properties
    if (attrProperty.isActivatable()) {
        showAttributeToggleEnable(attrProperty, firstEditedAC->isAttributeEnabled(myAttribute));
    } else if (myAttribute == GNE_ATTR_PARENT) {
        showAttributeReparent(attributeEnabled);
    } else if ((myAttribute == SUMO_ATTR_TYPE) && tagProperty.hasTypeParent()) {
        showAttributeInspectParent(attrProperty, attributeEnabled);
    } else if (attrProperty.isVClass() && (myAttribute != SUMO_ATTR_DISALLOW)) {
        showAttributeVClass(attrProperty, attributeEnabled);
    } else if (myAttribute == SUMO_ATTR_COLOR) {
        showAttributeColor(attrProperty, attributeEnabled);
    } else {
        showAttributeLabel(attrProperty);
    }
    // continue depending of type of attribute
    if (attrProperty.isBool()) {
        showValueCheckButton(value, attributeEnabled, computedAttribute);
    } else if (attrProperty.isDiscrete() || attrProperty.isVType()) {
        showValueComboBox(attrProperty, value, attributeEnabled, computedAttribute);
    } else {
        showValueString(value, attributeEnabled, computedAttribute);
    }
    // check if show move lane buttons
    if (!multipleEditedACs && !tagProperty.isNetworkElement() && (myAttribute == SUMO_ATTR_LANE)) {
        showMoveLaneButtons(value);
        myValueLaneUpButton->show();
        myValueLaneDownButton->show();
    } else {
        myValueLaneUpButton->hide();
        myValueLaneDownButton->hide();
    }
    // enable depending of supermode
    enableElements(attrProperty, forceDisable);
    // Show row
    show();
    return true;
}


bool
GNEAttributesEditorRow::hideAttributeRow() {
    hide();
    return false;
}


bool
GNEAttributesEditorRow::isAttributeRowShown() const {
    return shown();
}


long
GNEAttributesEditorRow::onCmdOpenColorDialog(FXObject*, FXSelector, void*) {
    const auto& attrProperty = myAttributeTable->myEditedACs.front()->getTagProperty().getAttributeProperties(myAttribute);
    // create FXColorDialog
    FXColorDialog colordialog(this, TL("Color Dialog"));
    colordialog.setTarget(this);
    colordialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL));
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text())));
    } else if (!attrProperty.getDefaultValue().empty()) {
        colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(attrProperty.getDefaultValue())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
    }
    // execute dialog to get a new color in the text field
    if (colordialog.execute()) {
        myValueTextField->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str(), TRUE);
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdOpenAllowDialog(FXObject*, FXSelector, void*) {
    // declare values to be modified
    std::string allowedVehicles = myValueTextField->getText().text();
    // declare accept changes
    bool acceptChanges = false;
    // open GNEAllowVClassesDialog (also used to modify SUMO_ATTR_CHANGE_LEFT etc)
    GNEAllowVClassesDialog(myAttributeTable->getFrameParent()->getViewNet(), myAttribute, &allowedVehicles, &acceptChanges).execute();
    // continue depending of acceptChanges
    if (acceptChanges) {
        myValueTextField->setText(allowedVehicles.c_str(), TRUE);
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdReparent(FXObject*, FXSelector, void*) {
    myAttributeTable->enableReparent();
    return 1;
}


long
GNEAttributesEditorRow::onCmdInspectParent(FXObject*, FXSelector, void*) {
    myAttributeTable->inspectParent();
    return 1;
}


long
GNEAttributesEditorRow::onCmdMoveLaneUp(FXObject*, FXSelector, void*) {
    myAttributeTable->moveLaneUp();
    return 1;
}


long
GNEAttributesEditorRow::onCmdMoveLaneDown(FXObject*, FXSelector, void*) {
    myAttributeTable->moveLaneDown();
    return 1;
}


long
GNEAttributesEditorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    if (myAttributeTable->myEditedACs.empty()) {
        return 0;
    }
    const auto& editedAC = myAttributeTable->myEditedACs.front();
    const auto& attrProperties = editedAC->getTagProperty().getAttributeProperties(myAttribute);
    // continue depending of clicked object
    if (obj == myValueCheckButton) {
        // Set true o false depending of the checkBox
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
        } else {
            myValueCheckButton->setText("false");
        }
        myAttributeTable->setAttribute(myAttribute, myValueCheckButton->getText().text());
    } else if (obj == myValueComboBox) {
        const std::string newValue = myValueComboBox->getText().text();
        // check if the new comboBox value is valid
        if (editedAC->isValid(myAttribute, newValue)) {
            myValueComboBox->setTextColor(TEXTCOLOR_BLACK);
            myValueComboBox->setBackColor(TEXTCOLOR_BACKGROUND_WHITE);
            myValueTextField->killFocus();
            myAttributeTable->setAttribute(myAttribute, newValue);
        } else {
            // edit colors
            myValueComboBox->setTextColor(TEXTCOLOR_RED);
            if (newValue.empty()) {
                myValueComboBox->setBackColor(TEXTCOLOR_BACKGROUND_RED);
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TLF("ComboBox value '%' for attribute % of % isn't valid", newValue, attrProperties.getAttrStr(), attrProperties.getTagPropertyParent().getTagStr()));
        }
    } else if (obj == myValueTextField) {
        // check if we're merging junction
        //if (!mergeJunction(myACAttr.getAttr(), newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)


        // first check if set default value
        if (myValueTextField->getText().empty() && attrProperties.hasDefaultValue() && !attrProperties.isVClass()) {
            // update text field without notify
            myValueTextField->setText(attrProperties.getDefaultValue().c_str(), FALSE);
        }
        // if we're editing an angle, check if filter between [0,360]
        if ((myAttribute == SUMO_ATTR_ANGLE) && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter anglea and update text field without notify
            const double angle = fmod(GNEAttributeCarrier::parse<double>(myValueTextField->getText().text()), 360);
            myValueTextField->setText(toString(angle).c_str(), FALSE);
        }
        // if we're editing a position or a shape, strip whitespace after comma
        if ((myAttribute == SUMO_ATTR_POSITION) || (myAttribute == SUMO_ATTR_SHAPE)) {
            std::string shape(myValueTextField->getText().text());
            while (shape.find(", ") != std::string::npos) {
                shape = StringUtils::replace(shape, ", ", ",");
            }
            myValueTextField->setText(toString(shape).c_str(), FALSE);
        }
        // if we're editing a int, strip decimal value
        if (attrProperties.isInt() && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            if ((doubleValue - (int)doubleValue) == 0) {
                myValueTextField->setText(toString((int)doubleValue).c_str(), FALSE);
            }

        }
        // after apply all filters, obtain value
        const std::string newValue = myValueTextField->getText().text();
        // check if the new textField value is valid
        if (editedAC->isValid(myAttribute, newValue)) {
            myValueTextField->setTextColor(TEXTCOLOR_BLACK);
            myValueComboBox->setBackColor(TEXTCOLOR_BACKGROUND_WHITE);
            myValueTextField->killFocus();
            myAttributeTable->setAttribute(myAttribute, newValue);
        } else {
            // edit colors
            myValueTextField->setTextColor(TEXTCOLOR_RED);
            if (newValue.empty()) {
                myValueTextField->setBackColor(TEXTCOLOR_BACKGROUND_RED);
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TLF("TextField value '%' for attribute % of % isn't valid", newValue, attrProperties.getAttrStr(), attrProperties.getTagPropertyParent().getTagStr()));
        }
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdToggleEnableAttribute(FXObject*, FXSelector, void*) {
    myAttributeTable->toggleEnableAttribute(myAttribute, myAttributeToggleEnableCheckButton->getCheck() == TRUE);
    return 0;
}


GNEAttributesEditorRow::GNEAttributesEditorRow() :
    myAttributeTable(nullptr) {
}


const std::string
GNEAttributesEditorRow::getAttributeValue(const bool enabled) const {
    if (enabled) {
        // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
        std::set<std::string> values;
        // iterate over edited attributes and insert every value in set
        for (const auto& editedAC : myAttributeTable->myEditedACs) {
            if (editedAC->hasAttribute(myAttribute)) {
                values.insert(editedAC->getAttribute(myAttribute));
            }
        }
        // merge all values in a single string
        std::ostringstream oss;
        for (auto it = values.begin(); it != values.end(); it++) {
            if (it != values.begin()) {
                oss << " ";
            }
            oss << *it;
        }
        // obtain value to be shown in row
        return oss.str();
    } else {
        return myAttributeTable->myEditedACs.front()->getAlternativeValueForDisabledAttributes(myAttribute);
    }
}


void
GNEAttributesEditorRow::showAttributeToggleEnable(const GNEAttributeProperties& attrProperty, const bool value) {
    myAttributeToggleEnableCheckButton->setText(attrProperty.getAttrStr().c_str());
    myAttributeToggleEnableCheckButton->setCheck(value);
    myAttributeToggleEnableCheckButton->enable();
    myAttributeToggleEnableCheckButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeReparentButton->hide();
    myAttributeInspectParentButton->hide();
    myAttributeVClassButton->hide();
    myAttributeColorButton->hide();
}


void
GNEAttributesEditorRow::showAttributeReparent(const bool enabled) {
    if (enabled) {
        myAttributeReparentButton->enable();
    } else {
        myAttributeReparentButton->disable();
    }
    myAttributeReparentButton->show();
    // hide other elements
    myAttributeInspectParentButton->hide();
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
    myAttributeVClassButton->hide();
    myAttributeColorButton->hide();
}


void
GNEAttributesEditorRow::showAttributeInspectParent(const GNEAttributeProperties& attrProperty, const bool enabled) {
    // set icon and text
    myAttributeInspectParentButton->setIcon(GUIIconSubSys::getIcon(attrProperty.getTagPropertyParent().getGUIIcon()));
    myAttributeInspectParentButton->setText(attrProperty.getAttrStr().c_str());
    if (enabled) {
        myAttributeInspectParentButton->enable();
    } else {
        myAttributeInspectParentButton->disable();
    }
    myAttributeInspectParentButton->show();
    // hide other elements
    myAttributeReparentButton->hide();
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
    myAttributeVClassButton->hide();
    myAttributeColorButton->hide();
}


void
GNEAttributesEditorRow::showAttributeVClass(const GNEAttributeProperties& attrProperty,
        const bool enabled) {
    // set icon and text
    myAttributeVClassButton->setText(attrProperty.getAttrStr().c_str());
    if (enabled) {
        myAttributeVClassButton->enable();
    } else {
        myAttributeVClassButton->disable();
    }
    myAttributeVClassButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
    myAttributeReparentButton->hide();
    myAttributeInspectParentButton->hide();
    myAttributeColorButton->hide();
}


void
GNEAttributesEditorRow::showAttributeColor(const GNEAttributeProperties& attrProperty,
        const bool enabled) {
    myAttributeColorButton->setText(attrProperty.getAttrStr().c_str());
    myAttributeColorButton->show();
    if (enabled) {
        myAttributeColorButton->enable();
    } else {
        myAttributeColorButton->disable();
    }
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
    myAttributeReparentButton->hide();
    myAttributeInspectParentButton->hide();
    myAttributeVClassButton->hide();
}


void
GNEAttributesEditorRow::showAttributeLabel(const GNEAttributeProperties& attrProperty) {
    myAttributeLabel->setText(attrProperty.getAttrStr().c_str());
    myAttributeLabel->show();
    // hide other elements
    myAttributeToggleEnableCheckButton->hide();
    myAttributeReparentButton->hide();
    myAttributeInspectParentButton->hide();
    myAttributeVClassButton->hide();
    myAttributeColorButton->hide();
}


void
GNEAttributesEditorRow::showValueCheckButton(const std::string& value,
        const bool enabled, const bool computed) {
    // first we need to check if all boolean values are equal
    bool allValuesEqual = true;
    // declare  boolean vector
    std::vector<bool> booleanVector;
    // check if value can be parsed to a boolean vector
    if (GNEAttributeCarrier::canParse<std::vector<bool> >(value)) {
        booleanVector = GNEAttributeCarrier::parse<std::vector<bool> >(value);
    }
    // iterate over booleans comparing all element with the first
    for (const auto& booleanValue : booleanVector) {
        if (booleanValue != booleanVector.front()) {
            allValuesEqual = false;
        }
    }
    // use checkbox or textfield depending if all booleans are equal
    if (allValuesEqual) {
        if (enabled) {
            myValueCheckButton->enable();
        } else {
            myValueCheckButton->disable();
        }
        // set check button
        if ((booleanVector.size() > 0) && booleanVector.front()) {
            myValueCheckButton->setCheck(true);
            myValueCheckButton->setText("true");
        } else {
            myValueCheckButton->setCheck(false);
            myValueCheckButton->setText("false");
        }
        // show check button
        myValueCheckButton->show();
        // hide other value elements
        myValueTextField->hide();
        myValueComboBox->hide();
        myValueLaneUpButton->hide();
        myValueLaneDownButton->hide();
    } else {
        // show value as string
        showValueString(value, enabled, computed);
    }
}


void
GNEAttributesEditorRow::showValueComboBox(const GNEAttributeProperties& attrProperty, const std::string& value,
        const bool enabled, const bool computed) {
    // first we need to check if all boolean values are equal
    bool allValuesEqual = true;
    // declare  boolean vector
    std::vector<std::string> stringVector;
    // check if value can be parsed to a boolean vector
    if (GNEAttributeCarrier::canParse<std::vector<std::string> >(value)) {
        stringVector = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
    }
    // iterate over string comparing all element with the first
    for (const auto& stringValue : stringVector) {
        if (stringValue != stringVector.front()) {
            allValuesEqual = false;
        }
    }
    // use checkbox or textfield depending if all booleans are equal
    if (allValuesEqual) {
        // clear and enable comboBox
        myValueComboBox->clearItems();
        myValueComboBox->setTextColor(TEXTCOLOR_BLACK);
        myValueComboBox->setBackColor(TEXTCOLOR_BACKGROUND_WHITE);
        if (enabled) {
            myValueComboBox->enable();
        } else {
            myValueComboBox->disable();
        }
        // fill depeding of ACAttr
        if (attrProperty.getAttr() == SUMO_ATTR_VCLASS) {
            // add all vClasses with their icons
            for (const auto& vClassStr : SumoVehicleClassStrings.getStrings()) {
                myValueComboBox->appendIconItem(vClassStr.c_str(), VClassIcons::getVClassIcon(getVehicleClassID(vClassStr)));
            }
        } else if (attrProperty.isVType()) {
            // get ACs
            const auto& ACs = myAttributeTable->myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
            // fill comboBox with all vTypes and vType distributions sorted by ID
            std::map<std::string, GNEDemandElement*> sortedTypes;
            for (const auto& type : ACs->getDemandElements().at(SUMO_TAG_VTYPE)) {
                sortedTypes[type.second->getID()] = type.second;
            }
            for (const auto& sortedType : sortedTypes) {
                myValueComboBox->appendIconItem(sortedType.first.c_str(), sortedType.second->getACIcon());
            }
            sortedTypes.clear();
            for (const auto& typeDistribution : ACs->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION)) {
                sortedTypes[typeDistribution.second->getID()] = typeDistribution.second;
            }
            for (const auto& sortedType : sortedTypes) {
                myValueComboBox->appendIconItem(sortedType.first.c_str(), sortedType.second->getACIcon());
            }
        } else if (attrProperty.getAttr() == SUMO_ATTR_ICON) {
            // add all POIIcons with their icons
            for (const auto& POIIcon : SUMOXMLDefinitions::POIIcons.getValues()) {
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::POIIcons.getString(POIIcon).c_str(), POIIcons::getPOIIcon(POIIcon));
            }
        } else if ((attrProperty.getAttr() == SUMO_ATTR_RIGHT_OF_WAY) && (myAttributeTable->myEditedACs.size() == 1) &&
                   (attrProperty.getTagPropertyParent().getTag() == SUMO_TAG_JUNCTION)) {
            // special case for junction types
            if (myAttributeTable->myEditedACs.front()->getAttribute(SUMO_ATTR_TYPE) == "priority") {
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::DEFAULT).c_str(), nullptr);
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::EDGEPRIORITY).c_str(), nullptr);
            } else if (myAttributeTable->myEditedACs.front()->getAttribute(SUMO_ATTR_TYPE) == "traffic_light") {
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::DEFAULT).c_str(), nullptr);
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::MIXEDPRIORITY).c_str(), nullptr);
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::ALLWAYSTOP).c_str(), nullptr);
            } else {
                myValueComboBox->disable();
            }
        } else {
            // fill comboBox with discrete values
            for (const auto& discreteValue : attrProperty.getDiscreteValues()) {
                myValueComboBox->appendIconItem(discreteValue.c_str(), nullptr);
            }
        }
        // set current value (or disable)
        const auto index = myValueComboBox->findItem(value.c_str());
        if (index < 0) {
            if (myValueComboBox->getNumItems() > 0) {
                myValueComboBox->setCurrentItem(0);
            } else {
                myValueComboBox->disable();
            }
        } else {
            myValueComboBox->setCurrentItem(index);
        }
        // show comboBox button
        myValueComboBox->show();
        // hide other value elements
        myValueTextField->hide();
        myValueCheckButton->hide();
        myValueLaneUpButton->hide();
        myValueLaneDownButton->hide();
    } else {
        // show value as string
        showValueString(value, enabled, computed);
    }
}


void
GNEAttributesEditorRow::showValueString(const std::string& value, const bool enabled, const bool computed) {
    // clear and enable comboBox
    myValueTextField->setText(value.c_str());
    if (computed) {
        myValueTextField->setTextColor(TEXTCOLOR_BLUE);
    } else {
        myValueTextField->setTextColor(TEXTCOLOR_BLACK);
    }
    if (enabled) {
        myValueTextField->enable();
    } else {
        myValueTextField->disable();
    }
    // show list of values
    myValueTextField->show();
    // hide other value elements
    myValueCheckButton->hide();
    myValueComboBox->hide();
}


void
GNEAttributesEditorRow::showMoveLaneButtons(const std::string& laneID) {
    // retrieve lane
    const auto lane = myAttributeTable->myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(laneID, false);
    // check lane
    if (lane) {
        // check if disable move up
        if ((lane->getIndex() + 1) >= (int)lane->getParentEdge()->getLanes().size()) {
            myValueLaneUpButton->disable();
        } else {
            myValueLaneUpButton->enable();
        }
        // check if disable move down
        if ((lane->getIndex() - 1) < 0) {
            myValueLaneDownButton->disable();
        } else {
            myValueLaneDownButton->enable();
        }
    } else {
        // if lane doesn't exist, disable both
        myValueLaneUpButton->disable();
        myValueLaneDownButton->disable();
    }
}


void
GNEAttributesEditorRow::enableElements(const GNEAttributeProperties& attrProperty, const bool forceDisable) {
    const auto& editModes = myAttributeTable->myFrameParent->getViewNet()->getEditModes();
    const auto& tagProperty = attrProperty.getTagPropertyParent();
    // by default we assume that elements are disabled
    bool enableElements = false;
    if (forceDisable) {
        enableElements = false;
    } else if (editModes.isCurrentSupermodeNetwork() && (tagProperty.isNetworkElement() || tagProperty.isAdditionalElement())) {
        enableElements = true;
    } else if (editModes.isCurrentSupermodeDemand() && tagProperty.isDemandElement()) {
        enableElements = true;
    } else if (editModes.isCurrentSupermodeData() && (tagProperty.isDataElement() || tagProperty.isMeanData())) {
        enableElements = true;
    }
    if (!enableElements) {
        myAttributeToggleEnableCheckButton->disable();
        myAttributeReparentButton->disable();
        myAttributeInspectParentButton->disable();
        myAttributeVClassButton->disable();
        myAttributeColorButton->disable();
        myValueTextField->disable();
        myValueComboBox->disable();
        myValueCheckButton->disable();
        myValueLaneUpButton->disable();
        myValueLaneDownButton->disable();
    }
}

/*
bool
GNEAttributesEditorRow::mergeJunction(SumoXMLAttr attr, const std::string& newVal) const {
    const auto viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    const auto& inspectedElements = viewNet->getInspectedElements();
    // check if we're editing junction position
    if (inspectedElements.isInspectingSingleElement() && (inspectedElements.getFirstAC()->getTagProperty().getTag() == SUMO_TAG_JUNCTION) && (attr == SUMO_ATTR_POSITION)) {
        // retrieve original junction
        GNEJunction* movedJunction = viewNet->getNet()->getAttributeCarriers()->retrieveJunction(inspectedElements.getFirstAC()->getID());
        // parse position
        const Position newPosition = GNEAttributeCarrier::parse<Position>(newVal);
        // iterate over network junction
        for (const auto& targetjunction : viewNet->getNet()->getAttributeCarriers()->getJunctions()) {
            // check distance position
            if ((targetjunction.second->getPositionInView().distanceTo2D(newPosition) < POSITION_EPS) &&
                    viewNet->askMergeJunctions(movedJunction, targetjunction.second)) {
                viewNet->getNet()->mergeJunctions(movedJunction, targetjunction.second, viewNet->getUndoList());
                return true;
            }
        }
    }
    // nothing to merge
    return false;
}
*/


/****************************************************************************/
