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
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewNet.h>

#include "GNEGenericParameterDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGenericParameterDialog) GNEGenericParameterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                      GNEGenericParameterDialog::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,                   GNEGenericParameterDialog::onCmdButtonPress),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_LOAD,             GNEGenericParameterDialog::onCmdLoadGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_SAVE,             GNEGenericParameterDialog::onCmdSaveGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_CLEAR,            GNEGenericParameterDialog::onCmdClearGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_SORT,             GNEGenericParameterDialog::onCmdSortGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                   GNEGenericParameterDialog::onCmdHelpGenericParameter),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,      GNEGenericParameterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,      GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,       GNEGenericParameterDialog::onCmdReset),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_CLOSE,    0,                                          GNEGenericParameterDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNEGenericParameterDialog, FXDialogBox, GNEGenericParameterDialogMap, ARRAYNUMBER(GNEGenericParameterDialogMap))

// ===========================================================================
// static members
// ===========================================================================

const int GNEGenericParameterDialog::myGenericParameterDialogWidth = 367;
const int GNEGenericParameterDialog::myGenericParameterColumnWidth = 227;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGenericParameterDialog::GNEGenericParameterDialog(GNEViewNet *viewNet, std::vector<std::pair<std::string, std::string> > *genericParameters) :
    FXDialogBox(viewNet->getApp(), "Edit generic parameters", GUIDesignDialogBox),
    myViewNet(viewNet),
    myGenericParameters(genericParameters),
    myCopyOfGenericParameters(*myGenericParameters) {
    assert(myGenericParameters);
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for Generic Parameters and options
    FXHorizontalFrame* horizontalFrameGenericParametersAndOptions = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    // create for frames for parameters
    for(int i = 0; i < 5; i++) {
        myGenericParametersColumns.push_back(new FXVerticalFrame(horizontalFrameGenericParametersAndOptions, GUIDesignAuxiliarVerticalFrame));
        FXHorizontalFrame *horizontalFrameLabels = new FXHorizontalFrame(myGenericParametersColumns.back(), GUIDesignAuxiliarHorizontalFrame);
        new FXLabel(horizontalFrameLabels, "key", 0, GUIDesignLabelThick100);
        new FXLabel(horizontalFrameLabels, "value", 0, GUIDesignLabelThick100);
        myGenericParametersColumns.back()->hide();
    }
    // create rows for all generic parameters in groups of 20 elements
    for (int i = 0; i < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS; i++) {
        myGenericParameterRows.push_back(GenericParameterRow(this, myGenericParametersColumns.at(i/20)));
    }
    // show always first column
    myGenericParametersColumns.at(0)->show();
    // create groupbox for options
    FXGroupBox* genericParametersGroupBox = new FXGroupBox(horizontalFrameGenericParametersAndOptions, "Options", GUIDesignGroupBoxFrame);
    mySortButton = new FXButton(genericParametersGroupBox, "Sort", GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_GNE_GENERICPARAMETERS_SORT, GUIDesignButtonRectangular100x23);
    myClearButton = new FXButton(genericParametersGroupBox, "Clear", GUIIconSubSys::getIcon(ICON_CLEANJUNCTIONS), this, MID_GNE_GENERICPARAMETERS_CLEAR, GUIDesignButtonRectangular100x23);
    myLoadButton = new FXButton(genericParametersGroupBox, "Load", GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_GNE_GENERICPARAMETERS_LOAD, GUIDesignButtonRectangular100x23);
    mySaveButton = new FXButton(genericParametersGroupBox, "Save", GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_GENERICPARAMETERS_SAVE, GUIDesignButtonRectangular100x23);
    myHelpButton = new FXButton(genericParametersGroupBox, "Help", GUIIconSubSys::getIcon(ICON_HELP), this, MID_HELP, GUIDesignButtonRectangular100x23);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
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
            // change key of Generic Parameter
            myGenericParameters->at(i).first = myGenericParameterRows.at(i).keyField->getText().text();
            // change color of text field depending if attribute is valid
            if(GNEAttributeCarrier::isValidID(myGenericParameters->at(i).first)) {
                myGenericParameterRows.at(i).keyField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myGenericParameterRows.at(i).keyField->setTextColor(FXRGB(255, 0, 0));
                myGenericParameterRows.at(i).keyField->killFocus();
            }
        } else if(myGenericParameterRows.at(i).valueField == obj) {
            // change value of Generic Parameter
            myGenericParameters->at(i).second = myGenericParameterRows.at(i).valueField->getText().text();
            // change color of text field depending if attribute is valid
            if(GNEAttributeCarrier::isValidName(myGenericParameters->at(i).second)) {
                myGenericParameterRows.at(i).valueField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myGenericParameterRows.at(i).valueField->setTextColor(FXRGB(255, 0, 0));
                myGenericParameterRows.at(i).valueField->killFocus();
            }
        }
    }
    // resize dialog
    resizeGenericParameterDialog();
    return 1;
}


