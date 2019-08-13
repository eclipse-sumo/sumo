/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEViewNet.h>

#include "GNEGenericParameterDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEGenericParameterDialog) GNEGenericParameterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT,      GNEGenericParameterDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL,      GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDITIONALDIALOG_BUTTONRESET,       GNEGenericParameterDialog::onCmdReset),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL,                     GNEGenericParameterDialog::onCmdCancel),
    FXMAPFUNC(SEL_CLOSE,    0,                                          GNEGenericParameterDialog::onCmdCancel),
};

FXDEFMAP(GNEGenericParameterDialog::GenericParametersValues) GenericParametersValuesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                      GNEGenericParameterDialog::GenericParametersValues::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,                   GNEGenericParameterDialog::GenericParametersValues::onCmdButtonPress),
    FXMAPFUNC(SEL_PAINT,    0,                                          GNEGenericParameterDialog::GenericParametersValues::onPaint),
};

FXDEFMAP(GNEGenericParameterDialog::GenericParametersOptions) GenericParametersOptionsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_LOAD,             GNEGenericParameterDialog::GenericParametersOptions::onCmdLoadGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_SAVE,             GNEGenericParameterDialog::GenericParametersOptions::onCmdSaveGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_CLEAR,            GNEGenericParameterDialog::GenericParametersOptions::onCmdClearGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_GENERICPARAMETERS_SORT,             GNEGenericParameterDialog::GenericParametersOptions::onCmdSortGenericParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                                   GNEGenericParameterDialog::GenericParametersOptions::onCmdHelpGenericParameter),
};

