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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,                   GNEGenericParameterDialog::onCmdRemoveAttribute),
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
    myGenericParameters(genericParameters),
    myCopyOfGenericParameters(*myGenericParameters) {
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
    // update values
    updateValues();
}


GNEGenericParameterDialog::~GNEGenericParameterDialog() {
}


long
GNEGenericParameterDialog::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find what value was changed
    for (int i = 0;  i < myGenericParameterRows.size(); i++) {
        if(myGenericParameterRows.at(i).keyField == obj) {
            myGenericParameters->at(i).key() = myGenericParameterRows.at(i).keyField->getText().text();
        } else if(myGenericParameterRows.at(i).valueField == obj) {
            myGenericParameters->at(i).value() = myGenericParameterRows.at(i).valueField->getText().text();
        }
    }
    return 1;
}


long 
GNEGenericParameterDialog::onCmdRemoveAttribute(FXObject* obj, FXSelector, void*) {
    // find what button was pressed
    for (int i = 0;  i < myGenericParameterRows.size(); i++) {
        if(myGenericParameterRows.at(i).button == obj) {
            // add a new parameter if add button was pressed, and remove it in other case
            if(myGenericParameterRows.at(i).isButtonInAddMode()) {
                myGenericParameters->push_back(GNEAttributeCarrier::GenericParameter("", ""));
                myGenericParameterRows.at(i).enableRow(myGenericParameters->back().key(), myGenericParameters->back().value());
                // toogle add button in the next row
                if((i+1) < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(i+1).toogleAddButton();
                }
            } else {
                // remove attribute moving back one position the next attributes
                for(auto j = i; j < (myGenericParameterRows.size()-1); j++) {
                    myGenericParameterRows.at(j).copyValues(myGenericParameterRows.at(j+1));
                }
                //disable add button of the next generic parameter
                if(myGenericParameters->size() < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(myGenericParameters->size()).disableRow();
                }
                // remove last generic parameter
                myGenericParameters->pop_back();
                // enable add button in the next empty row
                if(myGenericParameters->size() < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(myGenericParameters->size()).toogleAddButton();
                }
            }
            return 1;
        }
    }
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
    // restore copy of generic parameters
    (*myGenericParameters) = myCopyOfGenericParameters;
    // disable all rows
    for (auto i : myGenericParameterRows) {
        i.disableRow();
    }
    // update values
    updateValues();
    return 1;
}


GNEGenericParameterDialog::GenericParameterRow::GenericParameterRow(GNEGenericParameterDialog *genericParametersEditor, FXVerticalFrame* frame) {
    horizontalFrame = new FXHorizontalFrame(frame, GUIDesignAuxiliarHorizontalFrame);
    keyField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    button = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), genericParametersEditor, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // by defaults rows are disabled
    disableRow();
}


void 
GNEGenericParameterDialog::GenericParameterRow::disableRow() {
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void 
GNEGenericParameterDialog::GenericParameterRow::enableRow(const std::string &parameter, const std::string &value) const {
    keyField->setText(parameter.c_str());
    keyField->enable();
    valueField->setText(value.c_str());
    valueField->enable();
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void 
GNEGenericParameterDialog::GenericParameterRow::toogleAddButton() {
    // clear and disable parameter and value fields
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    // enable remove button and set "add" icon and focus
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_ADD));
    button->setFocus();
}


bool
GNEGenericParameterDialog::GenericParameterRow::isButtonInAddMode() const {
    return (button->getIcon() == GUIIconSubSys::getIcon(ICON_ADD));
}


void 
GNEGenericParameterDialog::GenericParameterRow::copyValues(const GenericParameterRow & other) {
    keyField->setText(other.keyField->getText());
    valueField->setText(other.valueField->getText());
}


void 
GNEGenericParameterDialog::updateValues() {
    int index = 0; 
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        myGenericParameterRows.at(index).enableRow(i->key(), i->value());
        index++;
    }
    // check if add button can be enabled
    if(index < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
        myGenericParameterRows.at(index).toogleAddButton();
    }
}

/****************************************************************************/
