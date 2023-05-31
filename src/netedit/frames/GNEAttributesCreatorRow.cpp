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
/// @file    GNEAttributesCreatorRow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Row used in AttributesCreator
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/SUMOVehicleClass.h>

#include "GNEAttributesCreatorRow.h"
#include "GNEAttributesCreator.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAttributesCreatorRow) RowCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEAttributesCreatorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEAttributesCreatorRow::onCmdOpenAttributeDialog),
};

// Object implementation
FXIMPLEMENT(GNEAttributesCreatorRow,         FXHorizontalFrame,      RowCreatorMap,                  ARRAYNUMBER(RowCreatorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesCreatorRow::GNEAttributesCreatorRow(GNEAttributesCreator* AttributesCreatorParent, const GNEAttributeProperties& attrProperties) :
    FXHorizontalFrame(AttributesCreatorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesCreatorParent(AttributesCreatorParent),
    myAttrProperties(attrProperties) {
    // Create left visual elements
    myAttributeLabel = new MFXLabelTooltip(this,
                                           AttributesCreatorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                           TL("name"), nullptr, GUIDesignLabelThickedFixed(100));
    myAttributeLabel->hide();
    myEnableAttributeCheckButton = new FXCheckButton(this, TL("name"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myEnableAttributeCheckButton->hide();
    myAttributeButton = new FXButton(this, TL("button"), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeButton->hide();
    // Create right visual elements
    myValueTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    myValueCheckButton = new FXCheckButton(this, TL("Disabled"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    myValueComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBox->hide();
    // only create if parent was created
    if (getParent()->id()) {
        // create GNEAttributesCreatorRow
        FXHorizontalFrame::create();
        // refresh row
        refreshRow();
        // show GNEAttributesCreatorRow
        show();
    }
}


void
GNEAttributesCreatorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


const GNEAttributeProperties&
GNEAttributesCreatorRow::getAttrProperties() const {
    return myAttrProperties;
}


std::string
GNEAttributesCreatorRow::getValue() const {
    if (myAttrProperties.isBool()) {
        return (myValueCheckButton->getCheck() == 1) ? "1" : "0";
    } else if (myAttrProperties.isDiscrete()) {
        return myValueComboBox->getText().text();
    } else {
        return myValueTextField->getText().text();
    }
}


bool
GNEAttributesCreatorRow::getAttributeCheckButtonCheck() const {
    if (shown()) {
        return myEnableAttributeCheckButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEAttributesCreatorRow::setAttributeCheckButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myEnableAttributeCheckButton->setCheck(value);
        // enable or disable input fields
        if (value) {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->enable();
            } else if (myAttrProperties.isDiscrete()) {
                myValueComboBox->enable();
            } else {
                myValueTextField->enable();
            }
        } else {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->disable();
            } else if (myAttrProperties.isDiscrete()) {
                myValueComboBox->disable();
            } else {
                myValueTextField->disable();
            }
        }
    }
}


void
GNEAttributesCreatorRow::enableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->enable();
    } else if (myAttrProperties.isDiscrete()) {
        myValueComboBox->enable();
    } else {
        return myValueTextField->enable();
    }
}


void
GNEAttributesCreatorRow::disableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->disable();
    } else if (myAttrProperties.isDiscrete()) {
        myValueComboBox->disable();
    } else {
        return myValueTextField->disable();
    }
}


bool
GNEAttributesCreatorRow::isAttributesCreatorRowEnabled() const {
    if (!shown()) {
        return false;
    } else if (myAttrProperties.isBool()) {
        return myValueCheckButton->isEnabled();
    } else if (myAttrProperties.isDiscrete()) {
        return myValueComboBox->isEnabled();
    } else {
        return myValueTextField->isEnabled();
    }
}


void
GNEAttributesCreatorRow::refreshRow() {
    // reset invalid value
    myInvalidValue.clear();
    // special case for attribute ID
    if ((myAttrProperties.getAttr() == SUMO_ATTR_ID) && myAttrProperties.hasAutomaticID()) {
        // show label
        myAttributeLabel->setText(myAttrProperties.getAttrStr().c_str());
        myAttributeLabel->setTipText(myAttrProperties.getDefinition().c_str());
        myAttributeLabel->show();
        // generate ID
        myValueTextField->setText(generateID().c_str());
        // show textField
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        myValueTextField->show();
    } else {
        // left
        if (myAttrProperties.isColor() || (myAttrProperties.getAttr() == SUMO_ATTR_ALLOW)) {
            // show color button
            myAttributeButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeButton->killFocus();
            myAttributeButton->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeButton->show();
        } else if (myAttrProperties.isActivatable()) {
            // show check button
            myEnableAttributeCheckButton->setText(myAttrProperties.getAttrStr().c_str());
            myEnableAttributeCheckButton->show();
            // enable or disable depending of template AC
            if (myAttributesCreatorParent->getCurrentTemplateAC()->isAttributeEnabled(myAttrProperties.getAttr())) {
                myEnableAttributeCheckButton->setCheck(TRUE);
            } else {
                myEnableAttributeCheckButton->setCheck(FALSE);
            }
        } else {
            // show label
            myAttributeLabel->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeLabel->setTipText(myAttrProperties.getDefinition().c_str());
            myAttributeLabel->show();
        }
        // right
        if (myAttrProperties.isBool()) {
            if (GNEAttributeCarrier::parse<bool>(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()))) {
                myValueCheckButton->setCheck(true);
                myValueCheckButton->setText("true");
            } else {
                myValueCheckButton->setCheck(false);
                myValueCheckButton->setText("false");
            }
            myValueCheckButton->show();
            // check if enable or disable
            if (myAttributesCreatorParent->getCurrentTemplateAC()->isAttributeEnabled(myAttrProperties.getAttr())) {
                myValueCheckButton->enable();
            } else {
                myValueCheckButton->disable();
            }
        } else if (myAttrProperties.isDiscrete()) {
            // fill textField
            myValueComboBox->clearItems();
            for (const auto& item : myAttrProperties.getDiscreteValues()) {
                myValueComboBox->appendItem(item.c_str());
            }
            // set number of visible items
            if (myAttrProperties.showAllDiscreteValues()) {
                myValueComboBox->setNumVisible(myValueComboBox->getNumItems());
            } else {
                myValueComboBox->setNumVisible(10);
            }
            myValueComboBox->setText(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()).c_str());
            if (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueComboBox->getText().text())) {
                myValueComboBox->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueComboBox->killFocus();
            }
            myValueComboBox->show();
            // check if enable or disable
            if (myAttributesCreatorParent->getCurrentTemplateAC()->isAttributeEnabled(myAttrProperties.getAttr())) {
                myValueComboBox->enable();
            } else {
                myValueComboBox->disable();
            }
        } else {
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->killFocus();
            myValueTextField->setText(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()).c_str());
            if (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueTextField->getText().text())) {
                myValueTextField->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
            myValueTextField->show();
            // check if enable or disable
            if (myAttributesCreatorParent->getCurrentTemplateAC()->isAttributeEnabled(myAttrProperties.getAttr())) {
                myValueTextField->enable();
            } else {
                myValueTextField->disable();
            }
        }
    }
}


