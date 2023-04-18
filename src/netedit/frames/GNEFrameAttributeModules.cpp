/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameAttributeModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliary class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrameAttributeModules.h"
#include "GNEFlowEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributeModules::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAttributeDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_VTYPE,    GNEFrameAttributeModules::AttributesEditorRow::onCmdInspectVTypeParent),
};

FXDEFMAP(GNEFrameAttributeModules::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributeModules::AttributesEditor::onCmdAttributesEditorHelp)
};

FXDEFMAP(GNEFrameAttributeModules::AttributesEditorExtended) AttributesEditorExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributeModules::AttributesEditorExtended::onCmdOpenDialog)
};

FXDEFMAP(GNEFrameAttributeModules::GenericDataAttributes) GenericDataAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorRow,          FXHorizontalFrame,       AttributesEditorRowMap,        ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditor,             MFXGroupBoxModule,       AttributesEditorMap,           ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorExtended,     MFXGroupBoxModule,       AttributesEditorExtendedMap,   ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameAttributeModules::GenericDataAttributes,        MFXGroupBoxModule,       GenericDataAttributesMap,      ARRAYNUMBER(GenericDataAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow(GNEFrameAttributeModules::AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr,
        const std::string& value, const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr),
    myACParent(ACParent) {
    // Create and hide label
    myAttributeLabel = new MFXLabelTooltip(this,
                                           attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                           "attributeLabel", nullptr, GUIDesignLabelThickedFixed(100));
    myAttributeLabel->hide();
    // Create and hide check button
    myAttributeCheckButton = new FXCheckButton(this, "attributeCheckButton", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myAttributeCheckButton->hide();
    // Create and hide ButtonCombinableChoices
    myAttributeAllowButton = new MFXButtonTooltip(this,
            attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            "attributeAllowButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeAllowButton->hide();
    // create and hide color editor
    myAttributeColorButton = new MFXButtonTooltip(this,
            attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            "attributeColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // create and hide color editor
    myAttributeVTypeButton = new MFXButtonTooltip(this,
            attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            "attributeVTypeButton", myACParent ? myACParent->getACIcon() : nullptr, this, MID_GNE_SET_ATTRIBUTE_VTYPE, GUIDesignButtonAttribute);
    myAttributeVTypeButton->hide();
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this,
            attributeEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    // Create and hide ComboBox
    myValueChoicesComboBox = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, (ACAttr.getAttr() == SUMO_ATTR_VCLASS), this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueChoicesComboBox->hide();
    // Create and hide checkButton
    myValueCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesEditorRow
        FXHorizontalFrame::create();
        // start enabling all elements, depending if attribute is enabled
        if (attributeEnabled == false) {
            myValueTextField->disable();
            myValueChoicesComboBox->disable();
            myValueCheckButton->disable();
        } else {
            myValueTextField->enable();
            myValueChoicesComboBox->enable();
            myValueCheckButton->enable();
        }
        // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
            if (isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr)) {
                myAttributeAllowButton->enable();
                myAttributeColorButton->enable();
                myAttributeVTypeButton->enable();
                myAttributeCheckButton->enable();
            } else {
                myAttributeColorButton->disable();
                myAttributeVTypeButton->disable();
                myAttributeCheckButton->disable();
                myValueTextField->disable();
                myValueChoicesComboBox->disable();
                myValueCheckButton->disable();
                myAttributeAllowButton->disable();
            }
        }
        // set left column
        if (myACParent) {
            // show color button and set color text depending of computed
            if (computed) {
                myAttributeVTypeButton->setTextColor(FXRGB(0, 0, 255));
            } else {
                myAttributeVTypeButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeVTypeButton->killFocus();
            }
            myAttributeVTypeButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeVTypeButton->setTipText((TL("Inspect vehicle ") + myACAttr.getAttrStr() + " parent").c_str());
            myAttributeVTypeButton->setHelpText((TL("Inspect vehicle ") + myACAttr.getAttrStr() + " parent").c_str());
            myAttributeVTypeButton->show();
        } else if (myACAttr.isColor()) {
            // show color button and set color text depending of computed
            if (computed) {
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 255));
            } else {
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeColorButton->killFocus();
            }
            myAttributeColorButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeColorButton->setTipText(TL("Open dialog for editing color"));
            myAttributeColorButton->setHelpText(TL("Open dialog for editing color"));
            myAttributeColorButton->show();
        } else if (myACAttr.getAttr() == SUMO_ATTR_ALLOW) {
            myAttributeAllowButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeAllowButton->setTipText(TL("Open dialog for editing allowed vClasses"));
            myAttributeAllowButton->setHelpText(TL("Open dialog for editing allowed vClasses"));
            myAttributeAllowButton->show();
        } else if (myACAttr.isActivatable()) {
            // show checkbox button and set color text depending of computed
            if (computed) {
                myAttributeCheckButton->setTextColor(FXRGB(0, 0, 255));
            } else {
                myAttributeCheckButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeCheckButton->killFocus();
            }
            myAttributeCheckButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeCheckButton->show();
            // check or uncheck depending of attributeEnabled
            if (attributeEnabled) {
                myAttributeCheckButton->setCheck(TRUE);
            } else {
                myAttributeCheckButton->setCheck(FALSE);
            }
        } else {
            // Show attribute Label
            myAttributeLabel->setText(myACAttr.getAttrStr().c_str());
            myAttributeLabel->setTipText(myACAttr.getDefinition().c_str());
            myAttributeLabel->show();
        }
        // Set field depending of the type of value
        if (myACAttr.isBool()) {
            // first we need to check if all boolean values are equal
            bool allBooleanValuesEqual = true;
            // declare  boolean vector
            std::vector<bool> booleanVector;
            // check if value can be parsed to a boolean vector
            if (GNEAttributeCarrier::canParse<std::vector<bool> >(value)) {
                booleanVector = GNEAttributeCarrier::parse<std::vector<bool> >(value);
            }
            // iterate over passed booleans comparing all element with the first
            for (const auto& booleanValue : booleanVector) {
                if (booleanValue != booleanVector.front()) {
                    allBooleanValuesEqual = false;
                }
            }
            // use checkbox or textfield depending if all booleans are equal
            if (allBooleanValuesEqual) {
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
            } else {
                // show list of bools (0 1)
                myValueTextField->setText(value.c_str());
                // set text depending of computed
                if (computed) {
                    myValueTextField->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueTextField->setTextColor(FXRGB(0, 0, 0));
                    myValueTextField->killFocus();
                }
                myValueTextField->show();
            }
        } else if (myACAttr.isDiscrete()) {
            // Check if are VClasses
            if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isVClasses()) {
                // hide label
                myAttributeLabel->hide();
                // Show button combinable choices
                myAttributeAllowButton->setText(myACAttr.getAttrStr().c_str());
                myAttributeAllowButton->setTipText(TL("Open dialog for editing allowed vClasses"));
                myAttributeAllowButton->setHelpText(TL("Open dialog for editing allowed vClasses"));
                myAttributeAllowButton->show();
                // Show string with the values
                myValueTextField->setText(value.c_str());
                // set color depending of computed
                if (computed) {
                    myValueTextField->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueTextField->setTextColor(FXRGB(0, 0, 0));
                    myValueTextField->killFocus();
                }
                myValueTextField->show();
            } else {
                // fill comboBox
                myValueChoicesComboBox->clearItems();
                // special case for VClass
                if (ACAttr.getAttr() == SUMO_ATTR_VCLASS) {
                    // add all vClasses with their icons
                    for (const auto& vClassStr : SumoVehicleClassStrings.getStrings()) {
                        myValueChoicesComboBox->appendIconItem(vClassStr.c_str(), VClassIcons::getVClassIcon(getVehicleClassID(vClassStr)));
                    }
                } else {
                    for (const auto& discreteValue : myACAttr.getDiscreteValues()) {
                        myValueChoicesComboBox->appendIconItem(discreteValue.c_str());
                    }
                }
                // show combo box with values
                if (myACAttr.showAllDiscreteValues()) {
                    myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems());
                } else {
                    myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems() < 10 ? myValueChoicesComboBox->getNumItems() : 10);
                }
                const int itemIndex = myValueChoicesComboBox->findItem(value.c_str());
                if (itemIndex == -1) {
                    myValueChoicesComboBox->setText(value.c_str());
                } else {
                    myValueChoicesComboBox->setCurrentItem(itemIndex);
                }
                // set color depending of computed
                if (computed) {
                    myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 0));
                    myValueChoicesComboBox->killFocus();
                }
                myValueChoicesComboBox->show();
            }
        } else if (ACParent && myACAttr.isVType() && (myACAttr.getAttr() == SUMO_ATTR_TYPE)) {
            // fill comboBox with vTypes
            myValueChoicesComboBox->clearItems();
            for (const auto& vType : ACParent->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
                myValueChoicesComboBox->appendIconItem(vType->getID().c_str(), vType->getACIcon());
            }
            // show only 10 vtypes
            myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems() < 10 ? myValueChoicesComboBox->getNumItems() : 10);
            const int itemIndex = myValueChoicesComboBox->findItem(value.c_str());
            if (itemIndex == -1) {
                myValueChoicesComboBox->setText(value.c_str());
            } else {
                myValueChoicesComboBox->setCurrentItem(itemIndex);
            }
            // set color depending of computed
            if (computed) {
                myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 255));
            } else {
                myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueChoicesComboBox->killFocus();
            }
            myValueChoicesComboBox->show();
        } else {
            // In any other case (String, list, etc.), show value as String
            myValueTextField->setText(value.c_str());
            // set color depending of computed
            if (computed) {
                myValueTextField->setTextColor(FXRGB(0, 0, 255));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
            myValueTextField->show();
        }
        // Show AttributesEditorRow
        show();
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value,
        const bool forceRefresh, const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent) {
    if (ACParent) {
        myACParent = ACParent;
    }
    // start enabling all elements, depending if attribute is enabled
    if (attributeEnabled == false) {
        myValueTextField->disable();
        myValueChoicesComboBox->disable();
        myValueCheckButton->disable();
    } else {
        myValueTextField->enable();
        myValueChoicesComboBox->enable();
        myValueCheckButton->enable();
    }
    // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
    if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
        if (isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr)) {
            myAttributeAllowButton->enable();
            myAttributeColorButton->enable();
            myAttributeCheckButton->enable();
        } else {
            myAttributeColorButton->disable();
            myAttributeCheckButton->disable();
            myValueTextField->disable();
            myValueChoicesComboBox->disable();
            myValueCheckButton->disable();
            myAttributeAllowButton->disable();
        }
    }
    // set check button
    if (myAttributeCheckButton->shown()) {
        myAttributeCheckButton->setCheck(attributeEnabled);
    }
    if ((myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) &&
            ACParent && myACAttr.isVType() && (myACAttr.getAttr() == SUMO_ATTR_TYPE)) {
        // fill comboBox with vTypes
        myValueChoicesComboBox->clearItems();
        for (const auto& vType : ACParent->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VTYPE)) {
            myValueChoicesComboBox->appendIconItem(vType->getID().c_str(), vType->getACIcon());
        }
        // show only 10 vtypes
        myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems() < 10 ? myValueChoicesComboBox->getNumItems() : 10);
        myValueChoicesComboBox->setCurrentItem(myValueChoicesComboBox->findItem(value.c_str()));
        myValueChoicesComboBox->show();
    } else if (myValueTextField->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextField->getTextColor() == FXRGB(0, 0, 0) || myValueTextField->getTextColor() == FXRGB(0, 0, 255) || forceRefresh) {
            myValueTextField->setText(value.c_str());
            // set blue color if is an computed value
            if (computed) {
                myValueTextField->setTextColor(FXRGB(0, 0, 255));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
        }
    } else if (myValueChoicesComboBox->shown()) {
        // fill comboBox
        myValueChoicesComboBox->clearItems();
        // special case for VClass
        if (myACAttr.getAttr() == SUMO_ATTR_VCLASS) {
            // add all vClasses with their icons
            for (const auto& vClassStr : SumoVehicleClassStrings.getStrings()) {
                myValueChoicesComboBox->appendIconItem(vClassStr.c_str(), VClassIcons::getVClassIcon(getVehicleClassID(vClassStr)));
            }
        } else {
            for (const auto& discreteValue : myACAttr.getDiscreteValues()) {
                myValueChoicesComboBox->appendIconItem(discreteValue.c_str(), nullptr);
            }
        }
        // show combo box with values
        if (myACAttr.showAllDiscreteValues()) {
            myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems());
        } else {
            myValueChoicesComboBox->setNumVisible(myValueChoicesComboBox->getNumItems() < 10 ? myValueChoicesComboBox->getNumItems() : 10);
        }
        myValueChoicesComboBox->setCurrentItem(myValueChoicesComboBox->findItem(value.c_str()));
        // set blue color if is an computed value
        if (computed) {
            myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 255));
        } else {
            myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 0));
            myValueChoicesComboBox->killFocus();
        }
        myValueChoicesComboBox->show();
    } else if (myValueCheckButton->shown()) {
        if (GNEAttributeCarrier::canParse<bool>(value)) {
            myValueCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
        } else {
            myValueCheckButton->setCheck(false);
        }
    }
}


