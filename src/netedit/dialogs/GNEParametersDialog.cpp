/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEParametersDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2018
///
// Dialog for edit parameters
/****************************************************************************/

#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/basic/GNEHelpBasicDialog.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEParametersDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEParametersDialog) GNEParametersDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEParametersDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNEParametersDialog::onCmdReset),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, GNEParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, GNEParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, GNEParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEParametersDialog::onCmdCancel),
};

FXDEFMAP(GNEParametersDialog::ParametersValues) ParametersValuesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNEParametersDialog::ParametersValues::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,   GNEParametersDialog::ParametersValues::onCmdButtonPress),
    FXMAPFUNC(SEL_PAINT,    0,                          GNEParametersDialog::ParametersValues::onPaint),
};

FXDEFMAP(GNEParametersDialog::ParametersOperations) ParametersOperationsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_LOAD,    GNEParametersDialog::ParametersOperations::onCmdLoadParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNEParametersDialog::ParametersOperations::onCmdSaveParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CLEAR,   GNEParametersDialog::ParametersOperations::onCmdClearParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SORT,    GNEParametersDialog::ParametersOperations::onCmdSortParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEParametersDialog::ParametersOperations::onCmdHelpParameter),
};

// Object implementation
FXIMPLEMENT(GNEParametersDialog,                          GNEDialog,   GNEParametersDialogMap,   ARRAYNUMBER(GNEParametersDialogMap))
FXIMPLEMENT(GNEParametersDialog::ParametersValues,        FXGroupBox,     ParametersValuesMap,            ARRAYNUMBER(ParametersValuesMap))
FXIMPLEMENT(GNEParametersDialog::ParametersOperations,    FXGroupBox,     ParametersOperationsMap,        ARRAYNUMBER(ParametersOperationsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEParametersDialog::ParametersValues - methods
// ---------------------------------------------------------------------------

GNEParametersDialog::ParametersValues::ParametersValues(FXHorizontalFrame* frame, const std::string& name) :
    FXGroupBox(frame, name.c_str(), GUIDesignGroupBoxFrameFill) {
    // create labels for keys and values
    FXHorizontalFrame* horizontalFrameLabels = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myKeyLabel = new FXLabel(horizontalFrameLabels, "key", nullptr, GUIDesignLabelThickedFixed(100));
    new FXLabel(horizontalFrameLabels, "value", nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // create scroll windows
    FXScrollWindow* scrollWindow = new FXScrollWindow(this, LAYOUT_FILL);
    // create vertical frame for rows
    myVerticalFrameRow = new FXVerticalFrame(scrollWindow, GUIDesignAuxiliarFrame);
}


GNEParametersDialog::ParametersValues::~ParametersValues() {}


void
GNEParametersDialog::ParametersValues::setParameters(const Parameterised::Map& newParameters) {
    // clear rows
    clearParameters();
    // iterate over parameters
    for (const auto& newParameter : newParameters) {
        addParameter(newParameter);
    }
}


void
GNEParametersDialog::ParametersValues::setParameters(const std::vector<std::pair<std::string, std::string> >& newParameters) {
    // clear rows
    clearParameters();
    // iterate over parameters
    for (const auto& newParameter : newParameters) {
        addParameter(newParameter);
    }
}


void
GNEParametersDialog::ParametersValues::addParameter(std::pair<std::string, std::string> newParameter) {
    // enable last row
    myParameterRows.back()->enableRow(newParameter.first, newParameter.second);
    // add row
    myParameterRows.push_back(new ParameterRow(this, myVerticalFrameRow));
    // enable add button in the last row
    myParameterRows.back()->toggleAddButton();
}


void
GNEParametersDialog::ParametersValues::clearParameters() {
    // iterate over all rows
    for (const auto& parameterRow : myParameterRows) {
        delete parameterRow;
    }
    //clear myParameterRows;
    myParameterRows.clear();
    // add row
    myParameterRows.push_back(new ParameterRow(this, myVerticalFrameRow));
    // enable add button in the last row
    myParameterRows.back()->toggleAddButton();
}


const std::vector<GNEParametersDialog::ParametersValues::ParameterRow*>
GNEParametersDialog::ParametersValues::getParameterRows() const {
    return myParameterRows;
}


bool
GNEParametersDialog::ParametersValues::keyExist(const std::string& key) const {
    // just iterate over myParameterRows and compare key
    for (const auto& row : myParameterRows) {
        if (row->keyField->getText().text() == key) {
            return true;
        }
    }
    return false;
}


long
GNEParametersDialog::ParametersValues::onPaint(FXObject* o, FXSelector f, void* p) {
    // size of key label has to be updated in every interation
    if (myParameterRows.size() > 0) {
        myKeyLabel->setWidth(myParameterRows.front()->keyField->getWidth());
    }
    return FXGroupBox::onPaint(o, f, p);
}


long
GNEParametersDialog::ParametersValues::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find what value was changed
    for (int i = 0; i < (int)myParameterRows.size(); i++) {
        if (myParameterRows.at(i)->keyField == obj) {
            // change color of text field depending if key is valid or empty
            if (myParameterRows.at(i)->keyField->getText().empty() || SUMOXMLDefinitions::isValidParameterKey(myParameterRows.at(i)->keyField->getText().text())) {
                myParameterRows.at(i)->keyField->setTextColor(GUIDesignTextColorBlack);
            } else {
                myParameterRows.at(i)->keyField->setTextColor(GUIDesignTextColorRed);
                myParameterRows.at(i)->keyField->killFocus();
            }
        }
    }
    return 1;
}


long
GNEParametersDialog::ParametersValues::onCmdButtonPress(FXObject* obj, FXSelector, void*) {
    // first check if add button was pressed
    if (myParameterRows.back()->button == obj) {
        // create new parameter
        addParameter(std::make_pair("", ""));
        return 1;
    } else {
        // in other case, button press was a "remove button". Find id and remove the Parameter
        for (int i = 0;  i < (int)myParameterRows.size(); i++) {
            if (myParameterRows.at(i)->button == obj) {
                // delete row
                delete myParameterRows.at(i);
                // just remove row
                myParameterRows.erase(myParameterRows.begin() + i);
                return 1;
            }
        }
    }
    // Nothing to do
    return 1;
}


GNEParametersDialog::ParametersValues::ParameterRow::ParameterRow(ParametersValues* ParametersValues, FXVerticalFrame* verticalFrameParent) {
    horizontalFrame = new FXHorizontalFrame(verticalFrameParent, GUIDesignAuxiliarHorizontalFrame);
    keyField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, ParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, ParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    button = GUIDesigns::buildFXButton(horizontalFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), ParametersValues, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // only create elements if vertical frame was previously created
    if (verticalFrameParent->id()) {
        horizontalFrame->create();
    }
    // by defaults rows are disabled
    disableRow();
}


GNEParametersDialog::ParametersValues::ParameterRow::~ParameterRow() {
    // simply delete horizontalFrame (rest of elements will be automatic deleted due they are children of horizontal frame)
    delete horizontalFrame;
}


void
GNEParametersDialog::ParametersValues::ParameterRow::disableRow() {
    // hide all
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(GUIIcon::REMOVE));
}


void
GNEParametersDialog::ParametersValues::ParameterRow::enableRow(const std::string& parameter, const std::string& value) const {
    // restore color and enable key field
    keyField->setText(parameter.c_str());
    if (parameter.empty() || SUMOXMLDefinitions::isValidParameterKey(parameter)) {
        keyField->setTextColor(GUIDesignTextColorBlack);
    } else {
        keyField->setTextColor(GUIDesignTextColorRed);
    }
    keyField->enable();
    // restore color and enable value field
    valueField->setText(value.c_str());
    valueField->enable();
    // enable button and set icon remove
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(GUIIcon::REMOVE));
}


