/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEGenericParameterDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2018
/// @version $Id$
///
// Dialog for edit rerouters
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewNet.h>

#include "GNEGenericParameterDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGenericParameterDialog) GNEGenericParameterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                      GNEGenericParameterDialog::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,      GNEGenericParameterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,      GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,       GNEGenericParameterDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEGenericParameterDialog, FXDialogBox, GNEGenericParameterDialogMap, ARRAYNUMBER(GNEGenericParameterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGenericParameterDialog::GNEGenericParameterDialog(GNEViewNet *viewNet, std::vector<GNEAttributeCarrier::GenericParameter> *genericParameters) :
    FXDialogBox(viewNet->getApp(), "Edit generic parameters", GUIDesignDialogBox),
    myViewNet(viewNet),
    myGenericParameters(genericParameters) {
    assert(myGenericParameters);
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create groupbox for options
    FXGroupBox* genericParametersGroupBox = new FXGroupBox(mainFrame, "Selection options", GUIDesignGroupBoxFrame);

    FXHorizontalFrame* horizontalFrameGenericParameters = new FXHorizontalFrame(genericParametersGroupBox, GUIDesignAuxiliarHorizontalFrame);

    FXVerticalFrame* verticalFrame1 = new FXVerticalFrame(horizontalFrameGenericParameters, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* verticalFrame2 = new FXVerticalFrame(horizontalFrameGenericParameters, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* verticalFrame3 = new FXVerticalFrame(horizontalFrameGenericParameters, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* verticalFrame4 = new FXVerticalFrame(horizontalFrameGenericParameters, GUIDesignAuxiliarVerticalFrame);
    FXVerticalFrame* verticalFrame5 = new FXVerticalFrame(horizontalFrameGenericParameters, GUIDesignAuxiliarVerticalFrame);

    for (int i = 0; i < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS; i++) {
        if(i < 20) {
            myGenericParameterRows.push_back(GenericParameterRow(this, verticalFrame1));
        } else if(i < 40) {
            myGenericParameterRows.push_back(GenericParameterRow(this,verticalFrame2));
        } else if(i < 60) {
            myGenericParameterRows.push_back(GenericParameterRow(this, verticalFrame3));
        } else if(i < 80) {
            myGenericParameterRows.push_back(GenericParameterRow(this, verticalFrame4));
        } else {
            myGenericParameterRows.push_back(GenericParameterRow(this, verticalFrame5));
        }
    }
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    // reset dialog
    onCmdReset(0, 0, 0);
}


GNEGenericParameterDialog::~GNEGenericParameterDialog() {
}


long
GNEGenericParameterDialog::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    FXButton* buttonPressed = dynamic_cast<FXButton*>(obj);
    return 1;
}


long
GNEGenericParameterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // chek if all vehicles are enabled and set new allowed vehicles
    // myAC->setAttribute(SUMO_ATTR_ALLOW, joinToString(allowedVehicles, " "), myViewNet->getUndoList());
    // Stop Modal
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEGenericParameterDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEGenericParameterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    int index = 0; 
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        if(index < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
            myGenericParameterRows.at(index).enableRow(i->parameter(), i->attribute());
        } else {
            // Maximun number of generic parameter reached
            return 1;
        }
        index++;
    }
    // check if add button can be enabled
    if(index < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
        myGenericParameterRows.at(index).toogleAddButton();
    }
    return 1;
}


GNEGenericParameterDialog::GenericParameterRow::GenericParameterRow(GNEGenericParameterDialog *genericParametersEditor, FXVerticalFrame* frame) {
    horizontalFrame = new FXHorizontalFrame(frame, GUIDesignAuxiliarHorizontalFrame);
    parameterField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    removeButton = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), genericParametersEditor, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // by defaults rows are disabled
    disableRow();
}


void 
GNEGenericParameterDialog::GenericParameterRow::disableRow() {
    parameterField->setText("");
    parameterField->disable();
    valueField->setText("");
    valueField->disable();
    removeButton->disable();
}


void 
GNEGenericParameterDialog::GenericParameterRow::enableRow(const std::string &parameter, const std::string &value) const {
    parameterField->setText(parameter.c_str());
    parameterField->enable();
    valueField->setText(value.c_str());
    valueField->enable();
    removeButton->enable();
    removeButton->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void 
GNEGenericParameterDialog::GenericParameterRow::toogleAddButton() const {
    removeButton->enable();
    removeButton->setIcon(GUIIconSubSys::getIcon(ICON_ADD));
}


void 
GNEGenericParameterDialog::GenericParameterRow::copyValues(const GenericParameterRow & other) {
    parameterField->setText(other.parameterField->getText());
    valueField->setText(other.valueField->getText());
}


/****************************************************************************/