void
GNEAttributesCreatorRow::disableRow() {
    myAttributeLabel->disable();
    myEnableAttributeCheckButton->disable();
    myAttributeButton->disable();
    myValueTextField->disable();
    myValueComboBox->disable();
    myValueCheckButton->disable();
}


bool
GNEAttributesCreatorRow::isAttributeValid() const {
    return (myValueTextField->getTextColor() != FXRGB(255, 0, 0) &&
            myValueComboBox->getTextColor() != FXRGB(255, 0, 0));
}


GNEAttributesCreator*
GNEAttributesCreatorRow::getAttributesCreatorParent() const {
    return myAttributesCreatorParent;
}


long
GNEAttributesCreatorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // check what object was called
    if (obj == myEnableAttributeCheckButton) {
        if (myEnableAttributeCheckButton->getCheck()) {
            // enable text field
            if (myValueTextField->shown()) {
                myValueTextField->enable();
            }
            // enable comboBox
            if (myValueComboBox->shown()) {
                myValueComboBox->enable();
            }
            // enable check button
            if (myValueCheckButton->shown()) {
                myValueCheckButton->enable();
            }
            myAttributesCreatorParent->getCurrentTemplateAC()->toggleAttribute(myAttrProperties.getAttr(), true);
        } else {
            // disable text field
            if (myValueTextField->shown()) {
                myValueTextField->disable();
            }
            // disable text field
            if (myValueComboBox->shown()) {
                myValueComboBox->disable();
            }
            // disable check button
            if (myValueCheckButton->shown()) {
                myValueCheckButton->disable();
            }
            myAttributesCreatorParent->getCurrentTemplateAC()->toggleAttribute(myAttrProperties.getAttr(), false);
        }
    } else if (obj == myValueCheckButton) {
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), "true");
        } else {
            myValueCheckButton->setText("false");
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), "false");
        }
    } else if (obj == myValueComboBox) {
        // check if use default value
        const bool useDefaultValue = (myValueComboBox->getText().empty() && myAttrProperties.hasDefaultValue());
        // change color of text field depending of myCurrentValueValid
        if (myAttributesCreatorParent->getCurrentTemplateAC()->isValid(myAttrProperties.getAttr(), myValueComboBox->getText().text()) || useDefaultValue) {
            // check color depending if is a default value
            if (useDefaultValue || (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueComboBox->getText().text()))) {
                myValueComboBox->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueComboBox->killFocus();
            }
            // check if use default value
            if (useDefaultValue) {
                myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myAttrProperties.getDefaultValue());
                // refresh entire GNEAttributesCreator
                myAttributesCreatorParent->refreshAttributesCreator();
            } else {
                myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myValueComboBox->getText().text());
            }
            // special case for trigger stops (in the future will be changed)
            if (myAttributesCreatorParent->getCurrentTemplateAC()->getTagProperty().isStop() && (myAttrProperties.getAttr() == SUMO_ATTR_TRIGGERED)) {
                // refresh entire GNEAttributesCreator
                myAttributesCreatorParent->refreshAttributesCreator();
            }
        } else {
            // if value of TextField isn't valid, change their color to Red
            myValueComboBox->setTextColor(FXRGB(255, 0, 0));
            myValueComboBox->killFocus();
        }
    } else if (obj == myValueTextField) {
        // check if use default value
        const bool useDefaultValue = (myValueTextField->getText().empty() && myAttrProperties.hasDefaultValue());
        // change color of text field depending of myCurrentValueValid
        if (myAttributesCreatorParent->getCurrentTemplateAC()->isValid(myAttrProperties.getAttr(), myValueTextField->getText().text()) || useDefaultValue) {
            // check color depending if is a default value
            if (useDefaultValue || (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueTextField->getText().text()))) {
                myValueTextField->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
            // check if use default value
            if (useDefaultValue) {
                myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myAttrProperties.getDefaultValue());
                // refresh entire GNEAttributesCreator
                myAttributesCreatorParent->refreshAttributesCreator();
            } else {
                myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myValueTextField->getText().text());
            }
        } else {
            // if value of TextField isn't valid, change their color to Red
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            myValueComboBox->killFocus();
        }
    }
    // Update row
    update();
    return 1;
}


