/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/demand/GNERouteHandler.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/dialogs/GNEMultipleParametersDialog.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEFrame.h"
#include "GNEFrameAttributeModules.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributeModules::AttributesCreatorRow) RowCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::AttributesCreatorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributeModules::AttributesCreatorRow::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNEFrameAttributeModules::AttributesCreator) AttributesCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,  GNEFrameAttributeModules::AttributesCreator::onCmdReset),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,       GNEFrameAttributeModules::AttributesCreator::onCmdHelp),
};

FXDEFMAP(GNEFrameAttributeModules::FlowEditor) FlowEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrameAttributeModules::FlowEditor::onCmdSetFlowAttribute),
};

FXDEFMAP(GNEFrameAttributeModules::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAttributeDialog),
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

FXDEFMAP(GNEFrameAttributeModules::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrameAttributeModules::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrameAttributeModules::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrameAttributeModules::DrawingShape::onCmdAbortDrawing)
};

FXDEFMAP(GNEFrameAttributeModules::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrameAttributeModules::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEFrameAttributeModules::NeteditAttributes::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributeModules::AttributesCreatorRow,         FXHorizontalFrame,      RowCreatorMap,                  ARRAYNUMBER(RowCreatorMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesCreator,            FXGroupBoxModule,       AttributesCreatorMap,           ARRAYNUMBER(AttributesCreatorMap))
FXIMPLEMENT(GNEFrameAttributeModules::FlowEditor,                   FXGroupBoxModule,       FlowEditorMap,                  ARRAYNUMBER(FlowEditorMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorRow,          FXHorizontalFrame,      AttributesEditorRowMap,         ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditor,             FXGroupBoxModule,       AttributesEditorMap,            ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorExtended,     FXGroupBoxModule,       AttributesEditorExtendedMap,    ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameAttributeModules::GenericDataAttributes,        FXGroupBoxModule,       GenericDataAttributesMap,       ARRAYNUMBER(GenericDataAttributesMap))
FXIMPLEMENT(GNEFrameAttributeModules::DrawingShape,                 FXGroupBoxModule,       DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrameAttributeModules::NeteditAttributes,            FXGroupBoxModule,       NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesCreatorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesCreatorRow::AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeProperties& attrProperties) :
    FXHorizontalFrame(AttributesCreatorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesCreatorParent(AttributesCreatorParent),
    myAttrProperties(attrProperties) {
    // Create left visual elements
    myAttributeLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    myEnableAttributeCheckButton = new FXCheckButton(this, "name", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myEnableAttributeCheckButton->hide();
    myAttributeButton = new FXButton(this, "button", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeButton->hide();
    // Create right visual elements
    myValueTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    myValueCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    myValueComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBox->hide();
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesCreatorRow
        FXHorizontalFrame::create();
        // refresh row
        refreshRow();
        // show AttributesCreatorRow
        show();
    }
}


void
GNEFrameAttributeModules::AttributesCreatorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


const GNEAttributeProperties&
GNEFrameAttributeModules::AttributesCreatorRow::getAttrProperties() const {
    return myAttrProperties;
}


std::string
GNEFrameAttributeModules::AttributesCreatorRow::getValue() const {
    if (myAttrProperties.isBool()) {
        return (myValueCheckButton->getCheck() == 1) ? "1" : "0";
    } else if (myAttrProperties.isDiscrete()) {
        return myValueComboBox->getText().text();
    } else {
        return myValueTextField->getText().text();
    }
}


bool
GNEFrameAttributeModules::AttributesCreatorRow::getAttributeCheckButtonCheck() const {
    if (shown()) {
        return myEnableAttributeCheckButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameAttributeModules::AttributesCreatorRow::setAttributeCheckButtonCheck(bool value) {
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
GNEFrameAttributeModules::AttributesCreatorRow::enableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->enable();
    } else if (myAttrProperties.isDiscrete()) {
        myValueComboBox->enable();
    } else {
        return myValueTextField->enable();
    }
}


void
GNEFrameAttributeModules::AttributesCreatorRow::disableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->disable();
    } else if (myAttrProperties.isDiscrete()) {
        myValueComboBox->disable();
    } else {
        return myValueTextField->disable();
    }
}


bool
GNEFrameAttributeModules::AttributesCreatorRow::isAttributesCreatorRowEnabled() const {
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
GNEFrameAttributeModules::AttributesCreatorRow::refreshRow() {
    // reset invalid value
    myInvalidValue.clear();
    // special case for attribute ID
    if ((myAttrProperties.getAttr() == SUMO_ATTR_ID) && myAttrProperties.hasAutomaticID()) {
        // show check button and disable it
        myEnableAttributeCheckButton->setText(myAttrProperties.getAttrStr().c_str());
        myEnableAttributeCheckButton->setCheck(false);
        myEnableAttributeCheckButton->show();
        // show text field and disable it
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->disable();
        // generate ID
        myValueTextField->setText(generateID().c_str());
        // show textField
        myValueTextField->show();
    } else {
        // left
        if (myAttrProperties.isColor() || (myAttrProperties.getAttr() == SUMO_ATTR_ALLOW) || (myAttrProperties.getAttr() == SUMO_ATTR_DISALLOW)) {
            // show color button
            myAttributeButton->setTextColor(FXRGB(0, 0, 0));
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
            myValueComboBox->setNumVisible(myValueComboBox->getNumItems());
            myValueComboBox->setTextColor(FXRGB(0, 0, 0));
            myValueComboBox->setText(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()).c_str());
            myValueComboBox->show();
            // check if enable or disable
            if (myAttributesCreatorParent->getCurrentTemplateAC()->isAttributeEnabled(myAttrProperties.getAttr())) {
                myValueComboBox->enable();
            } else {
                myValueComboBox->disable();
            }
        } else {
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setText(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()).c_str());
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
GNEFrameAttributeModules::AttributesCreatorRow::disableRow() {
    myAttributeLabel->disable();
    myEnableAttributeCheckButton->disable();
    myAttributeButton->disable();
    myValueTextField->disable();
    myValueComboBox->disable();
    myValueCheckButton->disable();
}


bool
GNEFrameAttributeModules::AttributesCreatorRow::isAttributeValid() const {
    return (myValueTextField->getTextColor() != FXRGB(255, 0, 0) &&
            myValueComboBox->getTextColor() != FXRGB(255, 0, 0));
}


GNEFrameAttributeModules::AttributesCreator*
GNEFrameAttributeModules::AttributesCreatorRow::getAttributesCreatorParent() const {
    return myAttributesCreatorParent;
}


long
GNEFrameAttributeModules::AttributesCreatorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
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
            myAttributesCreatorParent->getCurrentTemplateAC()->toogleAttribute(myAttrProperties.getAttr(), true);
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
            myAttributesCreatorParent->getCurrentTemplateAC()->toogleAttribute(myAttrProperties.getAttr(), false);
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
        // change color of text field depending of myCurrentValueValid
        if (myAttributesCreatorParent->getCurrentTemplateAC()->isValid(myAttrProperties.getAttr(), myValueComboBox->getText().text())) {
            // check color depending if is a default value
            if (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueComboBox->getText().text())) {
                myValueComboBox->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueComboBox->killFocus();
            }
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myValueComboBox->getText().text());
            // special case for trigger stops (in the future will be changed)
            if (myAttributesCreatorParent->getCurrentTemplateAC()->getTagProperty().isStop() && (myAttrProperties.getAttr() == SUMO_ATTR_TRIGGERED)) {
                // refresh entire AttributesCreator
                myAttributesCreatorParent->refreshAttributesCreator();
            }
        } else {
            // if value of TextField isn't valid, change their color to Red
            myValueComboBox->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myValueTextField) {
        // change color of text field depending of myCurrentValueValid
        if (myAttributesCreatorParent->getCurrentTemplateAC()->isValid(myAttrProperties.getAttr(), myValueTextField->getText().text())) {
            // check color depending if is a default value
            if (myAttrProperties.hasDefaultValue() && (myAttrProperties.getDefaultValue() == myValueTextField->getText().text())) {
                myValueTextField->setTextColor(FXRGB(128, 128, 128));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
            }
            myValueTextField->killFocus();
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), myValueTextField->getText().text());
        } else {
            // if value of TextField isn't valid, change their color to Red
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
        }
    }
    // Update row
    update();
    return 1;
}


