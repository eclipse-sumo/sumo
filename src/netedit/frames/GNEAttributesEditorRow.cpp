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
/// @file    GNEAttributesEditorRow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes in GNEAttributesEditorType
/****************************************************************************/

#include <netedit/dialogs/GNEVClassesDialog.h>
#include <netedit/dialogs/GNEColorDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNEInternalTest.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/common/Translation.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_COLOR,       GNEAttributesEditorRow::onCmdOpenColorDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_ALLOW,       GNEAttributesEditorRow::onCmdOpenVClassDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_FILE,        GNEAttributesEditorRow::onCmdOpenFileDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_REPARENT,               GNEAttributesEditorRow::onCmdReparent),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_INSPECTPARENT,          GNEAttributesEditorRow::onCmdInspectParent),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_MOVELANEUP,             GNEAttributesEditorRow::onCmdMoveLaneUp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_MOVELANEDOWN,           GNEAttributesEditorRow::onCmdMoveLaneDown)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditorRow, FXHorizontalFrame, GNEAttributeRowMap, ARRAYNUMBER(GNEAttributeRowMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditorRow::GNEAttributesEditorRow(GNEAttributesEditorType* attributeTable) :
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
    // create left button parent
    myAttributeButton = new MFXButtonTooltip(this, tooltipMenu, "button", nullptr, this,
            MID_GNE_ATTRIBUTESEDITORROW_REPARENT, GUIDesignButtonAttribute);
    // create right text field for string attributes
    myValueTextField = new MFXTextFieldIcon(this, tooltipMenu, GUIIcon::EMPTY, this, MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    // create right combo box for discrete attributes
    myValueComboBox = new MFXComboBoxIcon(this, tooltipMenu, true, GUIDesignComboBoxVisibleItems, this,
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
    // start hidden
    hide();
}


bool
GNEAttributesEditorRow::showAttributeRow(GNEAttributesEditorType* attributeTable, const GNEAttributeProperties* attrProperty, const bool forceDisable) {
    // update parent table
    myAttributeTable = attributeTable;
    if (myAttributeTable->myEditedACs.empty()) {
        return false;
    } else {
        reparent(myAttributeTable->getCollapsableFrame());
    }
    myAttrProperty = attrProperty;
    const auto attribute = myAttrProperty->getAttr();
    const auto tagPropertyParent = attrProperty->getTagPropertyParent();
    const auto firstEditedAC = myAttributeTable->myEditedACs.front();
    // check if we're editing multiple ACs
    const auto multipleEditedACs = (myAttributeTable->myEditedACs.size() > 1);
    // declare flag for show attribute enabled
    const bool attributeEnabled = isAttributeEnabled(attrProperty);
    // check if this attribute is computed
    const bool computedAttribute = multipleEditedACs ? false : firstEditedAC->isAttributeComputed(attribute);
    // get string value depending if attribute is enabled
    const std::string value = getAttributeValue(attributeEnabled);
    // get parent if we're editing single vTypes
    GNEAttributeCarrier* ACParent = nullptr;
    if (!multipleEditedACs && attrProperty->isVType()) {
        const auto& ACs = myAttributeTable->myFrameParent->getViewNet()->getNet()->getAttributeCarriers();
        // parent can be either type or distribution
        if (attribute == SUMO_ATTR_TYPE) {
            ACParent = ACs->retrieveDemandElement(SUMO_TAG_VTYPE, firstEditedAC->getAttribute(SUMO_ATTR_TYPE), false);
        }
        if (ACParent == nullptr) {
            ACParent = ACs->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, firstEditedAC->getAttribute(SUMO_ATTR_TYPE), false);
        }
    }
    // hide editing for unique attributes in case of multi-selection
    if (multipleEditedACs && attrProperty->isUnique()) {
        return hideAttributeRow();
    }
    // front element has their own button, and doesn't use the UndoList
    if (attribute == GNE_ATTR_FRONTELEMENT) {
        return hideAttributeRow();
    }
    // if we have a disabled flow attribute, don't show row
    if (attrProperty->isFlow() && !attributeEnabled) {
        return hideAttributeRow();
    }
    // expected and joins depend of triggered
    if (tagPropertyParent->isVehicleStop() && !attributeEnabled) {
        if (attribute == SUMO_ATTR_EXPECTED) {
            return hideAttributeRow();
        } else if (attribute == SUMO_ATTR_EXPECTED_CONTAINERS) {
            return hideAttributeRow();
        } else if (attribute == SUMO_ATTR_JOIN) {
            return hideAttributeRow();
        }
    }
    // don't show stop offset exception if stopOffset is zero
    if ((attribute == GNE_ATTR_STOPOEXCEPTION) && !attributeEnabled) {
        return hideAttributeRow();
    }
    // show elements depending of attribute properties
    if (attrProperty->isActivatable()) {
        showAttributeToggleEnable(attrProperty, attributeEnabled);
    } else if (attrProperty->isFileOpen() || attrProperty->isFileSave()) {
        showAttributeFile(attrProperty, attributeEnabled);
    } else if (attribute == GNE_ATTR_PARENT) {
        showAttributeReparent(attributeEnabled);
    } else if ((attribute == SUMO_ATTR_TYPE) && tagPropertyParent->hasTypeParent()) {
        showAttributeInspectParent(attrProperty, attributeEnabled);
    } else if (attrProperty->isVClass() && (attribute != SUMO_ATTR_DISALLOW)) {
        showAttributeVClass(attrProperty, attributeEnabled);
    } else if (attribute == SUMO_ATTR_COLOR) {
        showAttributeColor(attrProperty, attributeEnabled);
    } else if (attribute == GNE_ATTR_PARAMETERS) {
        hideAllAttributeElements();
    } else {
        showAttributeLabel(attrProperty);
    }
    // continue depending of type of attribute
    if (attrProperty->isBool()) {
        showValueCheckButton(value, attributeEnabled, computedAttribute);
    } else if (attrProperty->isDiscrete() || attrProperty->isVType()) {
        showValueComboBox(attrProperty, value, attributeEnabled, computedAttribute);
    } else {
        showValueString(value, attributeEnabled, computedAttribute);
    }
    // check if show move lane buttons
    if (!multipleEditedACs && !tagPropertyParent->isNetworkElement() && (attribute == SUMO_ATTR_LANE)) {
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


void
GNEAttributesEditorRow::disable() {
    // disable all elements
    myAttributeToggleEnableCheckButton->disable();
    myAttributeButton->disable();
    myValueTextField->disable();
    myValueComboBox->disable();
    myValueCheckButton->disable();
    myValueLaneUpButton->disable();
    myValueLaneDownButton->disable();
}


const GNEAttributeProperties*
GNEAttributesEditorRow::getAttrProperty() const {
    return myAttrProperty;
}


std::string
GNEAttributesEditorRow::getCurrentValue() const {
    if (myValueCheckButton->shown()) {
        return (myValueCheckButton->getCheck() == TRUE) ? "true" : "false";
    } else if (myValueComboBox->shown()) {
        return myValueComboBox->getText().text();
    } else if (myValueTextField->shown()) {
        return myValueTextField->getText().text();
    } else {
        return "";
    }
}


bool
GNEAttributesEditorRow::isValueValid() const {
    if (myValueCheckButton->shown()) {
        return true;
    } else if (myValueComboBox->shown()) {
        return (myValueComboBox->getTextColor() != GUIDesignTextColorRed) &&
               (myValueComboBox->getBackColor() != GUIDesignBackgroundColorRed);
    } else if (myValueTextField->shown()) {
        return (myValueTextField->getTextColor() != GUIDesignTextColorRed) &&
               (myValueTextField->getBackColor() != GUIDesignBackgroundColorRed);
    } else {
        return true;
    }
}


bool
GNEAttributesEditorRow::isAttributeRowShown() const {
    return shown();
}


SumoXMLAttr
GNEAttributesEditorRow::fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject, const bool insertDefaultValues) const {
    const auto attribute = myAttrProperty->getAttr();
    // check if this is the default value
    const bool usingDefaultValue = isValueValid() && (myAttrProperty->getDefaultStringValue() == getCurrentValue());
    // first check if insert default values
    if (!insertDefaultValues && usingDefaultValue) {
        return SUMO_ATTR_NOTHING;
    }
    // now check if attribute is activatable AND is enabled
    if (myAttrProperty->isActivatable() && (myAttributeToggleEnableCheckButton->getCheck() == FALSE)) {
        return SUMO_ATTR_NOTHING;
    }
    // continue depending of type
    if (myAttrProperty->isBool()) {
        baseObject->addBoolAttribute(attribute, myValueCheckButton->getCheck() == TRUE);
    } else if (myAttrProperty->isDiscrete()) {
        if ((myValueComboBox->getTextColor() == GUIDesignTextColorRed) ||
                (myValueComboBox->getBackColor() == GUIDesignBackgroundColorRed)) {
            return attribute;
        } else {
            baseObject->addStringAttribute(attribute, myValueComboBox->getText().text());
        }
    } else if ((myValueTextField->getTextColor() == GUIDesignTextColorRed) ||
               (myValueTextField->getBackColor() == GUIDesignBackgroundColorRed)) {
        return attribute;
    } else if (myAttrProperty->isInt()) {
        // int value
        if (GNEAttributeCarrier::canParse<int>(myValueTextField->getText().text())) {
            const auto intValue = GNEAttributeCarrier::parse<int>(myValueTextField->getText().text());
            if (myAttrProperty->isPositive() && (intValue < 0)) {
                return attribute;
            } else {
                baseObject->addIntAttribute(attribute, intValue);
            }
        } else if (myAttrProperty->hasDefaultValue() && (myValueTextField->getText().text() == myAttrProperty->getDefaultStringValue())) {
            baseObject->addIntAttribute(attribute, myAttrProperty->getDefaultIntValue());
        } else if (myAttrProperty->hasDefaultValue() && myValueTextField->getText().empty()) {
            baseObject->addIntAttribute(attribute, myAttrProperty->getDefaultIntValue());
        } else {
            return attribute;
        }
    } else if (myAttrProperty->isFloat()) {
        // double value
        if (myAttrProperty->isList()) {
            if (GNEAttributeCarrier::canParse<std::vector<double> >(myValueTextField->getText().text())) {
                const auto doubleListValue = GNEAttributeCarrier::parse<std::vector<double> >(myValueTextField->getText().text());
                // check every double
                for (const auto doubleValue : doubleListValue) {
                    if (myAttrProperty->isPositive() && (doubleValue < 0)) {
                        return attribute;
                    } else if (myAttrProperty->isFloat() && ((doubleValue < 0) || (doubleValue > 1))) {
                        return attribute;
                    }
                }
                baseObject->addDoubleListAttribute(attribute, doubleListValue);
            } else {
                return attribute;
            }
        } else if (GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            const auto doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // check using default value for certain default values (for example, length = -1)
            if (!usingDefaultValue && myAttrProperty->isPositive() && (doubleValue < 0)) {
                return attribute;
            } else if (!usingDefaultValue && myAttrProperty->isProbability() && ((doubleValue < 0) || (doubleValue > 1))) {
                return attribute;
            } else {
                baseObject->addDoubleAttribute(attribute, doubleValue);
            }
        } else if (myAttrProperty->hasDefaultValue() && (myValueTextField->getText().text() == myAttrProperty->getDefaultStringValue())) {
            baseObject->addDoubleAttribute(attribute, myAttrProperty->getDefaultDoubleValue());
        } else if (myAttrProperty->hasDefaultValue() && myValueTextField->getText().empty()) {
            baseObject->addDoubleAttribute(attribute, myAttrProperty->getDefaultDoubleValue());
        } else {
            return attribute;
        }
    } else if (myAttrProperty->isSUMOTime()) {
        // time value
        if (GNEAttributeCarrier::canParse<SUMOTime>(myValueTextField->getText().text())) {
            const auto timeValue = GNEAttributeCarrier::parse<SUMOTime>(myValueTextField->getText().text());
            if (timeValue < 0) {
                return attribute;
            } else {
                baseObject->addTimeAttribute(attribute, timeValue);
            }
        } else if (myAttrProperty->hasDefaultValue() && (myValueTextField->getText().text() == myAttrProperty->getDefaultStringValue())) {
            baseObject->addTimeAttribute(attribute, myAttrProperty->getDefaultTimeValue());
        } else if (myAttrProperty->hasDefaultValue() && myValueTextField->getText().empty()) {
            baseObject->addTimeAttribute(attribute, myAttrProperty->getDefaultTimeValue());
        } else {
            return attribute;
        }
    } else if (myAttrProperty->isPosition()) {
        // position value
        if (myAttrProperty->isList()) {
            if (GNEAttributeCarrier::canParse<PositionVector>(myValueTextField->getText().text())) {
                baseObject->addPositionVectorAttribute(attribute, GNEAttributeCarrier::parse<PositionVector>(myValueTextField->getText().text()));
            } else {
                return attribute;
            }
        } else if (GNEAttributeCarrier::canParse<Position>(myValueTextField->getText().text())) {
            baseObject->addPositionAttribute(attribute, GNEAttributeCarrier::parse<Position>(myValueTextField->getText().text()));
        } else if (myAttrProperty->hasDefaultValue() && (myValueTextField->getText().text() == myAttrProperty->getDefaultStringValue())) {
            baseObject->addPositionAttribute(attribute, myAttrProperty->getDefaultPositionValue());
        } else if (myAttrProperty->hasDefaultValue() && myValueTextField->getText().empty()) {
            baseObject->addPositionAttribute(attribute, myAttrProperty->getDefaultPositionValue());
        } else {
            return attribute;
        }
    } else if (myAttrProperty->isColor()) {
        // color value
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
            baseObject->addColorAttribute(attribute, GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text()));
        } else if (myAttrProperty->hasDefaultValue() && (myValueTextField->getText().text() == myAttrProperty->getDefaultStringValue())) {
            baseObject->addColorAttribute(attribute, myAttrProperty->getDefaultColorValue());
        } else if (myAttrProperty->hasDefaultValue() && myValueTextField->getText().empty()) {
            baseObject->addColorAttribute(attribute, myAttrProperty->getDefaultColorValue());
        } else {
            return attribute;
        }
    } else if (myAttrProperty->isList()) {
        baseObject->addStringListAttribute(attribute, GNEAttributeCarrier::parse<std::vector<std::string> >(myValueTextField->getText().text()));
    } else if (attribute == GNE_ATTR_PARAMETERS) {
        baseObject->addParameters(myValueTextField->getText().text());
    } else {
        baseObject->addStringAttribute(attribute, myValueTextField->getText().text());
    }
    // all ok, then return nothing
    return SUMO_ATTR_NOTHING;
}


