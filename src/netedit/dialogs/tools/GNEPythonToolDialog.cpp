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
/// @file    GNEPythonToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog for tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/gui/div/GUIDesigns.h>
#include <fstream>
#include <sstream>

#include "GNEPythonToolDialog.h"
#include "GNEPythonTool.h"

#define MARGING 4
#define MAXNUMCOLUMNS 4
#define NUMROWSBYCOLUMN 20

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPythonToolDialog) GNEPythonToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEPythonToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GNEPythonToolDialog::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GNEPythonToolDialog::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEPythonToolDialog::onCmdSetVisualization),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,     GNEPythonToolDialog::onCmdRun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEPythonToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNEPythonToolDialog::onCmdReset)
};

// Object implementation
FXIMPLEMENT(GNEPythonToolDialog, FXDialogBox, GNEPythonToolDialogMap, ARRAYNUMBER(GNEPythonToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEPythonToolDialog::GNEPythonToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignDialogBoxExplicit(0, 0)),
    myGNEApp(GNEApp) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON));
    // create options
    myOptionsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    // build options
    new FXButton(myOptionsFrame, (TL("Save") + std::string("\t\t") + TL("Save options")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_CHOOSEN_SAVE, GUIDesignButtonAccept);
    new FXButton(myOptionsFrame, (TL("Load") + std::string("\t\t") + TL("Load options")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_CHOOSEN_LOAD, GUIDesignButtonAccept);
    mySortedCheckButton = new FXCheckButton(myOptionsFrame, TL("Sorted by name"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myGroupedCheckButton = new FXCheckButton(myOptionsFrame, TL("Grouped by categories"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // add separator
    mySeparator = new FXSeparator(this);
    // build row frames
    auto horizontalFrameRows = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    for (int i = 0; i < MAXNUMCOLUMNS; i++) {
        myRowFrames.push_back(new FXVerticalFrame(horizontalFrameRows, GUIDesignAuxiliarFrameFixed));
    }
    // add header
    auto horizontalFrameLabel = new FXHorizontalFrame(getRowFrame(), GUIDesignAuxiliarHorizontalFrame);
    myParameterLabel = new FXLabel(horizontalFrameLabel, TL("Parameter"), nullptr, GUIDesignLabelThickedFixed(0));
    new FXLabel(horizontalFrameLabel, TL("Value"), nullptr, GUIDesignLabelThickedFixed(250));
    // add separator
    mySeparator = new FXSeparator(this);
    // create buttons centered
    myButtonsFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(myButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myButtonsFrame, (TL("Run") + std::string("\t\t") + TL("close accepting changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_RUN, GUIDesignButtonAccept);
    new FXButton(myButtonsFrame, (TL("Cancel") + std::string("\t\t") + TL("close discarding changes")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXButton(myButtonsFrame, (TL("Reset") + std::string("\t\t") + TL("reset to previous values")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXHorizontalFrame(myButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEPythonToolDialog::~GNEPythonToolDialog() {}


long
GNEPythonToolDialog::openDialog(GNEPythonTool* tool) {
    // set tool
    myPythonTool = tool;
    // set title
    setTitle(myPythonTool->getToolName().c_str());
    // reset checkboxes
    mySortedCheckButton->setCheck(FALSE);
    myGroupedCheckButton->setCheck(TRUE);
    // build arguments
    buildArguments(false, true);
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // resize dialog (Marging + contentFrame + MARGING separator + MARGING + buttonsFrame + MARGING)
    int rowFramesWidth = 0;
    int rowFramesHeight = 0;
    for (const auto &rowFrame : myRowFrames) {
        rowFramesWidth += rowFrame->getWidth();
        if (rowFrame->getHeight() > rowFramesHeight) {
            rowFramesHeight = rowFrame->getHeight();
        }
    }
    // resize dialog (rowFramesWidth, Marging + rowFramesHeight + MARGING separator + MARGING + buttonsFrame + MARGING)
    resize(rowFramesWidth, myOptionsFrame->getHeight() + rowFramesHeight + (2 * mySeparator->getHeight()) + myButtonsFrame->getHeight() + (4 * MARGING));
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    return myGNEApp->getApp()->runModalFor(this);
}


long
GNEPythonToolDialog::onCmdSave(FXObject*, FXSelector, void*) {
    // open save dialog
    const std::string file = GNEApplicationWindowHelper::openOptionFileDialog(this, true);
    // check file
    if (file.size() > 0) {
        myPythonTool->saveConfiguration(file);
    }
    return 1;
}


long
GNEPythonToolDialog::onCmdLoad(FXObject*, FXSelector, void*) {
    // open file dialog
    const std::string file = GNEApplicationWindowHelper::openOptionFileDialog(this, false);
    // check file
    if ((file.size() > 0) && myPythonTool->loadConfiguration(file)) {
        // rebuild arguments
        buildArguments((mySortedCheckButton->getCheck() == TRUE), (myGroupedCheckButton->getCheck() == TRUE));
    }
    return 1;
}


long
GNEPythonToolDialog::onCmdSetVisualization(FXObject*, FXSelector, void*) {
    // rebuild arguments
    buildArguments((mySortedCheckButton->getCheck() == TRUE), (myGroupedCheckButton->getCheck() == TRUE));
    return 1;
}


long
GNEPythonToolDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
    // run tool
    return myGNEApp->tryHandle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_RUNPYTHONTOOL), nullptr);
}


long
GNEPythonToolDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    hide();
    return 1;
}


long
GNEPythonToolDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // iterate over all arguments and reset values
    for (const auto& argument : myArguments) {
        argument->reset();
    }
    return 1;
}


GNEPythonToolDialog::CategoryOptions::CategoryOptions(const std::string &category) :
    std::string(category) {
}


void
GNEPythonToolDialog::CategoryOptions::addOption(const std::string &name, Option* option) {
    myOptions.push_back(std::make_pair(name, option));
}


const std::vector<std::pair<std::string, Option*> > &
GNEPythonToolDialog::CategoryOptions::getOptions() const {
    return myOptions;
}


void
GNEPythonToolDialog::CategoryOptions::sortByName() {
    // just sort vector with options
    std::sort(myOptions.begin(), myOptions.end());
}


GNEPythonToolDialog::GNEPythonToolDialog() :
    myGNEApp(nullptr) {
}


void
GNEPythonToolDialog::buildArguments(bool sortByName, bool groupedByCategories) {
    // clear arguments and categories
    for (const auto& argument : myArguments) {
        delete argument;
    }
    for (const auto& category : myCategories) {
        delete category;
    }
    myArguments.clear();
    myCategories.clear();
    // get argument sorted by name and grouped by categories
    auto categoryOptions = groupedByCategories? getOptionsByCategories(myPythonTool->getToolsOptions()) : getOptions(myPythonTool->getToolsOptions());
    // iterate over category options
    for (auto &categoryOption : categoryOptions) {
        // add category
        if (categoryOption.size() > 0) {
            myCategories.push_back(new GNEPythonToolDialogElements::Category(this, getRowFrame(), categoryOption));
        }
        // check if sort by name
        if (sortByName) {
            categoryOption.sortByName();
        }
        // add options
        for (const auto &option : categoryOption.getOptions()) {
            if (option.second->isInteger()) {
                myArguments.push_back(new GNEPythonToolDialogElements::IntArgument(this, getRowFrame(), option.first, option.second));
            } else if (option.second->isFloat()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FloatArgument(this, getRowFrame(), option.first, option.second));
            } else if (option.second->isBool()) {
                myArguments.push_back(new GNEPythonToolDialogElements::BoolArgument(this, getRowFrame(), option.first, option.second));
            } else if (option.second->isFileName()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FileNameArgument(this, getRowFrame(), option.first, option.second));       
            } else {
                myArguments.push_back(new GNEPythonToolDialogElements::StringArgument(this, getRowFrame(), option.first, option.second));
            }
        }
    }
    // adjust parameter column (call always after create elements)
    adjustParameterColumn();
}


void
GNEPythonToolDialog::adjustParameterColumn() {
    int maximumWidth = 0;
    // iterate over all arguments and find the maximum width
    for (const auto& argument : myArguments) {
        const auto label = argument->getParameterLabel();
        const int columnWidth = label->getFont()->getTextWidth(label->getText().text(), label->getText().length() + MARGING);
        if (columnWidth > maximumWidth) {
            maximumWidth = columnWidth;
        }
    }
    // set maximum width for all parameter labels
    for (const auto& argument : myArguments) {
        argument->getParameterLabel()->setWidth(maximumWidth);
    }
    // set parameter label width
    myParameterLabel->setWidth(maximumWidth);
    // set content frame size (MARGING + Parameter + TextField + MARGING)
    for (int i = 0; i < MAXNUMCOLUMNS; i++) {
        if (i <= getNumRowColums()) {
            myRowFrames.at(i)->setWidth(MARGING + maximumWidth + 250 + GUIDesignHeight + MARGING);
            myRowFrames.at(i)->setHeight(GUIDesignHeight * myRowFrames.at(i)->numChildren());
        } else {
            myRowFrames.at(i)->setWidth(0);
            myRowFrames.at(i)->setHeight(0);
        }
    }
}


std::vector<GNEPythonToolDialog::CategoryOptions>
GNEPythonToolDialog::getOptions(OptionsCont& optionsCont) const {
    // use a vector with only one empty category to reuse code of buildArguments
    std::vector<GNEPythonToolDialog::CategoryOptions> result = {GNEPythonToolDialog::CategoryOptions("")};
    // add all options to result
    for (const auto &option : optionsCont) {
        result.front().addOption(option.first, option.second);
    }
    return result;
}


std::vector<GNEPythonToolDialog::CategoryOptions>
GNEPythonToolDialog::getOptionsByCategories(OptionsCont& optionsCont) const {
    // declare vector with common categories
    const std::vector<std::string> commonCategories = {"input", "output", "processing", "time"};
    // fill categories
    std::vector<std::string> categories = commonCategories;
    for (const auto &option : optionsCont) {
        if (std::find(categories.begin(), categories.end(), option.second->getCategory()) == categories.end()) {
            categories.push_back(option.second->getCategory());
        }
    }
    // declare vector of category options and fill
    std::vector<GNEPythonToolDialog::CategoryOptions> result;
    for (const auto &category : categories) {
        result.push_back(GNEPythonToolDialog::CategoryOptions(category));
    }
    // fill result with options
    for (const auto &option : optionsCont) {
        auto category = std::find(result.begin(), result.end(), option.second->getCategory());
        // add option in category
        category->addOption(option.first, option.second);
    }
    // drop empty categories
    for (auto it = result.begin(); it != result.end(); it++) {
        if (it->getOptions().empty()) {
            it = result.erase(it);
        }
    }
    return result;
}


int
GNEPythonToolDialog::getNumRowColums() const {
    const int column = (int)myArguments.size() / NUMROWSBYCOLUMN;
    return (column < MAXNUMCOLUMNS)? column : (MAXNUMCOLUMNS - 1);
}


FXVerticalFrame*
GNEPythonToolDialog::getRowFrame() {
    return myRowFrames.at(getNumRowColums());
}

/****************************************************************************/