long
GNEFrameAttributeModules::AttributesCreatorRow::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // continue depending of attribute
    if (myAttrProperties.getAttr() == SUMO_ATTR_COLOR) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
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
    } else if ((myAttrProperties.getAttr() == SUMO_ATTR_ALLOW) || (myAttrProperties.getAttr() == SUMO_ATTR_DISALLOW)) {
        // get allow string
        std::string allow = myValueTextField->getText().text();
        // get accept changes
        bool acceptChanges = false;
        // opena allowDisallow dialog
        GNEAllowDisallow(myAttributesCreatorParent->getFrameParent()->getViewNet(), &allow, &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            /// @brief Constructor (For string
            myValueTextField->setText(allow.c_str(), TRUE);
        }
    }
    return 0;
}


std::string
GNEFrameAttributeModules::AttributesCreatorRow::generateID() const {
    // get attribute carriers
    const auto& GNEAttributeCarriers = myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
    // continue depending of type
    if (myAttrProperties.getTagPropertyParent().isShape()) {
        return GNEAttributeCarriers->generateShapeID(myAttrProperties.getTagPropertyParent().getXMLTag());
    } else if (myAttrProperties.getTagPropertyParent().isAdditionalElement()) {
        return GNEAttributeCarriers->generateAdditionalID(myAttrProperties.getTagPropertyParent().getTag());
    } else if (myAttrProperties.getTagPropertyParent().isDemandElement()) {
        return GNEAttributeCarriers->generateDemandElementID(myAttrProperties.getTagPropertyParent().getTag());
    } else {
        return "";
    }
}