// Object implementation
FXIMPLEMENT(GNEGenericParameterDialog, FXDialogBox, GNEGenericParameterDialogMap, ARRAYNUMBER(GNEGenericParameterDialogMap))
FXIMPLEMENT(GNEGenericParameterDialog::GenericParametersValues, FXGroupBox, GenericParametersValuesMap, ARRAYNUMBER(GenericParametersValuesMap))
FXIMPLEMENT(GNEGenericParameterDialog::GenericParametersOptions, FXGroupBox, GenericParametersOptionsMap, ARRAYNUMBER(GenericParametersOptionsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGenericParameterDialog::GenericParametersValues - methods
// ---------------------------------------------------------------------------

GNEGenericParameterDialog::GenericParametersValues::GenericParametersValues(FXHorizontalFrame* frame, std::vector<std::pair<std::string, std::string> >* genericParameters) :
    FXGroupBox(frame, "Generic Parameters", GUIDesignGroupBoxFrameFill),
    myGenericParameters(genericParameters) {
    // create labels for keys and values
    FXHorizontalFrame* horizontalFrameLabels = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myKeyLabel = new FXLabel(horizontalFrameLabels, "key", nullptr, GUIDesignLabelThick100);
    new FXLabel(horizontalFrameLabels, "value", nullptr, GUIDesignLabelCenterThick);
    // create scroll windows
    FXScrollWindow* scrollWindow = new FXScrollWindow(this, LAYOUT_FILL);
    // create vertical frame for rows
    myVerticalFrameRow = new FXVerticalFrame(scrollWindow, GUIDesignAuxiliarFrame);
    // update values
    updateValues();
}


GNEGenericParameterDialog::GenericParametersValues::~GenericParametersValues() {}


void
GNEGenericParameterDialog::GenericParametersValues::updateValues() {
    // first show the correct number of rows
    while ((myGenericParameters->size() + 1) < myGenericParameterRows.size()) {
        delete myGenericParameterRows.back();
        myGenericParameterRows.pop_back();
    }
    while ((myGenericParameters->size() + 1) > myGenericParameterRows.size()) {
        myGenericParameterRows.push_back(new GenericParameterRow(this, myVerticalFrameRow));
    }
    // fill rows
    for (int i = 0; i < (int)myGenericParameters->size(); i++) {
        myGenericParameterRows.at(i)->enableRow(myGenericParameters->at(i).first, myGenericParameters->at(i).second);
    }
    // set last myGenericParameterRows with the add button
    myGenericParameterRows.back()->toogleAddButton();
}


const std::vector<std::pair<std::string, std::string> >*
GNEGenericParameterDialog::GenericParametersValues::getGenericParameters() const {
    return myGenericParameters;
}


std::vector<std::pair<std::string, std::string> >
GNEGenericParameterDialog::GenericParametersValues::getCopyOfGenericParameters() const {
    return *myGenericParameters;
}


void
GNEGenericParameterDialog::GenericParametersValues::setGenericParameters(const std::vector<std::pair<std::string, std::string> >& newGenericParameters) {
    *myGenericParameters = newGenericParameters;
    // update values
    updateValues();
}


void
GNEGenericParameterDialog::GenericParametersValues::addGenericParameter(std::pair<std::string, std::string> newGenericParameter) {
    myGenericParameters->push_back(newGenericParameter);
    // update values
    updateValues();
}


void
GNEGenericParameterDialog::GenericParametersValues::clearGenericParameters() {
    myGenericParameters->clear();
    updateValues();
}


long
GNEGenericParameterDialog::GenericParametersValues::onPaint(FXObject* o, FXSelector f, void* p) {
    // size of key label has to be updated in every interation
    if (myGenericParameterRows.size() > 0) {
        myKeyLabel->setWidth(myGenericParameterRows.front()->keyField->getWidth());
    }
    return FXGroupBox::onPaint(o, f, p);
}


long
GNEGenericParameterDialog::GenericParametersValues::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find what value was changed
    for (int i = 0; i < (int)myGenericParameterRows.size(); i++) {
        if (myGenericParameterRows.at(i)->keyField == obj) {
            // change key of Generic Parameter
            myGenericParameters->at(i).first = myGenericParameterRows.at(i)->keyField->getText().text();
            // change color of text field depending if key is valid or empty
            if (myGenericParameters->at(i).first.empty() || SUMOXMLDefinitions::isValidGenericParameterKey(myGenericParameters->at(i).first)) {
                myGenericParameterRows.at(i)->keyField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myGenericParameterRows.at(i)->keyField->setTextColor(FXRGB(255, 0, 0));
                myGenericParameterRows.at(i)->keyField->killFocus();
            }
        } else if (myGenericParameterRows.at(i)->valueField == obj) {
            // change value of Generic Parameter
            myGenericParameters->at(i).second = myGenericParameterRows.at(i)->valueField->getText().text();
            // change color of text field depending if attribute is valid
            if (SUMOXMLDefinitions::isValidGenericParameterValue(myGenericParameters->at(i).second)) {
                myGenericParameterRows.at(i)->valueField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myGenericParameterRows.at(i)->valueField->setTextColor(FXRGB(255, 0, 0));
                myGenericParameterRows.at(i)->valueField->killFocus();
            }
        }
    }
    return 1;
}


long
GNEGenericParameterDialog::GenericParametersValues::onCmdButtonPress(FXObject* obj, FXSelector, void*) {
    // first check if add button was pressed
    if (myGenericParameterRows.back()->button == obj) {
        // create new generic parameter
        myGenericParameters->push_back(std::make_pair("", ""));
        // update values and finish
        updateValues();
        return 1;
    } else {
        // in other case, button press was a "remove button". Find id and remove the Generic Parameter
        for (int i = 0;  i < (int)myGenericParameterRows.size(); i++) {
            if (myGenericParameterRows.at(i)->button == obj && i < (int)myGenericParameters->size()) {
                // remove generic parameter
                myGenericParameters->erase(myGenericParameters->begin() + i);
                // update values and finish
                updateValues();
                return 1;
            }
        }
    }
    // Nothing to do
    return 1;
}


GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::GenericParameterRow(GenericParametersValues* genericParametersValues, FXVerticalFrame* verticalFrameParent) {
    horizontalFrame = new FXHorizontalFrame(verticalFrameParent, GUIDesignAuxiliarHorizontalFrame);
    keyField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, genericParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    button = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), genericParametersValues, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // only create elements if vertical frame was previously created
    if (verticalFrameParent->id()) {
        horizontalFrame->create();
    }
    // by defaults rows are disabled
    disableRow();
}


GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::~GenericParameterRow() {
    // simply delete horizontalFrame (rest of elements will be automatic deleted due they are children of horizontal frame)
    delete horizontalFrame;
}


void
GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::disableRow() {
    // hide all
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void
GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::enableRow(const std::string& parameter, const std::string& value) const {
    // restore color and enable key field
    keyField->setText(parameter.c_str());
    if (parameter.empty() || SUMOXMLDefinitions::isValidGenericParameterKey(parameter)) {
        keyField->setTextColor(FXRGB(0, 0, 0));
    } else {
        keyField->setTextColor(FXRGB(255, 0, 0));
    }
    keyField->enable();
    // restore color and enable value field
    valueField->setText(value.c_str());
    if (SUMOXMLDefinitions::isValidGenericParameterValue(value)) {
        valueField->setTextColor(FXRGB(0, 0, 0));
    } else {
        valueField->setTextColor(FXRGB(255, 0, 0));
    }
    valueField->enable();
    // enable button and set icon remove
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(ICON_REMOVE));
}


void
GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::toogleAddButton() {
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
GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::isButtonInAddMode() const {
    return (button->getIcon() == GUIIconSubSys::getIcon(ICON_ADD));
}


void
GNEGenericParameterDialog::GenericParametersValues::GenericParameterRow::copyValues(const GenericParameterRow& other) {
    keyField->setText(other.keyField->getText());
    valueField->setText(other.valueField->getText());
}

// ---------------------------------------------------------------------------
// GNEGenericParameterDialog::GenericParametersOptions - methods
// ---------------------------------------------------------------------------

GNEGenericParameterDialog::GenericParametersOptions::GenericParametersOptions(FXHorizontalFrame* frame, GNEGenericParameterDialog* genericParameterDialogParent) :
    FXGroupBox(frame, "Options", GUIDesignGroupBoxFrame100),
    myGenericParameterDialogParent(genericParameterDialogParent) {
    // create buttons
    mySortButton = new FXButton(this, "Sort", GUIIconSubSys::getIcon(ICON_RELOAD), this, MID_GNE_GENERICPARAMETERS_SORT, GUIDesignButtonRectangular100x23);
    myClearButton = new FXButton(this, "Clear", GUIIconSubSys::getIcon(ICON_CLEANJUNCTIONS), this, MID_GNE_GENERICPARAMETERS_CLEAR, GUIDesignButtonRectangular100x23);
    myLoadButton = new FXButton(this, "Load", GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_GNE_GENERICPARAMETERS_LOAD, GUIDesignButtonRectangular100x23);
    mySaveButton = new FXButton(this, "Save", GUIIconSubSys::getIcon(ICON_SAVE), this, MID_GNE_GENERICPARAMETERS_SAVE, GUIDesignButtonRectangular100x23);
    myHelpButton = new FXButton(this, "Help", GUIIconSubSys::getIcon(ICON_HELP), this, MID_HELP, GUIDesignButtonRectangular100x23);
}


GNEGenericParameterDialog::GenericParametersOptions::~GenericParametersOptions() {}


long
GNEGenericParameterDialog::GenericParametersOptions::onCmdLoadGenericParameters(FXObject*, FXSelector, void*) {
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
        int numberOfGenericParametersbeforeLoad = (int)myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->size();
        // Create additional handler and run parser
        GNEGenericParameterHandler handler(myGenericParameterDialogParent, file);
        if (!XMLSubSys::runParser(handler, file, false)) {
            WRITE_MESSAGE("Loading of Generic Parameters From " + file + " failed.");
        }
        // show loaded attributes
        WRITE_MESSAGE("Loaded " + toString((int)myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->size() - numberOfGenericParametersbeforeLoad) + " Generic Parameters.");
        // update values
        myGenericParameterDialogParent->myGenericParametersValues->updateValues();
    }
    return 1;
}