void
GNEParametersDialog::ParametersValues::ParameterRow::toggleAddButton() {
    // clear and disable parameter and value fields
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    // enable remove button and set "add" icon and focus
    button->enable();
    button->setIcon(GUIIconSubSys::getIcon(GUIIcon::ADD));
    button->setFocus();
}


bool
GNEParametersDialog::ParametersValues::ParameterRow::isButtonInAddMode() const {
    return (button->getIcon() == GUIIconSubSys::getIcon(GUIIcon::ADD));
}


void
GNEParametersDialog::ParametersValues::ParameterRow::copyValues(const ParameterRow& other) {
    keyField->setText(other.keyField->getText());
    valueField->setText(other.valueField->getText());
}

// ---------------------------------------------------------------------------
// GNEParametersDialog::ParametersOperations - methods
// ---------------------------------------------------------------------------

GNEParametersDialog::ParametersOperations::ParametersOperations(FXHorizontalFrame* frame, GNEParametersDialog* ParameterDialogParent) :
    FXGroupBox(frame, TL("Operations"), GUIDesignGroupBoxFrame100),
    myParameterDialogParent(ParameterDialogParent) {
    // create buttons
    mySortButton =  GUIDesigns::buildFXButton(this, TL("Sort"), "", "", GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_GNE_BUTTON_SORT, GUIDesignButtonFixed(100));
    myClearButton = GUIDesigns::buildFXButton(this, TL("Clear"), "", "", GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), this, MID_GNE_BUTTON_CLEAR, GUIDesignButtonFixed(100));
    myLoadButton =  GUIDesigns::buildFXButton(this, TL("Load"), "", "",  GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_BUTTON_LOAD, GUIDesignButtonFixed(100));
    mySaveButton =  GUIDesigns::buildFXButton(this, TL("Save"), "", "",  GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_BUTTON_SAVE, GUIDesignButtonFixed(100));
    myHelpButton =  GUIDesigns::buildFXButton(this, TL("Help"), "", "",  GUIIconSubSys::getIcon(GUIIcon::HELP), this, MID_HELP, GUIDesignButtonFixed(100));
}