bool
GNEFrameAttributeModules::AttributesCreatorRow::isValidID() const {
    if (myAttrProperties.getTagPropertyParent().isAdditionalElement()) {
        return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveAdditional(
                    myAttrProperties.getTagPropertyParent().getTag(), myValueTextField->getText().text(), false) == nullptr);
    } else if (myAttrProperties.getTagPropertyParent().isDemandElement()) {
        return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveAdditional(
                    myAttrProperties.getTagPropertyParent().getTag(), myValueTextField->getText().text(), false) == nullptr);
    } else {
        throw ProcessError("Unsuported additional ID");
    }
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesCreator - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesCreator::AttributesCreator(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->myContentFrame, "Internal attributes"),
    myFrameParent(frameParent),
    myTemplateAC(nullptr) {
    // resize myAttributesCreatorRows
    myAttributesCreatorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myFlowEditor
    myFlowEditor = new FlowEditor(frameParent->getViewNet(), frameParent->myContentFrame);
    // create reset and help button
    myFrameButtons = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    myResetButton = new FXButton(myFrameButtons, "", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
    new FXButton(myFrameButtons, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrameAttributeModules::AttributesCreator::~AttributesCreator() {}


void
GNEFrameAttributeModules::AttributesCreator::showAttributesCreatorModule(GNEAttributeCarrier* templateAC, const std::vector<SumoXMLAttr>& hiddenAttributes) {
    // destroy all rows
    for (auto& row : myAttributesCreatorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    if (templateAC) {
        // set current template AC and hidden attributes
        myTemplateAC = templateAC;
        myHiddenAttributes = hiddenAttributes;
        // refresh rows (new rows will be created)
        refreshRows(true);
        // enable reset
        myResetButton->enable();
        // show
        show();
    } else {
        throw ProcessError("invalid templateAC in showAttributesCreatorModule");
    }
}


void
GNEFrameAttributeModules::AttributesCreator::hideAttributesCreatorModule() {
    // hide attributes creator flow
    myFlowEditor->hideFlowEditor();
    // hide modul
    hide();
}


GNEFrame*
GNEFrameAttributeModules::AttributesCreator::getFrameParent() const {
    return myFrameParent;
}


void
GNEFrameAttributeModules::AttributesCreator::getAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, bool includeAll) const {
    // get standard parameters
    for (const auto& row : myAttributesCreatorRows) {
        if (row && row->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            const auto& attrProperties = row->getAttrProperties();
            // flag for row enabled
            const bool rowEnabled = row->isAttributesCreatorRowEnabled();
            // flag for default attributes
            const bool hasDefaultStaticValue = !attrProperties.hasDefaultValue() || (attrProperties.getDefaultValue() != row->getValue());
            // flag for enablitables attributes
            const bool isFlowDefinitionAttribute = attrProperties.isFlowDefinition();
            // flag for Terminatel attributes
            const bool isActivatableAttribute = attrProperties.isActivatable() && row->getAttributeCheckButtonCheck();
            // check if flags configuration allow to include values
            if (rowEnabled && (includeAll || hasDefaultStaticValue || isFlowDefinitionAttribute || isActivatableAttribute)) {
                // add attribute depending of type
                if (attrProperties.isInt()) {
                    const int intValue = GNEAttributeCarrier::canParse<int>(row->getValue()) ? GNEAttributeCarrier::parse<int>(row->getValue()) : GNEAttributeCarrier::parse<int>(attrProperties.getDefaultValue());
                    baseObject->addIntAttribute(attrProperties.getAttr(), intValue);
                } else if (attrProperties.isFloat()) {
                    const double doubleValue = GNEAttributeCarrier::canParse<double>(row->getValue()) ? GNEAttributeCarrier::parse<double>(row->getValue()) : GNEAttributeCarrier::parse<double>(attrProperties.getDefaultValue());
                    baseObject->addDoubleAttribute(attrProperties.getAttr(), doubleValue);
                } else if (attrProperties.isBool()) {
                    const bool boolValue = GNEAttributeCarrier::canParse<bool>(row->getValue()) ? GNEAttributeCarrier::parse<bool>(row->getValue()) : GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue());
                    baseObject->addBoolAttribute(attrProperties.getAttr(), boolValue);
                } else if (attrProperties.isposition()) {
                    const Position positionValue = GNEAttributeCarrier::canParse<Position>(row->getValue()) ? GNEAttributeCarrier::parse<Position>(row->getValue()) : GNEAttributeCarrier::parse<Position>(attrProperties.getDefaultValue());
                    baseObject->addPositionAttribute(attrProperties.getAttr(), positionValue);
                } else if (attrProperties.isSUMOTime()) {
                    const SUMOTime timeValue = GNEAttributeCarrier::canParse<SUMOTime>(row->getValue()) ? GNEAttributeCarrier::parse<SUMOTime>(row->getValue()) : GNEAttributeCarrier::parse<SUMOTime>(attrProperties.getDefaultValue());
                    baseObject->addTimeAttribute(attrProperties.getAttr(), timeValue);
                } else if (attrProperties.isColor()) {
                    const RGBColor colorValue = GNEAttributeCarrier::canParse<RGBColor>(row->getValue()) ? GNEAttributeCarrier::parse<RGBColor>(row->getValue()) : GNEAttributeCarrier::parse<RGBColor>(attrProperties.getDefaultValue());
                    baseObject->addColorAttribute(attrProperties.getAttr(), colorValue);
                } else if (attrProperties.isList()) {
                    if (attrProperties.isposition()) {
                        const PositionVector positionVectorValue = GNEAttributeCarrier::canParse<PositionVector>(row->getValue()) ? GNEAttributeCarrier::parse<PositionVector>(row->getValue()) : GNEAttributeCarrier::parse<PositionVector>(attrProperties.getDefaultValue());
                        baseObject->addPositionVectorAttribute(attrProperties.getAttr(), positionVectorValue);
                    } else {
                        const std::vector<std::string> stringVectorValue = GNEAttributeCarrier::canParse<std::vector<std::string> >(row->getValue()) ? GNEAttributeCarrier::parse<std::vector<std::string> >(row->getValue()) : GNEAttributeCarrier::parse<std::vector<std::string> >(attrProperties.getDefaultValue());
                        baseObject->addStringListAttribute(attrProperties.getAttr(), stringVectorValue);
                    }
                } else {
                    baseObject->addStringAttribute(attrProperties.getAttr(), row->getValue());
                }
            }
        }
    }
    // add extra flow attributes (only will updated if myFlowEditor is shown)
    if (myFlowEditor->shownFlowEditor()) {
        myFlowEditor->getFlowAttributes(baseObject);
    }
}


GNEAttributeCarrier*
GNEFrameAttributeModules::AttributesCreator::getCurrentTemplateAC() const {
    return myTemplateAC;
}


void
GNEFrameAttributeModules::AttributesCreator::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + myTemplateAC->getTagProperty().getTagStr();
    } else {
        errorMessage = "Invalid input parameter of " + myTemplateAC->getTagProperty().getTagStr() + ": " + extra;
    }
    // set message in status bar
    myFrameParent->myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


void
GNEFrameAttributeModules::AttributesCreator::refreshAttributesCreator() {
    // just refresh row without creating new rows
    if (shown() && myTemplateAC) {
        refreshRows(false);
    }
}


void
GNEFrameAttributeModules::AttributesCreator::disableAttributesCreator() {
    // disable all rows
    for (const auto& row : myAttributesCreatorRows) {
        if (row) {
            row->disableRow();
        }
    }
    // also disable reset
    myResetButton->disable();
}


bool
GNEFrameAttributeModules::AttributesCreator::areValuesValid() const {
    // iterate over standar parameters
    for (const auto& attribute : myTemplateAC->getTagProperty()) {
        // Return false if error message of attriuve isn't empty
        if (myAttributesCreatorRows.at(attribute.getPositionListed()) && !myAttributesCreatorRows.at(attribute.getPositionListed())->isAttributeValid()) {
            return false;
        }
    }
    // check flow attributes
    if (myFlowEditor->shownFlowEditor()) {
        return myFlowEditor->areFlowValuesValid();
    }
    return true;
}


long
GNEFrameAttributeModules::AttributesCreator::onCmdReset(FXObject*, FXSelector, void*) {
    if (myTemplateAC) {
        myTemplateAC->resetDefaultValues();
        refreshRows(false);
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesCreator::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTemplateAC);
    return 1;
}


void
GNEFrameAttributeModules::AttributesCreator::refreshRows(const bool createRows) {
    // declare a flag to show Flow editor
    bool showFlowEditor = false;
    // iterate over tag attributes and create AttributesCreatorRows for every attribute
    for (const auto& attribute : myTemplateAC->getTagProperty()) {
        // declare falg to check conditions for show attribute
        bool showAttribute = true;
        // check that only non-unique attributes (except ID) are created (And depending of includeExtendedAttributes)
        if (attribute.isUnique() && (attribute.getAttr() != SUMO_ATTR_ID)) {
            showAttribute = false;
        }
        // check if attribute must stay hidden
        if (std::find(myHiddenAttributes.begin(), myHiddenAttributes.end(), attribute.getAttr()) != myHiddenAttributes.end()) {
            showAttribute = false;
        }
        // check if attribute is a flow definitionattribute
        if (attribute.isFlowDefinition()) {
            showAttribute = false;
            showFlowEditor = true;
        }
        // check special case for vaporizer IDs
        if ((attribute.getAttr() == SUMO_ATTR_ID) && (attribute.getTagPropertyParent().getTag() == SUMO_TAG_VAPORIZER)) {
            showAttribute = false;
        }
        // check special case for VType IDs in vehicle Frame
        if ((attribute.getAttr() == SUMO_ATTR_TYPE) && (myFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) &&
                (myFrameParent->getViewNet()->getEditModes().demandEditMode == DemandEditMode::DEMAND_VEHICLE)) {
            showAttribute = false;
        }
        // show attribute depending of showAttribute flag
        if (showAttribute) {
            // check if we have to create a new row
            if (createRows) {
                myAttributesCreatorRows.at(attribute.getPositionListed()) = new AttributesCreatorRow(this, attribute);
            } else {
                myAttributesCreatorRows.at(attribute.getPositionListed())->refreshRow();
            }
        }
    }
    // reparent help button (to place it at bottom)
    myFrameButtons->reparent(getCollapsableFrame());
    // recalc
    recalc();
    // check if flow editor has to be shown
    if (showFlowEditor) {
        myFlowEditor->showFlowEditor({myTemplateAC});
    } else {
        myFlowEditor->hideFlowEditor();
    }
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::FlowEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::FlowEditor::FlowEditor(GNEViewNet* viewNet, FXVerticalFrame* contentFrame) :
    FXGroupBoxModule(contentFrame, "Flow attributes"),
    myViewNet(viewNet) {
    // create comboBox for option A
    FXHorizontalFrame* auxiliarHorizontalFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(auxiliarHorizontalFrame, "terminate", nullptr, GUIDesignLabelAttribute);
    myTerminateComboBox = new FXComboBox(auxiliarHorizontalFrame, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create comboBox for spacing
    mySpacingFrameComboBox = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(mySpacingFrameComboBox, "spacing", nullptr, GUIDesignLabelAttribute);
    mySpacingComboBox = new FXComboBox(mySpacingFrameComboBox, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create textField for option A
    myTerminateFrameTextField = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    myTerminateLabel = new FXLabel(myTerminateFrameTextField, "A", nullptr, GUIDesignLabelAttribute);
    myTerminateTextField = new FXTextField(myTerminateFrameTextField, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for spacing
    mySpacingFrameTextField = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    mySpacingLabel = new FXLabel(mySpacingFrameTextField, "B", nullptr, GUIDesignLabelAttribute);
    mySpacingTextField = new FXTextField(mySpacingFrameTextField, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill terminate
    myTerminateComboBox->appendItem(toString(SUMO_ATTR_END).c_str());
    myTerminateComboBox->appendItem(toString(SUMO_ATTR_NUMBER).c_str());
    myTerminateComboBox->appendItem((toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER)).c_str());
    myTerminateComboBox->setNumVisible(3);
    // fill comboBox B
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_VEHSPERHOUR).c_str());
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_PERIOD).c_str());
    mySpacingComboBox->appendItem(toString(SUMO_ATTR_PROB).c_str());
    mySpacingComboBox->setNumVisible(3);
}


GNEFrameAttributeModules::FlowEditor::~FlowEditor() {}


void
GNEFrameAttributeModules::FlowEditor::showFlowEditor(const std::vector<GNEAttributeCarrier*> editedFlows) {
    // update flows
    myEditedFlows = editedFlows;
    // check number of flows
    if (myEditedFlows.size() > 0) {
        // update per hour attr
        if (myEditedFlows.front()->getTagProperty().hasAttribute(SUMO_ATTR_PERSONSPERHOUR)) {
            myPerHourAttr = SUMO_ATTR_PERSONSPERHOUR;
        } else if (myEditedFlows.front()->getTagProperty().hasAttribute(SUMO_ATTR_CONTAINERSPERHOUR)) {
            myPerHourAttr = SUMO_ATTR_CONTAINERSPERHOUR;
        } else {
            myPerHourAttr = SUMO_ATTR_VEHSPERHOUR;
        }
        // clear and update comboBoxB
        mySpacingComboBox->clearItems();
        mySpacingComboBox->appendItem(toString(myPerHourAttr).c_str());
        mySpacingComboBox->appendItem(toString(SUMO_ATTR_PERIOD).c_str());
        mySpacingComboBox->appendItem(toString(SUMO_ATTR_PROB).c_str());
        mySpacingComboBox->setNumVisible(3);
        // refresh
        refreshFlowEditor();
        // show
        show();
    }
}


void
GNEFrameAttributeModules::FlowEditor::hideFlowEditor() {
    hide();
}


bool
GNEFrameAttributeModules::FlowEditor::shownFlowEditor() const {
    return shown();
}


void
GNEFrameAttributeModules::FlowEditor::refreshFlowEditor() {
    // show both attributes
    myTerminateFrameTextField->show();
    mySpacingFrameTextField->show();
    // continue depending of number of flow
    if (myEditedFlows.size() == 1) {
        refreshSingleFlow();
    } else if (myEditedFlows.size() > 1) {
        refreshMultipleFlows();
    }
    // recalc
    recalc();
}


void
GNEFrameAttributeModules::FlowEditor::getFlowAttributes(CommonXMLStructure::SumoBaseObject* baseObject) {
    // case end-number
    if (myTerminateLabel->getText().text() == toString(SUMO_ATTR_END)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_END, GNEAttributeCarrier::parse<double>(myTerminateTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_NUMBER)) {
        baseObject->addIntAttribute(SUMO_ATTR_NUMBER, GNEAttributeCarrier::parse<int>(mySpacingTextField->getText().text()));
    }
    // other cases
    if (myTerminateLabel->getText().text() == toString(SUMO_ATTR_NUMBER)) {
        baseObject->addIntAttribute(SUMO_ATTR_NUMBER, GNEAttributeCarrier::parse<int>(myTerminateTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(myPerHourAttr)) {
        baseObject->addDoubleAttribute(myPerHourAttr, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_PERIOD)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PERIOD, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
    if (mySpacingLabel->getText().text() == toString(SUMO_ATTR_PROB)) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PROB, GNEAttributeCarrier::parse<double>(mySpacingTextField->getText().text()));
    }
}


bool
GNEFrameAttributeModules::FlowEditor::areFlowValuesValid() const {
    // check text fields
    if (myTerminateFrameTextField->shown() && (myTerminateTextField->getTextColor() == FXRGB(0, 0, 0)) &&
        mySpacingFrameTextField->shown() && (mySpacingTextField->getTextColor() == FXRGB(0, 0, 0))) {
        return true;
    } else {
        return false;
    }
}


long
GNEFrameAttributeModules::FlowEditor::onCmdSetFlowAttribute(FXObject* obj, FXSelector, void*) {
    // check number of flows
    if (myEditedFlows.front()) {
        // declare vectors for enable/disable attributes
        std::vector<SumoXMLAttr> enableAttrs, disableAttrs;
        // check if all spacing attributes are disabled
        const bool spacingEnabled = myEditedFlows.front()->isAttributeEnabled(myPerHourAttr) || 
                                    myEditedFlows.front()->isAttributeEnabled(SUMO_ATTR_PERIOD) || 
                                    myEditedFlows.front()->isAttributeEnabled(SUMO_ATTR_PROB);
        // get terminate attribute
        SumoXMLAttr terminateAttribute = SUMO_ATTR_NOTHING;
        if (myTerminateComboBox->getText().text() == toString(SUMO_ATTR_END)) {
            terminateAttribute = SUMO_ATTR_END;
        } else if (myTerminateComboBox->getText().text() == toString(SUMO_ATTR_NUMBER)) {
            terminateAttribute = SUMO_ATTR_NUMBER;
        }
        // get spacing attribute
        SumoXMLAttr spacingAttribute = SUMO_ATTR_NOTHING;
        if (myTerminateComboBox->getText().text() == (toString(SUMO_ATTR_END) + "-" + toString(SUMO_ATTR_NUMBER))) {
            spacingAttribute = SUMO_ATTR_NUMBER;
        } else if (mySpacingComboBox->getText().text() == toString(myPerHourAttr)) {
            spacingAttribute = myPerHourAttr;
        } else if (mySpacingComboBox->getText().text() == toString(SUMO_ATTR_PERIOD)) {
            spacingAttribute = SUMO_ATTR_PERIOD;
        } else if (mySpacingComboBox->getText().text() == toString(SUMO_ATTR_PROB)) {
            spacingAttribute = SUMO_ATTR_PROB;
        }
        // check if obj is a comboBox or a text field
        if (obj == myTerminateComboBox) {
            if (terminateAttribute == SUMO_ATTR_END) {
                enableAttrs.push_back(SUMO_ATTR_END);
                disableAttrs.push_back(SUMO_ATTR_NUMBER);
                // at least enable one spacing attribute
                if (!spacingEnabled) {
                    enableAttrs.push_back(myPerHourAttr);
                }
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else if (terminateAttribute == SUMO_ATTR_NUMBER) {
                disableAttrs.push_back(SUMO_ATTR_END);
                enableAttrs.push_back(SUMO_ATTR_NUMBER);
                // at least enable one spacing attribute
                if (!spacingEnabled) {
                    enableAttrs.push_back(myPerHourAttr);
                }
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else if (spacingAttribute == SUMO_ATTR_NUMBER) {
                enableAttrs.push_back(SUMO_ATTR_END);
                enableAttrs.push_back(SUMO_ATTR_NUMBER);
                // disable others
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                myTerminateComboBox->setTextColor(FXRGB(0, 0, 0));
                myTerminateComboBox->killFocus();
            } else {
                // disable both
                disableAttrs.push_back(SUMO_ATTR_END);
                disableAttrs.push_back(SUMO_ATTR_NUMBER);
                // set invalid color
                myTerminateComboBox->setTextColor(FXRGB(255, 0, 0));
            }
        } else if (obj == mySpacingComboBox) {
            if (spacingAttribute == myPerHourAttr) {
                enableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else if (spacingAttribute == SUMO_ATTR_PERIOD) {
                disableAttrs.push_back(myPerHourAttr);
                enableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else if (spacingAttribute == SUMO_ATTR_PROB) {
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                enableAttrs.push_back(SUMO_ATTR_PROB);
                // reset color
                mySpacingComboBox->setTextColor(FXRGB(0, 0, 0));
                mySpacingComboBox->killFocus();
            } else {
                // disable all
                disableAttrs.push_back(myPerHourAttr);
                disableAttrs.push_back(SUMO_ATTR_PERIOD);
                disableAttrs.push_back(SUMO_ATTR_PROB);
                // set invalid color
                mySpacingComboBox->setTextColor(FXRGB(255, 0, 0));
            }
        } else if ((obj == myTerminateTextField) && (terminateAttribute != SUMO_ATTR_NOTHING)) {
            if (myEditedFlows.front()->isValid(terminateAttribute, myTerminateTextField->getText().text())) {
                // continue depending of flow
                if (myEditedFlows.front()->isTemplate()) {
                    // change attribute directly
                    myEditedFlows.front()->setAttribute(terminateAttribute, myTerminateTextField->getText().text());
                } else if (myEditedFlows.size() == 1) {
                    // change using undoList
                    myEditedFlows.front()->setAttribute(terminateAttribute, myTerminateTextField->getText().text(), myViewNet->getUndoList());
                } else {
                    // change all flows using undoList
                    myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), "change multiple flow attributes");
                    for (const auto &flow : myEditedFlows) {
                        flow->setAttribute(terminateAttribute, myTerminateTextField->getText().text(), myViewNet->getUndoList());
                    }
                    myViewNet->getUndoList()->end();
                }
                // reset color
                myTerminateTextField->setTextColor(FXRGB(0, 0, 0));
                myTerminateTextField->killFocus();
            } else {
                // set invalid color
                myTerminateTextField->setTextColor(FXRGB(255, 0, 0));
            }
        } else if ((obj == mySpacingTextField) && (spacingAttribute != SUMO_ATTR_NOTHING)) {
            if (myEditedFlows.front()->isValid(spacingAttribute, mySpacingTextField->getText().text())) {
                // continue depending of flow
                if (myEditedFlows.front()->isTemplate()) {
                    // change attribute directly
                    myEditedFlows.front()->setAttribute(spacingAttribute, mySpacingTextField->getText().text());
                } else if (myEditedFlows.size() == 1) {
                    // change using undoList
                    myEditedFlows.front()->setAttribute(spacingAttribute, mySpacingTextField->getText().text(), myViewNet->getUndoList());
                } else {
                    // change all flows using undoList
                    myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), "change multiple flow attributes");
                    for (const auto &flow : myEditedFlows) {
                        flow->setAttribute(spacingAttribute, mySpacingTextField->getText().text(), myViewNet->getUndoList());
                    }
                    myViewNet->getUndoList()->end();
                }
                // reset color
                mySpacingTextField->setTextColor(FXRGB(0, 0, 0));
                mySpacingTextField->killFocus();
            } else {
                // set invalid color
                mySpacingTextField->setTextColor(FXRGB(255, 0, 0));
            }
        }
        // enable and disable attributes
        for (const auto &attr : enableAttrs) {
            if (myEditedFlows.front()->isTemplate()) {
                // enable directly
                myEditedFlows.front()->toogleAttribute(attr, true);
            } else if (myEditedFlows.size() == 1) {
                // enable using undoList
                myEditedFlows.front()->enableAttribute(attr, myViewNet->getUndoList());
            } else {
                // enable in all flow using undoList
                myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), "enable multiple flow attributes");
                for (const auto &flow : myEditedFlows) {
                    flow->enableAttribute(attr, myViewNet->getUndoList());
                }
                myViewNet->getUndoList()->end();
            }
        }
        for (const auto &attr : disableAttrs) {
            if (myEditedFlows.front()->isTemplate()) {
                // disable directly
                myEditedFlows.front()->toogleAttribute(attr, false);
            } else if (myEditedFlows.size() == 1) {
                // disable using undoList
                myEditedFlows.front()->disableAttribute(attr, myViewNet->getUndoList());
            } else {
                // disable in all flow using undoList
                myViewNet->getUndoList()->begin(myEditedFlows.front()->getTagProperty().getGUIIcon(), "disable multiple flow attributes");
                for (const auto &flow : myEditedFlows) {
                    flow->disableAttribute(attr, myViewNet->getUndoList());
                }
                myViewNet->getUndoList()->end();
            }
        }
        // refresh attribute creator
        refreshFlowEditor();
    }
    return 1;
}


