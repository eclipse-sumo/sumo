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
/// @file    GNEToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog for tools
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/FileHelpers.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>

#include "GNEToolDialog.h"
#include "GNETool.h"

#define MARGING 4

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEToolDialog) GNEToolDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                      GNEToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,     GNEToolDialog::onCmdRun),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,  GNEToolDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RESET,   GNEToolDialog::onCmdReset)
};

// Object implementation
FXIMPLEMENT(GNEToolDialog, FXDialogBox, GNEToolDialogMap, ARRAYNUMBER(GNEToolDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNEToolDialog::GNEToolDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Tool", GUIDesignDialogBoxExplicit(0, 0)),
    myGNEApp(GNEApp) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::TOOL_PYTHON));
    // build horizontalFrame for content
    myContentFrame = new FXVerticalFrame(this, GUIDesignAuxiliarFrameFixed);
    // first add header
    auto headerFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myParameterLabel = new FXLabel(headerFrame, TL("Parameter"), nullptr, GUIDesignLabelThickedFixed(0));
    new FXLabel(headerFrame, TL("Value"), nullptr, GUIDesignLabelThickedFixed(250));
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


GNEToolDialog::~GNEToolDialog() {}


void
GNEToolDialog::openDialog(GNETool* tool) {
    // set tool
    myTool = tool;
    // set title
    setTitle(myTool->getToolName().c_str());
    // build arguments
    buildArguments();
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
    // resize dialog (Marging + contentFrame + MARGING separator + MARGING + buttonsFrame + MARGING)
    resize(myContentFrame->getWidth(), myContentFrame->getHeight() + mySeparator->getHeight() + myButtonsFrame->getHeight() + (4 * MARGING));
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    myGNEApp->getApp()->runModalFor(this);
}


void
GNEToolDialog::hide() {
    // stop modal
    myGNEApp->getApp()->stopModal(this);
    // hide dialog
    FXDialogBox::hide();
}


long
GNEToolDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // hide tool dialog
    hide();
    // run tool
    return myGNEApp->tryHandle(myTool->getMenuCommand(), FXSEL(SEL_COMMAND, MID_GNE_RUNTOOL), nullptr);
}


long
GNEToolDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // just hide tool dialog
    hide();
    return 1;
}


long
GNEToolDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // iterate over all arguments and reset values
    for (const auto& argument : myArguments) {
        argument->reset();
    }
    return 1;
}


GNEToolDialog::GNEToolDialog() :
    myGNEApp(nullptr) {
}


void
GNEToolDialog::buildArguments() {
    // first clear arguments
    for (const auto& argument : myArguments) {
        delete argument;
    }
    myArguments.clear();
    // iterate over options
    for (const auto &option : myTool->getToolsOptions()) {
        if (option.second->isInteger()) {
            myArguments.push_back(new GNEToolDialogElements::IntArgument(this, option.first, option.second));
        } else if (option.second->isFloat()) {
            myArguments.push_back(new GNEToolDialogElements::FloatArgument(this, option.first, option.second));
        } else if (option.second->isBool()) {
            myArguments.push_back(new GNEToolDialogElements::BoolArgument(this, option.first, option.second));
        } else if (option.second->isFileName()) {
            myArguments.push_back(new GNEToolDialogElements::FileNameArgument(this, option.first, option.second));       
        } else {
            myArguments.push_back(new GNEToolDialogElements::StringArgument(this, option.first, option.second));
        }
    }
    // adjust parameter column (call always after create elements)
    adjustParameterColumn();
}


void
GNEToolDialog::adjustParameterColumn() {
    int maximumWidth = 0;
    // iterate over all arguments and find the maximum width
    for (const auto &argument : myArguments) {
        const auto label = argument->getParameterLabel();
        const int columnWidth = label->getFont()->getTextWidth(label->getText().text(), label->getText().length() + MARGING);
        if (columnWidth > maximumWidth) {
            maximumWidth = columnWidth;
        }
    }
    // set maximum width for all parameter labels
    for (const auto &argument : myArguments) {
        argument->getParameterLabel()->setWidth(maximumWidth);
    }
    // set parameter label width
    myParameterLabel->setWidth(maximumWidth);
    // set content frame size (MARGING + Parameter + TextField + MARGING)
    myContentFrame->setWidth(MARGING + maximumWidth + 250 + GUIDesignHeight + MARGING);
    myContentFrame->setHeight(GUIDesignHeight * ((int)myArguments.size() + 1));
}

/****************************************************************************/
