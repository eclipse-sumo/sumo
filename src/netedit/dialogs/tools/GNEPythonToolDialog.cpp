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
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

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
    // build arguments
    buildArguments();
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
    resize(rowFramesWidth, rowFramesHeight + mySeparator->getHeight() + myButtonsFrame->getHeight() + (4 * MARGING));
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    return myGNEApp->getApp()->runModalFor(this);
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


FXVerticalFrame*
GNEPythonToolDialog::getRowFrame() {
    return myRowFrames.at(getNumRowColums());
}


int
GNEPythonToolDialog::getNumRowColums() const {
    const int column = (int)myArguments.size() / NUMROWSBYCOLUMN;
    return (column < MAXNUMCOLUMNS)? column : (MAXNUMCOLUMNS - 1);
}


GNEPythonToolDialog::GNEPythonToolDialog() :
    myGNEApp(nullptr) {
}


void
GNEPythonToolDialog::buildArguments() {
    // clear arguments and categories
    for (const auto& argument : myArguments) {
        delete argument;
    }
    for (const auto& category : myCategories) {
        delete category;
    }
    myArguments.clear();
    myCategories.clear();
    // get argument sorted by categories
    const auto categoryOptions = getOptionsByCategories(myPythonTool->getToolsOptions());
    // iterate over options
    for (const auto &categoryOption : categoryOptions) {
        // add category
        myCategories.push_back(new GNEPythonToolDialogElements::Category(this, categoryOption));
        // add options
        for (const auto &option : categoryOption.getOptions()) {
            if (option.second->isInteger()) {
                myArguments.push_back(new GNEPythonToolDialogElements::IntArgument(this, option.first, option.second));
            } else if (option.second->isFloat()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FloatArgument(this, option.first, option.second));
            } else if (option.second->isBool()) {
                myArguments.push_back(new GNEPythonToolDialogElements::BoolArgument(this, option.first, option.second));
            } else if (option.second->isFileName()) {
                myArguments.push_back(new GNEPythonToolDialogElements::FileNameArgument(this, option.first, option.second));       
            } else {
                myArguments.push_back(new GNEPythonToolDialogElements::StringArgument(this, option.first, option.second));
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

/****************************************************************************/
