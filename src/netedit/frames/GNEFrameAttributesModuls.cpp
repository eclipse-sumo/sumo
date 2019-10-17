/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrameAttributesModuls.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
/// @version $Id$
///
// Auxiliar class for GNEFrame Moduls (only for attributes edition)
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netedit/dialogs/GNEParametersDialog.h>
#include <netedit/netelements/GNELane.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>
#include <utils/vehicle/SUMOVehicleParameter.h>

#include "GNEFrame.h"
#include "GNEFrameAttributesModuls.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributesModuls::AttributesCreatorRow) RowCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,              GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,         GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectColorButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BUTTON,       GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectRadioButton)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesCreator) AttributesCreatorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributesModuls::AttributesCreator::onCmdHelp)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,              GNEFrameAttributesModuls::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,         GNEFrameAttributesModuls::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,       GNEFrameAttributesModuls::AttributesEditorRow::onCmdOpenAttributeDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BUTTON,       GNEFrameAttributesModuls::AttributesEditorRow::onCmdEnableAttribute)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributesModuls::AttributesEditor::onCmdAttributesEditorHelp)
};

FXDEFMAP(GNEFrameAttributesModuls::AttributesEditorExtended) AttributesEditorExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributesModuls::AttributesEditorExtended::onCmdOpenDialog)
};