void
GNEFrameAttributeModules::FlowEditor::refreshSingleFlow() {
    // get flow (only for code legibly)
    const auto flow = myEditedFlows.front();
    // continue depending of combinations
    if (flow->isAttributeEnabled(SUMO_ATTR_END) && flow->isAttributeEnabled(SUMO_ATTR_NUMBER)) {
        // set first comboBox
        myTerminateComboBox->setCurrentItem(2),
        // hide second comboBox
        mySpacingFrameComboBox->hide();
        // set label
        myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
        mySpacingLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
        // set text fields
        myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
    } else {
        // show second comboBox
        mySpacingFrameComboBox->show();
        // set first attribute
        if (myTerminateComboBox->getTextColor() == FXRGB(255, 0, 0)) { 
            // invalid combination, disable text field
            myTerminateFrameTextField->hide();
        } else if (flow->isAttributeEnabled(SUMO_ATTR_END)) {
            // set first comboBox
            myTerminateComboBox->setCurrentItem(0),
            // set label
            myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
            // set text fields
            myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_NUMBER)) {
            // set first comboBox
            myTerminateComboBox->setCurrentItem(1),
            // set label
            myTerminateLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
            // set text fields
            myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
        }
        // set second attribute
        if (mySpacingComboBox->getTextColor() == FXRGB(255, 0, 0)) {
            // invalid combination, disable text field
            mySpacingFrameTextField->hide();
        } else if (flow->isAttributeEnabled(myPerHourAttr)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(0),
            // set label
            mySpacingLabel->setText(toString(myPerHourAttr).c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(myPerHourAttr).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_PERIOD)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(1),
            // set label
            mySpacingLabel->setText(toString(SUMO_ATTR_PERIOD).c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PERIOD).c_str());
        } else if (flow->isAttributeEnabled(SUMO_ATTR_PROB)) {
            // set first comboBox
            mySpacingComboBox->setCurrentItem(2),
            // set label
            mySpacingLabel->setText(toString(SUMO_ATTR_PROB).c_str());
            // set text fields
            mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PROB).c_str());
        }
    }
}