long 
GNEGenericParameterDialog::onCmdButtonPress(FXObject* obj, FXSelector, void*) {
    // find what button was pressed
    for (int i = 0;  i < myGenericParameterRows.size(); i++) {
        if(myGenericParameterRows.at(i).button == obj) {
            // add a new parameter if add button was pressed, and remove it in other case
            if(myGenericParameterRows.at(i).isButtonInAddMode()) {
                myGenericParameters->push_back(std::make_pair("", ""));
                myGenericParameterRows.at(i).enableRow(myGenericParameters->back().first, myGenericParameters->back().second);
                // toogle add button in the next row
                if((i+1) < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(i+1).toogleAddButton();
                    // check if a new column of Rows has to be show
                    if(myGenericParameterRows.at(i).frameParent != myGenericParameterRows.at(i+1).frameParent) {
                        myGenericParameterRows.at(i+1).frameParent->show();
                    }
                }
            } else {
                // remove attribute moving back one position the next attributes
                for(auto j = i; j < (myGenericParameterRows.size()-1); j++) {
                    myGenericParameterRows.at(j).copyValues(myGenericParameterRows.at(j+1));
                }
                // disable add button of the next generic parameter
                if(myGenericParameters->size() < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(myGenericParameters->size()).disableRow();
                    // check if a current column of Rows has to be hidden
                    if((myGenericParameters->size() > 1) && 
                       (myGenericParameterRows.at(myGenericParameters->size()).frameParent != myGenericParameterRows.at(myGenericParameters->size()-1).frameParent)) {
                        myGenericParameterRows.at(myGenericParameters->size()).frameParent->hide();
                    }
                }
                // remove last generic parameter
                myGenericParameters->pop_back();
                // enable add button in the next empty row
                if(myGenericParameters->size() < myGenericParameterRows.size()) {
                    myGenericParameterRows.at(myGenericParameters->size()).toogleAddButton();
                }
            }
            // update values
            updateValues();
        }
    }
    return 1;
}


long
GNEGenericParameterDialog::onCmdLoadGenericParameters(FXObject*, FXSelector, void*) {
    // get the Additional file name
    FXFileDialog opendialog(this, "Open Generic Parameter Template");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_GREENVEHICLE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("Generic Parameter Template files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // save current number of generic parameters
        int numberOfGenericParametersbeforeLoad = (int)myGenericParameters->size();
        // Create additional handler and run parser
        if (!XMLSubSys::runParser(GNEGenericParameterHandler(this, file), file, false)) {
            WRITE_MESSAGE("Loading of Generic Parameters From " + file + " failed.");
        }
        // show loaded attributes
        WRITE_MESSAGE("Loaded " + toString((int)myGenericParameters->size() - numberOfGenericParametersbeforeLoad) + " Generic Parameters.");
        // update values
        updateValues();
        // resize dialog
        resizeGenericParameterDialog();
    }
    return 1;
}