FXDEFMAP(GNEFrameAttributesModuls::ParametersEditor) ParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributesModuls::ParametersEditor::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributesModuls::ParametersEditor::onCmdSetParameters)
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
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditorRow,          FXHorizontalFrame,  AttributesEditorRowMap,         ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditor,             FXGroupBox,         AttributesEditorMap,            ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::AttributesEditorExtended,     FXGroupBox,         AttributesEditorExtendedMap,    ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameAttributesModuls::ParametersEditor,             FXGroupBox,         ParametersEditorMap,            ARRAYNUMBER(ParametersEditorMap))
FXIMPLEMENT(GNEFrameAttributesModuls::DrawingShape,                 FXGroupBox,         DrawingShapeMap,                ARRAYNUMBER(DrawingShapeMap))
FXIMPLEMENT(GNEFrameAttributesModuls::NeteditAttributes,            FXGroupBox,         NeteditAttributesMap,           ARRAYNUMBER(NeteditAttributesMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesCreatorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesCreatorRow::AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeCarrier::AttributeProperties& attrProperties) :
    FXHorizontalFrame(AttributesCreatorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesCreatorParent(AttributesCreatorParent),
    myAttrProperties(attrProperties) {
    // Create left visual elements
    myAttributeLabel = new FXLabel(this, "name", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    myAttributeRadioButton = new FXRadioButton(this, "name", this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myAttributeRadioButton->hide();
    myAttributeCheckButton = new FXCheckButton(this, "name", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
    myAttributeCheckButton->hide();
    myAttributeColorButton = new FXButton(this, "ColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAttributeColorButton->hide();
    // Create right visual elements
    myValueTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    myValueCheckButton = new FXCheckButton(this, "Disabled", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // by default attribute check button is true (except for until)
    if ((attrProperties.getTagPropertyParent().isStop() || attrProperties.getTagPropertyParent().isPersonStop()) && (attrProperties.getAttr() == SUMO_ATTR_UNTIL)) {
        myAttributeCheckButton->setCheck(FALSE);
    } else {
        myAttributeCheckButton->setCheck(TRUE);
    }
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesCreatorRow
        FXHorizontalFrame::create();
        // reset invalid value
        myInvalidValue = "";
        // special case for attribute ID
        if (attrProperties.getAttr() == SUMO_ATTR_ID) {
            // show check button and disable it
            myAttributeCheckButton->setText(myAttrProperties.getAttrStr().c_str());
            myAttributeCheckButton->setCheck(false);
            myAttributeCheckButton->show();
            // show text field and disable it
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->disable();
            // generate ID
            myValueTextField->setText(generateID().c_str());
            // show textField
            myValueTextField->show();
        } else {
            // show label, button for edit colors or radio button
            if (myAttrProperties.isColor()) {
                myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeColorButton->setText(myAttrProperties.getAttrStr().c_str());
                myAttributeColorButton->show();
            } else if (myAttrProperties.isEnablitable()) {
                myAttributeRadioButton->setTextColor(FXRGB(0, 0, 0));
                myAttributeRadioButton->setText(myAttrProperties.getAttrStr().c_str());
                myAttributeRadioButton->show();
            } else if (myAttrProperties.isOptional()) {
                myAttributeCheckButton->setText(myAttrProperties.getAttrStr().c_str());
                myAttributeCheckButton->show();
                // special case for attributes "Parking", "until" and "duration" (by default disabled)
                if ((myAttrProperties.getTagPropertyParent().isStop() || myAttrProperties.getTagPropertyParent().isPersonStop()) && 
                    (myAttrProperties.getAttr() == SUMO_ATTR_UNTIL || myAttrProperties.getAttr() == SUMO_ATTR_EXTENSION || 
                     myAttrProperties.getAttr() == SUMO_ATTR_PARKING)) {
                    myAttributeCheckButton->setCheck(FALSE);
                } else {
                    myAttributeCheckButton->setCheck(TRUE);
                }
            } else {
                myAttributeLabel->setText(myAttrProperties.getAttrStr().c_str());
                myAttributeLabel->show();
            }
            if (myAttrProperties.isBool()) {
                if (GNEAttributeCarrier::parse<bool>(attrProperties.getDefaultValue())) {
                    myValueCheckButton->setCheck(true);
                    myValueCheckButton->setText("true");
                } else {
                    myValueCheckButton->setCheck(false);
                    myValueCheckButton->setText("false");
                }
                myValueCheckButton->show();
                // if it's associated to a radio button and is disabled, then disable myValueCheckButton
                if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                    myValueCheckButton->disable();
                }
                // if it's associated to a label button and is disabled, then disable myValueCheckButton
                if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
                    myValueCheckButton->disable();
                }
            } else {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->setText(attrProperties.getDefaultValue().c_str());
                myValueTextField->show();
                // if it's associated to a radio button and is disabled, then disable myValueTextField
                if (myAttributeRadioButton->shown() && (myAttributeRadioButton->getCheck() == FALSE)) {
                    myValueTextField->disable();
                }
                // if it's associated to a label button and is disabled, then disable myValueTextField
                if (myAttributeCheckButton->shown() && (myAttributeCheckButton->getCheck() == FALSE)) {
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


const GNEAttributeCarrier::AttributeProperties&
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
GNEFrameAttributesModuls::AttributesCreatorRow::getAttributeRadioButtonCheck() const {
    if (shown()) {
        return myAttributeRadioButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::setAttributeRadioButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myAttributeRadioButton->setCheck(value);
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


bool
GNEFrameAttributesModuls::AttributesCreatorRow::getAttributeCheckButtonCheck() const {
    if (shown()) {
        return myAttributeCheckButton->getCheck() == TRUE;
    } else {
        return false;
    }
}


void
GNEFrameAttributesModuls::AttributesCreatorRow::setAttributeCheckButtonCheck(bool value) {
    if (shown()) {
        // set radio button
        myAttributeCheckButton->setCheck(value);
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


const std::string&
GNEFrameAttributesModuls::AttributesCreatorRow::isAttributeValid() const {
    return myInvalidValue;
}


GNEFrameAttributesModuls::AttributesCreator*
GNEFrameAttributesModuls::AttributesCreatorRow::getAttributesCreatorParent() const {
    return myAttributesCreatorParent;
}


long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // We assume that current value is valid
    myInvalidValue = "";
    // Check if format of current value of myTextField is correct
    if (obj == myValueCheckButton) {
        if (myValueCheckButton->getCheck()) {
            myValueCheckButton->setText("true");
        } else {
            myValueCheckButton->setText("false");
        }
        // update disjoint attribute
        myAttributesCreatorParent->updateDisjointAttributes(nullptr);
    } else if (myAttrProperties.isComplex()) {
        // check complex attribute
        myInvalidValue = checkComplexAttribute(myValueTextField->getText().text());
    } else if (myAttrProperties.isInt()) {
        // first filter int attributes
        if (GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // obtain parsed attribute
            double parsedDouble = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // check if parsed double doesn't have decimal part (or is 0)
            if ((parsedDouble - (int)parsedDouble) == 0) {
                myValueTextField->setText(toString((int)parsedDouble).c_str(), FALSE);
                // Check if int value must be positive
                if (myAttrProperties.isPositive() && (parsedDouble < 0)) {
                    myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
                }
            } else {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'int' format";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'int' format";
        }

    } else if (myAttrProperties.getAttr() == SUMO_ATTR_ANGLE) {
        if (GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter angle
            double angle = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // filter if angle isn't between [0,360]
            if ((angle < 0) || (angle > 360)) {
                // apply modul
                angle = fmod (angle,360);
            }
            // update Textfield
            myValueTextField->setText(toString(angle).c_str(), FALSE);
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'float' format between [0, 360]";
        }
    } else if (myAttrProperties.isSUMOTime()) {
        // time attributes work as positive doubles
        if (!GNEAttributeCarrier::canParse<SUMOTime>(myValueTextField->getText().text())) {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid SUMOTime format";
        }
    } else if (myAttrProperties.isFloat()) {
        if (GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // convert string to double
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // Check if double value must be positive
            if (myAttrProperties.isPositive() && (doubleValue < 0)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' cannot be negative";
                // check if double value is a probability
            } else if (myAttrProperties.isProbability() && ((doubleValue < 0) || doubleValue > 1)) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between 0 and 1";
            } else if (myAttrProperties.hasAttrRange() && ((doubleValue < myAttrProperties.getMinimumRange()) || doubleValue > myAttrProperties.getMaximumRange())) {
                myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' takes only values between " + toString(myAttrProperties.getMinimumRange()) + " and " + toString(myAttrProperties.getMaximumRange());
            } else if ((myAttributesCreatorParent->getCurrentTagProperties().getTag() == SUMO_TAG_E2DETECTOR) && (myAttrProperties.getAttr() == SUMO_ATTR_LENGTH) && (doubleValue == 0)) {
                myInvalidValue = "E2 length cannot be 0";
            }
        } else {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'float' format";
        }
    } else if (myAttrProperties.isColor()) {
        // check if filename format is valid
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text()) == false) {
            myInvalidValue = "'" + myAttrProperties.getAttrStr() + "' doesn't have a valid 'RBGColor' format";
        }
    } else if (myAttrProperties.isFilename()) {
        const std::string file = myValueTextField->getText().text();
        // check if filename format is valid
        if (SUMOXMLDefinitions::isValidFilename(file) == false) {
            myInvalidValue = "input contains invalid characters for a filename";
        } else if (myAttrProperties.getAttr() == SUMO_ATTR_IMGFILE) {
            if (!file.empty()) {
                // only load value if file exist and can be loaded
                if (GUITexturesHelper::getTextureID(file) == -1) {
                    myInvalidValue = "doesn't exist image '" + file + "'";
                }
            }
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_NAME) {
        const std::string name = myValueTextField->getText().text();
        // check if name format is valid
        if (SUMOXMLDefinitions::isValidAttribute(name) == false) {
            myInvalidValue = "input contains invalid characters";
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_VTYPES) {
        const std::string types = myValueTextField->getText().text();
        // if list of VTypes isn't empty, check that all characters are valid
        if (!types.empty() && !SUMOXMLDefinitions::isValidListOfTypeID(types)) {
            myInvalidValue = "list of IDs contains invalid characters";
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_INDEX) {
        // special case for stop indx
        const std::string index = myValueTextField->getText().text();
        if ((index != "fit") && (index != "end") && !GNEAttributeCarrier::canParse<int>(index)) {
            myInvalidValue = "index isn't either 'fit' or 'end' or a valid positive int";
        } else if (GNEAttributeCarrier::canParse<int>(index) && (GNEAttributeCarrier::parse<int>(index) < 0)) {
            myInvalidValue = "index cannot be negative";
        }
    } else if ((myAttrProperties.getAttr() == SUMO_ATTR_EXPECTED) || (myAttrProperties.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS)) {
        // check if attribute can be parsed in a list of Ids
        std::vector<std::string> vehicleIDs = GNEAttributeCarrier::parse<std::vector<std::string> >(myValueTextField->getText().text());
        // check every ID
        for (const auto &i : vehicleIDs) {
            if (!SUMOXMLDefinitions::isValidVehicleID(i)) {
                myInvalidValue = "invalid id used in " + myAttrProperties.getAttrStr();
            }
        }
    } else if (myAttrProperties.getAttr() == SUMO_ATTR_TRIP_ID) {
        if (!SUMOXMLDefinitions::isValidVehicleID(myValueTextField->getText().text())) {
            myInvalidValue = "invalid id used in " + myAttrProperties.getAttrStr();
        }
    }
    // change color of text field depending of myCurrentValueValid
    if (myInvalidValue.size() == 0) {
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
    } else {
        // IF value of TextField isn't valid, change their color to Red
        myValueTextField->setTextColor(FXRGB(255, 0, 0));
    }
    // Update aditional frame
    update();
    return 1;
}


long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextField->enable();
        // refresh row
        refreshRow();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextField->disable();
    }
    return 0;
}


long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectColorButton(FXObject*, FXSelector, void*) {
    // create FXColorDialog
    FXColorDialog colordialog(this, tr("Color Dialog"));
    colordialog.setTarget(this);
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myValueTextField->getText().text())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myAttrProperties.getDefaultValue())));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        myValueTextField->setText(toString(MFXUtils::getRGBColor(colordialog.getRGBA())).c_str());
        onCmdSetAttribute(nullptr, 0, nullptr);
    }
    return 0;
}

long
GNEFrameAttributesModuls::AttributesCreatorRow::onCmdSelectRadioButton(FXObject*, FXSelector, void*) {
    // write debug (for Netedit tests)
    WRITE_DEBUG("Selected radio button for attribute '" + myAttrProperties.getAttrStr() + "'");
    // update disjoint attributes in AC Attributes parent
    myAttributesCreatorParent->updateDisjointAttributes(this);
    return 0;
}


std::string
GNEFrameAttributesModuls::AttributesCreatorRow::checkComplexAttribute(const std::string& value) {
    // declare values needed to check if given complex parameters are valid
    std::string errorMessage;
    DepartDefinition dd;
    DepartLaneDefinition dld;
    DepartPosDefinition dpd;
    DepartPosLatDefinition dpld;
    ArrivalLaneDefinition ald;
    DepartSpeedDefinition dsd;
    ArrivalPosDefinition apd;
    ArrivalPosLatDefinition apld;
    ArrivalSpeedDefinition asd;
    SVCPermissions mode;
    int valueInt;
    double valueDouble;
    SUMOTime valueSUMOTime;
    // check complex attribute
    switch (myAttrProperties.getAttr()) {
        case SUMO_ATTR_DEPART:
        case SUMO_ATTR_BEGIN:
            SUMOVehicleParameter::parseDepart(value, myAttrProperties.getAttrStr(), "", valueSUMOTime, dd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTLANE:
            SUMOVehicleParameter::parseDepartLane(value, myAttrProperties.getAttrStr(), "", valueInt, dld, errorMessage);
            break;
        case SUMO_ATTR_DEPARTPOS:
            SUMOVehicleParameter::parseDepartPos(value, myAttrProperties.getAttrStr(), "", valueDouble, dpd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTSPEED:
            SUMOVehicleParameter::parseDepartSpeed(value, myAttrProperties.getAttrStr(), "", valueDouble, dsd, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALLANE:
            SUMOVehicleParameter::parseArrivalLane(value, myAttrProperties.getAttrStr(), "", valueInt, ald, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALPOS:
            SUMOVehicleParameter::parseArrivalPos(value, myAttrProperties.getAttrStr(), "", valueDouble, apd, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALSPEED:
            SUMOVehicleParameter::parseArrivalSpeed(value, myAttrProperties.getAttrStr(), "", valueDouble, asd, errorMessage);
            break;
        case SUMO_ATTR_DEPARTPOS_LAT:
            SUMOVehicleParameter::parseDepartPosLat(value, myAttrProperties.getAttrStr(), "", valueDouble, dpld, errorMessage);
            break;
        case SUMO_ATTR_ARRIVALPOS_LAT:
            SUMOVehicleParameter::parseArrivalPosLat(value, myAttrProperties.getAttrStr(), "", valueDouble, apld, errorMessage);
            break;
        case SUMO_ATTR_MODES:
            SUMOVehicleParameter::parsePersonModes(value, myAttrProperties.getAttrStr(), "", mode, errorMessage);
            break;
        default:
            throw ProcessError("Invalid complex attribute");
    }
    // return error message (Will be empty if value is valid)
    return errorMessage;
}


std::string
GNEFrameAttributesModuls::AttributesCreatorRow::generateID() const {
    if (myAttrProperties.getTagPropertyParent().isShape()) {
        return myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->generateShapeID(myAttrProperties.getTagPropertyParent().getTag());
    } else if (myAttrProperties.getTagPropertyParent().isAdditional()) {
        return myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->generateAdditionalID(myAttrProperties.getTagPropertyParent().getTag());
    } else if (myAttrProperties.getTagPropertyParent().isDemandElement()) {
        return myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->generateDemandElementID("", myAttrProperties.getTagPropertyParent().getTag());
    } else {
        return "";
    }
}


bool
GNEFrameAttributesModuls::AttributesCreatorRow::isValidID() const {
    return (myAttributesCreatorParent->getFrameParent()->getViewNet()->getNet()->retrieveAdditional(
            myAttrProperties.getTagPropertyParent().getTag(), 
            myValueTextField->getText().text(), false) == nullptr);
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesCreator - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesCreator::AttributesCreator(GNEFrame* frameParent) :
    FXGroupBox(frameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(frameParent) {
    // resize myAttributesCreatorRows
    myAttributesCreatorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEFrameAttributesModuls::AttributesCreator::~AttributesCreator() {}


void
GNEFrameAttributesModuls::AttributesCreator::showAttributesCreatorModul(const GNEAttributeCarrier::TagProperties& tagProperties, const std::vector<SumoXMLAttr> &hiddenAttributes) {
    // set current tag Properties
    myTagProperties = tagProperties;
    // destroy all rows
    for (int i = 0; i < (int)myAttributesCreatorRows.size(); i++) {
        // destroy and delete all rows
        if (myAttributesCreatorRows.at(i) != nullptr) {
            myAttributesCreatorRows.at(i)->destroy();
            delete myAttributesCreatorRows.at(i);
            myAttributesCreatorRows.at(i) = nullptr;
        }
    }
    // iterate over tag attributes and create a AttributesCreatorRow
    for (const auto& i : myTagProperties) {
        // declare falg to check conditions for show attribute
        bool showAttribute = true;
        // check that only non-unique attributes (except ID) are created (And depending of includeExtendedAttributes)
        if (i.isUnique() && (i.getAttr() != SUMO_ATTR_ID)) {
            showAttribute = false;
        }
        // check if attribute must stay hidden
        if (std::find(hiddenAttributes.begin(), hiddenAttributes.end(), i.getAttr()) != hiddenAttributes.end()) {
            showAttribute = false;
        }
        // check special case for vaporizer IDs
        if ((i.getAttr() == SUMO_ATTR_ID) && (i.getTagPropertyParent().getTag() == SUMO_TAG_VAPORIZER)) {
            showAttribute = false;
        }
        // show attribute depending of showAttribute flag
        if (showAttribute) {
            myAttributesCreatorRows.at(i.getPositionListed()) = new AttributesCreatorRow(this, i);
        }
    }
    // update disjoint attributes
    updateDisjointAttributes(nullptr);
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
    // recalc
    recalc();
    // show
    show();
}


void
GNEFrameAttributesModuls::AttributesCreator::hideAttributesCreatorModul() {
    hide();
}


GNEFrame* 
GNEFrameAttributesModuls::AttributesCreator::getFrameParent() const {
    return myFrameParent;
}


std::map<SumoXMLAttr, std::string>
GNEFrameAttributesModuls::AttributesCreator::getAttributesAndValues(bool includeAll) const {
    std::map<SumoXMLAttr, std::string> values;
    // get standard parameters
    for (int i = 0; i < (int)myAttributesCreatorRows.size(); i++) {
        if (myAttributesCreatorRows.at(i) && myAttributesCreatorRows.at(i)->getAttrProperties().getAttr() != SUMO_ATTR_NOTHING) {
            // flag for row enabled
            bool rowEnabled = myAttributesCreatorRows.at(i)->isAttributesCreatorRowEnabled();
            // flag for default attributes
            bool hasDefaultStaticValue = !myAttributesCreatorRows.at(i)->getAttrProperties().hasStaticDefaultValue() || (myAttributesCreatorRows.at(i)->getAttrProperties().getDefaultValue() != myAttributesCreatorRows.at(i)->getValue());
            // flag for enablitables attributes
            bool isEnablitableAttribute = myAttributesCreatorRows.at(i)->getAttrProperties().isEnablitable();
            // flag for optional attributes
            bool isOptionalAttribute = myAttributesCreatorRows.at(i)->getAttrProperties().isOptional() && myAttributesCreatorRows.at(i)->getAttributeCheckButtonCheck();
            // check if flags configuration allow to include values
            if (rowEnabled && (includeAll || hasDefaultStaticValue || isEnablitableAttribute || isOptionalAttribute)) {
                values[myAttributesCreatorRows.at(i)->getAttrProperties().getAttr()] = myAttributesCreatorRows.at(i)->getValue();
            }
        }
    }
    return values;
}


GNEAttributeCarrier::TagProperties
GNEFrameAttributesModuls::AttributesCreator::getCurrentTagProperties() const {
    return myTagProperties;
}


void
GNEFrameAttributesModuls::AttributesCreator::showWarningMessage(std::string extra) const {
    std::string errorMessage;
    // iterate over standar parameters
    for (const auto& i : myTagProperties) {
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

    // set message in status bar
    myFrameParent->myViewNet->setStatusBarText(errorMessage);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG(errorMessage);
}


bool
GNEFrameAttributesModuls::AttributesCreator::areValuesValid() const {
    // iterate over standar parameters
    for (auto i : myTagProperties) {
        // Return false if error message of attriuve isn't empty
        if (myAttributesCreatorRows.at(i.getPositionListed()) && myAttributesCreatorRows.at(i.getPositionListed())->isAttributeValid().size() != 0) {
            return false;
        }
    }
    return true;
}


void
GNEFrameAttributesModuls::AttributesCreator::updateDisjointAttributes(AttributesCreatorRow* row) {
    // currently only Flows supports disjoint attributes
    if ((myTagProperties.getTag() == SUMO_TAG_ROUTEFLOW) || (myTagProperties.getTag() == SUMO_TAG_FLOW) || (myTagProperties.getTag() == SUMO_TAG_PERSONFLOW)) {
        // obtain all rows (to improve code legibility)
        AttributesCreatorRow* endRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_END).getPositionListed()];
        AttributesCreatorRow* numberRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_NUMBER).getPositionListed()];
        AttributesCreatorRow* vehsperhourRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_VEHSPERHOUR).getPositionListed()];
        AttributesCreatorRow* periodRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PERIOD).getPositionListed()];
        AttributesCreatorRow* probabilityRow = myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_PROB).getPositionListed()];
        if (row == nullptr) {
            // by default routeFlows uses end and number
            endRow->setAttributeRadioButtonCheck(true);
            numberRow->setAttributeRadioButtonCheck(true);
            vehsperhourRow->setAttributeRadioButtonCheck(false);
            periodRow->setAttributeRadioButtonCheck(false);
            probabilityRow->setAttributeRadioButtonCheck(false);
        } else {
            // check what row was clicked
            switch (row->getAttrProperties().getAttr()) {
                // end has more priority as number
                case SUMO_ATTR_END:
                    endRow->setAttributeRadioButtonCheck(true);
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_NUMBER:
                    numberRow->setAttributeRadioButtonCheck(true);
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck()) {
                        endRow->setAttributeRadioButtonCheck(false);
                    } else {
                        // disable other combinations
                        vehsperhourRow->setAttributeRadioButtonCheck(false);
                        periodRow->setAttributeRadioButtonCheck(false);
                        probabilityRow->setAttributeRadioButtonCheck(false);
                    }
                    break;
                case SUMO_ATTR_VEHSPERHOUR:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(true);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PERIOD:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(true);
                    probabilityRow->setAttributeRadioButtonCheck(false);
                    break;
                case SUMO_ATTR_PROB:
                    // disable number if begin and end are enabled because end has more priority as number
                    if (endRow->getAttributeRadioButtonCheck() && numberRow->getAttributeRadioButtonCheck()) {
                        numberRow->setAttributeRadioButtonCheck(false);
                    }
                    // disable other combinations
                    vehsperhourRow->setAttributeRadioButtonCheck(false);
                    periodRow->setAttributeRadioButtonCheck(false);
                    probabilityRow->setAttributeRadioButtonCheck(true);
                    break;
                default:
                    break;
            }
        }
    } else if (myTagProperties.isStop() || myTagProperties.isPersonStop()) {
        // check if expected has to be enabled or disabled
        if (myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_TRIGGERED).getPositionListed()]->getValue() == "1") {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED).getPositionListed()]->enableAttributesCreatorRow();
        } else {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED).getPositionListed()]->disableAttributesCreatorRow();
        }
        // check if expected contaienrs has to be enabled or disabled
        if (myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_CONTAINER_TRIGGERED).getPositionListed()]->getValue() == "1") {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED_CONTAINERS).getPositionListed()]->enableAttributesCreatorRow();
        } else {
            myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_EXPECTED_CONTAINERS).getPositionListed()]->disableAttributesCreatorRow();
        }
    }
}


 void 