void 
GNEFrameAttributeModules::FlowEditor::refreshMultipleFlows() {
    // get first flow (only for code legibly)
    const auto flow = myEditedFlows.front();
    // get values of first flow
    const bool end = flow->isAttributeEnabled(SUMO_ATTR_END);
    const bool number = flow->isAttributeEnabled(SUMO_ATTR_NUMBER);
    const bool perhour = flow->isAttributeEnabled(myPerHourAttr);
    const bool period = flow->isAttributeEnabled(SUMO_ATTR_PERIOD);
    const bool probability = flow->isAttributeEnabled(SUMO_ATTR_PROB);
    // we need to check if attributes are defined differents in flows
    std::vector<std::string> terminateDifferent;
    std::vector<std::string> spacingDifferent;
    // iterate over all flows
    for (const auto &flow : myEditedFlows) {
        if (flow->isAttributeEnabled(SUMO_ATTR_END) != end) {
            terminateDifferent.push_back(toString(SUMO_ATTR_END));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_NUMBER) != number) {
            terminateDifferent.push_back(toString(SUMO_ATTR_NUMBER));
        }
        if (flow->isAttributeEnabled(myPerHourAttr) != perhour) {
            spacingDifferent.push_back(toString(myPerHourAttr));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_PERIOD) != period) {
            spacingDifferent.push_back(toString(SUMO_ATTR_PERIOD));
        }
        if (flow->isAttributeEnabled(SUMO_ATTR_PROB) != probability) {
            spacingDifferent.push_back(toString(SUMO_ATTR_PROB));
        }
    }
    // special case for end and number
    if (end && number && terminateDifferent.empty() && spacingDifferent.empty()) {
        // set first comboBox
        myTerminateComboBox->setCurrentItem(2),
        // hide second comboBox
        mySpacingFrameComboBox->hide();
        // set label
        myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
        mySpacingLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
        // set text fields
        myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
        mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
    } else {
        // show second comboBox
        mySpacingFrameComboBox->show();
        // check terminateDifferent
        if (terminateDifferent.size() > 0) {
            myTerminateComboBox->setText(("different: " + terminateDifferent.front() + " " + terminateDifferent.back()).c_str());
            // hide textField
            myTerminateFrameTextField->hide();
        } else {
            // show textField
            myTerminateFrameTextField->show();
            // set first attribute
            if (myTerminateComboBox->getTextColor() == FXRGB(255, 0, 0)) { 
                // invalid combination, disable text field
                myTerminateFrameTextField->hide();
            } else if (end) {
                // set first comboBox
                myTerminateComboBox->setCurrentItem(0),
                // set label
                myTerminateLabel->setText(toString(SUMO_ATTR_END).c_str());
                // set text fields
                myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_END).c_str());
            } else if (number) {
                // set first comboBox
                myTerminateComboBox->setCurrentItem(1),
                // set label
                myTerminateLabel->setText(toString(SUMO_ATTR_NUMBER).c_str());
                // set text fields
                myTerminateTextField->setText(getFlowAttribute(SUMO_ATTR_NUMBER).c_str());
            }
        }
        // check terminateDifferent
        if (spacingDifferent.size() > 0) {
            mySpacingComboBox->setText(("different: " + spacingDifferent.front() + " " + spacingDifferent.back()).c_str());
            // hide textField
            mySpacingFrameTextField->hide();
        } else {
            // show textField
            mySpacingFrameTextField->show();
            // set second attribute
            if (mySpacingComboBox->getTextColor() == FXRGB(255, 0, 0)) {
                // invalid combination, disable text field
                mySpacingFrameTextField->hide();
            } else if (perhour) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(0),
                // set label
                mySpacingLabel->setText(toString(myPerHourAttr).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(myPerHourAttr).c_str());
            } else if (period) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(1),
                // set label
                mySpacingLabel->setText(toString(SUMO_ATTR_PERIOD).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PERIOD).c_str());
            } else if (probability) {
                // set first comboBox
                mySpacingComboBox->setCurrentItem(2),
                // set label
                mySpacingLabel->setText(toString(SUMO_ATTR_PROB).c_str());
                // set text fields
                mySpacingTextField->setText(getFlowAttribute(SUMO_ATTR_PROB).c_str());
            }
        }
    }
}