bool
GNEFrameAttributeModules::AttributesEditorRow::isAttributesEditorRowValid() const {
    return ((myValueTextField->getTextColor() == FXRGB(0, 0, 0)) || (myValueTextField->getTextColor() == FXRGB(0, 0, 255))) &&
           ((myValueChoicesComboBox->getTextColor() == FXRGB(0, 0, 0)) || (myValueChoicesComboBox->getTextColor() == FXRGB(0, 0, 255)));
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    GNEViewNet* viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    if (obj == myAttributeColorButton) {
        // create FXColorDialog
        FXColorDialog colordialog(this, TL("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text())));
        } else if (!myACAttr.getDefaultValue().empty()) {
            colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myACAttr.getDefaultValue())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
            myValueTextField->setText(newValue.c_str());
            if (viewNet->getInspectedAttributeCarriers().front()->isValid(myACAttr.getAttr(), newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (ACs.size() > 1) {
                    viewNet->getUndoList()->begin(ACs.front()->getTagProperty().getGUIIcon(), TL("change multiple attributes"));
                }
                // Set new value of attribute in all selected ACs
                for (const auto& inspectedAC : viewNet->getInspectedAttributeCarriers()) {
                    inspectedAC->setAttribute(myACAttr.getAttr(), newValue, viewNet->getUndoList());
                }
                // finish change multiple attributes
                if (ACs.size() > 1) {
                    viewNet->getUndoList()->end();
                }
                // If previously value was incorrect, change font color to black
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
        }
        return 0;
    } else if (obj == myAttributeAllowButton) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (ACs.size() > 1) {
            viewNet->getUndoList()->begin(ACs.front()->getTagProperty().getGUIIcon(), TL("change multiple attributes"));
        }
        // declare accept changes
        bool acceptChanges = false;
        // open GNEAllowVClassesDialog (also used to modify SUMO_ATTR_CHANGE_LEFT etc
        GNEAllowVClassesDialog(viewNet,
                               viewNet->getInspectedAttributeCarriers().front(), myACAttr.getAttr(), &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            std::string allowed = viewNet->getInspectedAttributeCarriers().front()->getAttribute(myACAttr.getAttr());
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : viewNet->getInspectedAttributeCarriers()) {
                inspectedAC->setAttribute(myACAttr.getAttr(), allowed, viewNet->getUndoList());
            }
            // finish change multiple attributes
            if (ACs.size() > 1) {
                viewNet->getUndoList()->end();
            }
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
        }
        return 1;
    } else {
        throw ProcessError("Invalid call to onCmdOpenAttributeDialog");
    }
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdInspectVTypeParent(FXObject*, FXSelector, void*) {
    auto viewnet = myAttributesEditorParent->getFrameParent()->getViewNet();
    viewnet->getViewParent()->getInspectorFrame()->inspectChild(myACParent, viewnet->getInspectedAttributeCarriers().front());
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // First, obtain the string value of the new attribute depending of their type
    if (myACAttr.isBool()) {
        // first check if we're editing boolean as a list of string or as a checkbox
        if (myValueCheckButton->shown()) {
            // Set true o false depending of the checkBox
            if (myValueCheckButton->getCheck()) {
                myValueCheckButton->setText("true");
                newVal = "true";
            } else {
                myValueCheckButton->setText("false");
                newVal = "false";
            }
        } else {
            // obtain boolean value of myValueTextField (because we're inspecting multiple attribute carriers with different values)
            newVal = myValueTextField->getText().text();
        }
    } else if (myACAttr.isDiscrete()) {
        // Check if are VClasses
        if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isVClasses()) {
            // Get value obtained using AttributesEditor
            newVal = myValueTextField->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myValueChoicesComboBox->getText().text();
        }
    } else if (myACParent && myACAttr.isVType() && (myACAttr.getAttr() == SUMO_ATTR_TYPE)) {
        // Get value of ComboBox
        if (myValueChoicesComboBox->shown()) {
            newVal = myValueChoicesComboBox->getText().text();
        } else {
            newVal = myValueTextField->getText().text();
        }
    } else {
        // Check if default value of attribute must be set
        if (myValueTextField->getText().empty() && myACAttr.hasDefaultValue()) {
            newVal = myACAttr.getDefaultValue();
            myValueTextField->setText(newVal.c_str());
        } else if (myACAttr.isInt() && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter int attributes
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // check if myValueTextField has to be updated
            if ((doubleValue - (int)doubleValue) == 0) {
                newVal = toString((int)doubleValue);
                myValueTextField->setText(newVal.c_str(), FALSE);
            }
        } else if ((myACAttr.getAttr() == SUMO_ATTR_ANGLE) && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter angle
            double angle = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // filter if angle isn't between [0,360]
            if ((angle < 0) || (angle > 360)) {
                // apply module
                angle = fmod(angle, 360);
            }
            // set newVal
            newVal = toString(angle);
            // update Textfield
            myValueTextField->setText(newVal.c_str(), FALSE);
        } else {
            // obtain value of myValueTextField
            newVal = myValueTextField->getText().text();
        }
    }
    // we need a extra check for Position and Shape Values, due #2658
    if ((myACAttr.getAttr() == SUMO_ATTR_POSITION) || (myACAttr.getAttr() == SUMO_ATTR_SHAPE)) {
        newVal = stripWhitespaceAfterComma(newVal);
    }
    // get inspected ACs (for code cleaning)
    const auto& inspectedACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // check if use default value
    const bool useDefaultValue = (newVal.empty() && myACAttr.hasDefaultValue());
    // Check if attribute must be changed
    if ((inspectedACs.size() > 0) && (inspectedACs.front()->isValid(myACAttr.getAttr(), newVal) || useDefaultValue)) {
        // check if we're merging junction
        if (!mergeJunction(myACAttr.getAttr(), inspectedACs, newVal)) {
            // if its valid for the first AC than its valid for all (of the same type)
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->begin(inspectedACs.front()->getTagProperty().getGUIIcon(), TL("change multiple attributes"));
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                // IDs attribute has to be encapsulated
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->begin(inspectedACs.front()->getTagProperty().getGUIIcon(), TL("change ") + myACAttr.getTagPropertyParent().getTagStr() + TL(" attribute"));
            }
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : inspectedACs) {
                if (useDefaultValue) {
                    inspectedAC->setAttribute(myACAttr.getAttr(), myACAttr.getDefaultValue(), myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
                } else {
                    inspectedAC->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
                }
            }
            // finish change multiple attributes or ID Attributes
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->end();
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->end();
            }
            // If previously value was incorrect, change font color to black
            if (myACAttr.isVClasses()) {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->setBackColor(FXRGB(255, 255, 255));
                myValueTextField->killFocus();
                // in this case, we need to refresh the other values (For example, allow/Disallow objects)
                myAttributesEditorParent->refreshAttributeEditor(false, false);
            } else if (myACAttr.isDiscrete()) {
                myValueChoicesComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueChoicesComboBox->setBackColor(FXRGB(255, 255, 255));
                myValueChoicesComboBox->killFocus();
            } else if (myValueTextField != nullptr) {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->setBackColor(FXRGB(255, 255, 255));
                myValueTextField->killFocus();
            }
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (myACAttr.isVClasses()) {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
        } else if (myACAttr.isDiscrete()) {
            myValueChoicesComboBox->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueChoicesComboBox->setBackColor(FXRGBA(255, 213, 213, 255));
            }
        } else if (myValueTextField != nullptr) {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ") + myACAttr.getAttrStr() + TL(" of ") + myACAttr.getTagPropertyParent().getTagStr() + TL(" isn't valid"));
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // check if we have to enable or disable
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextField->enable();
        // enable attribute
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), TL("enable attribute '") + myACAttr.getAttrStr() + "'");
        ACs.front()->enableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextField->disable();
        // disable attribute
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), TL("disable attribute '") + myACAttr.getAttrStr() + "'");
        ACs.front()->disableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    }
    return 0;
}


GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow() :
    myAttributesEditorParent(nullptr) {
}


std::string
GNEFrameAttributeModules::AttributesEditorRow::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}


bool
GNEFrameAttributeModules::AttributesEditorRow::mergeJunction(SumoXMLAttr attr, const std::vector<GNEAttributeCarrier*>& inspectedACs, const std::string& newVal) const {
    // check if we're editing junction position
    if ((inspectedACs.size() == 1) && (inspectedACs.front()->getTagProperty().getTag() == SUMO_TAG_JUNCTION) && (attr == SUMO_ATTR_POSITION)) {
        // retrieve original junction
        GNEJunction* movedJunction = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveJunction(inspectedACs.front()->getID());
        // parse position
        const Position newPosition = GNEAttributeCarrier::parse<Position>(newVal);
        // iterate over network junction
        for (const auto& junction : myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->getJunctions()) {
            // check distance position
            if ((junction.second->getPositionInView().distanceTo2D(newPosition) < POSITION_EPS) &&
                    myAttributesEditorParent->getFrameParent()->getViewNet()->mergeJunctions(movedJunction, junction.second)) {
                return true;
            }
        }
    }
    // nothing to merge
    return false;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditor::AttributesEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myAttributesFlowEditor
    myAttributesEditorFlow = new GNEFlowEditor(frameParent->getViewNet(), frameParent);
    // leave it hidden
    myAttributesEditorFlow->hideFlowEditor();
    // Create help button
    myHelpButton = new FXButton(getCollapsableFrame(), TL("Help"), nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEFrameAttributeModules::AttributesEditor::showAttributeEditorModule(bool includeExtended, bool forceAttributeEnabled) {
    myIncludeExtended = includeExtended;
    // first remove all rows
    for (auto& row : myAttributesEditorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // get inspected ACs
    const auto& ACs = myFrameParent->getViewNet()->getInspectedAttributeCarriers();
    // declare flag to check if flow editor has to be shown
    bool showFlowEditor = false;
    if (ACs.size() > 0) {
        // Iterate over attributes
        for (const auto& attrProperty : ACs.front()->getTagProperty()) {
            // declare flag to show/hide attribute
            bool editAttribute = true;
            // disable editing for unique attributes in case of multi-selection
            if ((ACs.size() > 1) && attrProperty.isUnique()) {
                editAttribute = false;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (attrProperty.isExtended() && !includeExtended) {
                editAttribute = false;
            }
            // disable editing of flow definition attributes, but enable flow editor
            if (attrProperty.isFlowDefinition()) {
                editAttribute = false;
                showFlowEditor = true;
            }
            // continue if attribute is editable
            if (editAttribute) {
                // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occurringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occurringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occurringValues.begin(); values != occurringValues.end(); values++) {
                    if (values != occurringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // overwrite value if attribute is disabled (used by LinkIndex)
                if (attributeEnabled == false) {
                    value = ACs.front()->getAlternativeValueForDisabledAttributes(attrProperty.getAttr());
                }
                // extra check for Triggered and container Triggered
                if (ACs.front()->getTagProperty().isStop() || ACs.front()->getTagProperty().isStopPerson()) {
                    if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    } else if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    }
                }
                // if forceAttributeEnabled is enabled, force attributeEnabled (except for ID)
                if (forceAttributeEnabled && (attrProperty.getAttr() != SUMO_ATTR_ID)) {
                    attributeEnabled = true;
                }
                // check if this attribute is computed
                const bool computed = (ACs.size() > 1) ? false : ACs.front()->isAttributeComputed(attrProperty.getAttr());
                // if is a Vtype, obtain icon
                GNEAttributeCarrier* ACParent = nullptr;
                if ((ACs.size() == 1) && attrProperty.isVType()) {
                    if (attrProperty.getAttr() == SUMO_ATTR_TYPE) {
                        ACParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, ACs.front()->getAttribute(SUMO_ATTR_TYPE));
                    } else if (ACs.front()->getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION).size() > 0) {
                        ACParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, ACs.front()->getAttribute(GNE_ATTR_VTYPE_DISTRIBUTION));
                    }
                }
                // create attribute editor row
                myAttributesEditorRows[attrProperty.getPositionListed()] = new AttributesEditorRow(this, attrProperty, value, attributeEnabled, computed, ACParent);
            }
        }
        // check if Flow editor has to be shown
        if (showFlowEditor) {
            myAttributesEditorFlow->showFlowEditor(ACs);
        } else {
            myAttributesEditorFlow->hideFlowEditor();
        }
        // show AttributesEditor
        show();
    } else {
        myAttributesEditorFlow->hideFlowEditor();
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNEFrameAttributeModules::AttributesEditor::hideAttributesEditorModule() {
    // hide AttributesEditorFlowModule
    myAttributesEditorFlow->hideFlowEditor();
    // hide also AttributesEditor
    hide();
}


void
GNEFrameAttributeModules::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    // get inspected ACs
    const auto& ACs = myFrameParent->getViewNet()->getInspectedAttributeCarriers();
    // first check if there is inspected attribute carriers
    if (ACs.size() > 0) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : ACs.front()->getTagProperty()) {
            // declare flag to show/hide attribute
            bool editAttribute = true;
            // disable editing for unique attributes in case of multi-selection
            if ((ACs.size() > 1) && attrProperty.isUnique()) {
                editAttribute = false;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (attrProperty.isExtended() && !myIncludeExtended) {
                editAttribute = false;
            }
            // disable editing of flow definition attributes, but enable flow editor
            if (attrProperty.isFlowDefinition()) {
                editAttribute = false;
            }
            // continue if attribute is editable
            if (editAttribute) {
                // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occurringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occurringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occurringValues.begin(); values != occurringValues.end(); values++) {
                    if (values != occurringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // overwrite value if attribute is disabled (used by LinkIndex)
                if (attributeEnabled == false) {
                    value = ACs.front()->getAlternativeValueForDisabledAttributes(attrProperty.getAttr());
                }
                // extra check for Triggered and container Triggered
                if (ACs.front()->getTagProperty().isStop() || ACs.front()->getTagProperty().isStopPerson()) {
                    if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    } else if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    }
                }
                // check if this attribute is computed
                const bool computed = (ACs.size() > 1) ? false : ACs.front()->isAttributeComputed(attrProperty.getAttr());
                // Check if Position or Shape refresh has to be forced
                if ((attrProperty.getAttr() == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed, nullptr);
                } else if ((attrProperty.getAttr()  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed, nullptr);
                } else if (attrProperty.getAttr()  == SUMO_ATTR_TYPE && (attrProperty.getTagPropertyParent().isVehicle() || attrProperty.getTagPropertyParent().isPerson() ||
                           attrProperty.getTagPropertyParent().isContainer())) {
                    // update vType parent
                    auto vTypeParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, ACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, false, attributeEnabled, computed, vTypeParent);
                } else {
                    // Refresh attributes maintain invalid values
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, false, attributeEnabled, computed, nullptr);
                }
            }
        }
        // check if flow editor has to be update
        if (myAttributesEditorFlow->shownFlowEditor()) {
            myAttributesEditorFlow->refreshFlowEditor();
        }
    }
}