GNEFrameAttributesModuls::AttributesCreator::refreshRows() {
     // currently only row with attribute ID must be refresh
     if (myTagProperties.hasAttribute(SUMO_ATTR_ID) && (myTagProperties.getTag() != SUMO_TAG_VAPORIZER)) {
         myAttributesCreatorRows[myTagProperties.getAttributeProperties(SUMO_ATTR_ID).getPositionListed()]->refreshRow();
     }
}

long
GNEFrameAttributesModuls::AttributesCreator::onCmdHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog
    myFrameParent->openHelpAttributesDialog(myTagProperties);
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditorRow::AttributesEditorRow(GNEFrameAttributesModuls::AttributesEditor* attributeEditorParent, const GNEAttributeCarrier::AttributeProperties& ACAttr, const std::string& value, bool attributeEnabled) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr),
    myMultiple(GNEAttributeCarrier::parse<std::vector<std::string>>(value).size() > 1) {
    // Create and hide label
    myAttributeLabel = new FXLabel(this, "attributeLabel", nullptr, GUIDesignLabelAttribute);
    myAttributeLabel->hide();
    // Create and hide radio button
    myAttributeRadioButton = new FXRadioButton(this, "attributeRadioButton", this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignRadioButtonAttribute);
    myAttributeRadioButton->hide();
    // Create and hide check button
    myAttributeCheckButton = new FXCheckButton(this, "attributeCheckButton", this, MID_GNE_SET_ATTRIBUTE_BUTTON, GUIDesignCheckButtonAttribute);
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
        // start enabling all elements, depending if attribute is editable and enabled
        if (myACAttr.isNonEditable() || !attributeEnabled) {
            myValueTextField->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
            myAttributeColorButton->disable();
            myAttributeCheckButton->disable();
        } else {
            myValueTextField->enable();
            myValueComboBoxChoices->enable();
            myValueCheckButton->enable();
            myAttributeButtonCombinableChoices->enable();
            myAttributeColorButton->enable();
            myAttributeCheckButton->enable();
        }
        // set left column
        if (myACAttr.isColor()) {
            myAttributeColorButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeColorButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeColorButton->show();
        } else if (myACAttr.isOptional()) {
            myAttributeCheckButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeCheckButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeCheckButton->setCheck(FALSE);
            myAttributeCheckButton->show();
            // enable or disable depending if is editable
            if (myACAttr.isNonEditable()) {
                myAttributeCheckButton->disable();
            } else {
                myAttributeCheckButton->enable();
            }
            // check if radio button has to be check
            if (attributeEnabled) {
                myAttributeCheckButton->setCheck(TRUE);
            } else {
                myAttributeCheckButton->setCheck(FALSE);
            }
        } else if (myACAttr.isEnablitable()) {
            myAttributeRadioButton->setTextColor(FXRGB(0, 0, 0));
            myAttributeRadioButton->setText(myACAttr.getAttrStr().c_str());
            myAttributeRadioButton->show();
            // enable or disable depending if is editable
            if (myACAttr.isNonEditable()) {
                myAttributeRadioButton->disable();
            } else {
                myAttributeRadioButton->enable();
            }
            // check if radio button has to be check
            if (attributeEnabled) {
                myAttributeRadioButton->setCheck(TRUE);
            } else {
                myAttributeRadioButton->setCheck(FALSE);
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
            for (const auto& i : booleanVector) {
                if (i != booleanVector.front()) {
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
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->show();
                // enable or disable depending if attribute is editable and is enabled (used by disjoint attributes)
                if (myACAttr.isNonEditable() || !attributeEnabled) {
                    myValueTextField->disable();
                }
            }
        } else if (myACAttr.isDiscrete()) {
            // Check if are combinable choices
            if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isCombinable()) {
                // hide label
                myAttributeLabel->hide();
                // Show button combinable choices
                myAttributeButtonCombinableChoices->setText(myACAttr.getAttrStr().c_str());
                myAttributeButtonCombinableChoices->show();
                // Show string with the values
                myValueTextField->setText(value.c_str());
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->show();
            } else if (!myMultiple) {
                // fill comboBox
                myValueComboBoxChoices->clearItems();
                for (const auto& it : myACAttr.getDiscreteValues()) {
                    myValueComboBoxChoices->appendItem(it.c_str());
                }
                // show combo box with values
                myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
                myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
                myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
                myValueComboBoxChoices->show();
            } else {
                // represent combinable choices in multiple selections always with a textfield instead with a comboBox
                myValueTextField->setText(value.c_str());
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->show();
            }
        } else {
            // In any other case (String, list, etc.), show value as String
            myValueTextField->setText(value.c_str());
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->show();
        }
        // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACAttr.getTagPropertyParent().isDemandElement()) ||
                ((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACAttr.getTagPropertyParent().isDemandElement())) {
            myAttributeColorButton->disable();
            myAttributeRadioButton->disable();
            myAttributeCheckButton->disable();
            myValueTextField->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
        }
        // special case for Traffic Lights
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_JUNCTION) && (myACAttr.getAttr() == SUMO_ATTR_TLID) && value.empty()) {
            myValueTextField->disable();
        }
        // special case for Default vehicle types (ID cannot be edited)
        if ((ACAttr.getTagPropertyParent().getTag() == SUMO_TAG_VTYPE) && (ACAttr.getAttr() == SUMO_ATTR_ID) &&
                ((value == DEFAULT_VTYPE_ID) || (value == DEFAULT_PEDTYPE_ID) || (value == DEFAULT_BIKETYPE_ID))) {
            myValueTextField->disable();
        }
        // special case for stops over stopping places (value cannot be changed)
        if ((ACAttr.getTagPropertyParent().isStop() || ACAttr.getTagPropertyParent().isPersonStop()) &&
                ((ACAttr.getAttr() == SUMO_ATTR_BUS_STOP) || (ACAttr.getAttr() == SUMO_ATTR_CONTAINER_STOP) ||
                 (ACAttr.getAttr() == SUMO_ATTR_CHARGING_STATION) || (ACAttr.getAttr() == SUMO_ATTR_PARKING_AREA))) {
            myValueTextField->disable();
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
GNEFrameAttributesModuls::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value, bool forceRefresh, bool attributeEnabled) {
    // start enabling all elements
    myValueTextField->enable();
    myValueComboBoxChoices->enable();
    myValueCheckButton->enable();
    myAttributeButtonCombinableChoices->enable();
    myAttributeColorButton->enable();
    myAttributeRadioButton->enable();
    myAttributeCheckButton->enable();
    // set radio buton
    if (myAttributeRadioButton->shown()) {
        myAttributeRadioButton->setCheck(attributeEnabled);
    }
    // set check buton
    if (myAttributeCheckButton->shown()) {
        myAttributeCheckButton->setCheck(attributeEnabled);
    }
    if (myValueTextField->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myValueTextField->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myValueTextField->setText(value.c_str());
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !attributeEnabled) {
            myValueTextField->disable();
        }
    } else if (myValueComboBoxChoices->shown()) {
        // fill comboBox again
        myValueComboBoxChoices->clearItems();
        for (const auto& it : myACAttr.getDiscreteValues()) {
            myValueComboBoxChoices->appendItem(it.c_str());
        }
        // show combo box with values
        myValueComboBoxChoices->setNumVisible((int)myACAttr.getDiscreteValues().size());
        myValueComboBoxChoices->setCurrentItem(myValueComboBoxChoices->findItem(value.c_str()));
        myValueComboBoxChoices->setTextColor(FXRGB(0, 0, 0));
        myValueComboBoxChoices->show();
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !attributeEnabled) {
            myValueComboBoxChoices->disable();
        }
    } else if (myValueCheckButton->shown()) {
        if (GNEAttributeCarrier::canParse<bool>(value)) {
            myValueCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
        } else {
            myValueCheckButton->setCheck(false);
        }
        // disable depending of disjointAttributeEnabled
        if (myACAttr.isNonEditable() || !attributeEnabled) {
            myValueCheckButton->disable();
        }
    }
    // if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
    if (myACAttr.getAttr() != SUMO_ATTR_NOTHING) {
        if (((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACAttr.getTagPropertyParent().isDemandElement()) ||
            ((myAttributesEditorParent->getFrameParent()->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACAttr.getTagPropertyParent().isDemandElement())) {
            myAttributeColorButton->disable();
            myAttributeRadioButton->disable();
            myAttributeCheckButton->disable();
            myValueTextField->disable();
            myValueComboBoxChoices->disable();
            myValueCheckButton->disable();
            myAttributeButtonCombinableChoices->disable();
        }
        // special case for Traffic Lights
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_JUNCTION) && (myACAttr.getAttr() == SUMO_ATTR_TLID) && value.empty()) {
            myValueTextField->disable();
        }
        // special case for Default vehicle types (ID cannot be edited)
        if ((myACAttr.getTagPropertyParent().getTag() == SUMO_TAG_VTYPE) && (myACAttr.getAttr() == SUMO_ATTR_ID) &&
                ((value == DEFAULT_VTYPE_ID) || (value == DEFAULT_PEDTYPE_ID) || (value == DEFAULT_BIKETYPE_ID))) {
            myValueTextField->disable();
        }
        // special case for stops over stopping places (value cannot be changed)
        if ((myACAttr.getTagPropertyParent().isStop() || myACAttr.getTagPropertyParent().isPersonStop()) &&
                ((myACAttr.getAttr() == SUMO_ATTR_BUS_STOP) || (myACAttr.getAttr() == SUMO_ATTR_CONTAINER_STOP) ||
                 (myACAttr.getAttr() == SUMO_ATTR_CHARGING_STATION) || (myACAttr.getAttr() == SUMO_ATTR_PARKING_AREA))) {
            myValueTextField->disable();
        }
    }
}