GNEParametersDialog::ParametersOperations::~ParametersOperations() {}


long
GNEParametersDialog::ParametersOperations::onCmdLoadParameters(FXObject*, FXSelector, void*) {
    // get the Additional file name
    FXFileDialog opendialog(this, TL("Open Parameter Template"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(SUMOXMLDefinitions::XMLFileExtensions.getMultilineString().c_str());
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // save current number of parameters
        const int numberOfParametersbeforeLoad = (int)myParameterDialogParent->myParametersValues->getParameterRows().size();
        // Create additional handler and run parser
        GNEParameterHandler handler(this, file);
        if (!XMLSubSys::runParser(handler, file, false)) {
            WRITE_MESSAGEF(TL("Loading of Parameters From % failed."), file);
        }
        // show loaded attributes
        WRITE_MESSAGEF(TL("Loaded % Parameters."), toString((int)myParameterDialogParent->myParametersValues->getParameterRows().size() - numberOfParametersbeforeLoad));
    }
    return 1;
}


long
GNEParametersDialog::ParametersOperations::onCmdSaveParameters(FXObject*, FXSelector, void*) {
    // obtain file to save parameters
    FXString file = MFXUtils::getFilename2Write(this,
                    TL("Save Parameter Template file"),
                    SUMOXMLDefinitions::XMLFileExtensions.getMultilineString().c_str(),
                    GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE),
                    gCurrentFolder);
    if (file == "") {
        // None parameter file was selected, then stop function
        return 1;
    } else {
        // open device
        OutputDevice& device = OutputDevice::getDevice(file.text());
        // write header
        device.writeXMLHeader("Parameter", "parameter_file.xsd");
        // iterate over all parameters and save it in the filename
        for (const auto& row : myParameterDialogParent->myParametersValues->getParameterRows()) {
            // write all except last
            if (row != myParameterDialogParent->myParametersValues->getParameterRows().back()) {
                // open tag
                device.openTag(SUMO_TAG_PARAM);
                // write key
                device.writeAttr(SUMO_ATTR_KEY, row->keyField->getText().text());
                // write value
                device.writeAttr(SUMO_ATTR_VALUE, row->valueField->getText().text());
                // close tag
                device.closeTag();
            }
        }
        // close device
        device.close();
    }
    return 1;
}