long
GNEAttributesCreatorRow::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // declare bool for accept changes
    bool acceptChanges = false;
    // continue depending of attribute
    if (myAttrProperties.getAttr() == SUMO_ATTR_COLOR) {
        // create FXColorDialog
        FXColorDialog colordialog(this, TL("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myAttrProperties.getDefaultValue())));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            myValueTextField->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str(), TRUE);
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_ALLOW) {
        // get allow string
        std::string allow = myValueTextField->getText().text();
        // opena allowDisallow dialog
        GNEAllowVClassesDialog(myAttributesCreatorParent->getFrameParent()->getViewNet(), &allow, &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            // update text field
            myValueTextField->setText(allow.c_str(), TRUE);
        }
    }
    return 0;
}


std::string
GNEAttributesCreatorRow::generateID() const {
    // get attribute carriers
    const auto& GNEAttributeCarriers = myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
    // continue depending of type
    if (myAttrProperties.getTagPropertyParent().isAdditionalElement()) {
        return GNEAttributeCarriers->generateAdditionalID(myAttrProperties.getTagPropertyParent().getTag());
    } else if (myAttrProperties.getTagPropertyParent().isDemandElement()) {
        return GNEAttributeCarriers->generateDemandElementID(myAttrProperties.getTagPropertyParent().getTag());
    } else {
        return "";
    }
}


bool
GNEAttributesCreatorRow::isValidID() const {
    if (myAttrProperties.getTagPropertyParent().isAdditionalElement()) {
        return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveAdditional(
                    myAttrProperties.getTagPropertyParent().getTag(), myValueTextField->getText().text(), false) == nullptr);
    } else if (myAttrProperties.getTagPropertyParent().isDemandElement()) {
        return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveAdditional(
                    myAttrProperties.getTagPropertyParent().getTag(), myValueTextField->getText().text(), false) == nullptr);
    } else {
        throw ProcessError(TL("Unsupported additional ID"));
    }
}

/****************************************************************************/