GNEFrame*
GNEFrameAttributeModules::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEFrameAttributeModules::AttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myFrameParent->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // open Help attributes dialog
        myFrameParent->openHelpAttributesDialog(myFrameParent->getViewNet()->getInspectedAttributeCarriers().front());
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditorExtended- methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditorExtended::AttributesEditorExtended(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Extended attributes")),
    myFrameParent(frameParent) {
    // Create open dialog button
    new FXButton(getCollapsableFrame(), TL("Open attributes editor"), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::AttributesEditorExtended::~AttributesEditorExtended() {}


void
GNEFrameAttributeModules::AttributesEditorExtended::showAttributesEditorExtendedModule() {
    show();
}


void
GNEFrameAttributeModules::AttributesEditorExtended::hideAttributesEditorExtendedModule() {
    hide();
}


long
GNEFrameAttributeModules::AttributesEditorExtended::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // open GNEAttributesCreator extended dialog
    myFrameParent->attributesEditorExtendedDialogOpened();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::GenericDataAttributes - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::GenericDataAttributes::GenericDataAttributes(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Attributes")),
    myFrameParent(frameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = new FXButton(getCollapsableFrame(), TL("Edit attributes"), nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::GenericDataAttributes::~GenericDataAttributes() {}


void
GNEFrameAttributeModules::GenericDataAttributes::showGenericDataAttributes() {
    // refresh GenericDataAttributes
    refreshGenericDataAttributes();
    // show groupbox
    show();
}


void
GNEFrameAttributeModules::GenericDataAttributes::hideGenericDataAttributes() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributeModules::GenericDataAttributes::refreshGenericDataAttributes() {
    myTextFieldParameters->setText(getParametersStr().c_str());
    myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
    myTextFieldParameters->killFocus();
}


const Parameterised::Map&
GNEFrameAttributeModules::GenericDataAttributes::getParametersMap() const {
    return myParameters;
}


std::string
GNEFrameAttributeModules::GenericDataAttributes::getParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : myParameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEFrameAttributeModules::GenericDataAttributes::getParameters() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : myParameters) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEFrameAttributeModules::GenericDataAttributes::setParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
    // declare result string
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : parameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    // set result in textField (and call onCmdEditParameters)
    myTextFieldParameters->setText(result.c_str(), TRUE);
}


GNEFrame*
GNEFrameAttributeModules::GenericDataAttributes::getFrameParent() const {
    return myFrameParent;
}


bool
GNEFrameAttributeModules::GenericDataAttributes::areAttributesValid() const {
    if (myTextFieldParameters->getText().empty()) {
        return true;
    } else if (myTextFieldParameters->getTextColor() == FXRGB(255, 0, 0)) {
        return false;
    } else {
        return Parameterised::areAttributesValid(getParametersStr());
    }
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open single parameters dialog");
    if (GNESingleParametersDialog(this).execute()) {
        // write debug information
        WRITE_DEBUG("Close single parameters dialog");
        // Refresh parameter EditorCreator
        refreshGenericDataAttributes();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel single parameters dialog");
    }
    return 1;
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters(FXObject*, FXSelector, void*) {
    // clear current existent parameters
    myParameters.clear();
    // check if current given string is valid
    if (Parameterised::areParametersValid(myTextFieldParameters->getText().text(), true)) {
        // parsed parameters ok, then set text field black and continue
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        myTextFieldParameters->killFocus();
        // obtain parameters "key=value"
        std::vector<std::string> parameters = StringTokenizer(myTextFieldParameters->getText().text(), "|", true).getVector();
        // iterate over parameters
        for (const auto& parameter : parameters) {
            // obtain key, value
            std::vector<std::string> keyParam = StringTokenizer(parameter, "=", true).getVector();
            // save it in myParameters
            myParameters[keyParam.front()] = keyParam.back();
        }
        // overwrite myTextFieldParameters (to remove duplicated parameters
        myTextFieldParameters->setText(getParametersStr().c_str(), FALSE);
    } else {
        myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
            (AC->getTagProperty().isNetworkElement() || AC->getTagProperty().isAdditionalElement())) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               AC->getTagProperty().isDemandElement()) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeData() &&
               (AC->getTagProperty().isDataElement() || AC->getTagProperty().isMeanData())) {
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr) {
    if (ACAttr.getTagPropertyParent().isNetworkElement() || ACAttr.getTagPropertyParent().isAdditionalElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeNetwork());
    } else if (ACAttr.getTagPropertyParent().isDemandElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeDemand());
    } else if (ACAttr.getTagPropertyParent().isDataElement() || ACAttr.getTagPropertyParent().isMeanData()) {
        return (viewNet->getEditModes().isCurrentSupermodeData());
    } else {
        return false;
    }
}

/****************************************************************************/