long
GNEParametersDialog::ParametersOperations::onCmdClearParameters(FXObject*, FXSelector, void*) {
    // simply clear parameters from ParametersValues
    myParameterDialogParent->myParametersValues->clearParameters();
    return 1;
}


long
GNEParametersDialog::ParametersOperations::onCmdSortParameters(FXObject*, FXSelector, void*) {
    // declare two containers for parameters
    std::vector<std::pair<std::string, std::string> > nonEmptyKeyValues;
    std::vector<std::string> emptyKeyValues;
    // first extract empty values
    for (const auto& parameterRow : myParameterDialogParent->myParametersValues->getParameterRows()) {
        // check if key is empty
        if (!parameterRow->keyField->getText().empty()) {
            nonEmptyKeyValues.push_back(std::make_pair(parameterRow->keyField->getText().text(), parameterRow->valueField->getText().text()));
        } else if (!parameterRow->valueField->getText().empty()) {
            emptyKeyValues.push_back(parameterRow->valueField->getText().text());
        }
    }
    // sort non-empty parameters
    std::sort(nonEmptyKeyValues.begin(), nonEmptyKeyValues.end());
    // sort non-empty parameters
    std::sort(emptyKeyValues.begin(), emptyKeyValues.end());
    // add values without key
    for (const auto& emptyKeyValue : emptyKeyValues) {
        nonEmptyKeyValues.push_back(std::make_pair("", emptyKeyValue));
    }
    // finally setparameters in myParametersValues
    myParameterDialogParent->myParametersValues->setParameters(nonEmptyKeyValues);
    return 1;
}


long
GNEParametersDialog::ParametersOperations::onCmdHelpParameter(FXObject*, FXSelector, void*) {
    // set help text
    std::ostringstream help;
    help
            << TL("- Parameters are defined by a Key and a Value.") << "\n"
            << TL("- In Netedit can be defined using format key1=parameter1|key2=parameter2|...") << "\n"
            << TL("- Duplicated and empty Keys aren't valid.") << "\n"
            << TL("- Whitespace and certain characters aren't allowed (@$%^&/|\\....)");
    // create and open dialog
    GNEHelpBasicDialog(myParameterDialogParent->getApplicationWindow(),
                       TL("Parameters Help"), help);
    return 1;
}


GNEParametersDialog::ParametersOperations::GNEParameterHandler::GNEParameterHandler(ParametersOperations* ParametersOperationsParent, const std::string& file) :
    SUMOSAXHandler(file),
    myParametersOperationsParent(ParametersOperationsParent) {
}


GNEParametersDialog::ParametersOperations::GNEParameterHandler::~GNEParameterHandler() {}


