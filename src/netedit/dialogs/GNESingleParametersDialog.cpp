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
/// @file    GNESingleParametersDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2018
///
// Dialog for edit parameters
/****************************************************************************/
#include <config.h>

#include <netbuild/NBLoadedSUMOTLDef.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/XMLSubSys.h>

#include "GNESingleParametersDialog.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNESingleParametersDialog) GNESingleParametersDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ACCEPT,  GNESingleParametersDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNESingleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNESingleParametersDialog::onCmdReset),
    FXMAPFUNC(SEL_CHORE,    FXDialogBox::ID_CANCEL, GNESingleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_TIMEOUT,  FXDialogBox::ID_CANCEL, GNESingleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  FXDialogBox::ID_CANCEL, GNESingleParametersDialog::onCmdCancel),
    FXMAPFUNC(SEL_CLOSE,    0,                      GNESingleParametersDialog::onCmdCancel),
};

FXDEFMAP(GNESingleParametersDialog::ParametersValues) ParametersValuesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNESingleParametersDialog::ParametersValues::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVE_ATTRIBUTE,   GNESingleParametersDialog::ParametersValues::onCmdButtonPress),
    FXMAPFUNC(SEL_PAINT,    0,                          GNESingleParametersDialog::ParametersValues::onPaint),
};

FXDEFMAP(GNESingleParametersDialog::ParametersOperations) ParametersOperationsMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_LOAD,    GNESingleParametersDialog::ParametersOperations::onCmdLoadParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SAVE,    GNESingleParametersDialog::ParametersOperations::onCmdSaveParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CLEAR,   GNESingleParametersDialog::ParametersOperations::onCmdClearParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_SORT,    GNESingleParametersDialog::ParametersOperations::onCmdSortParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNESingleParametersDialog::ParametersOperations::onCmdHelpParameter),
};