bool
GNEFrameAttributesModuls::AttributesEditorRow::isAttributesEditorRowValid() const {
    return ((myValueTextField->getTextColor() == FXRGB(0, 0, 0)) && (myValueComboBoxChoices->getTextColor() == FXRGB(0, 0, 0)));
}


long
GNEFrameAttributesModuls::AttributesEditorRow::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myAttributeColorButton) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myValueTextField->getText().text())));
        } else if (!myACAttr.getDefaultValue().empty()) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myACAttr.getDefaultValue())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
            myValueTextField->setText(newValue.c_str());
            if (myAttributesEditorParent->getEditedACs().front()->isValid(myACAttr.getAttr(), newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (myAttributesEditorParent->getEditedACs().size() > 1) {
                    myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
                }
                // Set new value of attribute in all selected ACs
                for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
                    it_ac->setAttribute(myACAttr.getAttr(), newValue, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
                }
                // If previously value was incorrect, change font color to black
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->killFocus();
            }
        }
        return 0;
    } else if (obj == myAttributeButtonCombinableChoices) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
        }
        // open GNEAllowDisallow
        GNEAllowDisallow(myAttributesEditorParent->getFrameParent()->myViewNet, myAttributesEditorParent->getEditedACs().front()).execute();
        std::string allowed = myAttributesEditorParent->getEditedACs().front()->getAttribute(SUMO_ATTR_ALLOW);
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
            it_ac->setAttribute(SUMO_ATTR_ALLOW, allowed, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
        }
        // finish change multiple attributes
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        }
        // update frame parent after attribute sucesfully set
        myAttributesEditorParent->getFrameParent()->attributeUpdated();
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
        // Check if are combinable choices (for example, Vehicle Types)
        if ((myACAttr.getDiscreteValues().size() > 0) && myACAttr.isCombinable()) {
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
                angle = fmod (angle,360);
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
    // Check if attribute must be changed
    if (myAttributesEditorParent->getEditedACs().front()->isValid(myACAttr.getAttr(), newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("Change multiple attributes");
        } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
            // IDs attribute has to be encapsulated
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_begin("change " + myACAttr.getTagPropertyParent().getTagStr() + " attribute");
        }
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getEditedACs()) {
            it_ac->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList());
        }
        // finish change multiple attributes or ID Attributes
        if (myAttributesEditorParent->getEditedACs().size() > 1) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
            myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList()->p_end();
        }
        // If previously value was incorrect, change font color to black
        if (myACAttr.isCombinable()) {
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
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (myACAttr.isCombinable()) {
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
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextField->enable();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextField->disable();
    }
    return 0;
}