void
GNEParametersDialog::ParametersOperations::GNEParameterHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // only continue if tag is valid
    if (element != SUMO_TAG_NOTHING) {
        // Call parse and build depending of tag
        switch (element) {
            case SUMO_TAG_PARAM:
                // Check that format of Parameter is correct
                if (!attrs.hasAttribute(SUMO_ATTR_KEY)) {
                    WRITE_WARNING(TL("Key of Parameter not defined"));
                } else if (!attrs.hasAttribute(SUMO_ATTR_VALUE)) {
                    WRITE_WARNING(TL("Value of Parameter not defined"));
                } else {
                    // obtain Key and value
                    std::string key = attrs.getString(SUMO_ATTR_KEY);
                    std::string value = attrs.getString(SUMO_ATTR_VALUE);
                    // check that parsed values are correct
                    if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
                        if (key.size() == 0) {
                            WRITE_WARNING(TL("Key of Parameter cannot be empty"));
                        } else {
                            WRITE_WARNINGF(TL("Key '%' of Parameter contains invalid characters"), key);
                        }
                    } else if (myParametersOperationsParent->myParameterDialogParent->myParametersValues->keyExist(key)) {
                        WRITE_WARNINGF(TL("Key '%' already exist"), key);
                    } else {
                        // add parameter to vector of myParameterDialogParent
                        myParametersOperationsParent->myParameterDialogParent->myParametersValues->addParameter(std::make_pair(key, value));
                    }
                }
                break;
            default:
                break;
        }
    }
}

// ---------------------------------------------------------------------------
// GNEParametersDialog - methods
// ---------------------------------------------------------------------------

GNEParametersDialog::GNEParametersDialog(GNEApplicationWindow* applicationWindow, const Parameterised::Map& parameters) :
    GNEDialog(applicationWindow, TL("Edit parameters"), GUIIcon::APP_TABLE, DialogType::PARAMETERS,
              GNEDialog::Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, GNEDialog::ResizeMode::RESIZABLE, 400, 300),
    myOriginalParameters(parameters) {
    // create frame for Parameters and operations
    FXHorizontalFrame* horizontalFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarFrame);
    // create parameters values
    myParametersValues = new ParametersValues(horizontalFrame, TL("Parameters"));
    // create parameters operations
    myParametersOperations = new ParametersOperations(horizontalFrame, this);
    // fill myParametersValues
    myParametersValues->setParameters(parameters);
    // open modal dialog
    openDialog();
}

GNEParametersDialog::~GNEParametersDialog() {}


void
GNEParametersDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do (yet)
}


std::vector<std::pair<std::string, std::string> >
GNEParametersDialog::getEditedParameters() const {
    std::vector<std::pair<std::string, std::string> > parameters;
    for (const auto& parameterRow : myParametersValues->getParameterRows()) {
        // ignore last row (the row with + button)
        if (parameterRow != myParametersValues->getParameterRows().back()) {
            // insert in parameters
            parameters.push_back(std::make_pair(parameterRow->keyField->getText().text(), parameterRow->valueField->getText().text()));
        }
    }
    return parameters;
}


long
GNEParametersDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // declare vector for parameters in stringvector format
    std::vector<std::pair<std::string, std::string> > editedParameters = getEditedParameters();
    // check keys
    for (const auto& editedParameter : editedParameters) {
        if (editedParameter.first.empty()) {
            // open warning Box
            GNEWarningBasicDialog(myApplicationWindow, TL("Empty Parameter key"), TL("Parameters with empty keys aren't allowed"));
            return 1;
        } else if (!SUMOXMLDefinitions::isValidParameterKey(editedParameter.first)) {
            // open warning Box
            GNEWarningBasicDialog(myApplicationWindow, TL("Invalid Parameter key"), TL("There are keys with invalid characters"));
            return 1;
        }
    }
    // sort sortedParameters
    std::sort(editedParameters.begin(), editedParameters.end());
    // check if there is duplicated keys
    for (auto i = editedParameters.begin(); (i + 1) != editedParameters.end(); i++) {
        if ((i->first) == (i + 1)->first) {
            // open warning Box
            GNEWarningBasicDialog(myApplicationWindow, TL("Duplicated Parameters"), TL("Parameters with the same key aren't allowed"));
            return 1;
        }
    }
    // close dialog accepting
    return closeDialogAccepting();
}


long
GNEParametersDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // restore original parameters
    myParametersValues->setParameters(myOriginalParameters);
    return 1;
}

/****************************************************************************/