long 
GNEGenericParameterDialog::onCmdSaveGenericParameters(FXObject*, FXSelector, void*) {
    // obtain file to save generic parameters
    FXString file = MFXUtils::getFilename2Write(this,
                    "Select name of the Generic Parameter Template file", ".xml",
                    GUIIconSubSys::getIcon(ICON_GREENVEHICLE),
                    gCurrentFolder);
    if (file == "") {
        // None generic parameter file was selected, then stop function
        return 1;
    } else {
        OutputDevice& device = OutputDevice::getDevice(file.text());
        device.writeXMLHeader("genericParameter", "genericparameter_file.xsd");
        // iterate over all generic parameters and save it in the filename
        for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
            device.openTag(SUMO_TAG_PARAM);
            device.writeAttr(SUMO_ATTR_KEY, i->first);
            device.writeAttr(SUMO_ATTR_VALUE, i->second);
            device.closeTag();
        }
        device.close();
    }
    return 1;
}


long 
GNEGenericParameterDialog::onCmdClearGenericParameters(FXObject*, FXSelector, void*) {
    // simply clear all parameters and disble all rows
    myGenericParameters->clear();
    for (auto i : myGenericParameterRows) {
        i.disableRow();
    }
    // update values
    updateValues();
    return 1;
}


long 
GNEGenericParameterDialog::onCmdSortGenericParameters(FXObject*, FXSelector, void*) {
    // simply sort generic parameters using std function
    std::sort(myGenericParameters->begin(), myGenericParameters->end());
    // update values
    updateValues();
    return 1;
}


long 
GNEGenericParameterDialog::onCmdHelpGenericParameter(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEGenericParameterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // check if all edited generic parameters are valid
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        if(!GNEAttributeCarrier::isValidID(i->first) || !GNEAttributeCarrier::isValidName(i->second)) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox of type 'warning'");
            }
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Invalid Generic Parameters", "%s", "There are Generic Parameters with invalid characters");
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
            }
            return 1;
        }
    }
    // now check if there is duplicates generic parameters
    std::vector<std::pair<std::string, std::string> > sortedGenericParameters = (*myGenericParameters);
    std::sort(sortedGenericParameters.begin(), sortedGenericParameters.end());
    for (auto i = sortedGenericParameters.begin(); i != sortedGenericParameters.end(); i++) {
        if(((i+1) != sortedGenericParameters.end()) && (i->first) == (i+1)->first) {
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox of type 'warning'");
            }
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Duplicated Generic Parameters", "%s", "There are Generic Parameters with the same Key");
            // write warning if netedit is running in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Closed FXMessageBox of type 'warning' with 'OK'");
            }
            return 1;
        }
    }
    // all ok, then close dialog
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEGenericParameterDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // restore copy of generic parameters
    (*myGenericParameters) = myCopyOfGenericParameters;
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


GNEGenericParameterDialog::GenericParameterRow::GenericParameterRow(GNEGenericParameterDialog *genericParametersEditor, FXVerticalFrame* _frameParent) : 
    frameParent(_frameParent) {
    horizontalFrame = new FXHorizontalFrame(frameParent, GUIDesignAuxiliarHorizontalFrame);
    keyField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersEditor, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth100);
    button = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), genericParametersEditor, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // by defaults rows are disabled
    disableRow();
}