long
GNEFrameAttributesModuls::AttributesEditorRow::onCmdEnableAttribute(FXObject* obj, FXSelector, void*) {
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->myViewNet->getUndoList();
    // begin undoList
    if (obj == myAttributeRadioButton) {
        // write debug (for Netedit tests)
        WRITE_DEBUG("Selected radio button for attribute '" + myACAttr.getAttrStr() + "'");
        // begin undo list
        undoList->p_begin("enable attribute '" + myACAttr.getAttrStr() + "'");
        // change attribute with undo/redo
        myAttributesEditorParent->getEditedACs().front()->enableAttribute(myACAttr.getAttr(), undoList);
        // end undoList
    undoList->p_end();
    } else if (obj == myAttributeCheckButton) { 
        // write debug (for Netedit tests)
        WRITE_DEBUG("Selected checkBox for attribute '" + myACAttr.getAttrStr() + "'");
        if (myAttributeCheckButton->getCheck() == TRUE) {
            // begin undo list
            undoList->p_begin("enable attribute '" + myACAttr.getAttrStr() + "'");
            // enable attribute with undo/redo
            myAttributesEditorParent->getEditedACs().front()->enableAttribute(myACAttr.getAttr(), undoList);
        } else {
            // begin undo list
            undoList->p_begin("disable attribute '" + myACAttr.getAttrStr() + "'");
            // disable attribute with undo/redo
            myAttributesEditorParent->getEditedACs().front()->disableAttribute(myACAttr.getAttr(), undoList);
        }
        // end undoList
        undoList->p_end();
    }
    // refresh Attributes edito parent
    myAttributesEditorParent->refreshAttributeEditor(false, false);
    return 0;
}