// Object implementation
FXIMPLEMENT(GNESingleParametersDialog,                          FXDialogBox,    GNESingleParametersDialogMap,   ARRAYNUMBER(GNESingleParametersDialogMap))
FXIMPLEMENT(GNESingleParametersDialog::ParametersValues,        FXGroupBox,     ParametersValuesMap,            ARRAYNUMBER(ParametersValuesMap))
FXIMPLEMENT(GNESingleParametersDialog::ParametersOperations,    FXGroupBox,     ParametersOperationsMap,        ARRAYNUMBER(ParametersOperationsMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNESingleParametersDialog::ParametersValues - methods
// ---------------------------------------------------------------------------

GNESingleParametersDialog::ParametersValues::ParametersValues(FXHorizontalFrame* frame) :
    FXGroupBox(frame, " Parameters", GUIDesignGroupBoxFrameFill) {
    // create labels for keys and values
    FXHorizontalFrame* horizontalFrameLabels = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myKeyLabel = new FXLabel(horizontalFrameLabels, "key", nullptr, GUIDesignLabelThick100);
    new FXLabel(horizontalFrameLabels, "value", nullptr, GUIDesignLabelCenterThick);
    // create scroll windows
    FXScrollWindow* scrollWindow = new FXScrollWindow(this, LAYOUT_FILL);
    // create vertical frame for rows
    myVerticalFrameRow = new FXVerticalFrame(scrollWindow, GUIDesignAuxiliarFrame);
}


GNESingleParametersDialog::ParametersValues::~ParametersValues() {}


void
GNESingleParametersDialog::ParametersValues::setParameters(const std::vector<std::pair<std::string, std::string> >& newParameters) {
    // clear rows
    clearParameters();
    // iterate over parameteres
    for (const auto& newParameter : newParameters) {
        addParameter(newParameter);
    }
}


void
GNESingleParametersDialog::ParametersValues::addParameter(std::pair<std::string, std::string> newParameter) {
    // enable last row
    myParameterRows.back()->enableRow(newParameter.first, newParameter.second);
    // add row
    myParameterRows.push_back(new ParameterRow(this, myVerticalFrameRow));
    // enable add button in the last row
    myParameterRows.back()->toggleAddButton();
}


void
GNESingleParametersDialog::ParametersValues::clearParameters() {
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


const std::vector<GNESingleParametersDialog::ParametersValues::ParameterRow*>
GNESingleParametersDialog::ParametersValues::getParameterRows() const {
    return myParameterRows;
}


bool
GNESingleParametersDialog::ParametersValues::keyExist(const std::string& key) const {
    // just interate over myParameterRows and compare key
    for (const auto& row : myParameterRows) {
        if (row->keyField->getText().text() == key) {
            return true;
        }
    }
    return false;
}


long
GNESingleParametersDialog::ParametersValues::onPaint(FXObject* o, FXSelector f, void* p) {
    // size of key label has to be updated in every interation
    if (myParameterRows.size() > 0) {
        myKeyLabel->setWidth(myParameterRows.front()->keyField->getWidth());
    }
    return FXGroupBox::onPaint(o, f, p);
}


long
GNESingleParametersDialog::ParametersValues::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // find what value was changed
    for (int i = 0; i < (int)myParameterRows.size(); i++) {
        if (myParameterRows.at(i)->keyField == obj) {
            // change color of text field depending if key is valid or empty
            if (myParameterRows.at(i)->keyField->getText().empty() || SUMOXMLDefinitions::isValidParameterKey(myParameterRows.at(i)->keyField->getText().text())) {
                myParameterRows.at(i)->keyField->setTextColor(FXRGB(0, 0, 0));
            } else {
                myParameterRows.at(i)->keyField->setTextColor(FXRGB(255, 0, 0));
                myParameterRows.at(i)->keyField->killFocus();
            }
        }
    }
    return 1;
}


long
GNESingleParametersDialog::ParametersValues::onCmdButtonPress(FXObject* obj, FXSelector, void*) {
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


GNESingleParametersDialog::ParametersValues::ParameterRow::ParameterRow(ParametersValues* ParametersValues, FXVerticalFrame* verticalFrameParent) {
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


GNESingleParametersDialog::ParametersValues::ParameterRow::~ParameterRow() {
    // simply delete horizontalFrame (rest of elements will be automatic deleted due they are children of horizontal frame)
    delete horizontalFrame;
}


void
GNESingleParametersDialog::ParametersValues::ParameterRow::disableRow() {
    // hide all
    keyField->setText("");
    keyField->disable();
    valueField->setText("");
    valueField->disable();
    button->disable();
    button->setIcon(GUIIconSubSys::getIcon(GUIIcon::REMOVE));
}


void
GNESingleParametersDialog::ParametersValues::ParameterRow::enableRow(const std::string& parameter, const std::string& value) const {
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
GNESingleParametersDialog::ParametersValues::ParameterRow::toggleAddButton() {
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
GNESingleParametersDialog::ParametersValues::ParameterRow::isButtonInAddMode() const {
    return (button->getIcon() == GUIIconSubSys::getIcon(GUIIcon::ADD));
}


void
GNESingleParametersDialog::ParametersValues::ParameterRow::copyValues(const ParameterRow& other) {
    keyField->setText(other.keyField->getText());
    valueField->setText(other.valueField->getText());
}

// ---------------------------------------------------------------------------
// GNESingleParametersDialog::ParametersOperations - methods
// ---------------------------------------------------------------------------

GNESingleParametersDialog::ParametersOperations::ParametersOperations(FXHorizontalFrame* frame, GNESingleParametersDialog* ParameterDialogParent) :
    FXGroupBox(frame, "Operations", GUIDesignGroupBoxFrame100),
    myParameterDialogParent(ParameterDialogParent) {
    // create buttons
    mySortButton =  new FXButton(this, "Sort",  GUIIconSubSys::getIcon(GUIIcon::RELOAD), this, MID_GNE_BUTTON_SORT, GUIDesignButtonRectangular100);
    myClearButton = new FXButton(this, "Clear", GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), this, MID_GNE_BUTTON_CLEAR, GUIDesignButtonRectangular100);
    myLoadButton =  new FXButton(this, "Load",  GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG), this, MID_GNE_BUTTON_LOAD, GUIDesignButtonRectangular100);
    mySaveButton =  new FXButton(this, "Save",  GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_GNE_BUTTON_SAVE, GUIDesignButtonRectangular100);
    myHelpButton =  new FXButton(this, "Help",  GUIIconSubSys::getIcon(GUIIcon::HELP), this, MID_HELP, GUIDesignButtonRectangular100);
}


GNESingleParametersDialog::ParametersOperations::~ParametersOperations() {}


long
GNESingleParametersDialog::ParametersOperations::onCmdLoadParameters(FXObject*, FXSelector, void*) {
    // get the Additional file name
    FXFileDialog opendialog(this, "Open Parameter Template");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::GREENVEHICLE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList(" Parameter Template files (*.xml)\nAll files (*)");
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
            WRITE_MESSAGE("Loading of Parameters From " + file + " failed.");
        }
        // show loaded attributes
        WRITE_MESSAGE("Loaded " + toString((int)myParameterDialogParent->myParametersValues->getParameterRows().size() - numberOfParametersbeforeLoad) + " Parameters.");
    }
    return 1;
}


long
GNESingleParametersDialog::ParametersOperations::onCmdSaveParameters(FXObject*, FXSelector, void*) {
    // obtain file to save parameters
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save Parameter Template file", ".xml",
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
GNESingleParametersDialog::ParametersOperations::onCmdClearParameters(FXObject*, FXSelector, void*) {
    // simply clear parameters from ParametersValues
    myParameterDialogParent->myParametersValues->clearParameters();
    return 1;
}


long
GNESingleParametersDialog::ParametersOperations::onCmdSortParameters(FXObject*, FXSelector, void*) {
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
GNESingleParametersDialog::ParametersOperations::onCmdHelpParameter(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* ParameterHelpDialog = new FXDialogBox(this, " Parameters Help", GUIDesignDialogBox);
    ParameterHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TABLE));
    // set help text
    std::ostringstream help;
    help
            << "- Parameters are defined by a Key and a Value.\n"
            << "- In Netedit can be defined using format key1=parameter1|key2=parameter2|...\n"
            << " - Duplicated and empty Keys aren't valid.\n"
            << " - Certain characters aren't allowed (\t\n\r@$%^&/|\\....)\n";
    // Create label with the help text
    new FXLabel(ParameterHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(ParameterHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(ParameterHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), ParameterHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
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


GNESingleParametersDialog::ParametersOperations::GNEParameterHandler::GNEParameterHandler(ParametersOperations* ParametersOperationsParent, const std::string& file) :
    SUMOSAXHandler(file),
    myParametersOperationsParent(ParametersOperationsParent) {
}


GNESingleParametersDialog::ParametersOperations::GNEParameterHandler::~GNEParameterHandler() {}


void
GNESingleParametersDialog::ParametersOperations::GNEParameterHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // only continue if tag is valid
    if (element != SUMO_TAG_NOTHING) {
        // Call parse and build depending of tag
        switch (element) {
            case SUMO_TAG_PARAM:
                // Check that format of Parameter is correct
                if (!attrs.hasAttribute(SUMO_ATTR_KEY)) {
                    WRITE_WARNING("Key of Parameter not defined");
                } else if (!attrs.hasAttribute(SUMO_ATTR_VALUE)) {
                    WRITE_WARNING("Value of Parameter not defined");
                } else {
                    // obtain Key and value
                    std::string key = attrs.getString(SUMO_ATTR_KEY);
                    std::string value = attrs.getString(SUMO_ATTR_VALUE);
                    // check that parsed values are correct
                    if (!SUMOXMLDefinitions::isValidParameterKey(key)) {
                        if (key.size() == 0) {
                            WRITE_WARNING("Key of Parameter cannot be empty");
                        } else {
                            WRITE_WARNING("Key '" + key + "' of Parameter contains invalid characters");
                        }
                    } else if (myParametersOperationsParent->myParameterDialogParent->myParametersValues->keyExist(key)) {
                        WRITE_WARNING("Key '" + key + "' already exist");
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
// GNESingleParametersDialog - methods
// ---------------------------------------------------------------------------

GNESingleParametersDialog::GNESingleParametersDialog(GNEFrameAttributeModules::ParametersEditorCreator* parametersEditorCreator) :
    FXDialogBox(parametersEditorCreator->getFrameParent()->getViewNet()->getApp(), "Edit parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myParametersEditorCreator(parametersEditorCreator),
    myParametersEditorInspector(nullptr),
    VTypeAttributeRow(nullptr),
    myAttributeCarrier(nullptr),
    myTLDef(nullptr) {
    // call auxiliar constructor for elements
    constructor();
    // fill myParametersValues
    myParametersValues->setParameters(parametersEditorCreator->getParameters());
}


GNESingleParametersDialog::GNESingleParametersDialog(GNEInspectorFrame::ParametersEditorInspector* parametersEditorInspector) :
    FXDialogBox(parametersEditorInspector->getInspectorFrameParent()->getViewNet()->getApp(), "Edit parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myParametersEditorCreator(nullptr),
    myParametersEditorInspector(parametersEditorInspector),
    VTypeAttributeRow(nullptr),
    myAttributeCarrier(nullptr),
    myTLDef(nullptr) {
    // call auxiliar constructor
    constructor();
    // get AC Front
    const GNEAttributeCarrier* AC = parametersEditorInspector->getInspectorFrameParent()->getViewNet()->getInspectedAttributeCarriers().front();
    // fill myParametersValues
    myParametersValues->setParameters(AC->getACParameters<std::vector<std::pair<std::string, std::string> > >());
}



GNESingleParametersDialog::GNESingleParametersDialog(GNEVehicleTypeDialog::VTypeAtributes::VTypeAttributeRow* VTypeAttributeRow, GNEViewNet* viewNet) :
    FXDialogBox(viewNet->getApp(), "Edit parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myParametersEditorCreator(nullptr),
    myParametersEditorInspector(nullptr),
    VTypeAttributeRow(VTypeAttributeRow),
    myAttributeCarrier(nullptr),
    myTLDef(nullptr) {
    // call auxiliar constructor
    constructor();
    // fill myEditedParameters
    myParametersValues->setParameters(VTypeAttributeRow->getParametersVectorStr());
}


GNESingleParametersDialog::GNESingleParametersDialog(GNEAttributeCarrier* attributeCarrier) :
    FXDialogBox(attributeCarrier->getNet()->getViewNet()->getApp(), "Edit parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myParametersEditorCreator(nullptr),
    myParametersEditorInspector(nullptr),
    VTypeAttributeRow(nullptr),
    myAttributeCarrier(attributeCarrier),
    myTLDef(nullptr) {
    // call auxiliar constructor
    constructor();
    // fill myEditedParameters
    myParametersValues->setParameters(myAttributeCarrier->getACParameters<std::vector<std::pair<std::string, std::string> > >());
}


GNESingleParametersDialog::GNESingleParametersDialog(FXApp* app, NBLoadedSUMOTLDef* TLDef) :
    FXDialogBox(app, "Edit parameters", GUIDesignDialogBoxExplicitStretchable(400, 300)),
    myParametersEditorCreator(nullptr),
    myParametersEditorInspector(nullptr),
    VTypeAttributeRow(nullptr),
    myAttributeCarrier(nullptr),
    myTLDef(TLDef) {
    // call auxiliar constructor
    constructor();
    // transform parameters to a=b|c=d... format
    std::vector<std::pair<std::string, std::string> > parametersStr;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : TLDef->getParametersMap()) {
        parametersStr.push_back(std::make_pair(parameter.first, parameter.second));
    }
    // set parameters
    myParametersValues->setParameters(parametersStr);
}


GNESingleParametersDialog::~GNESingleParametersDialog() {}


long
GNESingleParametersDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // declare vector for parameters in stringvector format
    std::vector<std::pair<std::string, std::string> > parameters;
    // check if all edited parameters are valid
    for (const auto& parameterRow : myParametersValues->getParameterRows()) {
        // ignore last row
        if (parameterRow != myParametersValues->getParameterRows().back()) {
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
            parameters.push_back(std::make_pair(parameterRow->keyField->getText().text(), parameterRow->valueField->getText().text()));
        }
    }
    // sort sortedParameters
    std::sort(parameters.begin(), parameters.end());
    // check if there is duplicated keys
    for (auto i = parameters.begin(); i != parameters.end(); i++) {
        if (((i + 1) != parameters.end()) && (i->first) == (i + 1)->first) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox of type 'warning'");
            // open warning Box
            FXMessageBox::warning(getApp(), MBOX_OK, "Duplicated Parameters", "%s", "Parameters with the same Key aren't allowed");
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox of type 'warning' with 'OK'");
            return 1;
        }
    }
    // set parameters in Parameters editor parents
    if (myParametersEditorCreator) {
        // set parameter in editor creator
        myParametersEditorCreator->setParameters(parameters);
    } else if (myParametersEditorInspector) {
        // get inspected AC
        GNEAttributeCarrier* AC = myParametersEditorInspector->getInspectorFrameParent()->getViewNet()->getInspectedAttributeCarriers().front();
        // set parameter in AC using undoList
        myParametersEditorInspector->getInspectorFrameParent()->getViewNet()->getUndoList()->begin(AC->getTagProperty().getGUIIcon(), "change parameters");
        AC->setACParameters(parameters, myParametersEditorInspector->getInspectorFrameParent()->getViewNet()->getUndoList());
        myParametersEditorInspector->getInspectorFrameParent()->getViewNet()->getUndoList()->end();
    } else if (VTypeAttributeRow) {
        // set parameter in VTypeAttributeRow
        VTypeAttributeRow->setParameters(parameters);
    } else if (myAttributeCarrier) {
        // set parameter in AC using undoList
        myAttributeCarrier->getNet()->getViewNet()->getUndoList()->begin(myAttributeCarrier->getTagProperty().getGUIIcon(), "change parameters");
        myAttributeCarrier->setACParameters(parameters, myAttributeCarrier->getNet()->getViewNet()->getUndoList());
        myAttributeCarrier->getNet()->getViewNet()->getUndoList()->end();
    } else if (myTLDef) {
        // declare parametersMap
        std::map<std::string, std::string> parametersMap;
        // Generate an string using the following structure: "key1=value1|key2=value2|...
        for (const auto& parameter : parameters) {
            parametersMap[parameter.first] = parameter.second;
        }
        // set setACParameters map
        myTLDef->setParametersMap(parametersMap);
    }
    // all ok, then close dialog
    getApp()->stopModal(this, TRUE);
    return 1;
}


long
GNESingleParametersDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Stop Modal
    getApp()->stopModal(this, FALSE);
    return 1;
}


long
GNESingleParametersDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // restore original parameters
    if (myParametersEditorCreator) {
        myParametersValues->setParameters(myParametersEditorCreator->getParameters());
    } else if (myParametersEditorInspector) {
        const GNEAttributeCarrier* AC = myParametersEditorInspector->getInspectorFrameParent()->getViewNet()->getInspectedAttributeCarriers().front();
        myParametersValues->setParameters(AC->getACParameters<std::vector<std::pair<std::string, std::string> > >());
    } else if (VTypeAttributeRow) {
        myParametersValues->setParameters(VTypeAttributeRow->getParametersVectorStr());
    } else if (myAttributeCarrier) {
        myParametersValues->setParameters(myAttributeCarrier->getACParameters<std::vector<std::pair<std::string, std::string> > >());
    } else if (myTLDef) {
        // transform parameters to a=b|c=d... format
        std::vector<std::pair<std::string, std::string> > parametersStr;
        // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
        for (const auto& parameter : myTLDef->getParametersMap()) {
            parametersStr.push_back(std::make_pair(parameter.first, parameter.second));
        }
        // set parameters
        myParametersValues->setParameters(parametersStr);
    }
    return 1;
}


void
GNESingleParametersDialog::constructor() {
    // set vehicle icon for this dialog
    setIcon(GUIIconSubSys::getIcon(GUIIcon::APP_TABLE));
    // create main frame
    FXVerticalFrame* mainFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrame);
    // create frame for Parameters and operations
    FXHorizontalFrame* horizontalFrameExtras = new FXHorizontalFrame(mainFrame, GUIDesignAuxiliarFrame);
    // create parameters values
    myParametersValues = new ParametersValues(horizontalFrameExtras);
    // create parameters operations
    myParametersOperations = new ParametersOperations(horizontalFrameExtras, this);
    // add separator
    new FXHorizontalSeparator(mainFrame, GUIDesignHorizontalSeparator);
    // create dialog buttons bot centered
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(mainFrame, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
    myAcceptButton = new FXButton(buttonsFrame, "accept\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_ACCEPT, GUIDesignButtonAccept);
    myCancelButton = new FXButton(buttonsFrame, "cancel\t\tclose", GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    myResetButton = new FXButton(buttonsFrame,  "reset\t\tclose",  GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(buttonsFrame, GUIDesignAuxiliarHorizontalFrame);
}

/****************************************************************************/
