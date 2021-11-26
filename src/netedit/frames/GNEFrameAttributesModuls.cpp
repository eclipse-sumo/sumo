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
/// @file    GNEFrameAttributesModuls.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Moduls (only for attributes edition)
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
#include "GNEFrameAttributesModuls.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributesModuls::AttributesCreatorRow) RowCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectDialog),
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesCreator) AttributesCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributesModuls::AttributesCreator::onCmdHelp)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesCreatorFlow) AttributesCreatorFlowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::AttributesCreatorFlow::onCmdSetFlowAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BUTTON,   GNEFrameAttributesModuls::AttributesCreatorFlow::onCmdSelectFlowRadioButton),
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,     GNEFrameAttributesModuls::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributesModuls::AttributesEditorRow::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributesModuls::AttributesEditor::onCmdAttributesEditorHelp)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditorFlow) AttributesEditorFlowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::AttributesEditorFlow::onCmdSetFlowAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BUTTON,   GNEFrameAttributesModuls::AttributesEditorFlow::onCmdSelectFlowRadioButton),
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditorExtended) AttributesEditorExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributesModuls::AttributesEditorExtended::onCmdOpenDialog)
};

FXDEFMAP(GNEFrameAttributesModuls::ParametersEditorCreator) ParametersEditorCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributesModuls::ParametersEditorCreator::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::ParametersEditorCreator::onCmdSetParameters)
};

FXDEFMAP(GNEFrameAttributesModuls::DrawingShape) DrawingShapeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STARTDRAWING,   GNEFrameAttributesModuls::DrawingShape::onCmdStartDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_STOPDRAWING,    GNEFrameAttributesModuls::DrawingShape::onCmdStopDrawing),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ABORTDRAWING,   GNEFrameAttributesModuls::DrawingShape::onCmdAbortDrawing)
};