const std::string
GNEFrameAttributeModules::FlowEditor::getFlowAttribute(SumoXMLAttr attr) {
    if (myEditedFlows.size() == 1) {
        return myEditedFlows.front()->getAttribute(attr);
    } else {
        std::string solution;
        std::set<std::string> values;
        // extract all values (avoiding duplicated)
        for (const auto &flow : myEditedFlows) {
            values.insert(flow->getAttribute(attr));
        }
        // insert value and space
        for (const auto &value : values) {
            solution.append(value + " ");
        }
        // remove last space
        if (solution.size() > 0) {
            solution.pop_back();
        }
        return solution;
    }
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow(GNEFrameAttributeModules::AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr,
        const std::string& value, const bool attributeEnabled, const bool computed) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr),
    myMultiple(GNEAttributeCarrier::parse<std::vector<std::string>>(value).size() > 1) {
    // Create and hide label
    myAttributeLabel = new FXLabel(this, "attributeLabel", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    // Create and hide check button
    myAttributeCheckButton = new FXCheckButton(this, "attributeCheckButton", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myAttributeCheckButton->hide();
    // Create and hide ButtonCombinableChoices
    myAttributeButtonCombinableChoices = new FXButton(this, "attributeButtonCombinableChoices", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeButtonCombinableChoices->hide();
    // create and hidde color editor
    myAttributeColorButton = new FXButton(this, "attributeColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // Create and hide textField for string attributes
    myValueTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    // Create and hide ComboBox
    myValueComboBoxChoices = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBoxChoices->hide();
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
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
        } else {
            myValueTextField->enable();
            myValueComboBoxChoices->enable();
            myValueCheckButton->enable();
        }
        // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
            if (isSupermodeValid(myAttributesEditorParent->getFrameParent()->myViewNet, myACAttr)) {
                myAttributeButtonCombinableChoices->enable();
                myAttributeColorButton->enable();
                myAttributeCheckButton->enable();
            } else {
                myAttributeColorButton->disable();
                myAttributeCheckButton->disable();
                myValueTextField->disable();
                myValueComboBoxChoices->disable();
                myValueCheckButton->disable();
                myAttributeButtonCombinableChoices->disable();
            }
        }
        // set left column
        if (myACAttr.isColor()) {
            // show color button and set color text depending of computed
            if (computed) {
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 255));
            } else {
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
            }
            myAttributeColorButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeColorButton->show();
        } else if ((myACAttr.getAttr() == SUMO_ATTR_ALLOW) || (myACAttr.getAttr() == SUMO_ATTR_DISALLOW)) {
            myAttributeButtonCombinableChoices->setText(myACAttr.getAttrStr().c_str());
            myAttributeButtonCombinableChoices->show();
        } else if (myACAttr.isActivatable()) {
            // show checkbox button and set color text depending of computed
            if (computed) {
                myAttributeCheckButton->setTextColor(FXRGB(0, 0, 255));
            } else {
                myAttributeCheckButton->setTextColor(FXRGB(0, 0, 0));
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
            // iterate over pased booleans comparing all element with the first
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
                }
                myValueTextField->show();
            }
        } else if (myACAttr.isDiscrete()) {
            // Check if are VClasses
            if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isVClasses()) {
                // hide label
                myAttributeLabel->hide();
                // Show button combinable choices
                myAttributeButtonCombinableChoices->setText(myACAttr.getAttrStr().c_str());
                myAttributeButtonCombinableChoices->show();
                // Show string with the values
                myValueTextField->setText(value.c_str());
                // set color depending of computed
                if (computed) {
                    myValueTextField->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueTextField->setTextColor(FXRGB(0, 0, 0));
                }
                myValueTextField->show();
            } else if (!myMultiple) {
                // fill comboBox
                myValueComboBoxChoices->clearItems();
                for (const auto& discreteValue : myACAttr.getDiscreteValues()) {
                    myValueComboBoxChoices->appendItem(discreteValue.c_str());
                }
                // show combo box with values
                myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
                myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
                // set color depending of computed
                if (computed) {
                    myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
                }
                myValueComboBoxChoices->show();
            } else {
                // represent combinable choices in multiple selections always with a textfield instead with a comboBox
                myValueTextField->setText(value.c_str());
                // set color depending of computed
                if (computed) {
                    myValueTextField->setTextColor(FXRGB(0, 0, 255));
                } else {
                    myValueTextField->setTextColor(FXRGB(0, 0, 0));
                }
                myValueTextField->show();
            }
        } else {
            // In any other case (String, list, etc.), show value as String
            myValueTextField->setText(value.c_str());
            // set color depending of computed
            if (computed) {
                myValueTextField->setTextColor(FXRGB(0, 0, 255));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
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
        const bool forceRefresh, const bool attributeEnabled, const bool computed) {
    // start enabling all elements, depending if attribute is enabled
    if (attributeEnabled == false) {
        myValueTextField->disable();
        myValueComboBoxChoices->disable();
        myValueCheckButton->disable();
    } else {
        myValueTextField->enable();
        myValueComboBoxChoices->enable();
        myValueCheckButton->enable();
    }
    // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
    if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
        if (isSupermodeValid(myAttributesEditorParent->getFrameParent()->myViewNet, myACAttr)) {
            myAttributeButtonCombinableChoices->enable();
            myAttributeColorButton->enable();
            myAttributeCheckButton->enable();
        } else {
            myAttributeColorButton->disable();
            myAttributeCheckButton->disable();
            myValueTextField->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
        }
    }
    // set check buton
    if (myAttributeCheckButton->shown()) {
        myAttributeCheckButton->setCheck(attributeEnabled);
    }
    if (myValueTextField->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextField->getTextColor() == FXRGB(0, 0, 0) || myValueTextField->getTextColor() == FXRGB(0, 0, 255) || forceRefresh) {
            myValueTextField->setText(value.c_str());
            // set blue color if is an computed value
            if (computed) {
                myValueTextField->setTextColor(FXRGB(0, 0, 255));
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
            }
        }
    } else if (myValueComboBoxChoices->shown()) {
        // fill terminategain
        myValueComboBoxChoices->clearItems();
        for (const auto& discreteValue : myACAttr.getDiscreteValues()) {
            myValueComboBoxChoices->appendItem(discreteValue.c_str());
        }
        // show combo box with values
        myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
        myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
        // set blue color if is an computed value
        if (computed) {
            myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 255));
        } else {
            myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
        }
        myValueComboBoxChoices->show();
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
           ((myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 0)) || (myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 255)));
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    if (obj == myAttributeColorButton) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
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
            if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->isValid(myACAttr.getAttr(), newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (ACs.size() > 1) {
                    myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->begin(ACs.front()->getTagProperty().getGUIIcon(), "Change multiple attributes");
                }
                // Set new value of attribute in all selected ACs
                for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
                    inspectedAC->setAttribute(myACAttr.getAttr(), newValue, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
                }
                // finish change multiple attributes
                if (ACs.size() > 1) {
                    myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->end();
                }
                // If previously value was incorrect, change font color to black
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
        }
        return 0;
    } else if (obj == myAttributeButtonCombinableChoices) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (ACs.size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->begin(ACs.front()->getTagProperty().getGUIIcon(), "Change multiple attributes");
        }
        // get attribute to modify
        SumoXMLAttr modifiedAttr = myACAttr.getAttr() == SUMO_ATTR_DISALLOW ? SUMO_ATTR_ALLOW : myACAttr.getAttr();
        // declare accept changes
        bool acceptChanges = false;
        // open GNEAllowDisallow (also used to modify SUMO_ATTR_CHANGE_LEFT etc
        GNEAllowDisallow(myAttributesEditorParent->getFrameParent()->myViewNet, myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front(), modifiedAttr, &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            std::string allowed = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAttribute(modifiedAttr);
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
                inspectedAC->setAttribute(modifiedAttr, allowed, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
            }
            // finish change multiple attributes
            if (ACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->end();
            }
            // update frame parent after attribute sucesfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated();
        }
        return 1;
    } else {
        throw ProcessError("Invalid call to onCmdOpenAttributeDialog");
    }
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
        } else if (!myMultiple) {
            // Get value of ComboBox
            newVal = myValueComboBoxChoices->getText().text();
        } else {
            // due this is a multiple selection, obtain value of myValueTextField instead of comboBox
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
                // apply modul
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
    // Check if attribute must be changed
    if ((inspectedACs.size() > 0) && inspectedACs.front()->isValid(myACAttr.getAttr(), newVal)) {
        // check if we're merging junction
        if (!mergeJunction(myACAttr.getAttr(), inspectedACs, newVal)) {
            // if its valid for the first AC than its valid for all (of the same type)
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->begin(inspectedACs.front()->getTagProperty().getGUIIcon(), "Change multiple attributes");
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                // IDs attribute has to be encapsulated
                myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->begin(inspectedACs.front()->getTagProperty().getGUIIcon(), "change " + myACAttr.getTagPropertyParent().getTagStr() + " attribute");
            }
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : inspectedACs) {
                inspectedAC->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
            }
            // finish change multiple attributes or ID Attributes
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->end();
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->end();
            }
            // If previously value was incorrect, change font color to black
            if (myACAttr.isVClasses()) {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
                // in this case, we need to refresh the other values (For example, allow/Disallow objects)
                myAttributesEditorParent->refreshAttributeEditor(false, false);
            } else if (myACAttr.isDiscrete()) {
                myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
                myValueComboBoxChoices->killFocus();
            } else if (myValueTextField != nullptr) {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
            // update frame parent after attribute sucesfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated();
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (myACAttr.isVClasses()) {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            myValueTextField->killFocus();
        } else if (myACAttr.isDiscrete()) {
            myValueComboBoxChoices->setTextColor(FXRGB(255, 0, 0));
            myValueComboBoxChoices->killFocus();
        } else if (myValueTextField != nullptr) {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Value '" + newVal + "' for attribute " + myACAttr.getAttrStr() + " of " + myACAttr.getTagPropertyParent().getTagStr() + " isn't valid");
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList();
    // check if we have to enable or disable
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextField->enable();
        // enable attribute
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "enable attribute '" + myACAttr.getAttrStr() + "'");
        ACs.front()->enableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextField->disable();
        // disable attribute
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "disable attribute '" + myACAttr.getAttrStr() + "'");
        ACs.front()->disableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    }
    return 0;
}


GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow() :
    myAttributesEditorParent(nullptr),
    myMultiple(false) {
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

GNEFrameAttributeModules::AttributesEditor::AttributesEditor(GNEFrame* FrameParent) :
    FXGroupBoxModule(FrameParent->myContentFrame, "Internal attributes"),
    myFrameParent(FrameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myAttributesFlowEditor
    myAttributesEditorFlow = new FlowEditor(FrameParent->getViewNet(), FrameParent->myContentFrame);
    // leave it hidden
    myAttributesEditorFlow->hideFlowEditor();
    // Create help button
    myHelpButton = new FXButton(getCollapsableFrame(), "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
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
            // declare flag to show/hidde atribute
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
                // Declare a set of occuring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occuringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occuringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occuringValues.begin(); values != occuringValues.end(); values++) {
                    if (values != occuringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // overwritte value if attribute is disabled (used by LinkIndex)
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
                // if forceEnablellAttribute is enable, force attributeEnabled (except for ID)
                if (forceAttributeEnabled && (attrProperty.getAttr() != SUMO_ATTR_ID)) {
                    attributeEnabled = true;
                }
                // check if this attribute is computed
                const bool computed = (ACs.size() > 1) ? false : ACs.front()->isAttributeComputed(attrProperty.getAttr());
                // create attribute editor row
                myAttributesEditorRows[attrProperty.getPositionListed()] = new AttributesEditorRow(this, attrProperty, value, attributeEnabled, computed);
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
            // declare flag to show/hidde atribute
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
                // Declare a set of occuring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occuringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occuringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occuringValues.begin(); values != occuringValues.end(); values++) {
                    if (values != occuringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // overwritte value if attribute is disabled (used by LinkIndex)
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
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed);
                } else if ((attrProperty.getAttr()  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                    // Refresh attributes maintain invalid values
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed);
                } else {
                    // Refresh attributes maintain invalid values
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, false, attributeEnabled, computed);
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
    FXGroupBoxModule(frameParent->myContentFrame, "Extended attributes"),
    myFrameParent(frameParent) {
    // Create open dialog button
    new FXButton(getCollapsableFrame(), "Open attributes editor", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
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
    // open AttributesCreator extended dialog
    myFrameParent->attributesEditorExtendedDialogOpened();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::GenericDataAttributes - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::GenericDataAttributes::GenericDataAttributes(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->myContentFrame, "Attributes"),
    myFrameParent(frameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = new FXButton(getCollapsableFrame(), "Edit attributes", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
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
}


const std::map<std::string, std::string>&
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
        // overwritte myTextFieldParameters (to remove duplicated parameters
        myTextFieldParameters->setText(getParametersStr().c_str(), FALSE);
    } else {
        myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::DrawingShape::DrawingShape(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->myContentFrame, "Drawing"),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(getCollapsableFrame(), "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(getCollapsableFrame(), "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(getCollapsableFrame(), "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);
    // create information label
    std::ostringstream information;
    information
            << "- 'Start drawing' or ENTER\n"
            << "  draws shape boundary.\n"
            << "- 'Stop drawing' or ENTER\n"
            << "  creates shape.\n"
            << "- 'Shift + Click'removes\n"
            << "  last created point.\n"
            << "- 'Abort drawing' or ESC\n"
            << "  removes drawed shape.";
    myInformationLabel = new FXLabel(getCollapsableFrame(), information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrameAttributeModules::DrawingShape::~DrawingShape() {}


void GNEFrameAttributeModules::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBoxModule
    FXGroupBoxModule::show();
}


void GNEFrameAttributeModules::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBoxModule
    FXGroupBoxModule::hide();
}


void
GNEFrameAttributeModules::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrameAttributeModules::DrawingShape::stopDrawing() {
    // try to build shape
    if (myFrameParent->shapeDrawed()) {
        // clear created points
        myTemporalShapeShape.clear();
        myFrameParent->myViewNet->updateViewNet();
        // change buttons
        myStartDrawingButton->enable();
        myStopDrawingButton->disable();
        myAbortDrawingButton->disable();
    } else {
        // abort drawing if shape cannot be created
        abortDrawing();
    }
}


void
GNEFrameAttributeModules::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->myViewNet->updateViewNet();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrameAttributeModules::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrameAttributeModules::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrameAttributeModules::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrameAttributeModules::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrameAttributeModules::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrameAttributeModules::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrameAttributeModules::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrameAttributeModules::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrameAttributeModules::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBoxModule(frameParent->myContentFrame, "Netedit attributes"),
    myFrameParent(frameParent),
    myCurrentLengthValid(true),
    myActualAdditionalReferencePoint(AdditionalReferencePoint::LEFT) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    myLengthFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myLengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(myLengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block close polygon and checkBox (By default disabled)
    myCloseShapeFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCloseShapeFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(myCloseShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // Create Frame for center element after creation (By default enabled)
    myCenterViewAfterCreationFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCenterViewAfterCreationFrame, "Center view", 0, GUIDesignLabelAttribute);
    myCenterViewAfterCreationButton = new FXCheckButton(myCenterViewAfterCreationFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myCenterViewAfterCreationButton->setCheck(true);
    // Create help button
    helpReferencePoint = new FXButton(getCollapsableFrame(), "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEFrameAttributeModules::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrameAttributeModules::NeteditAttributes::showNeteditAttributesModule(const GNETagProperties& tagProperty) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if length text field has to be showed
    if (tagProperty.canMaskStartEndPos()) {
        myLengthFrame->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthFrame->hide();
        myReferencePointMatchBox->hide();
    }
    // check if close shape check button has to be show
    if (tagProperty.canCloseShape()) {
        myCloseShapeFrame->show();
        showFrame = true;
    } else {
        myCloseShapeFrame->hide();
    }
    // check if center camera after creation check button has to be show
    if (tagProperty.canCenterCameraAfterCreation()) {
        myCenterViewAfterCreationFrame->show();
        showFrame = true;
    } else {
        myCenterViewAfterCreationFrame->hide();
    }
    // if at least one element is show, show modul
    if (showFrame) {
        recalc();
        show();
    } else {
        hide();
    }
}


void
GNEFrameAttributeModules::NeteditAttributes::hideNeteditAttributesModule() {
    hide();
}


bool
GNEFrameAttributeModules::NeteditAttributes::getNeteditAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, const GNELane* lane) const {
    // check if we need to obtain a start and end position over an edge
    if (myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myFrameParent->myViewNet->snapToActiveGrid(myFrameParent->myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myActualAdditionalReferencePoint == AdditionalReferencePoint::INVALID) {
                std::string errorMessage = "Current selected reference point isn't valid";
                myFrameParent->myViewNet->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain length
                double length = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                baseObject->addDoubleAttribute(SUMO_ATTR_STARTPOS, setStartPosition(mousePositionOverLane, length));
                baseObject->addDoubleAttribute(SUMO_ATTR_ENDPOS, setEndPosition(mousePositionOverLane, length));
            }
        } else {
            return false;
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CLOSE_SHAPE, myCloseShapeCheckButton->getCheck() == 1);
    }
    // check center element after creation
    if (myCenterViewAfterCreationButton->shown()) {
        baseObject->addBoolAttribute(GNE_ATTR_CENTER_AFTER_CREATION, myCenterViewAfterCreationButton->getCheck() == 1);
    }
    // all ok, then return true to continue creating element
    return true;
}


long
GNEFrameAttributeModules::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myCloseShapeCheckButton) {
        if (myCloseShapeCheckButton->getCheck()) {
            myCloseShapeCheckButton->setText("true");
        } else {
            myCloseShapeCheckButton->setText("false");
        }
    } else if (obj == myCenterViewAfterCreationButton) {
        if (myCenterViewAfterCreationButton->getCheck()) {
            myCenterViewAfterCreationButton->setText("true");
        } else {
            myCenterViewAfterCreationButton->setText("false");
        }
    } else if (obj == myLengthTextField) {
        // change color of text field depending of the input length
        if (GNEAttributeCarrier::canParse<double>(myLengthTextField->getText().text()) &&
                GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text()) > 0) {
            myLengthTextField->setTextColor(FXRGB(0, 0, 0));
            myLengthTextField->killFocus();
            myCurrentLengthValid = true;
        } else {
            myLengthTextField->setTextColor(FXRGB(255, 0, 0));
            myCurrentLengthValid = false;
        }
        // Update aditional frame
        update();
    } else if (obj == myReferencePointMatchBox) {
        // Cast actual reference point type
        if (myReferencePointMatchBox->getText() == "reference left") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = AdditionalReferencePoint::LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference right") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = AdditionalReferencePoint::RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference center") {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = AdditionalReferencePoint::CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = AdditionalReferencePoint::INVALID;
            myLengthTextField->disable();
        }
    }

    return 1;
}


long
GNEFrameAttributeModules::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- Referece point: Mark the initial position of the additional element.\n"
            << "  Example: If you want to create a busStop with a length of 30 in the point 100 of the lane:\n"
            << "  - Reference Left will create it with startPos = 70 and endPos = 100.\n"
            << "  - Reference Right will create it with startPos = 100 and endPos = 130.\n"
            << "  - Reference Center will create it with startPos = 85 and endPos = 115.\n"
            << "\n"
            << "- Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "- Center view: if is enabled, view will be center over created element.";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening NeteditAttributes help dialog");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing NeteditAttributes help dialog");
    return 1;
    /**********
    help from PolygonFrame
            << "- Block movement: If enabled, the created polygon element will be blocked. i.e. cannot be moved with\n"
            << "  the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Block shape: If enabled, the shape of created polygon element will be blocked. i.e. their geometry points\n"
            << "  cannot be edited be moved with the mouse. This option can be modified inspecting element.\n"
            << "\n"
            << "- Close shape: If enabled, the created polygon element will be closed. i.e. the last created geometry point\n"
            << "  will be connected with the first geometry point automatically. This option can be modified inspecting element.";

    ****************/
}


double
GNEFrameAttributeModules::NeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
    switch (myActualAdditionalReferencePoint) {
        case AdditionalReferencePoint::LEFT:
            return positionOfTheMouseOverLane;
        case AdditionalReferencePoint::RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case AdditionalReferencePoint::CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEFrameAttributeModules::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
    switch (myActualAdditionalReferencePoint) {
        case AdditionalReferencePoint::LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case AdditionalReferencePoint::RIGHT:
            return positionOfTheMouseOverLane;
        case AdditionalReferencePoint::CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() && (
                AC->getTagProperty().isNetworkElement() ||
                AC->getTagProperty().isAdditionalElement() ||
                AC->getTagProperty().isShape() ||
                AC->getTagProperty().isTAZElement())) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               AC->getTagProperty().isDemandElement()) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeData() &&
               AC->getTagProperty().isDataElement()) {
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr) {
    if (ACAttr.getTagPropertyParent().isNetworkElement() || ACAttr.getTagPropertyParent().isAdditionalElement() ||
            ACAttr.getTagPropertyParent().isShape() || ACAttr.getTagPropertyParent().isTAZElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeNetwork());
    } else if (ACAttr.getTagPropertyParent().isDemandElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeDemand());
    } else if (ACAttr.getTagPropertyParent().isDataElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeData());
    } else {
        return false;
    }
}

/****************************************************************************/
