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
/// @file    GNEMultipleParametersDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2020
///
// Dialog for edit multiple parameters
/****************************************************************************/

#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>

#include "GNEMultipleParametersDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMultipleParametersDialog) GNEMultipleParametersDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNEMultipleParametersDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEMultipleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNEMultipleParametersDialog::onCmdReset),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, GNEMultipleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, GNEMultipleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, GNEMultipleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEMultipleParametersDialog::onCmdCancel),
};

FXDEFMAP(GNEMultipleParametersDialog::ParametersValues) ParametersValuesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNEMultipleParametersDialog::ParametersValues::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,   GNEMultipleParametersDialog::ParametersValues::onCmdButtonPress),
    FXMAPFUNC(SEL_PAINT,    0,                          GNEMultipleParametersDialog::ParametersValues::onPaint),
};

FXDEFMAP(GNEMultipleParametersDialog::ParametersOperations) ParametersOperationsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_LOAD,    GNEMultipleParametersDialog::ParametersOperations::onCmdLoadParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNEMultipleParametersDialog::ParametersOperations::onCmdSaveParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CLEAR,   GNEMultipleParametersDialog::ParametersOperations::onCmdClearParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SORT,    GNEMultipleParametersDialog::ParametersOperations::onCmdSortParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEMultipleParametersDialog::ParametersOperations::onCmdHelpParameter),
};