GNEFrameAttributesModuls::AttributesEditorRow::AttributesEditorRow() :
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

// ---------------------------------------------------------------------------
// GNEFrameAttributesModuls::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::AttributesEditor::AttributesEditor(GNEFrame* FrameParent) :
    FXGroupBox(FrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myFrameParent(FrameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEFrameAttributesModuls::AttributesEditor::showAttributeEditorModul(const std::vector<GNEAttributeCarrier*>& ACs, bool includeExtended, bool forceAttributeEnabled) {
    myEditedACs = ACs;
    myIncludeExtended = includeExtended;
    // remove all rows
    for (int i = 0; i < (int)myAttributesEditorRows.size(); i++) {
        // destroy and delete all rows
        if (myAttributesEditorRows.at(i) != nullptr) {
            myAttributesEditorRows.at(i)->destroy();
            delete myAttributesEditorRows.at(i);
            myAttributesEditorRows.at(i) = nullptr;
        }
    }
    if (myEditedACs.size() > 0) {
        // Iterate over attributes
        for (const auto& i : myEditedACs.front()->getTagProperty()) {
            // disable editing for unique attributes in case of multi-selection
            if ((myEditedACs.size() > 1) && i.isUnique()) {
                continue;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (i.isExtended() && !includeExtended) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item (note: We use a set to avoid repeated values)
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myEditedACs) {
                occuringValues.insert(it_ac->getAttribute(i.getAttr()));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            std::string value = oss.str();
            // show AttributesEditor
            show();
            // declare a flag for enabled attributes
            bool attributeEnabled = myEditedACs.front()->isAttributeEnabled(i.getAttr());
            // extra check for Triggered and container Triggered
            if (myEditedACs.front()->getTagProperty().isStop() || myEditedACs.front()->getTagProperty().isPersonStop()) {
                if((i.getAttr() == SUMO_ATTR_EXPECTED) && (myEditedACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                    attributeEnabled = false;
                } else if ((i.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (myEditedACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                    attributeEnabled = false;
                }
            }
            // if forceEnablellAttribute is enable, force attributeEnabled
            if (forceAttributeEnabled) {
                attributeEnabled = true;
            }
            // create attribute editor row
            myAttributesEditorRows[i.getPositionListed()] = new AttributesEditorRow(this, i, value, attributeEnabled);
        }
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNEFrameAttributesModuls::AttributesEditor::hideAttributesEditorModul() {
    // clear myEditedACs
    myEditedACs.clear();
    // hide also AttributesEditor
    hide();
}


void
GNEFrameAttributesModuls::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    if (myEditedACs.size() > 0) {
        // Iterate over attributes
        for (const auto& i : myEditedACs.front()->getTagProperty()) {
            // disable editing for unique attributes in case of multi-selection
            if ((myEditedACs.size() > 1) && i.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myEditedACs) {
                occuringValues.insert(it_ac->getAttribute(i.getAttr()));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // declare a flag for enabled attributes
            bool attributeEnabled = myEditedACs.front()->isAttributeEnabled(i.getAttr());
            // extra check for Triggered and container Triggered
            if (myEditedACs.front()->getTagProperty().isStop() || myEditedACs.front()->getTagProperty().isPersonStop()) {
                if((i.getAttr() == SUMO_ATTR_EXPECTED) && (myEditedACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                    attributeEnabled = false;
                } else if ((i.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (myEditedACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                    attributeEnabled = false;
                }
            }
            // Check if refresh of Position or Shape has to be forced
            if ((i.getAttr()  == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), true, attributeEnabled);
            } else if ((i.getAttr()  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                // Refresh attributes maintain invalid values
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), true, attributeEnabled);
            } else {
                // Refresh attributes maintain invalid values
                myAttributesEditorRows[i.getPositionListed()]->refreshAttributesEditorRow(oss.str(), false, attributeEnabled);
            }
        }
    }
}


GNEFrame*
GNEFrameAttributesModuls::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


const std::vector<GNEAttributeCarrier*>&
GNEFrameAttributesModuls::AttributesEditor::getEditedACs() const {
    return myEditedACs;
}


void
GNEFrameAttributesModuls::AttributesEditor::removeEditedAC(GNEAttributeCarrier* AC) {
    // Only remove if there is inspected ACs
    if (myEditedACs.size() > 0) {
        // Try to find AC in myACs
        auto i = std::find(myEditedACs.begin(), myEditedACs.end(), AC);
        // if was found
        if (i != myEditedACs.end()) {
            // erase AC from inspected ACs
            myEditedACs.erase(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Removed inspected element from Inspected ACs. " + toString(myEditedACs.size()) + " ACs remains.");
            // Inspect multi selection again (To refresh Modul)
            showAttributeEditorModul(myEditedACs, myIncludeExtended, false);
        }
    }
}


long
GNEFrameAttributesModuls::AttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myEditedACs.size() > 0) {
        // open Help attributes dialog
        myFrameParent->openHelpAttributesDialog(myEditedACs.front()->getTagProperty());
    }
    return 1;
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
// GNEFrameAttributesModuls::ParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributesModuls::ParametersEditor::ParametersEditor(GNEFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Parameters", GUIDesignGroupBoxFrame),
    myFrameParent(inspectorFrameParent),
    myAC(nullptr) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = new FXButton(this, "Edit parameters", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameAttributesModuls::ParametersEditor::~ParametersEditor() {}


void
GNEFrameAttributesModuls::ParametersEditor::showParametersEditor(GNEAttributeCarrier* AC) {
    if ((AC != nullptr) && AC->getTagProperty().hasParameters()) {
        myAC = AC;
        myACs.clear();
        // obtain a copy of AC parameters
        if (myAC) {
            // obtain string
            std::string parametersStr = myAC->getAttribute(GNE_ATTR_PARAMETERS);
            // clear parameters
            myParameters.clear();
            // separate value in a vector of string using | as separator
            StringTokenizer parameters(parametersStr, "|", true);
            // iterate over all values
            while (parameters.hasNext()) {
                // obtain key and value and save it in myParameters
                StringTokenizer keyValue(parameters.next(), "=", true);
                myParameters[keyValue.front()] = keyValue.next();
            }
        }
        // refresh ParametersEditor
        refreshParametersEditor();
        // show groupbox
        show();
    } else {
        hide();
    }
}


void
GNEFrameAttributesModuls::ParametersEditor::showParametersEditor(std::vector<GNEAttributeCarrier*> ACs) {
    if ((ACs.size() > 0) && ACs.front()->getTagProperty().hasParameters()) {
        myAC = nullptr;
        myACs = ACs;
        // check if parameters are different
        bool differentsParameters = false;
        std::string firstParameters = myACs.front()->getAttribute(GNE_ATTR_PARAMETERS);
        for (auto i : myACs) {
            if (firstParameters != i->getAttribute(GNE_ATTR_PARAMETERS)) {
                differentsParameters = true;
            }
        }
        // set parameters editor
        if (differentsParameters) {
            myParameters.clear();
        } else {
            // obtain string
            std::string parametersStr = myACs.front()->getAttribute(GNE_ATTR_PARAMETERS);
            // clear parameters
            myParameters.clear();
            // separate value in a vector of string using | as separator
            std::vector<std::string> parameters = StringTokenizer(parametersStr, "|", true).getVector();
            // iterate over all values
            for (const auto &i : parameters) {
                // obtain key and value and save it in myParameters
                std::vector<std::string> keyValue = StringTokenizer(i, "=", true).getVector();
                myParameters[keyValue.front()] = keyValue.back();
            }
        }
        // refresh ParametersEditor
        refreshParametersEditor();
        // show groupbox
        show();
    } else {
        hide();
    }
}


void
GNEFrameAttributesModuls::ParametersEditor::hideParametersEditor() {
    myAC = nullptr;
    // hide groupbox
    hide();
}


void
GNEFrameAttributesModuls::ParametersEditor::refreshParametersEditor() {
    // update text field depending of AC
    if (myAC) {
        myTextFieldParameters->setText(myAC->getAttribute(GNE_ATTR_PARAMETERS).c_str());
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldParameters if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
        if (((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myAC->getTagProperty().isDemandElement()) ||
                ((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myAC->getTagProperty().isDemandElement())) {
            myTextFieldParameters->disable();
            myButtonEditParameters->disable();
        } else {
            myTextFieldParameters->enable();
            myButtonEditParameters->enable();
        }
    } else if (myACs.size() > 0) {
        // check if parameters of all inspected ACs are different
        std::string parameters = myACs.front()->getAttribute(GNE_ATTR_PARAMETERS);
        for (auto i : myACs) {
            if (parameters != i->getAttribute(GNE_ATTR_PARAMETERS)) {
                parameters = "different parameters";
            }
        }
        myTextFieldParameters->setText(parameters.c_str());
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        // disable myTextFieldParameters if we're in demand mode and inspected AC isn't a demand element (or viceversa)
        if (((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myACs.front()->getTagProperty().isDemandElement()) ||
                ((myFrameParent->myViewNet->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myACs.front()->getTagProperty().isDemandElement())) {
            myTextFieldParameters->disable();
            myButtonEditParameters->disable();
        } else {
            myTextFieldParameters->enable();
            myButtonEditParameters->enable();
        }
    }
}


const std::map<std::string, std::string> &
GNEFrameAttributesModuls::ParametersEditor::getParametersMap() const {
    return myParameters;
}


std::string
GNEFrameAttributesModuls::ParametersEditor::getParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto &i : myParameters) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}

std::vector<std::pair<std::string, std::string> >
GNEFrameAttributesModuls::ParametersEditor::getParametersVectorStr() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate an vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto &i : myParameters) {
        result.push_back(std::make_pair(i.first, i.second));
    }
    return result;
}


void 
GNEFrameAttributesModuls::ParametersEditor::setParameters(const std::vector<std::pair<std::string, std::string> > &parameters) {
    // declare result string
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto &i : parameters) {
        result += i.first + "=" + i.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    // set result in textField (and call onCmdEditParameters)
    myTextFieldParameters->setText(result.c_str(), TRUE);
}


GNEFrame* 
GNEFrameAttributesModuls::ParametersEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEFrameAttributesModuls::ParametersEditor::onCmdEditParameters(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open parameters dialog");
    // edit parameters using dialog
    if (GNEParametersDialog(this).execute()) {
        // write debug information
        WRITE_DEBUG("Close parameters dialog");
        // set values edited in Parameter dialog in Edited AC
        if (myAC) {
            myAC->setAttribute(GNE_ATTR_PARAMETERS, getParametersStr(), myFrameParent->myViewNet->getUndoList());
        } else if (myACs.size() > 0) {
            myFrameParent->myViewNet->getUndoList()->p_begin("Change multiple parameters");
            for (auto i : myACs) {
                i->setAttribute(GNE_ATTR_PARAMETERS, getParametersStr(), myFrameParent->myViewNet->getUndoList());
            }
            myFrameParent->myViewNet->getUndoList()->p_end();
            // update frame parent after attribute sucesfully set
            myFrameParent->attributeUpdated();
        }
        // Refresh parameter editor
        refreshParametersEditor();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel parameters dialog");
    }
    return 1;
}


long
GNEFrameAttributesModuls::ParametersEditor::onCmdSetParameters(FXObject*, FXSelector, void*) {
    // check if current given string is valid
    if (Parameterised::areParametersValid(myTextFieldParameters->getText().text(), true) == false) {
        myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
    } else {
        // parsed parameters ok, then set text field black and continue
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        myTextFieldParameters->killFocus();
        // obtain parameters "key=value"
        std::vector<std::string> parameters = StringTokenizer(myTextFieldParameters->getText().text(), "|", true).getVector();
        // clear current existent parameters and set parsed parameters
        myParameters.clear();
        // iterate over parameters
        for (const auto &i : parameters) {
            // obtain key, value
            std::vector<std::string> keyParam = StringTokenizer(i, "=", true).getVector();
            // save it in myParameters
            myParameters[keyParam.front()] = keyParam.back();
        }
        // overwritte myTextFieldParameters (to remove duplicated parameters
        myTextFieldParameters->setText(getParametersStr().c_str(), FALSE);
        // if we're editing parameters of an AttributeCarrier, set it
        if (myAC) {
            // begin undo list
            myFrameParent->myViewNet->getUndoList()->p_begin("change parameters");
            // set parameters
            myAC->setAttribute(GNE_ATTR_PARAMETERS, getParametersStr(), myFrameParent->myViewNet->getUndoList());
            // end undo list
            myFrameParent->myViewNet->getUndoList()->p_end();
        } else if (myACs.size() > 0) {
            // begin undo list
            myFrameParent->myViewNet->getUndoList()->p_begin("change multiple parameters");
            // set parameters in all ACs
            for (const auto &i : myACs) {
                i->setAttribute(GNE_ATTR_PARAMETERS, getParametersStr(), myFrameParent->myViewNet->getUndoList());
            }
            // end undo list
            myFrameParent->myViewNet->getUndoList()->p_end();
            // update frame parent after attribute sucesfully set
            myFrameParent->attributeUpdated();
        }
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
        myFrameParent->myViewNet->update();
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
    myFrameParent->myViewNet->update();
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
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT) {
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
    // Create Frame for block movement label and checkBox (By default disabled)
    myBlockMovementFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myBlockMovementFrame, "block move", 0, GUIDesignLabelAttribute);
    myBlockMovementCheckButton = new FXCheckButton(myBlockMovementFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myBlockMovementCheckButton->setCheck(false);
    // Create Frame for block shape label and checkBox (By default disabled)
    myBlockShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myBlockShapeFrame, "block shape", 0, GUIDesignLabelAttribute);
    myBlockShapeCheckButton = new FXCheckButton(myBlockShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // Create Frame for block close polygon and checkBox (By default disabled)
    myCloseShapeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(myCloseShapeFrame, "Close shape", 0, GUIDesignLabelAttribute);
    myCloseShapeCheckButton = new FXCheckButton(myCloseShapeFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myBlockShapeCheckButton->setCheck(false);
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
GNEFrameAttributesModuls::NeteditAttributes::showNeteditAttributesModul(const GNEAttributeCarrier::TagProperties& tagProperty) {
    // we assume that frame will not be show
    bool showFrame = false;
    // check if lenght text field has to be showed
    if (tagProperty.canMaskStartEndPos()) {
        myLengthFrame->show();
        myReferencePointMatchBox->show();
        showFrame = true;
    } else {
        myLengthFrame->hide();
        myReferencePointMatchBox->hide();
    }
    // check if block movement check button has to be show
    if (tagProperty.canBlockMovement()) {
        myBlockMovementFrame->show();
        showFrame = true;
    } else {
        myBlockMovementFrame->hide();
    }
    // check if block shape check button has to be show
    if (tagProperty.canBlockShape()) {
        myBlockShapeFrame->show();
        showFrame = true;
    } else {
        myBlockShapeFrame->hide();
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
GNEFrameAttributesModuls::NeteditAttributes::getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string>& valuesMap, const GNELane* lane) const {
    // check if we need to obtain a start and end position over an edge
    if (myReferencePointMatchBox->shown()) {
        // we need a valid lane to calculate position over lane
        if (lane == nullptr) {
            return false;
        } else if (myCurrentLengthValid) {
            // Obtain position of the mouse over lane (limited over grid)
            double mousePositionOverLane = lane->getGeometry().shape.nearest_offset_to_point2D(myFrameParent->myViewNet->snapToActiveGrid(myFrameParent->myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
            // check if current reference point is valid
            if (myActualAdditionalReferencePoint == GNE_ADDITIONALREFERENCEPOINT_INVALID) {
                std::string errorMessage = "Current selected reference point isn't valid";
                myFrameParent->myViewNet->setStatusBarText(errorMessage);
                // Write Warning in console if we're in testing mode
                WRITE_DEBUG(errorMessage);
                return false;
            } else {
                // obtain lenght
                double lenght = GNEAttributeCarrier::parse<double>(myLengthTextField->getText().text());
                // set start and end position
                valuesMap[SUMO_ATTR_STARTPOS] = toString(setStartPosition(mousePositionOverLane, lenght));
                valuesMap[SUMO_ATTR_ENDPOS] = toString(setEndPosition(mousePositionOverLane, lenght));
            }
        } else {
            return false;
        }
    }
    // Save block value if element can be blocked
    if (myBlockMovementCheckButton->shown()) {
        if (myBlockMovementCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_MOVEMENT] = "0";
        }
    }
    // Save block shape value if shape's element can be blocked
    if (myBlockShapeCheckButton->shown()) {
        if (myBlockShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_BLOCK_SHAPE] = "0";
        }
    }
    // Save close shape value if shape's element can be closed
    if (myCloseShapeCheckButton->shown()) {
        if (myCloseShapeCheckButton->getCheck() == 1) {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "1";
        } else {
            valuesMap[GNE_ATTR_CLOSE_SHAPE] = "0";
        }
    }
    // check center element after creation
    if (myCenterViewAfterCreationButton->shown() && (myCenterViewAfterCreationButton->getCheck() == 1)) {
        valuesMap[GNE_ATTR_CENTER_AFTER_CREATION] = "1";
    }
    // all ok, then return true to continue creating element
    return true;
}


long
GNEFrameAttributesModuls::NeteditAttributes::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myBlockMovementCheckButton) {
        if (myBlockMovementCheckButton->getCheck()) {
            myBlockMovementCheckButton->setText("true");
        } else {
            myBlockMovementCheckButton->setText("false");
        }
    } else if (obj == myBlockShapeCheckButton) {
        if (myBlockShapeCheckButton->getCheck()) {
            myBlockShapeCheckButton->setText("true");
        } else {
            myBlockShapeCheckButton->setText("false");
        }
    } else if (obj == myCloseShapeCheckButton) {
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
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference right") {
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_RIGHT;
            myLengthTextField->enable();
        } else if (myReferencePointMatchBox->getText() == "reference center") {
            myLengthTextField->enable();
            myReferencePointMatchBox->setTextColor(FXRGB(0, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_CENTER;
            myLengthTextField->enable();
        } else {
            myReferencePointMatchBox->setTextColor(FXRGB(255, 0, 0));
            myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_INVALID;
            myLengthTextField->disable();
        }
    }

    return 1;
}


long
GNEFrameAttributesModuls::NeteditAttributes::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_MODEADDITIONAL));
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
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
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
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane - lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane - lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}


double
GNEFrameAttributesModuls::NeteditAttributes::setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional)  const {
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT:
            return positionOfTheMouseOverLane + lengthOfAdditional;
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER:
            return positionOfTheMouseOverLane + lengthOfAdditional / 2;
        default:
            throw InvalidArgument("Reference Point invalid");
    }
}

/****************************************************************************/
