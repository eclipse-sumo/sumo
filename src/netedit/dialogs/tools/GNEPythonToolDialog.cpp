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
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/tools/GNEPythonTool.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPythonToolDialog.h"

#define MARGING 4
#define MAXNUMCOLUMNS 4
#define NUMROWSBYCOLUMN 20

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPythonToolDialog) GNEPythonToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEPythonToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_SHOWTOOLTIPS_MENU,  GNEPythonToolDialog::onCmdShowToolTipsMenu),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GNEPythonToolDialog::onCmdSave),
    FXMAPFUNC(SEL_UPDATE,   MID_CHOOSEN_SAVE,       GNEPythonToolDialog::onUpdRequiredAttributes),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GNEPythonToolDialog::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEPythonToolDialog::onCmdSetVisualization),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,     GNEPythonToolDialog::onCmdRun),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_BUTTON_RUN,     GNEPythonToolDialog::onUpdRequiredAttributes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEPythonToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNEPythonToolDialog::onCmdReset)
};

// Object implementation
FXIMPLEMENT(GNEPythonToolDialog, FXDialogBox, GNEPythonToolDialogMap, ARRAYNUMBER(GNEPythonToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEPythonToolDialog::GNEPythonToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignAuxiliarDialogBoxResizable),
    myGNEApp(GNEApp) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON));
    // create main content frame
    auto verticalContentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);
    // create options
    auto horizontalOptionsFrame = new FXHorizontalFrame(verticalContentFrame, GUIDesignHorizontalFrame);
    // build options
    myShowToolTipsMenu = new MFXCheckableButton(false, horizontalOptionsFrame,
        GNEApp->getStaticTooltipMenu(), "\tToggle Menu Tooltips\tToggles whether tooltips in the menu shall be shown.",
        GUIIconSubSys::getIcon(GUIIcon::SHOWTOOLTIPS_MENU), this, MID_SHOWTOOLTIPS_MENU, GUIDesignMFXCheckableButtonSquare);
    auto saveFile = new MFXButtonTooltip(horizontalOptionsFrame, GNEApp->getStaticTooltipMenu(), TL("Save toolcfg"),
        GUIIconSubSys::getIcon(GUIIcon::SAVE), this, MID_CHOOSEN_SAVE, GUIDesignButtonConfiguration);
        saveFile->setTipText(TL("Save file with tool configuration"));
    auto loadFile = new MFXButtonTooltip(horizontalOptionsFrame, GNEApp->getStaticTooltipMenu(), TL("Load toolcfg") ,
        GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_CHOOSEN_LOAD, GUIDesignButtonConfiguration);
        loadFile->setTipText(TL("Load file with tool configuration"));
    mySortedCheckButton = new FXCheckButton(horizontalOptionsFrame, TL("Sorted by name"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myGroupedCheckButton = new FXCheckButton(horizontalOptionsFrame, TL("Grouped by categories"), this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    // add separators
    new FXSeparator(verticalContentFrame);
    // Create scroll frame for content rows
    auto contentScrollWindow = new FXScrollWindow(verticalContentFrame, GUIDesignScrollWindow);
    auto horizontalRowFrames = new FXHorizontalFrame(contentScrollWindow, LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH);
    myArgumentFrameLeft = new FXVerticalFrame(horizontalRowFrames, GUIDesignAuxiliarFrame);
    myArgumentFrameRight = new FXVerticalFrame(horizontalRowFrames, GUIDesignAuxiliarFrame);
    // add separator
    new FXSeparator(verticalContentFrame);
    // create buttons centered
    auto horizontalButtonsFrame = new FXHorizontalFrame(verticalContentFrame, GUIDesignHorizontalFrame);
    auto blueLabel = new FXLabel(horizontalButtonsFrame, TL("Blue options are mandatory"), nullptr, GUIDesignLabelFixed(200));
    blueLabel->setTextColor(FXRGB(0, 0, 255));
    new FXHorizontalFrame(horizontalButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(horizontalButtonsFrame, (TL("Run") + std::string("\t\t") + TL("run python tool")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_RUN, GUIDesignButtonAccept);
    new FXButton(horizontalButtonsFrame, (TL("Close") + std::string("\t\t") + TL("close tool dialog")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::CANCEL), this, MID_GNE_BUTTON_CANCEL, GUIDesignButtonCancel);
    new FXButton(horizontalButtonsFrame, (TL("Reset") + std::string("\t\t") + TL("reset to default values")).c_str(),
        GUIIconSubSys::getIcon(GUIIcon::RESET),  this, MID_GNE_BUTTON_RESET,  GUIDesignButtonReset);
    new FXLabel(horizontalButtonsFrame, "", nullptr, GUIDesignLabelFixed(200));
    new FXHorizontalFrame(horizontalButtonsFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNEPythonToolDialog::~GNEPythonToolDialog() {}


void
GNEPythonToolDialog::openDialog(GNEPythonTool* tool) {
    // set tool
    myPythonTool = tool;
    // set title
    setTitle(myPythonTool->getToolName().c_str());
    // reset checkboxes
    mySortedCheckButton->setCheck(FALSE);
    myGroupedCheckButton->setCheck(TRUE);
    // set myShowToolTipsMenu
    myShowToolTipsMenu->setChecked(getApp()->reg().readIntEntry("gui", "menuToolTips", 0) != 1);
    // set current values in options (like current folders and similar)
    myPythonTool->setCurrentValues();
    // build arguments
    buildArguments(false, true);
    // get maximum height
    const int maximumHeight = myArgumentFrameLeft->numChildren() * GUIDesignHeight + 120;
    // resize
    resize(1024, maximumHeight <= 768 ? maximumHeight : 768);
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
}


GNEApplicationWindow*
GNEPythonToolDialog::getGNEApplicationWindow() const {
    return myGNEApp;
}


const GNEPythonTool*
GNEPythonToolDialog::getPythonTool() const {
    return myPythonTool;
}


long
GNEPythonToolDialog::onCmdShowToolTipsMenu(FXObject*, FXSelector, void*) {
    // toggle check
    myShowToolTipsMenu->setChecked(!myShowToolTipsMenu->amChecked());
    if (myGNEApp->getViewNet()) {
        myGNEApp->getViewNet()->getViewParent()->getShowToolTipsMenu()->setChecked(myShowToolTipsMenu->amChecked());
        myGNEApp->getViewNet()->getViewParent()->getShowToolTipsMenu()->update();
    }
    // enable/disable static tooltip
    myGNEApp->getStaticTooltipMenu()->enableStaticToolTip(myShowToolTipsMenu->amChecked());
    // save in registry
    getApp()->reg().writeIntEntry("gui", "menuToolTips", myShowToolTipsMenu->amChecked() ? 0 : 1);
    update();

    return 1;
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
    // hide dialog
    hide();
    // run tool
    return myGNEApp->tryHandle(myPythonTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_RUNPYTHONTOOL), nullptr);
}


long
GNEPythonToolDialog::onCmdCancel(FXObject*, FXSelector, void*) {
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


long
GNEPythonToolDialog::onUpdRequiredAttributes(FXObject* sender, FXSelector, void*) {
    // iterate over all arguments and check if required attribute is set
    for (const auto& argument : myArguments) {
        if (argument->requiredAttributeSet() == false) {
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        }
    }
    return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


GNEPythonToolDialog::CategoryOptions::CategoryOptions(const std::string& category) :
    std::string(category) {
}


void
GNEPythonToolDialog::CategoryOptions::addOption(const std::string& name, Option* option) {
    myOptions.push_back(std::make_pair(name, option));
}


const std::vector<std::pair<std::string, Option*> >&
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
    auto categoryOptions = groupedByCategories ? getOptionsByCategories(myPythonTool->getToolsOptions()) : getOptions(myPythonTool->getToolsOptions());
    // calculate number of arguments
    int numArguments = 0;
    for (auto& categoryOption : categoryOptions) {
        numArguments += (int)categoryOption.getOptions().size() + 1;
    }
    const int halfNumArguments = numArguments / 2;
    // declare counter for number of inserted arguments
    int numInsertedArguments = 0;
    // iterate over category options
    for (auto& categoryOption : categoryOptions) {
        // get argument frame
        auto argumentFrame = (numInsertedArguments < halfNumArguments) ? myArgumentFrameLeft : myArgumentFrameRight;
        // add category
        if (categoryOption.size() > 0) {
            myCategories.push_back(new GNEPythonToolDialogElements::Category(argumentFrame, categoryOption));
            numInsertedArguments++;
        }
        // check if sort by name
        if (sortByName) {
            categoryOption.sortByName();
        }
        // add options
        for (const auto& option : categoryOption.getOptions()) {
            // get argument frame (again)
            argumentFrame = (numInsertedArguments < halfNumArguments) ? myArgumentFrameLeft : myArgumentFrameRight;
            // continue depending of type
            if (option.second->isInteger()) {
                myArguments.push_back(new GNEPythonToolDialogElements::IntArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isFloat()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FloatArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isBool()) {
                myArguments.push_back(new GNEPythonToolDialogElements::BoolArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isFileName()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FileNameArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isNetwork()) {
                myArguments.push_back(new GNEPythonToolDialogElements::NetworkArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isAdditional()) {
                myArguments.push_back(new GNEPythonToolDialogElements::AdditionalArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isRoute()) {
                myArguments.push_back(new GNEPythonToolDialogElements::RouteArgument(this, argumentFrame, option.first, option.second));
            } else if (option.second->isData()) {
                myArguments.push_back(new GNEPythonToolDialogElements::DataArgument(this, argumentFrame, option.first, option.second));
            } else {
                myArguments.push_back(new GNEPythonToolDialogElements::StringArgument(this, argumentFrame, option.first, option.second));
            }
            numInsertedArguments++;
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
}


std::vector<GNEPythonToolDialog::CategoryOptions>
GNEPythonToolDialog::getOptions(OptionsCont& optionsCont) const {
    // use a vector with only one empty category to reuse code of buildArguments
    std::vector<GNEPythonToolDialog::CategoryOptions> result = {GNEPythonToolDialog::CategoryOptions("")};
    // add all options to result
    for (const auto& option : optionsCont) {
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
    for (const auto& option : optionsCont) {
        if (std::find(categories.begin(), categories.end(), option.second->getSubTopic()) == categories.end()) {
            categories.push_back(option.second->getSubTopic());
        }
    }
    // declare vector of category options and fill
    std::vector<GNEPythonToolDialog::CategoryOptions> result;
    for (const auto& category : categories) {
        result.push_back(GNEPythonToolDialog::CategoryOptions(category));
    }
    // fill result with options
    for (const auto& option : optionsCont) {
        auto category = std::find(result.begin(), result.end(), option.second->getSubTopic());
        // add option in category
        category->addOption(option.first, option.second);
    }
    // drop empty categories
    auto it = result.begin();
    while (it != result.end()) {
        if (it->getOptions().empty()) {
            it = result.erase(it);
        } else {
            it++;
        }
    }
    return result;
}


int
GNEPythonToolDialog::getNumRowColums() const {
    const int column = (int)myArguments.size() / NUMROWSBYCOLUMN;
    return (column < MAXNUMCOLUMNS) ? column : (MAXNUMCOLUMNS - 1);
}


FXVerticalFrame*
GNEPythonToolDialog::getArgumentFrameLeft() const {
    return myArgumentFrameLeft;
}


FXVerticalFrame*
GNEPythonToolDialog::getArgumentFrameRight() const {
    return myArgumentFrameRight;
}

/****************************************************************************/