// Object implementation
FXIMPLEMENT(GNEMultipleParametersDialog,                    FXDialogBox,   GNEMultipleParametersDialogMap,  ARRAYNUMBER(GNEMultipleParametersDialogMap))
FXIMPLEMENT(GNEMultipleParametersDialog::ParametersValues,  FXGroupBox,    ParametersValuesMap,             ARRAYNUMBER(ParametersValuesMap))
FXIMPLEMENT(GNEMultipleParametersDialog::ParametersOperations, FXGroupBox,    ParametersOperationsMap,            ARRAYNUMBER(ParametersOperationsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMultipleParametersDialog::ParametersValues - methods
// ---------------------------------------------------------------------------

GNEMultipleParametersDialog::ParametersValues::ParametersValues(FXHorizontalFrame* frame) :
    FXGroupBox(frame, "Parameters", GUIDesignGroupBoxFrameFill) {
    // create labels for keys and values
    FXHorizontalFrame* horizontalFrameLabels = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myKeyLabel = new FXLabel(horizontalFrameLabels, "key", nullptr, GUIDesignLabelThickedFixed(100));
    new FXLabel(horizontalFrameLabels, "value", nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    new FXLabel(horizontalFrameLabels, "", nullptr, GUIDesignLabelIconThick);
    // create scroll windows
    FXScrollWindow* scrollWindow = new FXScrollWindow(this, LAYOUT_FILL);
    // create vertical frame for rows
    myVerticalFrameRow = new FXVerticalFrame(scrollWindow, GUIDesignAuxiliarFrame);
}


GNEMultipleParametersDialog::ParametersValues::~ParametersValues() {}


void
GNEMultipleParametersDialog::ParametersValues::setParameters(const std::vector<std::pair<std::string, std::string> >& newParameters) {
    // clear rows
    clearParameters();
    // iterate over parameteres
    for (const auto& newParameter : newParameters) {
        addParameter(newParameter);
    }
}


void
GNEMultipleParametersDialog::ParametersValues::addParameter(std::pair<std::string, std::string> newParameter) {
    // enable last row
    myParameterRows.back()->enableRow(newParameter.first, newParameter.second);
    // add row
    myParameterRows.push_back(new ParameterRow(this, myVerticalFrameRow));
    // enable add button in the last row
    myParameterRows.back()->toggleAddButton();
}


void
GNEMultipleParametersDialog::ParametersValues::clearParameters() {
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


const std::vector<GNEMultipleParametersDialog::ParametersValues::ParameterRow*>
GNEMultipleParametersDialog::ParametersValues::getParameterRows() const {
    return myParameterRows;
}


bool
GNEMultipleParametersDialog::ParametersValues::keyExist(const std::string& key) const {
    // just interate over myParameterRows and compare key
    for (const auto& row : myParameterRows) {
        if (row->keyField->getText().text() == key) {
            return true;
        }
    }
    return false;
}


long
GNEMultipleParametersDialog::ParametersValues::onPaint(FXObject* o, FXSelector f, void* p) {
    // size of key label has to be updated in every interation
    if (myParameterRows.size() > 0) {
        myKeyLabel->setWidth(myParameterRows.front()->keyField->getWidth());
    }
    return FXGroupBox::onPaint(o, f, p);
}


long
GNEMultipleParametersDialog::ParametersValues::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find what value was changed
    for (int i = 0; i < (int)myParameterRows.size(); i++) {
        if (myParameterRows.at(i)->keyField == obj) {
            // change color of text field depending if key is valid or empty
            if (myParameterRows.at(i)->keyField->getText().empty() || SUMOXMLDefinitions::isValidParameterKey(myParameterRows.at(i)->keyField->getText().text())) {
                myParameterRows.at(i)->keyField->setTextColor(FXRGB(0, 0, 255));
                myParameterRows.at(i)->valueChanged = true;
            } else {
                myParameterRows.at(i)->keyField->setTextColor(FXRGB(255, 0, 0));
                myParameterRows.at(i)->keyField->killFocus();
            }
        } else if (myParameterRows.at(i)->valueField == obj) {
            myParameterRows.at(i)->valueField->setTextColor(FXRGB(0, 0, 255));
            myParameterRows.at(i)->valueChanged = true;
        }
    }
    return 1;
}


long
GNEMultipleParametersDialog::ParametersValues::onCmdButtonPress(FXObject* obj, FXSelector, void*) {
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


GNEMultipleParametersDialog::ParametersValues::ParameterRow::ParameterRow(ParametersValues* ParametersValues, FXVerticalFrame* verticalFrameParent) :
    valueChanged(false) {
    horizontalFrame = new FXHorizontalFrame(verticalFrameParent, GUIDesignAuxiliarHorizontalFrame);
    keyField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, ParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    valueField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, ParametersValues, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    button = new FXButton(horizontalFrame, "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), ParametersValues, MID_GNE_REMOVE_ATTRIBUTE, GUIDesignButtonIcon);
    // only create elements if vertical frame was previously created
    if (verticalFrameParent->id()) {
        horizontalFrame->create();
    }
    // by defaults rows are disabled
    disableRow();
}


GNEMultipleParametersDialog::ParametersValues::ParameterRow::~ParameterRow() {
    // simply delete horizontalFrame (rest of elements will be automatic deleted due they are children of horizontal frame)
    delete horizontalFrame;
}


void
GNEMultipleParametersDialog::ParametersValues::ParameterRow::disableRow() {
    // hide all
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(GUIIcon::REMOVE));
}


void
GNEMultipleParametersDialog::ParametersValues::ParameterRow::enableRow(const std::string& parameter, const std::string& value) const {
    // restore color and enable key field
    keyField->setText(parameter.c_str());
    if (parameter.empty() || SUMOXMLDefinitions::isValidParameterKey(parameter)) {
        keyField->setTextColor(FXRGB(0, 0, 0));
    } else {
        keyField->setTextColor(FXRGB(255, 0, 0));
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
GNEMultipleParametersDialog::ParametersValues::ParameterRow::toggleAddButton() {
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
GNEMultipleParametersDialog::ParametersValues::ParameterRow::isButtonInAddMode() const {
    return (button->getIcon() == GUIIconSubSys::getIcon(GUIIcon::ADD));
}


void
GNEMultipleParametersDialog::ParametersValues::ParameterRow::copyValues(const ParameterRow& other) {
    keyField->setText(other.keyField->getText());
    valueField->setText(other.valueField->getText());
}

// ---------------------------------------------------------------------------
// GNEMultipleParametersDialog::ParametersOperations - methods
// ---------------------------------------------------------------------------

GNEMultipleParametersDialog::ParametersOperations::ParametersOperations(FXVerticalFrame* frame, GNEMultipleParametersDialog* ParameterDialogParent) :
    FXGroupBox(frame, "Operations", GUIDesignGroupBoxFrame100),
    myParameterDialogParent(ParameterDialogParent) {
    // create buttons
    mySortButton = new FXButton(this, TL("Sort"), GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_GNE_BUTTON_SORT, GUIDesignButtonRectangular100);
    myClearButton = new FXButton(this, TL("Clear"), GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), this, MID_GNE_BUTTON_CLEAR, GUIDesignButtonRectangular100);
    myLoadButton = new FXButton(this, TL("Load"), GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_BUTTON_LOAD, GUIDesignButtonRectangular100);
    mySaveButton = new FXButton(this, TL("Save"), GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_BUTTON_SAVE, GUIDesignButtonRectangular100);
    myHelpButton = new FXButton(this, TL("Help"), GUIIconSubSys::getIcon(GUIIcon::HELP), this, MID_HELP, GUIDesignButtonRectangular100);
}


GNEMultipleParametersDialog::ParametersOperations::~ParametersOperations() {}


long
GNEMultipleParametersDialog::ParametersOperations::onCmdLoadParameters(FXObject*, FXSelector, void*) {
    // get the Additional file name
    FXFileDialog opendialog(this, TL("Open Parameter Template"));
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(" Parameter Template files (*.xml,*.xml.gz)\nAll files (*)");
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
GNEMultipleParametersDialog::ParametersOperations::onCmdSaveParameters(FXObject*, FXSelector, void*) {
    // obtain file to save parameters
    FXString file = MFXUtils::getFilename2Write(this,
                    TL("Save Parameter Template file"), ".xml",
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
GNEMultipleParametersDialog::ParametersOperations::onCmdClearParameters(FXObject*, FXSelector, void*) {
    // simply clear parameters from ParametersValues
    myParameterDialogParent->myParametersValues->clearParameters();
    return 1;
}


long
GNEMultipleParametersDialog::ParametersOperations::onCmdSortParameters(FXObject*, FXSelector, void*) {
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
GNEMultipleParametersDialog::ParametersOperations::onCmdHelpParameter(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* ParameterHelpDialog = new FXDialogBox(this, " Parameters Help", GUIDesignDialogBox);
    ParameterHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TABLE));
    // set help text
    std::ostringstream help;
    help
            << TL("- Parameters are defined by a Key and a Value.\n")
            << TL("- In Netedit can be defined using format key1=parameter1|key2=parameter2|...\n")
            << TL(" - Duplicated and empty Keys aren't valid.\n")
            << TL(" - Whitespace and certain characters aren't allowed (@$%^&/|\\....)\n");
    // Create label with the help text
    new FXLabel(ParameterHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(ParameterHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(ParameterHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, (TL("OK") + std::string("\t\t") + TL("close")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), ParameterHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening Parameter help dialog");
    // create Dialog
    ParameterHelpDialog->create();
    // show in the given position
    ParameterHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(ParameterHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Closing Parameter help dialog");
    return 1;
}


GNEMultipleParametersDialog::ParametersOperations::GNEParameterHandler::GNEParameterHandler(ParametersOperations* ParametersOperationsParent, const std::string& file) :
    SUMOSAXHandler(file),
    myParametersOperationsParent(ParametersOperationsParent) {
}


GNEMultipleParametersDialog::ParametersOperations::GNEParameterHandler::~GNEParameterHandler() {}


void
GNEMultipleParametersDialog::ParametersOperations::GNEParameterHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // only continue if tag is valid
    if (element == SUMO_TAG_PARAM) {
        // Check that format of Parameter is correct
        if (!attrs.hasAttribute(SUMO_ATTR_KEY)) {
            WRITE_WARNING(TL("Key of Parameter not defined"));
        } else if (!attrs.hasAttribute(SUMO_ATTR_VALUE)) {
            WRITE_WARNING(TL("Value of Parameter not defined"));
        } else {
            // obtain Key and value
            const std::string key = attrs.getString(SUMO_ATTR_KEY);
            const std::string value = attrs.getString(SUMO_ATTR_VALUE);
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
    }
}

// ---------------------------------------------------------------------------
// GNEMultipleParametersDialog::ParametersOptions - methods
// ---------------------------------------------------------------------------

GNEMultipleParametersDialog::ParametersOptions::ParametersOptions(FXVerticalFrame* frame) :
    FXGroupBox(frame, "Options", GUIDesignGroupBoxFrame100) {
    myOnlyForExistentKeys = new FXCheckButton(this, TL("Only for\nexistent keys"), this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonExtraHeight);
}


GNEMultipleParametersDialog::ParametersOptions::~ParametersOptions() {}


bool
GNEMultipleParametersDialog::ParametersOptions::onlyForExistentKeys() const {
    return (myOnlyForExistentKeys->getCheck() == TRUE);
}

// ---------------------------------------------------------------------------
// GNEMultipleParametersDialog - methods
// ---------------------------------------------------------------------------

GNEMultipleParametersDialog::GNEMultipleParametersDialog(GNEInspectorFrame::ParametersEditor* parametersEditorInspector) :
    FXDialogBox(parametersEditorInspector->getInspectorFrameParent()->getViewNet()->getApp(), "Edit parameters", GUIDesignDialogBoxExplicitStretchable(430, 300)),
    myParametersEditor(parametersEditorInspector) {
    // call auxiliar constructor
    constructor();
    // reset
    onCmdReset(nullptr, 0, nullptr);
}


GNEMultipleParametersDialog::~GNEMultipleParametersDialog() {}


long
GNEMultipleParametersDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    const auto& ACs = myParametersEditor->getInspectorFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    if (ACs.size() > 0) {
        // get undo list
        GNEUndoList* undoList = myParametersEditor->getInspectorFrameParent()->getViewNet()->getUndoList();
        // declare vector for parameters in stringvector format
        std::vector<std::pair<std::string, std::string> > parametersChanged;
        // declare keep keys vector
        std::vector<std::string> keepKeys;
        // check if all edited parameters are valid
        for (const auto& parameterRow : myParametersValues->getParameterRows()) {
            // ignore last row
            if (parameterRow != myParametersValues->getParameterRows().back()) {
                // insert in keepKeys
                keepKeys.push_back(parameterRow->keyField->getText().text());
                // continue if we're going to modify key
                if (parameterRow->valueChanged) {
                    if (parameterRow->keyField->getText().empty()) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
                        // open warning Box
                        FXMessageBox::warning(getApp(), MBOX_OK, "Empty Parameter key", "%s", "Parameters with empty keys aren't allowed");
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
                        return 1;
                    } else if (!SUMOXMLDefinitions::isValidParameterKey(parameterRow->keyField->getText().text())) {
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
                        // open warning Box
                        FXMessageBox::warning(getApp(), MBOX_OK, "Invalid Parameter key", "%s", "There are keys with invalid characters");
                        // write warning if netedit is running in testing mode
                        WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
                        return 1;
                    }
                    // insert in parameters
                    parametersChanged.push_back(std::make_pair(parameterRow->keyField->getText().text(), parameterRow->valueField->getText().text()));
                }
            }
        }
        // sort sortedParameters
        std::sort(parametersChanged.begin(), parametersChanged.end());
        // check if there is duplicated keys
        for (auto i = parametersChanged.begin(); i != parametersChanged.end(); i++) {
            if (((i + 1) != parametersChanged.end()) && (i->first) == (i + 1)->first) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
                // open warning Box
                FXMessageBox::warning(getApp(), MBOX_OK, "Duplicated Parameters", "%s", "Parameters with the same Key aren't allowed");
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
                return 1;
            }
        }
        // begin change
        undoList->begin(ACs.front()->getTagProperty().getGUIIcon(), "change parameters");
        // iterate over ACs
        for (const auto& AC : ACs) {
            // remove keys
            AC->removeACParametersKeys(keepKeys, undoList);
            // update parameters
            for (const auto& parameter : parametersChanged) {
                if (myParametersOptions->onlyForExistentKeys() && (AC->getACParametersMap().count(parameter.first) == 0)) {
                    continue;
                } else {
                    AC->addACParameters(parameter.first, parameter.second, undoList);
                }
            }
        }
        // end change
        undoList->end();
    }
    // all ok, then close dialog
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNEMultipleParametersDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNEMultipleParametersDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // declare a map for key-values
    std::map<std::string, std::vector<std::string> > keyValuesMap;
    // fill keys
    for (const auto& AC : myParametersEditor->getInspectorFrameParent()->getViewNet()->getInspectedAttributeCarriers()) {
        for (const auto& keyAttribute : AC->getACParametersMap()) {
            keyValuesMap[keyAttribute.first].push_back(keyAttribute.second);
        }
    }
    // transform map to string vector
    std::vector<std::pair<std::string, std::string> > keyValues;
    for (const auto& keyAttribute : keyValuesMap) {
        // remove duplicated values
        std::set<std::string> valuesNonDuplicated;
        for (const auto& value : keyAttribute.second) {
            valuesNonDuplicated.insert(value);
        }
        // merge values
        std::string values;
        for (const auto& value : valuesNonDuplicated) {
            values.append(value + " ");
        }
        if (!values.empty()) {
            values.pop_back();
        }
        // update key values
        keyValues.push_back(std::make_pair(keyAttribute.first, values));
    }
    // fill myParametersValues
    myParametersValues->setParameters(keyValues);
    return 1;
}


void
GNEMultipleParametersDialog::constructor() {
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TABLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for Parameters, operations and options
    FXHorizontalFrame* horizontalFrameExtras = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarFrame);
    // create parameters values
    myParametersValues = new ParametersValues(horizontalFrameExtras);
    // create vertical frame frame
    FXVerticalFrame* verticalFrameExtras = new FXVerticalFrame(horizontalFrameExtras, GUIDesignAuxiliarVerticalFrame);
    // create parameters operations
    myParametersOperations = new ParametersOperations(verticalFrameExtras, this);
    // create parameters options
    myParametersOptions = new ParametersOptions(verticalFrameExtras);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, (TL("accept") + std::string("\t\t") + TL("close")).c_str(), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, (TL("cancel") + std::string("\t\t") + TL("close")).c_str(), GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame, (TL("reset") + std::string("\t\t") + TL("close")).c_str(),  GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}

/****************************************************************************/