FXDEFMAP(GNEFrameAttributesModuls::NeteditAttributes) NeteditAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEFrameAttributesModuls::NeteditAttributes::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEFrameAttributesModuls::NeteditAttributes::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesCreatorRow,         FXHorizontalFrame,  RowCreatorMap,                  ARRAYNUMBER(RowCreatorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesCreator,            FXGroupBox,         AttributesCreatorMap,           ARRAYNUMBER(AttributesCreatorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesCreatorFlow,        FXGroupBox,         AttributesCreatorFlowMap,       ARRAYNUMBER(AttributesCreatorFlowMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditorRow,          FXHorizontalFrame,  AttributesEditorRowMap,         ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditor,             FXGroupBox,         AttributesEditorMap,            ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditorFlow,         FXGroupBox,         AttributesEditorFlowMap,        ARRAYNUMBER(AttributesEditorFlowMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditorExtended,     FXGroupBox,         AttributesEditorExtendedMap,    ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameAttributesModuls::ParametersEditorCreator,      FXGroupBox,         ParametersEditorCreatorMap,     ARRAYNUMBER(ParametersEditorCreatorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::DrawingShape,                 FXGroupBox,         DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrameAttributesModuls::NeteditAttributes,            FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesCreatorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesCreatorRow::AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeProperties& attrProperties) :
    FXHorizontalFrame(AttributesCreatorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesCreatorParent(AttributesCreatorParent),
    myAttrProperties(attrProperties) {
    // Create left visual elements
    myAttributeLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    myEnableAttributeCheckButton = new FXCheckButton(this, "name", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myEnableAttributeCheckButton->hide();
    myAttributeColorButton = new FXButton(this, "ColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // Create right visual elements
    myValueTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    myValueCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesCreatorRow
        FXHorizontalFrame::create();
        // reset invalid value
        myInvalidValue = "";
        // special case for attribute ID
        if (attrProperties.getAttr() == SUMO_ATTR_ID) {
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
            if (myAttrProperties.isColor()) {
                // show color button
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeColorButton->setText(myAttrProperties.getAttrStr().c_str());
                myAttributeColorButton->show();
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
                // if it's associated to a label button and is disabled, then disable myValueCheckButton
                if (myEnableAttributeCheckButton->shown() && (myEnableAttributeCheckButton->getCheck() == FALSE)) {
                    myValueCheckButton->disable();
                }
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->setText(myAttributesCreatorParent->getCurrentTemplateAC()->getAttribute(myAttrProperties.getAttr()).c_str());
                myValueTextField->show();
                // if it's associated to a label button and is disabled, then disable myValueTextField
                if (myEnableAttributeCheckButton->shown() && (myEnableAttributeCheckButton->getCheck() == FALSE)) {
                    myValueTextField->disable();
                }
            }
        }
        // show AttributesCreatorRow
        show();
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


const GNEAttributeProperties&
GNEFrameAttributesModuls::AttributesCreatorRow::getAttrProperties() const {
    return myAttrProperties;
}


std::string
GNEFrameAttributesModuls::AttributesCreatorRow::getValue() const {
    if (myAttrProperties.isBool()) {
        return (myValueCheckButton->getCheck() == 1) ? "1" : "0";
    } else {
        return myValueTextField->getText().text();
    }
}


bool
GNEFrameAttributesModuls::AttributesCreatorRow::getAttributeCheckButtonCheck() const {
    if (shown()) {
        return myEnableAttributeCheckButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::setAttributeCheckButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myEnableAttributeCheckButton->setCheck(value);
        // enable or disable input fields
        if (value) {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->enable();
            } else {
                myValueTextField->enable();
            }
        } else {
            if (myAttrProperties.isBool()) {
                myValueCheckButton->disable();
            } else {
                myValueTextField->disable();
            }
        }
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::enableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->enable();
    } else {
        return myValueTextField->enable();
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::disableAttributesCreatorRow() {
    if (myAttrProperties.isBool()) {
        return myValueCheckButton->disable();
    } else {
        return myValueTextField->disable();
    }
}


bool
GNEFrameAttributesModuls::AttributesCreatorRow::isAttributesCreatorRowEnabled() const {
    if (!shown()) {
        return false;
    } else if (myAttrProperties.isBool()) {
        return myValueCheckButton->isEnabled();
    } else {
        return myValueTextField->isEnabled();
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::refreshRow() const {
    // currently only row with ID attribute must be updated
    if (myAttrProperties.getAttr() == SUMO_ATTR_ID) {
        myValueTextField->setText(generateID().c_str());
    }
}


bool
GNEFrameAttributesModuls::AttributesCreatorRow::isAttributeValid() const {
    return (myValueTextField->getTextColor() != FXRGB(255, 0, 0));
}


GNEFrameAttributesModuls::AttributesCreator*
GNEFrameAttributesModuls::AttributesCreatorRow::getAttributesCreatorParent() const {
    return myAttributesCreatorParent;
}


long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // check what object was called
    if (obj == myEnableAttributeCheckButton) {
        if (myEnableAttributeCheckButton->getCheck()) {
            // enable text field
            if (myValueTextField->shown()) {
                myValueTextField->enable();
            }
            // enable check button
            if (myValueCheckButton->shown()) {
                myValueCheckButton->enable();
            }
            myAttributesCreatorParent->getCurrentTemplateAC()->toogleAttribute(myAttrProperties.getAttr(), true, -1);
        } else {
            // enable text field
            if (myValueTextField->shown()) {
                myValueTextField->disable();
            }
            // enable check button
            if (myValueCheckButton->shown()) {
                myValueCheckButton->disable();
            }
            myAttributesCreatorParent->getCurrentTemplateAC()->toogleAttribute(myAttrProperties.getAttr(), false, -1);
        }
    } else if (obj == myValueCheckButton) {
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), "true");
        } else {
            myValueCheckButton->setText("false");
            myAttributesCreatorParent->getCurrentTemplateAC()->setAttribute(myAttrProperties.getAttr(), "false");
        }
    } else if (obj == myValueTextField) {
        // change color of text field depending of myCurrentValueValid
        if (myAttributesCreatorParent->getCurrentTemplateAC()->isValid(myAttrProperties.getAttr(), myValueTextField->getText().text())) {
            // check color depending if is a default value
            if (myAttrProperties.hasStaticDefaultValue() && (myAttrProperties.getDefaultValue() == myValueTextField->getText().text())) {
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
    // Update aditional frame
    update();
    return 1;
}


long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectDialog(FXObject*, FXSelector, void*) {
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
        myValueTextField->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(nullptr, 0, nullptr);
    }
    return 0;
}


std::string
GNEFrameAttributesModuls::AttributesCreatorRow::generateID() const {
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
GNEFrameAttributesModuls::AttributesCreatorRow::isValidID() const {
    return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveAdditional(
                myAttrProperties.getTagPropertyParent().getTag(),
                myValueTextField->getText().text(), false) == nullptr);
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesCreator - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesCreator::AttributesCreator(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myTemplateAC(nullptr) {
    // resize myAttributesCreatorRows
    myAttributesCreatorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myAttributesCreatorFlow
    myAttributesCreatorFlow = new AttributesCreatorFlow(this);
    // create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrameAttributesModuls::AttributesCreator::~AttributesCreator() {}


void
GNEFrameAttributesModuls::AttributesCreator::showAttributesCreatorModul(GNEAttributeCarrier *templateAC, const std::vector<SumoXMLAttr>& hiddenAttributes) {
    // set current template
    myTemplateAC = templateAC;
    // continue if myTemplateAC is valid
    if (myTemplateAC) {
        // first destroy all rows
        for (auto& row : myAttributesCreatorRows) {
            // destroy and delete all rows
            if (row != nullptr) {
                row->destroy();
                delete row;
                row = nullptr;
            }
        }
        // now declare a flag to show Flow editor
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
            if (std::find(hiddenAttributes.begin(), hiddenAttributes.end(), attribute.getAttr()) != hiddenAttributes.end()) {
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
                myAttributesCreatorRows.at(attribute.getPositionListed()) = new AttributesCreatorRow(this, attribute);
            }
        }
        // reparent help button (to place it at bottom)
        myHelpButton->reparent(this);
        // recalc
        recalc();
        // check if flow editor has to be shown
        if (showFlowEditor) {
            myAttributesCreatorFlow->showAttributesCreatorFlowModul(
                myTemplateAC->getTagProperty().hasAttribute(SUMO_ATTR_VEHSPERHOUR) || 
                myTemplateAC->getTagProperty().hasAttribute(SUMO_ATTR_PERSONSPERHOUR) ||
                myTemplateAC->getTagProperty().hasAttribute(SUMO_ATTR_CONTAINERSPERHOUR));
        } else {
            myAttributesCreatorFlow->hideAttributesCreatorFlowModul();
        }
        // show
        show();
    } else {
        throw ProcessError("invalid templateAC in showAttributesCreatorModul");
    }
}


void
GNEFrameAttributesModuls::AttributesCreator::hideAttributesCreatorModul() {
    // hide attributes creator flow
    myAttributesCreatorFlow->hideAttributesCreatorFlowModul();
    // hide modul
    hide();
}


GNEFrame*
GNEFrameAttributesModuls::AttributesCreator::getFrameParent() const {
    return myFrameParent;
}


void
GNEFrameAttributesModuls::AttributesCreator::getAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, bool includeAll) const {
    // get standard parameters
    for (const auto& row : myAttributesCreatorRows) {
        if (row && row->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            const auto& attrProperties = row->getAttrProperties();
            // flag for row enabled
            bool rowEnabled = row->isAttributesCreatorRowEnabled();
            // flag for default attributes
            bool hasDefaultStaticValue = !attrProperties.hasStaticDefaultValue() || (attrProperties.getDefaultValue() != row->getValue());
            // flag for enablitables attributes
            bool isFlowDefinitionAttribute = attrProperties.isFlowDefinition();
            // flag for optional attributes
            bool isActivatableAttribute = attrProperties.isActivatable() && row->getAttributeCheckButtonCheck();
            // check if flags configuration allow to include values
            if (rowEnabled && (includeAll || hasDefaultStaticValue || isFlowDefinitionAttribute || isActivatableAttribute)) {
                // add attribute depending of type
                if (attrProperties.isInt()) {
                    const int intValue = GNEAttributeCarrier::canParse<int>(row->getValue())? GNEAttributeCarrier::parse<int>(row->getValue()) : GNEAttributeCarrier::parse<int>(attrProperties.getDefaultValue());
                    baseObject->addIntAttribute(attrProperties.getAttr(), intValue);
                } else if (attrProperties.isFloat()) {
                    const double doubleValue = GNEAttributeCarrier::canParse<double>(row->getValue())? GNEAttributeCarrier::parse<double>(row->getValue()) : GNEAttributeCarrier::parse<double>(attrProperties.getDefaultValue());
                    baseObject->addDoubleAttribute(attrProperties.getAttr(), doubleValue);
                } else if (attrProperties.isBool()) {
                    const bool boolValue = GNEAttributeCarrier::canParse<bool>(row->getValue())? GNEAttributeCarrier::parse<bool>(row->getValue()) : GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue());
                    baseObject->addBoolAttribute(attrProperties.getAttr(), boolValue);
                } else if (attrProperties.isposition()) {
                    const Position positionValue = GNEAttributeCarrier::canParse<Position>(row->getValue())? GNEAttributeCarrier::parse<Position>(row->getValue()) : GNEAttributeCarrier::parse<Position>(attrProperties.getDefaultValue());
                    baseObject->addPositionAttribute(attrProperties.getAttr(), positionValue);
                } else if (attrProperties.isSUMOTime()) {
                    const SUMOTime timeValue = GNEAttributeCarrier::canParse<SUMOTime>(row->getValue())? GNEAttributeCarrier::parse<SUMOTime>(row->getValue()) : GNEAttributeCarrier::parse<SUMOTime>(attrProperties.getDefaultValue());
                    baseObject->addTimeAttribute(attrProperties.getAttr(), timeValue);
                } else if (attrProperties.isColor()) {
                    const RGBColor colorValue = GNEAttributeCarrier::canParse<RGBColor>(row->getValue())? GNEAttributeCarrier::parse<RGBColor>(row->getValue()) : GNEAttributeCarrier::parse<RGBColor>(attrProperties.getDefaultValue());
                    baseObject->addColorAttribute(attrProperties.getAttr(), colorValue);
                } else if (attrProperties.isList()) {
                    if (attrProperties.isposition()) {
                        const PositionVector positionVectorValue = GNEAttributeCarrier::canParse<PositionVector>(row->getValue())? GNEAttributeCarrier::parse<PositionVector>(row->getValue()) : GNEAttributeCarrier::parse<PositionVector>(attrProperties.getDefaultValue());
                        baseObject->addPositionVectorAttribute(attrProperties.getAttr(), positionVectorValue);
                    } else {
                        const std::vector<std::string> stringVectorValue = GNEAttributeCarrier::canParse<std::vector<std::string> >(row->getValue())? GNEAttributeCarrier::parse<std::vector<std::string> >(row->getValue()) : GNEAttributeCarrier::parse<std::vector<std::string> >(attrProperties.getDefaultValue());
                        baseObject->addStringListAttribute(attrProperties.getAttr(), stringVectorValue);
                    }
                } else {
                    baseObject->addStringAttribute(attrProperties.getAttr(), row->getValue());
                }
            }
        }
    }
    // add extra flow attributes (only will updated if myAttributesCreatorFlow is shown)
    myAttributesCreatorFlow->setFlowParameters(baseObject);
}


GNEAttributeCarrier*
GNEFrameAttributesModuls::AttributesCreator::getCurrentTemplateAC() const {
    return myTemplateAC;
}


void
GNEFrameAttributesModuls::AttributesCreator::showWarningMessage(std::string extra) const {
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


bool
GNEFrameAttributesModuls::AttributesCreator::areValuesValid() const {
    // iterate over standar parameters
    for (const auto& attribute : myTemplateAC->getTagProperty()) {
        // Return false if error message of attriuve isn't empty
        if (myAttributesCreatorRows.at(attribute.getPositionListed()) && !myAttributesCreatorRows.at(attribute.getPositionListed())->isAttributeValid()) {
            return false;
        }
    }
    return true;
}


void
GNEFrameAttributesModuls::AttributesCreator::refreshRows() {
    // currently only row with attribute ID must be refresh
    if (myTemplateAC->getTagProperty().hasAttribute(SUMO_ATTR_ID) && (myTemplateAC->getTagProperty().getTag() != SUMO_TAG_VAPORIZER)) {
        myAttributesCreatorRows[myTemplateAC->getTagProperty().getAttributeProperties(SUMO_ATTR_ID).getPositionListed()]->refreshRow();
    }
}

long
GNEFrameAttributesModuls::AttributesCreator::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTemplateAC);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesCreatorFlow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesCreatorFlow::AttributesCreatorFlow(AttributesCreator* attributesCreatorParent) :
    FXGroupBox(attributesCreatorParent->getFrameParent()->myContentFrame, "Flow attributes", GUIDesignGroupBoxFrame),
    myAttributesCreatorParent(attributesCreatorParent),
    myFlowParameters(VEHPARS_END_SET | VEHPARS_NUMBER_SET) {
    // declare auxiliar horizontal frame
    FXHorizontalFrame* auxiliarHorizontalFrame = nullptr;
    // create elements for end attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeEndRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_END).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueEndTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for number attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeNumberRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_NUMBER).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueNumberTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for vehsPerHour attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeVehsPerHourRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_VEHSPERHOUR).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueVehsPerHourTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for period attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributePeriodRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_PERIOD).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValuePeriodTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for Probability attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeProbabilityRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_PROB).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueProbabilityTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // set default values
    myValueEndTextField->setText("3600");
    myValueNumberTextField->setText("1800");
    myValueVehsPerHourTextField->setText("1800");
    myValuePeriodTextField->setText("2");
    myValueProbabilityTextField->setText("0.5");
    // refresh attributes
    refreshAttributesCreatorFlow();
}


GNEFrameAttributesModuls::AttributesCreatorFlow::~AttributesCreatorFlow() {}


void
GNEFrameAttributesModuls::AttributesCreatorFlow::showAttributesCreatorFlowModul(const bool persons) {
    if (persons) {
        myAttributeVehsPerHourRadioButton->setText(toString(SUMO_ATTR_PERSONSPERHOUR).c_str());
    } else {
        myAttributeVehsPerHourRadioButton->setText(toString(SUMO_ATTR_VEHSPERHOUR).c_str());
    }
    // show
    show();
}


void
GNEFrameAttributesModuls::AttributesCreatorFlow::hideAttributesCreatorFlowModul() {
    hide();
}


void
GNEFrameAttributesModuls::AttributesCreatorFlow::refreshAttributesCreatorFlow() {
    if (myFlowParameters & VEHPARS_END_SET) {
        myAttributeEndRadioButton->setCheck(TRUE);
        myValueEndTextField->enable();
    } else {
        myAttributeEndRadioButton->setCheck(FALSE);
        myValueEndTextField->disable();
    }
    if (myFlowParameters & VEHPARS_NUMBER_SET) {
        myAttributeNumberRadioButton->setCheck(TRUE);
        myValueNumberTextField->enable();
    } else {
        myAttributeNumberRadioButton->setCheck(FALSE);
        myValueNumberTextField->disable();
    }
    if (myFlowParameters & VEHPARS_VPH_SET) {
        myAttributeVehsPerHourRadioButton->setCheck(TRUE);
        myValueVehsPerHourTextField->enable();
    } else {
        myAttributeVehsPerHourRadioButton->setCheck(FALSE);
        myValueVehsPerHourTextField->disable();
    }
    if (myFlowParameters & VEHPARS_PERIOD_SET) {
        myAttributePeriodRadioButton->setCheck(TRUE);
        myValuePeriodTextField->enable();
    } else {
        myAttributePeriodRadioButton->setCheck(FALSE);
        myValuePeriodTextField->disable();
    }
    if (myFlowParameters & VEHPARS_PROB_SET) {
        myAttributeProbabilityRadioButton->setCheck(TRUE);
        myValueProbabilityTextField->enable();
    } else {
        myAttributeProbabilityRadioButton->setCheck(FALSE);
        myValueProbabilityTextField->disable();
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorFlow::setFlowParameters(CommonXMLStructure::SumoBaseObject* baseObject) {
    if (myFlowParameters & VEHPARS_END_SET) {
        baseObject->addDoubleAttribute(SUMO_ATTR_END, GNEAttributeCarrier::parse<double>(myValueEndTextField->getText().text()));
    }
    if (myFlowParameters & VEHPARS_NUMBER_SET) {
        baseObject->addIntAttribute(SUMO_ATTR_NUMBER, GNEAttributeCarrier::parse<int>(myValueNumberTextField->getText().text()));
    }
    if (myFlowParameters & VEHPARS_VPH_SET) {
        if (myAttributeVehsPerHourRadioButton->getText().text() == toString(SUMO_ATTR_VEHSPERHOUR)) {
            baseObject->addDoubleAttribute(SUMO_ATTR_VEHSPERHOUR, GNEAttributeCarrier::parse<double>(myValueVehsPerHourTextField->getText().text()));
        } else {
            baseObject->addDoubleAttribute(SUMO_ATTR_PERSONSPERHOUR, GNEAttributeCarrier::parse<double>(myValueVehsPerHourTextField->getText().text()));
        }
    }
    if (myFlowParameters & VEHPARS_PERIOD_SET) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PERIOD, GNEAttributeCarrier::parse<double>(myValuePeriodTextField->getText().text()));
    }
    if (myFlowParameters & VEHPARS_PROB_SET) {
        baseObject->addDoubleAttribute(SUMO_ATTR_PROB, GNEAttributeCarrier::parse<double>(myValueProbabilityTextField->getText().text()));
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorFlow::showWarningMessage(std::string /* extra */) const {
    std::string errorMessage;
    /*
    // iterate over standar parameters
    for (const auto &i : myTagProperties) {
        if (errorMessage.empty() && myAttributesCreatorRows.at(i.getPositionListed())) {
            // Return string with the error if at least one of the parameter isn't valid
            std::string attributeValue = myAttributesCreatorRows.at(i.getPositionListed())->isAttributeValid();
            if (attributeValue.size() != 0) {
                errorMessage = attributeValue;
            }
        }
    }
    // show warning box if input parameters aren't invalid
    if (extra.size() == 0) {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + errorMessage;
    } else {
        errorMessage = "Invalid input parameter of " + myTagProperties.getTagStr() + ": " + extra;
    }
    */
    // set message in status bar
    myAttributesCreatorParent->getFrameParent()->myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEFrameAttributesModuls::AttributesCreatorFlow::areValuesValid() const {
    // check every flow attribute
    if (myFlowParameters & VEHPARS_END_SET) {
        if (GNEAttributeCarrier::canParse<double>(myValueEndTextField->getText().text())) {
            if (GNEAttributeCarrier::parse<double>(myValueEndTextField->getText().text()) < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (myFlowParameters & VEHPARS_NUMBER_SET) {
        if (GNEAttributeCarrier::canParse<int>(myValueNumberTextField->getText().text())) {
            if (GNEAttributeCarrier::parse<int>(myValueNumberTextField->getText().text()) < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (myFlowParameters & VEHPARS_VPH_SET) {
        if (GNEAttributeCarrier::canParse<double>(myValueVehsPerHourTextField->getText().text())) {
            if (GNEAttributeCarrier::parse<double>(myValueVehsPerHourTextField->getText().text()) < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (myFlowParameters & VEHPARS_PERIOD_SET) {
        if (GNEAttributeCarrier::canParse<double>(myValuePeriodTextField->getText().text())) {
            if (GNEAttributeCarrier::parse<double>(myValuePeriodTextField->getText().text()) < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (myFlowParameters & VEHPARS_PROB_SET) {
        if (GNEAttributeCarrier::canParse<double>(myValueProbabilityTextField->getText().text())) {
            if (GNEAttributeCarrier::parse<double>(myValueProbabilityTextField->getText().text()) < 0) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}


long
GNEFrameAttributesModuls::AttributesCreatorFlow::onCmdSetFlowAttribute(FXObject* obj, FXSelector, void*) {
    // obtain clicked textfield
    FXTextField* textField = nullptr;
    // check what text field was pressed
    if (obj == myValueEndTextField) {
        textField = myValueEndTextField;
    } else if (obj == myValueNumberTextField) {
        textField = myValueNumberTextField;
    } else if (obj == myValueVehsPerHourTextField) {
        textField = myValueVehsPerHourTextField;
    } else if (obj == myValuePeriodTextField) {
        textField = myValuePeriodTextField;
    } else if (obj == myValueProbabilityTextField) {
        textField = myValueProbabilityTextField;
    } else {
        throw ProcessError("Invalid text field");
    }
    // check if value is valid
    if (GNEAttributeCarrier::canParse<double>(textField->getText().text()) && (GNEAttributeCarrier::parse<double>(textField->getText().text()) >= 0)) {
        textField->setTextColor(FXRGB(0, 0, 0));
    } else {
        textField->setTextColor(FXRGB(255, 0, 0));
    }
    textField->killFocus();
    return 1;
}


long
GNEFrameAttributesModuls::AttributesCreatorFlow::onCmdSelectFlowRadioButton(FXObject* obj, FXSelector, void*) {
    // check what check button was pressed
    if (obj == myAttributeEndRadioButton) {
        GNERouteHandler::setFlowParameters(SUMO_ATTR_END, myFlowParameters);
    } else if (obj == myAttributeNumberRadioButton) {
        GNERouteHandler::setFlowParameters(SUMO_ATTR_NUMBER, myFlowParameters);
    } else if (obj == myAttributeVehsPerHourRadioButton) {
        if (myAttributeVehsPerHourRadioButton->getText().text() == toString(SUMO_ATTR_VEHSPERHOUR)) {
            GNERouteHandler::setFlowParameters(SUMO_ATTR_VEHSPERHOUR, myFlowParameters);
        } else {
            GNERouteHandler::setFlowParameters(SUMO_ATTR_PERSONSPERHOUR, myFlowParameters);
        }
    } else if (obj == myAttributePeriodRadioButton) {
        GNERouteHandler::setFlowParameters(SUMO_ATTR_PERIOD, myFlowParameters);
    } else if (obj == myAttributeProbabilityRadioButton) {
        GNERouteHandler::setFlowParameters(SUMO_ATTR_PROB, myFlowParameters);
    } else {
        throw ProcessError("Invalid Radio Button");
    }
    // refresh attributes
    refreshAttributesCreatorFlow();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditorRow::AttributesEditorRow(GNEFrameAttributesModuls::AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr,
        const std::string& value, const bool attributeEnabled, const bool computed) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
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
GNEFrameAttributesModuls::AttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEFrameAttributesModuls::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value,
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
        // fill comboBox again
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
GNEFrameAttributesModuls::AttributesEditorRow::isAttributesEditorRowValid() const {
    return ((myValueTextField->getTextColor() == FXRGB(0, 0, 0)) || (myValueTextField->getTextColor() == FXRGB(0, 0, 255))) &&
           ((myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 0)) || (myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 255)));
}


long
GNEFrameAttributesModuls::AttributesEditorRow::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
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
GNEFrameAttributesModuls::AttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
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
        if (myValueTextField->getText().empty() && myACAttr.hasStaticDefaultValue()) {
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
GNEFrameAttributesModuls::AttributesEditorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
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


GNEFrameAttributesModuls::AttributesEditorRow::AttributesEditorRow() :
    myAttributesEditorParent(nullptr),
    myMultiple(false) {
}


std::string
GNEFrameAttributesModuls::AttributesEditorRow::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}


bool
GNEFrameAttributesModuls::AttributesEditorRow::mergeJunction(SumoXMLAttr attr, const std::vector<GNEAttributeCarrier*>& inspectedACs, const std::string& newVal) const {
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
// GNEFrameAttributesModuls::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditor::AttributesEditor(GNEFrame* FrameParent) :
    FXGroupBox(FrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(FrameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myAttributesFlowEditor
    myAttributesEditorFlow = new AttributesEditorFlow(this);
    // leave it hidden
    myAttributesEditorFlow->hideAttributesEditorFlowModul();
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEFrameAttributesModuls::AttributesEditor::showAttributeEditorModul(bool includeExtended, bool forceAttributeEnabled) {
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
            myAttributesEditorFlow->showAttributeEditorFlowModul();
        } else {
            myAttributesEditorFlow->hideAttributesEditorFlowModul();
        }
        // show AttributesEditor
        show();
    } else {
        myAttributesEditorFlow->hideAttributesEditorFlowModul();
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNEFrameAttributesModuls::AttributesEditor::hideAttributesEditorModul() {
    // hide AttributesEditorFlowModul
    myAttributesEditorFlow->hideAttributesEditorFlowModul();
    // hide also AttributesEditor
    hide();
}


void
GNEFrameAttributesModuls::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
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
        if (myAttributesEditorFlow->isAttributesEditorFlowModulShown()) {
            myAttributesEditorFlow->refreshAttributeEditorFlow();
        }
    }
}


GNEFrame*
GNEFrameAttributesModuls::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEFrameAttributesModuls::AttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myFrameParent->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // open Help attributes dialog
        myFrameParent->openHelpAttributesDialog(myFrameParent->getViewNet()->getInspectedAttributeCarriers().front());
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesEditorFlow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditorFlow::AttributesEditorFlow(AttributesEditor* attributesEditorParent) :
    FXGroupBox(attributesEditorParent->getFrameParent()->myContentFrame, "Flow attributes", GUIDesignGroupBoxFrame),
    myAttributesEditorParent(attributesEditorParent) {
    // declare auxiliar horizontal frame
    FXHorizontalFrame* auxiliarHorizontalFrame = nullptr;
    // create elements for end attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeEndRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_END).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueEndTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for number attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeNumberRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_NUMBER).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueNumberTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for vehsPerHour attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeVehsPerHourRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_VEHSPERHOUR).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueVehsPerHourTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for period attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributePeriodRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_PERIOD).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValuePeriodTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create elements for Probability attribute
    auxiliarHorizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAttributeProbabilityRadioButton = new FXRadioButton(auxiliarHorizontalFrame, toString(SUMO_ATTR_PROB).c_str(), this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myValueProbabilityTextField = new FXTextField(auxiliarHorizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::showAttributeEditorFlowModul() {
    if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // refresh attributeEditorFlowModul
        refreshAttributeEditorFlow();
        // show flow
        show();
    } else {
        hide();
    }
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::hideAttributesEditorFlowModul() {
    // simply hide modul
    hide();
}


bool
GNEFrameAttributesModuls::AttributesEditorFlow::isAttributesEditorFlowModulShown() const {
    return shown();
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshAttributeEditorFlow() {
    if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // simply refresh every flow attribute
        refreshEnd();
        refreshNumber();
        refreshVehsPerHour();
        refreshPeriod();
        refreshProbability();
    }
}


long
GNEFrameAttributesModuls::AttributesEditorFlow::onCmdSetFlowAttribute(FXObject* obj, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList();
    SumoXMLAttr attr = SUMO_ATTR_NOTHING;
    std::string value;
    // check what check button was pressed
    if (obj == myValueEndTextField) {
        attr = SUMO_ATTR_END;
        value = myValueEndTextField->getText().text();
    } else if (obj == myValueNumberTextField) {
        attr = SUMO_ATTR_NUMBER;
        value = myValueNumberTextField->getText().text();
    } else if (obj == myValueVehsPerHourTextField) {
        // check attribute
        if (ACs.front()->getTagProperty().hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
            attr = SUMO_ATTR_VEHSPERHOUR;
        } else {
            attr = SUMO_ATTR_PERSONSPERHOUR;
        }
        value = myValueVehsPerHourTextField->getText().text();
    } else if (obj == myValuePeriodTextField) {
        attr = SUMO_ATTR_PERIOD;
        value = myValuePeriodTextField->getText().text();
    } else if (obj == myValueProbabilityTextField) {
        attr = SUMO_ATTR_PROB;
        value = myValueProbabilityTextField->getText().text();
    } else {
        throw ProcessError("Invalid text field");
    }
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected checkBox for attribute '" + toString(attr) + "'");
    // check if we're editing multiple attributes
    if (ACs.size() > 1) {
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "Change multiple " + toString(attr) + " attributes");
    }
    // enable attribute with undo/redo
    for (const auto& inspectedAC : ACs) {
        inspectedAC->setAttribute(attr, value, undoList);
    }
    // check if we're editing multiple attributes
    if (ACs.size() > 1) {
        undoList->end();
    }
    // refresh Attributes edito parent
    refreshAttributeEditorFlow();
    return 1;
}


long
GNEFrameAttributesModuls::AttributesEditorFlow::onCmdSelectFlowRadioButton(FXObject* obj, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList();
    SumoXMLAttr attr = SUMO_ATTR_NOTHING;
    // check what check button was pressed
    if (obj == myAttributeEndRadioButton) {
        attr = SUMO_ATTR_END;
    } else if (obj == myAttributeNumberRadioButton) {
        attr = SUMO_ATTR_NUMBER;
    } else if (obj == myAttributeVehsPerHourRadioButton) {
        attr = SUMO_ATTR_VEHSPERHOUR;
    } else if (obj == myAttributePeriodRadioButton) {
        attr = SUMO_ATTR_PERIOD;
    } else if (obj == myAttributeProbabilityRadioButton) {
        attr = SUMO_ATTR_PROB;
    } else {
        throw ProcessError("Invalid Radio Button");
    }
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected checkBox for attribute '" + toString(attr) + "'");
    // begin undo list
    if (ACs.size() > 1) {
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "enable multiple " + toString(attr) + " attributes");
    } else {
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "enable attribute '" + toString(attr) + "'");
    }
    // enable attribute with undo/redo
    for (const auto& inspectedAC : ACs) {
        inspectedAC->enableAttribute(attr, undoList);
    }
    // end undoList
    undoList->end();
    // refresh Attributes edito parent
    refreshAttributeEditorFlow();
    return 1;
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshEnd() {
    // first we need to check if all attributes are enabled or disabled
    int allAttributesEnabledOrDisabled = 0;
    for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        allAttributesEnabledOrDisabled += inspectedAC->isAttributeEnabled(SUMO_ATTR_END);
    }
    if (allAttributesEnabledOrDisabled == (int)myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size()) {
        // Declare a set of occuring values and insert attribute's values of item
        std::set<std::string> occuringValues;
        for (const auto& values : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
            occuringValues.insert(values->getAttribute(SUMO_ATTR_END));
        }
        // get current value
        std::ostringstream endValue;
        for (const auto& occuringValue : occuringValues) {
            if (occuringValue != *occuringValues.begin()) {
                endValue << " ";
            }
            endValue << occuringValue;
        }
        // set radio button and text field
        myValueEndTextField->enable();
        myValueEndTextField->setText(endValue.str().c_str());
        myAttributeEndRadioButton->setCheck(TRUE);
    } else {
        // disable radio button and text field
        myValueEndTextField->disable();
        // check if we set an special value in textField
        if ((allAttributesEnabledOrDisabled > 0) && (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 1)) {
            myValueEndTextField->setText("Different flow attributes");
        } else if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
            myValueEndTextField->setText(myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAlternativeValueForDisabledAttributes(SUMO_ATTR_END).c_str());
        } else {
            myValueEndTextField->setText("");
        }
        myAttributeEndRadioButton->setCheck(FALSE);
    }
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshNumber() {
    // first we need to check if all attributes are enabled or disabled
    int allAttributesEnabledOrDisabled = 0;
    for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        allAttributesEnabledOrDisabled += inspectedAC->isAttributeEnabled(SUMO_ATTR_NUMBER);
    }
    if (allAttributesEnabledOrDisabled == (int)myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size()) {
        // Declare a set of occuring values and insert attribute's values of item
        std::set<std::string> occuringValues;
        for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
            occuringValues.insert(inspectedAC->getAttribute(SUMO_ATTR_NUMBER));
        }
        // get current value
        std::ostringstream numberValues;
        for (const auto& occuringValue : occuringValues) {
            if (occuringValue != *occuringValues.begin()) {
                numberValues << " ";
            }
            numberValues << occuringValue;
        }
        // set radio button and text field
        myValueNumberTextField->enable();
        myValueNumberTextField->setText(numberValues.str().c_str());
        myAttributeNumberRadioButton->setCheck(TRUE);
    } else {
        // disable radio button
        myValueNumberTextField->disable();
        // check if we set an special value in textField
        if ((allAttributesEnabledOrDisabled > 0) && (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 1)) {
            myValueNumberTextField->setText("Different flow attributes");
        } else if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
            myValueNumberTextField->setText(myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAlternativeValueForDisabledAttributes(SUMO_ATTR_NUMBER).c_str());
        } else {
            myValueNumberTextField->setText("");
        }
        myAttributeNumberRadioButton->setCheck(FALSE);
    }
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshVehsPerHour() {
    // declare attribute
    SumoXMLAttr attr = SUMO_ATTR_VEHSPERHOUR;
    // first change attribute
    if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getTagProperty().hasAttribute(SUMO_ATTR_PERSONSPERHOUR)) {
        attr = SUMO_ATTR_PERSONSPERHOUR;
    }
    // update radio button
    myAttributeVehsPerHourRadioButton->setText(toString(attr).c_str());
    // we need to check if all attributes are enabled or disabled
    int allAttributesEnabledOrDisabled = 0;
    for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        allAttributesEnabledOrDisabled += inspectedAC->isAttributeEnabled(attr);
    }
    if (allAttributesEnabledOrDisabled == (int)myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size()) {
        // Declare a set of occuring values and insert attribute's values of item
        std::set<std::string> occuringValues;
        for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
            occuringValues.insert(inspectedAC->getAttribute(attr));
        }
        // get current value
        std::ostringstream vehsPerHourValues;
        for (const auto& occuringValue : occuringValues) {
            if (occuringValue != *occuringValues.begin()) {
                vehsPerHourValues << " ";
            }
            vehsPerHourValues << occuringValue;
        }
        // set radio button and text field
        myValueVehsPerHourTextField->enable();
        myValueVehsPerHourTextField->setText(vehsPerHourValues.str().c_str());
        myAttributeVehsPerHourRadioButton->setCheck(TRUE);
    } else {
        // disable radio button
        myValueVehsPerHourTextField->disable();
        // check if we set an special value in textField
        if ((allAttributesEnabledOrDisabled > 0) && (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 1)) {
            myValueVehsPerHourTextField->setText("Different flow attributes");
        } else if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
            myValueVehsPerHourTextField->setText(myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAlternativeValueForDisabledAttributes(attr).c_str());
        } else {
            myValueVehsPerHourTextField->setText("");
        }
        myAttributeVehsPerHourRadioButton->setCheck(FALSE);
    }
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshPeriod() {
    // first we need to check if all attributes are enabled or disabled
    int allAttributesEnabledOrDisabled = 0;
    for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        allAttributesEnabledOrDisabled += inspectedAC->isAttributeEnabled(SUMO_ATTR_PERIOD);
    }
    if (allAttributesEnabledOrDisabled == (int)myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size()) {
        // Declare a set of occuring values and insert attribute's values of item
        std::set<std::string> occuringValues;
        for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
            occuringValues.insert(inspectedAC->getAttribute(SUMO_ATTR_PERIOD));
        }
        // get current value
        std::ostringstream periodValues;
        for (const auto& occuringValue : occuringValues) {
            if (occuringValue != *occuringValues.begin()) {
                periodValues << " ";
            }
            periodValues << occuringValue;
        }
        // set radio button and text field
        myValuePeriodTextField->enable();
        myValuePeriodTextField->setText(periodValues.str().c_str());
        myAttributePeriodRadioButton->setCheck(TRUE);
    } else {
        // disable radio button and text field
        myValuePeriodTextField->disable();
        // check if we set an special value in textField
        if ((allAttributesEnabledOrDisabled > 0) && (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 1)) {
            myValuePeriodTextField->setText("Different flow attributes");
        } else if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
            myValuePeriodTextField->setText(myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAlternativeValueForDisabledAttributes(SUMO_ATTR_PERIOD).c_str());
        } else {
            myValuePeriodTextField->setText("");
        }
        myAttributePeriodRadioButton->setCheck(FALSE);
    }
}


void
GNEFrameAttributesModuls::AttributesEditorFlow::refreshProbability() {
    // first we need to check if all attributes are enabled or disabled
    int allAttributesEnabledOrDisabled = 0;
    for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        allAttributesEnabledOrDisabled += inspectedAC->isAttributeEnabled(SUMO_ATTR_PROB);
    }
    if (allAttributesEnabledOrDisabled == (int)myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size()) {
        // Declare a set of occuring values and insert attribute's values of item
        std::set<std::string> occuringValues;
        for (const auto& inspectedAC : myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
            occuringValues.insert(inspectedAC->getAttribute(SUMO_ATTR_PROB));
        }
        // get current value
        std::ostringstream probabilityValues;
        for (const auto& occuringValue : occuringValues) {
            if (occuringValue != *occuringValues.begin()) {
                probabilityValues << " ";
            }
            probabilityValues << occuringValue;
        }
        // set radio button and text field
        myValueProbabilityTextField->enable();
        myValueProbabilityTextField->setText(probabilityValues.str().c_str());
        myAttributeProbabilityRadioButton->enable();
        myAttributeProbabilityRadioButton->setCheck(TRUE);
    } else {
        // disable radio button and text field
        myValueProbabilityTextField->disable();
        // check if we set an special value in textField
        if ((allAttributesEnabledOrDisabled > 0) && (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() > 1)) {
            myValueProbabilityTextField->setText("Different flow attributes");
        } else if (myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().size() == 1) {
            myValueProbabilityTextField->setText(myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers().front()->getAlternativeValueForDisabledAttributes(SUMO_ATTR_PROB).c_str());
        } else {
            myValueProbabilityTextField->setText("");
        }
        myAttributeProbabilityRadioButton->setCheck(FALSE);
    }
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesEditorExtended- methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditorExtended::AttributesEditorExtended(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Extended attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // Create open dialog button
    new FXButton(this, "Open attributes editor", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameAttributesModuls::AttributesEditorExtended::~AttributesEditorExtended() {}


void
GNEFrameAttributesModuls::AttributesEditorExtended::showAttributesEditorExtendedModul() {
    show();
}


void
GNEFrameAttributesModuls::AttributesEditorExtended::hideAttributesEditorExtendedModul() {
    hide();
}


long
GNEFrameAttributesModuls::AttributesEditorExtended::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // open AttributesCreator extended dialog
    myFrameParent->attributesEditorExtendedDialogOpened();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::ParametersEditorCreator - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::ParametersEditorCreator::ParametersEditorCreator(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Parameters", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = new FXButton(this, "Edit parameters", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributesModuls::ParametersEditorCreator::~ParametersEditorCreator() {}


void
GNEFrameAttributesModuls::ParametersEditorCreator::showParametersEditorCreator() {
    // refresh ParametersEditorCreator
    refreshParametersEditorCreator();
    // show groupbox
    show();
}


void
GNEFrameAttributesModuls::ParametersEditorCreator::hideParametersEditorCreator() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributesModuls::ParametersEditorCreator::refreshParametersEditorCreator() {
    myTextFieldParameters->setText(getParametersStr().c_str());
    myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
}


const std::map<std::string, std::string>&
GNEFrameAttributesModuls::ParametersEditorCreator::getParametersMap() const {
    return myParameters;
}


std::string
GNEFrameAttributesModuls::ParametersEditorCreator::getParametersStr() const {
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
GNEFrameAttributesModuls::ParametersEditorCreator::getParameters() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : myParameters) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEFrameAttributesModuls::ParametersEditorCreator::setParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
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
GNEFrameAttributesModuls::ParametersEditorCreator::getFrameParent() const {
    return myFrameParent;
}


long
GNEFrameAttributesModuls::ParametersEditorCreator::onCmdEditParameters(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open single parameters dialog");
    if (GNESingleParametersDialog(this).execute()) {
        // write debug information
        WRITE_DEBUG("Close single parameters dialog");
        // Refresh parameter EditorCreator
        refreshParametersEditorCreator();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel single parameters dialog");
    }
    return 1;
}


long
GNEFrameAttributesModuls::ParametersEditorCreator::onCmdSetParameters(FXObject*, FXSelector, void*) {
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
// GNEFrameAttributesModuls::DrawingShape - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::DrawingShape::DrawingShape(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Drawing", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myDeleteLastCreatedPoint(false) {
    // create start and stop buttons
    myStartDrawingButton = new FXButton(this, "Start drawing", 0, this, MID_GNE_STARTDRAWING, GUIDesignButton);
    myStopDrawingButton = new FXButton(this, "Stop drawing", 0, this, MID_GNE_STOPDRAWING, GUIDesignButton);
    myAbortDrawingButton = new FXButton(this, "Abort drawing", 0, this, MID_GNE_ABORTDRAWING, GUIDesignButton);
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
    myInformationLabel = new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // disable stop and abort functions as init
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


GNEFrameAttributesModuls::DrawingShape::~DrawingShape() {}


void GNEFrameAttributesModuls::DrawingShape::showDrawingShape() {
    // abort current drawing before show
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::show();
}


void GNEFrameAttributesModuls::DrawingShape::hideDrawingShape() {
    // abort current drawing before hide
    abortDrawing();
    // show FXGroupBox
    FXGroupBox::hide();
}


void
GNEFrameAttributesModuls::DrawingShape::startDrawing() {
    // Only start drawing if DrawingShape modul is shown
    if (shown()) {
        // change buttons
        myStartDrawingButton->disable();
        myStopDrawingButton->enable();
        myAbortDrawingButton->enable();
    }
}


void
GNEFrameAttributesModuls::DrawingShape::stopDrawing() {
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
GNEFrameAttributesModuls::DrawingShape::abortDrawing() {
    // clear created points
    myTemporalShapeShape.clear();
    myFrameParent->myViewNet->updateViewNet();
    // change buttons
    myStartDrawingButton->enable();
    myStopDrawingButton->disable();
    myAbortDrawingButton->disable();
}


void
GNEFrameAttributesModuls::DrawingShape::addNewPoint(const Position& P) {
    if (myStopDrawingButton->isEnabled()) {
        myTemporalShapeShape.push_back(P);
    } else {
        throw ProcessError("A new point cannot be added if drawing wasn't started");
    }
}


void
GNEFrameAttributesModuls::DrawingShape::removeLastPoint() {

}


const PositionVector&
GNEFrameAttributesModuls::DrawingShape::getTemporalShape() const {
    return myTemporalShapeShape;
}


bool
GNEFrameAttributesModuls::DrawingShape::isDrawing() const {
    return myStopDrawingButton->isEnabled();
}


void
GNEFrameAttributesModuls::DrawingShape::setDeleteLastCreatedPoint(bool value) {
    myDeleteLastCreatedPoint = value;
}


bool
GNEFrameAttributesModuls::DrawingShape::getDeleteLastCreatedPoint() {
    return myDeleteLastCreatedPoint;
}


long
GNEFrameAttributesModuls::DrawingShape::onCmdStartDrawing(FXObject*, FXSelector, void*) {
    startDrawing();
    return 0;
}


long
GNEFrameAttributesModuls::DrawingShape::onCmdStopDrawing(FXObject*, FXSelector, void*) {
    stopDrawing();
    return 0;
}


long
GNEFrameAttributesModuls::DrawingShape::onCmdAbortDrawing(FXObject*, FXSelector, void*) {
    abortDrawing();
    return 0;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::NeteditAttributes- methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::NeteditAttributes::NeteditAttributes(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent),
    myCurrentLengthValid(true),
    myActualAdditionalReferencePoint(AdditionalReferencePoint::LEFT) {
    // Create FXListBox for the reference points and fill it
    myReferencePointMatchBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBox);
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");
    // Create Frame for Length Label and textField
    myLengthFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myLengthFrame, toString(SUMO_ATTR_LENGTH).c_str(), 0, GUIDesignLabelAttribute);
    myLengthTextField = new FXTextField(myLengthFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myLengthTextField->setText("10");
    // Create Frame for block close polygon and checkBox (By default disabled)
    myCloseShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCloseShapeFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(myCloseShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // Create Frame for center element after creation (By default enabled)
    myCenterViewAfterCreationFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCenterViewAfterCreationFrame, "Center view", 0, GUIDesignLabelAttribute);
    myCenterViewAfterCreationButton = new FXCheckButton(myCenterViewAfterCreationFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myCenterViewAfterCreationButton->setCheck(true);
    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEFrameAttributesModuls::NeteditAttributes::~NeteditAttributes() {}


void
GNEFrameAttributesModuls::NeteditAttributes::showNeteditAttributesModul(const GNETagProperties& tagProperty) {
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
GNEFrameAttributesModuls::NeteditAttributes::hideNeteditAttributesModul() {
    hide();
}


bool
GNEFrameAttributesModuls::NeteditAttributes::getNeteditAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, const GNELane* lane) const {
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
GNEFrameAttributesModuls::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
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
GNEFrameAttributesModuls::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
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
GNEFrameAttributesModuls::NeteditAttributes::setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const {
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
GNEFrameAttributesModuls::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
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
GNEFrameAttributesModuls::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
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
GNEFrameAttributesModuls::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr) {
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