long
GNEAttributesEditorRow::onCmdOpenColorDialog(FXObject*, FXSelector, void*) {
    RGBColor color = RGBColor::BLACK;
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
        color = GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text());
    } else if (myAttrProperty->hasDefaultValue()) {
        color = myAttrProperty->getDefaultColorValue();
    }
    // declare colorDialog
    const auto colorDialog = new GNEColorDialog(myAttributeTable->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows(), color);
    // continue depending of result
    if (colorDialog->getResult() == GNEDialog::Result::ACCEPT) {
        myValueTextField->setText(toString(colorDialog->getColor()).c_str(), TRUE);
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdOpenVClassDialog(FXObject*, FXSelector, void*) {
    // declare allowVClassesDialog
    const auto allowVClassesDialog = new GNEVClassesDialog(myAttributeTable->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows(),
            myAttrProperty->getAttr(), myValueTextField->getText().text());
    // continue depending of result
    if (allowVClassesDialog->getResult() == GNEDialog::Result::ACCEPT) {
        myValueTextField->setText(allowVClassesDialog->getModifiedVClasses().c_str(), TRUE);
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdOpenFileDialog(FXObject*, FXSelector, void*) {
    // open dialog
    const auto fileDialog = GNEFileDialog(myAttributeTable->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows(),
                                          myAttrProperty->getAttrStr(),
                                          myAttrProperty->getFilenameExtensions(),
                                          myAttrProperty->isFileSave() ? GNEFileDialog::OpenMode::SAVE : GNEFileDialog::OpenMode::LOAD_SINGLE,
                                          GNEFileDialog::ConfigType::NETEDIT);
    // update text field
    if (fileDialog.getResult() == GNEDialog::Result::ACCEPT) {
        myValueTextField->setText(fileDialog.getFilename().c_str(), TRUE);
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
    const auto attribute = myAttrProperty->getAttr();
    // continue depending of clicked object
    if (obj == myValueCheckButton) {
        // Set true o false depending of the checkBox
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
        } else {
            myValueCheckButton->setText("false");
        }
        myAttributeTable->setAttribute(attribute, myValueCheckButton->getText().text());
    } else if (obj == myValueComboBox) {
        const std::string newValue = myValueComboBox->getText().text();
        // check if the new comboBox value is valid
        if (editedAC->isValid(attribute, newValue)) {
            myValueComboBox->setTextColor(GUIDesignTextColorBlack);
            myValueComboBox->setBackColor(GUIDesignBackgroundColorWhite);
            myValueComboBox->killFocus();
            myAttributeTable->setAttribute(attribute, newValue);
        } else {
            // edit colors
            myValueComboBox->setTextColor(GUIDesignTextColorRed);
            if (newValue.empty()) {
                myValueComboBox->setBackColor(GUIDesignBackgroundColorRed);
            }
        }
    } else if (obj == myValueTextField) {
        // first check if set default value
        if (myValueTextField->getText().empty() && myAttrProperty->hasDefaultValue() && !myAttrProperty->isVClass()) {
            // update text field without notify
            myValueTextField->setText(myAttrProperty->getDefaultStringValue().c_str(), FALSE);
        }
        // if we're editing an angle, check if filter between [0,360]
        if ((attribute == SUMO_ATTR_ANGLE) && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter anglea and update text field without notify
            const double angle = fmod(GNEAttributeCarrier::parse<double>(myValueTextField->getText().text()), 360);
            myValueTextField->setText(toString(angle).c_str(), FALSE);
        }
        // if we're editing a position or a shape, strip whitespace after comma
        if ((attribute == SUMO_ATTR_POSITION) || (attribute == SUMO_ATTR_SHAPE)) {
            std::string shape(myValueTextField->getText().text());
            while (shape.find(", ") != std::string::npos) {
                shape = StringUtils::replace(shape, ", ", ",");
            }
            myValueTextField->setText(toString(shape).c_str(), FALSE);
        }
        // if we're editing a int, strip decimal value
        if (myAttrProperty->isInt() && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            if ((doubleValue - (int)doubleValue) == 0) {
                myValueTextField->setText(toString((int)doubleValue).c_str(), FALSE);
            }
        }
        // after apply all filters, obtain value
        const std::string newValue = myValueTextField->getText().text();
        // check if the new textField value is valid
        if (editedAC->isValid(attribute, newValue)) {
            myValueTextField->setTextColor(GUIDesignTextColorBlack);
            myValueTextField->setBackColor(GUIDesignBackgroundColorWhite);
            myValueTextField->killFocus();
            if (myAttributeTable->isEditorTypeEditor() || newValue.empty() || (attribute != SUMO_ATTR_ID)) {
                myAttributeTable->setAttribute(attribute, newValue);
            }
        } else {
            // edit colors
            myValueTextField->setTextColor(GUIDesignTextColorRed);
            if (newValue.empty()) {
                myValueTextField->setBackColor(GUIDesignBackgroundColorRed);
            }
        }
    }
    return 1;
}


long
GNEAttributesEditorRow::onCmdToggleEnableAttribute(FXObject*, FXSelector, void*) {
    myAttributeTable->toggleEnableAttribute(myAttrProperty->getAttr(), myAttributeToggleEnableCheckButton->getCheck() == TRUE);
    return 0;
}


GNEAttributesEditorRow::GNEAttributesEditorRow() :
    myAttributeTable(nullptr) {
}


const std::string
GNEAttributesEditorRow::getAttributeValue(const bool enabled) const {
    const auto attribute = myAttrProperty->getAttr();
    // if we're in creator mode, generate ID
    if ((attribute == SUMO_ATTR_ID) && (myAttributeTable->myEditorType == GNEAttributesEditorType::EditorType::CREATOR)) {
        const auto& ACs = myAttributeTable->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
        const auto parentTag = myAttrProperty->getTagPropertyParent()->getTag();
        if (myAttrProperty->getTagPropertyParent()->getTag() == SUMO_TAG_EDGE) {
            return ACs->generateEdgeID();
        } else if (myAttrProperty->getTagPropertyParent()->isAdditionalElement()) {
            return ACs->generateAdditionalID(parentTag);
        } else if (myAttrProperty->getTagPropertyParent()->isDemandElement()) {
            return ACs->generateDemandElementID(parentTag);
        } else if (myAttrProperty->getTagPropertyParent()->isMeanData()) {
            return ACs->generateMeanDataID(parentTag);
        } else if (parentTag == SUMO_TAG_TYPE) {
            return ACs->generateEdgeTypeID();
        } else if (parentTag == SUMO_TAG_DATASET) {
            return ACs->generateDataSetID();
        }
    }
    if (enabled) {
        // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
        std::set<std::string> values;
        // iterate over edited attributes and insert every value in set
        for (const auto& editedAC : myAttributeTable->myEditedACs) {
            if (editedAC->hasAttribute(attribute)) {
                values.insert(editedAC->getAttribute(attribute));
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
        return myAttributeTable->myEditedACs.front()->getAlternativeValueForDisabledAttributes(attribute);
    }
}


void
GNEAttributesEditorRow::showAttributeToggleEnable(const GNEAttributeProperties* attrProperty, const bool value) {
    myAttributeToggleEnableCheckButton->setText(attrProperty->getAttrStr().c_str());
    myAttributeToggleEnableCheckButton->setCheck(value);
    myAttributeToggleEnableCheckButton->enable();
    myAttributeToggleEnableCheckButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeButton->hide();
}


void
GNEAttributesEditorRow::showAttributeReparent(const bool enabled) {
    // update attribute button
    myAttributeButton->setText(TL("Reparent"));
    myAttributeButton->setHelpText(TL("Change parent of this element"));
    myAttributeButton->setTipText(myAttributeButton->getHelpText());
    myAttributeButton->setIcon(nullptr);
    myAttributeButton->setSelector(MID_GNE_ATTRIBUTESEDITORROW_REPARENT);
    if (enabled) {
        myAttributeButton->enable();
    } else {
        myAttributeButton->disable();
    }
    myAttributeButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
}


void
GNEAttributesEditorRow::showAttributeInspectParent(const GNEAttributeProperties* attrProperty, const bool enabled) {
    // update attribute button
    myAttributeButton->setText(attrProperty->getAttrStr().c_str());
    myAttributeButton->setHelpText(TLF("Inspect % parent", attrProperty->getAttrStr()).c_str());
    myAttributeButton->setTipText(myAttributeButton->getHelpText());
    myAttributeButton->setIcon(GUIIconSubSys::getIcon(attrProperty->getTagPropertyParent()->getGUIIcon()));
    myAttributeButton->setSelector(MID_GNE_ATTRIBUTESEDITORROW_INSPECTPARENT);
    if (enabled) {
        myAttributeButton->enable();
    } else {
        myAttributeButton->disable();
    }
    myAttributeButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
}


void
GNEAttributesEditorRow::showAttributeVClass(const GNEAttributeProperties* attrProperty, const bool enabled) {
    // update attribute button
    myAttributeButton->setText(attrProperty->getAttrStr().c_str());
    myAttributeButton->setHelpText(TL("Open dialog for editing vClasses"));
    myAttributeButton->setTipText(myAttributeButton->getHelpText());
    myAttributeButton->setIcon(nullptr);
    myAttributeButton->setSelector(MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_ALLOW);
    if (enabled) {
        myAttributeButton->enable();
    } else {
        myAttributeButton->disable();
    }
    myAttributeButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
}


void
GNEAttributesEditorRow::showAttributeColor(const GNEAttributeProperties* attrProperty, const bool enabled) {
    // update attribute button
    myAttributeButton->setText(attrProperty->getAttrStr().c_str());
    myAttributeButton->setHelpText(TL("Open dialog for editing color"));
    myAttributeButton->setTipText(myAttributeButton->getHelpText());
    myAttributeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL));
    myAttributeButton->setSelector(MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_COLOR);
    if (enabled) {
        myAttributeButton->enable();
    } else {
        myAttributeButton->disable();
    }
    myAttributeButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
}


void
GNEAttributesEditorRow::showAttributeFile(const GNEAttributeProperties* attrProperty, const bool enabled) {
    // update attribute button
    myAttributeButton->setText(attrProperty->getAttrStr().c_str());
    if (attrProperty->isFileOpen()) {
        myAttributeButton->setHelpText(TL("Open dialog to select an existent file"));
    } else {
        myAttributeButton->setHelpText(TL("Open dialog to select or create an existent file"));
    }
    myAttributeButton->setTipText(myAttributeButton->getHelpText());
    myAttributeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN));
    myAttributeButton->setSelector(MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_FILE);
    if (enabled) {
        myAttributeButton->enable();
    } else {
        myAttributeButton->disable();
    }
    myAttributeButton->show();
    // hide other elements
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
}


void
GNEAttributesEditorRow::showAttributeLabel(const GNEAttributeProperties* attrProperty) {
    myAttributeLabel->setText(attrProperty->getAttrStr().c_str());
    myAttributeLabel->setTipText(attrProperty->getDefinition().c_str());
    myAttributeLabel->show();
    // hide other elements
    myAttributeToggleEnableCheckButton->hide();
    myAttributeButton->hide();
}


void
GNEAttributesEditorRow::hideAllAttributeElements() {
    myAttributeLabel->hide();
    myAttributeToggleEnableCheckButton->hide();
    myAttributeButton->hide();
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
GNEAttributesEditorRow::showValueComboBox(const GNEAttributeProperties* attrProperty, const std::string& value,
        const bool enabled, const bool computed) {
    // first we need to check if all boolean values are equal
    bool allValuesEqual = true;
    // declare  boolean vector
    std::vector<std::string> stringVector = GNEAttributeCarrier::parse<std::vector<std::string> >(value);
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
        myValueComboBox->setTextColor(GUIDesignTextColorBlack);
        myValueComboBox->setBackColor(GUIDesignBackgroundColorWhite);
        if (enabled) {
            myValueComboBox->enable();
        } else {
            myValueComboBox->disable();
        }
        // fill depeding of ACAttr
        if (attrProperty->getAttr() == SUMO_ATTR_VCLASS) {
            // add all vClasses with their icons
            for (const auto& vClassStr : SumoVehicleClassStrings.getStrings()) {
                myValueComboBox->appendIconItem(vClassStr.c_str(), VClassIcons::getVClassIcon(getVehicleClassID(vClassStr)));
            }
        } else if (attrProperty->isVType()) {
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
        } else if (attrProperty->getAttr() == SUMO_ATTR_ICON) {
            // add all POIIcons with their icons
            for (const auto& POIIcon : SUMOXMLDefinitions::POIIcons.getValues()) {
                myValueComboBox->appendIconItem(SUMOXMLDefinitions::POIIcons.getString(POIIcon).c_str(), POIIcons::getPOIIcon(POIIcon));
            }
        } else if ((attrProperty->getAttr() == SUMO_ATTR_RIGHT_OF_WAY) && (myAttributeTable->myEditedACs.size() == 1) &&
                   (attrProperty->getTagPropertyParent()->getTag() == SUMO_TAG_JUNCTION)) {
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
            for (const auto& discreteValue : attrProperty->getDiscreteValues()) {
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
        myValueTextField->setTextColor(GUIDesignTextColorBlue);
    } else {
        myValueTextField->setTextColor(GUIDesignTextColorBlack);
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
        if ((lane->getIndex() + 1) >= (int)lane->getParentEdge()->getChildLanes().size()) {
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
GNEAttributesEditorRow::enableElements(const GNEAttributeProperties* attrProperty, const bool forceDisable) {
    const auto& editModes = myAttributeTable->myFrameParent->getViewNet()->getEditModes();
    const auto tagProperty = attrProperty->getTagPropertyParent();
    // by default we assume that elements are disabled
    bool enableElements = false;
    if (forceDisable) {
        enableElements = false;
    } else if (editModes.isCurrentSupermodeNetwork()) {
        if (tagProperty->isNetworkElement() || tagProperty->isAdditionalElement()) {
            enableElements = true;
        } else if ((tagProperty->getTag() == SUMO_TAG_TAZSOURCE) || (tagProperty->getTag() == SUMO_TAG_TAZSINK)) {
            enableElements = true;
        }
    } else if (editModes.isCurrentSupermodeDemand() && tagProperty->isDemandElement()) {
        enableElements = true;
    } else if (editModes.isCurrentSupermodeData() && (tagProperty->isDataElement() || tagProperty->isMeanData())) {
        enableElements = true;
    }
    if (!enableElements) {
        myAttributeToggleEnableCheckButton->disable();
        myAttributeButton->disable();
        myValueTextField->disable();
        myValueComboBox->disable();
        myValueCheckButton->disable();
        myValueLaneUpButton->disable();
        myValueLaneDownButton->disable();
    }
}


bool
GNEAttributesEditorRow::isAttributeEnabled(const GNEAttributeProperties* attrProperty) const {
    if (attrProperty->isAlwaysEnabled()) {
        return true;
    } else {
        for (const auto& AC : myAttributeTable->myEditedACs) {
            if (AC->isAttributeEnabled(attrProperty->getAttr())) {
                return true;
            }
        }
        return false;
    }
}

/****************************************************************************/