void 
GNEGenericParameterDialog::GenericParameterRow::disableRow() {
    // hide all (including frame parent)
    frameParent->hide();
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void 
GNEGenericParameterDialog::GenericParameterRow::enableRow(const std::string &parameter, const std::string &value) const {
    // show Horizontal Frame parent
    frameParent->show();
    // restore color and enable key field
    keyField->setText(parameter.c_str());
    keyField->setTextColor(FXRGB(0, 0, 0));
    keyField->enable();
    // restore color and enable value field
    valueField->setText(value.c_str());
    valueField->setTextColor(FXRGB(0, 0, 0));
    valueField->enable();
    // enable button and set icon remove
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void 
GNEGenericParameterDialog::GenericParameterRow::toogleAddButton() {
    // show Horizontal Frame parent
    frameParent->show();
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


GNEGenericParameterDialog::GNEGenericParameterHandler::GNEGenericParameterHandler(GNEGenericParameterDialog* genericParameterDialogParent, const std::string& file) :
    SUMOSAXHandler(file),
    myGenericParameterDialogParent(genericParameterDialogParent),
    myMaximumNumberOfAttributesShown(false) {
}


GNEGenericParameterDialog::GNEGenericParameterHandler::~GNEGenericParameterHandler() {}


void 
GNEGenericParameterDialog::GNEGenericParameterHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // only continue if tag is valid
    if(tag != SUMO_TAG_NOTHING) {
        // Call parse and build depending of tag
        switch (tag) {
            case SUMO_TAG_PARAM:
                // first check that number of generic parameter is lower than limit
                if(myGenericParameterDialogParent->myGenericParameters->size() < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
                    // Check that format of Generic Parameter is correct
                    if(!attrs.hasAttribute(SUMO_ATTR_KEY)) {
                        WRITE_WARNING("Key of Generic Parameter not defined");
                    } else if (!attrs.hasAttribute(SUMO_ATTR_VALUE)) {
                        WRITE_WARNING("Value of Generic Parameter not defined");
                    } else {
                        // obtain Key and value
                        std::string key = attrs.getString(SUMO_ATTR_KEY);
                        std::string value = attrs.getString(SUMO_ATTR_VALUE);
                        // check that parsed values are correct
                        if(!GNEAttributeCarrier::isValidID(key)) {
                            if(key.size() == 0) {
                                WRITE_WARNING("Key of Generic Parameter cannot be empty");
                            } else {
                                WRITE_WARNING("Key '" + key + "' of Generic Parameter contains invalid characters");
                            }
                        } else if(!GNEAttributeCarrier::isValidName(key)) {
                            WRITE_WARNING("Value '" + value + "'of Generic Parameter contains invalid characters");
                        } else {
                            // add generic parameter to vector of myGenericParameterDialogParent
                            myGenericParameterDialogParent->myGenericParameters->push_back(std::make_pair(key, value));
                        }
                    }
                } else {
                    // only show warning one time)
                    if(!myMaximumNumberOfAttributesShown) {
                        WRITE_WARNING("Maximun number of Generic Parameters reached");
                        myMaximumNumberOfAttributesShown = true;
                    }
                }
                break;
            default:
                break;
        }
    }
}


void 
GNEGenericParameterDialog::updateValues() {
    int index = 0; 
    // enanble rows
    for (auto i = myGenericParameters->begin(); i != myGenericParameters->end(); i++) {
        if(index < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
            myGenericParameterRows.at(index).enableRow(i->first, i->second);
            index++;
        }
    }
    // disable rest of rows
    for (int i = index; i < myGenericParameterRows.size(); i++) {
        myGenericParameterRows.at(i).disableRow();
    }
    // check if add button can be enabled
    if(index < GNEAttributeCarrier::MAXNUMBER_GENERICPARAMETERS) {
        myGenericParameterRows.at(index).toogleAddButton();
    }
    // resize dialog
    resizeGenericParameterDialog();
}


 void 
GNEGenericParameterDialog::resizeGenericParameterDialog() {
    // save index of the last shown column (to avoid Flick)
    int indexColumn = 0;
    for (int i = 0; i < myGenericParametersColumns.size(); i++) {
        if (myGenericParametersColumns.at(i)->shown()) {
            indexColumn = i;
        }
    }
    resize(myGenericParameterDialogWidth + (indexColumn * myGenericParameterColumnWidth), getHeight());
 }

/****************************************************************************/