long
GNEGenericParameterDialog::GenericParametersOptions::onCmdSaveGenericParameters(FXObject*, FXSelector, void*) {
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
        for (auto i = myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->begin(); i != myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->end(); i++) {
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
GNEGenericParameterDialog::GenericParametersOptions::onCmdClearGenericParameters(FXObject*, FXSelector, void*) {
    // simply clear generic parameters from GenericParametersValues
    myGenericParameterDialogParent->myGenericParametersValues->clearGenericParameters();
    return 1;
}


long
GNEGenericParameterDialog::GenericParametersOptions::onCmdSortGenericParameters(FXObject*, FXSelector, void*) {
    std::vector<std::pair<std::string, std::string> > genericParametersNoEmpty;
    std::vector<std::string> valuesEmpty;
    // first extract empty values
    for (auto i = myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->begin(); i != myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->end(); i++) {
        if (!i->first.empty()) {
            genericParametersNoEmpty.push_back(*i);
        } else if (i->first.empty() && !i->second.empty()) {
            valuesEmpty.push_back(i->second);
        }
    }
    // now sort non-empty generic parameters
    std::sort(genericParametersNoEmpty.begin(), genericParametersNoEmpty.end());
    // add values without key
    for (auto i : valuesEmpty) {
        genericParametersNoEmpty.push_back(std::make_pair("", i));
    }
    // fill genericParametersNoEmpty with empty values
    while (genericParametersNoEmpty.size() < myGenericParameterDialogParent->myGenericParametersValues->getGenericParameters()->size()) {
        genericParametersNoEmpty.push_back(std::make_pair("", ""));
    }
    // finally replace generic parameters in myGenericParametersValues with genericParametersNoEmpty
    myGenericParameterDialogParent->myGenericParametersValues->setGenericParameters(genericParametersNoEmpty);
    // update values
    myGenericParameterDialogParent->myGenericParametersValues->updateValues();
    return 1;
}


long
GNEGenericParameterDialog::GenericParametersOptions::onCmdHelpGenericParameter(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* GenericParameterHelpDialog = new FXDialogBox(this, "Generic Parameters Help", GUIDesignDialogBox);
    GenericParameterHelpDialog->setIcon(GUIIconSubSys::getIcon(ICON_APP_TABLE));
    // set help text
    std::ostringstream help;
    help
            << "- Generic Parameters are defined by a Key and a Value.\n"
            << "- In Netedit can be defined using format key1=parameter1|key2=parameter2|...\n"
            << " - Duplicated and empty Keys aren't valid.\n"
            << " - Certain characters aren't allowed (\t\n\r@$%^&/|\\....)\n";
    // Create label with the help text
    new FXLabel(GenericParameterHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(GenericParameterHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(GenericParameterHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), GenericParameterHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening Generic Parameter help dialog");
    // create Dialog
    GenericParameterHelpDialog->create();
    // show in the given position
    GenericParameterHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(GenericParameterHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing Generic Parameter help dialog");
    return 1;
}


GNEGenericParameterDialog::GenericParametersOptions::GNEGenericParameterHandler::GNEGenericParameterHandler(GNEGenericParameterDialog* genericParameterDialogParent, const std::string& file) :
    SUMOSAXHandler(file),
    myGenericParameterDialogParent(genericParameterDialogParent) {
}


GNEGenericParameterDialog::GenericParametersOptions::GNEGenericParameterHandler::~GNEGenericParameterHandler() {}


void
GNEGenericParameterDialog::GenericParametersOptions::GNEGenericParameterHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // only continue if tag is valid
    if (tag != SUMO_TAG_NOTHING) {
        // Call parse and build depending of tag
        switch (tag) {
            case SUMO_TAG_PARAM:
                // Check that format of Generic Parameter is correct
                if (!attrs.hasAttribute(SUMO_ATTR_KEY)) {
                    WRITE_WARNING("Key of Generic Parameter not defined");
                } else if (!attrs.hasAttribute(SUMO_ATTR_VALUE)) {
                    WRITE_WARNING("Value of Generic Parameter not defined");
                } else {
                    // obtain Key and value
                    std::string key = attrs.getString(SUMO_ATTR_KEY);
                    std::string value = attrs.getString(SUMO_ATTR_VALUE);
                    // check that parsed values are correct
                    if (!SUMOXMLDefinitions::isValidGenericParameterKey(key)) {
                        if (key.size() == 0) {
                            WRITE_WARNING("Key of Generic Parameter cannot be empty");
                        } else {
                            WRITE_WARNING("Key '" + key + "' of Generic Parameter contains invalid characters");
                        }
                    } else if (!SUMOXMLDefinitions::isValidGenericParameterValue(value)) {
                        WRITE_WARNING("Value '" + value + "'of Generic Parameter contains invalid characters");
                    } else {
                        // add generic parameter to vector of myGenericParameterDialogParent
                        myGenericParameterDialogParent->myGenericParametersValues->addGenericParameter(std::make_pair(key, value));
                    }
                }
                break;
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEGenericParameterDialog - methods
// ---------------------------------------------------------------------------

GNEGenericParameterDialog::GNEGenericParameterDialog(GNEViewNet* viewNet, std::vector<std::pair<std::string, std::string> >* genericParameters) :
    FXDialogBox(viewNet->getApp(), "Edit generic parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myViewNet(viewNet),
    myCopyOfGenericParameters(*genericParameters) {
    assert(genericParameters);
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(ICON_APP_TABLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for Generic Parameters and options
    FXHorizontalFrame* horizontalFrameGenericParametersAndOptions = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarFrame);
    // create generic parameters values
    myGenericParametersValues = new GenericParametersValues(horizontalFrameGenericParametersAndOptions, genericParameters);
    // create generic parameters options
    myGenericParametersOptions = new GenericParametersOptions(horizontalFrameGenericParametersAndOptions, this);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), this, MID_GNE_ADDITIONALDIALOG_BUTTONACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(ICON_CANCEL), this, MID_GNE_ADDITIONALDIALOG_BUTTONCANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(ICON_RESET), this, MID_GNE_ADDITIONALDIALOG_BUTTONRESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEGenericParameterDialog::~GNEGenericParameterDialog() {}


long
GNEGenericParameterDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // check if all edited generic parameters are valid
    for (auto i = myGenericParametersValues->getGenericParameters()->begin(); i != myGenericParametersValues->getGenericParameters()->end(); i++) {
        if (i->first.empty()) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Empty Generic Parameter key", "%s", "Generic Parameters with empty keys aren't allowed");
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
            return 1;
        } else if (!SUMOXMLDefinitions::isValidGenericParameterKey(i->first)) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Invalid Generic Parameter key", "%s", "There are keys of Generic Parameters with invalid characters");
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
            return 1;
        } else if (!SUMOXMLDefinitions::isValidGenericParameterValue(i->second)) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Invalid Generic Parameter value", "%s", "There are values of Generic Parameters with invalid characters");
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");

            return 1;
        }
    }
    // now check if there is duplicates generic parameters
    std::vector<std::pair<std::string, std::string> > sortedGenericParameters = myGenericParametersValues->getCopyOfGenericParameters();
    std::sort(sortedGenericParameters.begin(), sortedGenericParameters.end());
    for (auto i = sortedGenericParameters.begin(); i != sortedGenericParameters.end(); i++) {
        if (((i + 1) != sortedGenericParameters.end()) && (i->first) == (i + 1)->first) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Duplicated Generic Parameters", "%s", "Generic Parameters with the same Key aren't allowed");
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
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
    myGenericParametersValues->setGenericParameters(myCopyOfGenericParameters);
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEGenericParameterDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // simply restore copy of generic parameters and continue editing
    myGenericParametersValues->setGenericParameters(myCopyOfGenericParameters);
    return 1;
}

/****************************************************************************/
