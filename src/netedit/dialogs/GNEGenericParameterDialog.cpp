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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ALLOWDISALLOW_CHANGE,               GNEGenericParameterDialog::onCmdValueChanged),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,      GNEGenericParameterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,      GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,       GNEGenericParameterDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEGenericParameterDialog, FXDialogBox, GNEGenericParameterDialogMap, ARRAYNUMBER(GNEGenericParameterDialogMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGenericParameterDialog::GNEGenericParameterDialog(GNEViewNet *viewNet, GNEAttributeCarrier *AC) :
    FXDialogBox(viewNet->getApp(), ("Edit " + toString(SUMO_ATTR_ALLOW) + " " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignDialogBox),
    myViewNet(viewNet),
    myAC(AC) {
    assert(GNEAttributeCarrier::getTagProperties(AC->getTag()).hasAttribute(SUMO_ATTR_ALLOW));
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create groupbox for options
    FXGroupBox* myGroupBoxOptions = new FXGroupBox(mainFrame, "Selection options", GUIDesignGroupBoxFrame);
    FXHorizontalFrame* myOptionsFrame = new FXHorizontalFrame(myGroupBoxOptions, GUIDesignAuxiliarHorizontalFrame);

    new FXLabel(myOptionsFrame, "Allow all vehicles", nullptr, GUIDesignLabelLeftThick);

    new FXLabel(myOptionsFrame, "Allow only non-road vehicles", nullptr, GUIDesignLabelLeftThick);

    new FXLabel(myOptionsFrame, "Disallow all vehicles", nullptr, GUIDesignLabelLeftThick);
    // create groupbox for vehicles
    FXGroupBox* myGroupBoxVehiclesFrame = new FXGroupBox(mainFrame, ("Select " + toString(SUMO_ATTR_VCLASS) + "es").c_str(), GUIDesignGroupBoxFrame);
    // Create frame for vehicles's columns
    FXHorizontalFrame* myVehiclesFrame = new FXHorizontalFrame(myGroupBoxVehiclesFrame, GUIDesignContentsFrame);
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
GNEGenericParameterDialog::onCmdValueChanged(FXObject* obj, FXSelector, void*) {
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
    return 1;
}


GNEGenericParameterDialog::GenericParameterRow::GenericParameterRow(GNEGenericParameterDialog *genericParametersEditor) {
    horizontalFrame = new FXHorizontalFrame(genericParametersEditor, GUIDesignAuxiliarHorizontalFrame);
    parameterField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    removeButton = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), genericParametersEditor, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // by defaults rows are hidden
    hideRow();
}


void 
GNEGenericParameterDialog::GenericParameterRow::GenericParameterRow::showRow(bool clear) {
    horizontalFrame->show();
    parameterField->show();
    valueField->show();
    removeButton->show();
    if(clear) {
        parameterField->setText("");
        valueField->setText("");
    }
    horizontalFrame->getParent()->recalc();
}


void 
GNEGenericParameterDialog::GenericParameterRow::hideRow() {
    horizontalFrame->hide();
    parameterField->hide();
    valueField->hide();
    removeButton->hide();
    horizontalFrame->getParent()->recalc();
}


bool 
GNEGenericParameterDialog::GenericParameterRow::shown() const {
    return horizontalFrame->shown();
}


void 
GNEGenericParameterDialog::GenericParameterRow::copyValues(const GenericParameterRow & other) {
    parameterField->setText(other.parameterField->getText());
    valueField->setText(other.valueField->getText());
}


/****************************************************************************/
