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
/// @file    GNENetgenerateDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for netgenerate
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetgenerateDialog.h"

// ===========================================================================
// Defines
// ===========================================================================

#define MARGIN 4
#define MAXNUMCOLUMNS 4
#define NUMROWSBYCOLUMN 20

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNENetgenerateDialog) GNENetgenerateDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN,                   GNENetgenerateDialog::onCmdOpenOutputFile),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNENetgenerateDialog::onCmdSetOutput),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_GRID,       GNENetgenerateDialog::onCmdSetGridNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_SPIDER,     GNENetgenerateDialog::onCmdSetSpiderNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_RANDOMGRID, GNENetgenerateDialog::onCmdSetRandomNetworkGridNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_RANDOM,     GNENetgenerateDialog::onCmdSetRandomNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADVANCED,        GNENetgenerateDialog::onCmdAdvanced),
};

// Object implementation
FXIMPLEMENT(GNENetgenerateDialog, GNEDialog, GNENetgenerateDialogMap, ARRAYNUMBER(GNENetgenerateDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetgenerateDialog::GNENetgenerateDialog(GNEApplicationWindow* applicationWindow) :
    GNEDialog(applicationWindow, "Netgenerate", GUIIcon::NETGENERATE,
              DialogType::NETGENERATE, GNEDialog::Buttons::RUN_ADVANCED_CANCEL,
              GNEDialog::OpenType::MODAL, ResizeMode::STATIC) {
    // build labels
    auto horizontalFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myGridNetworkLabel = new FXLabel(horizontalFrame, TL("Grid"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    mySpiderNetworkLabel = new FXLabel(horizontalFrame, TL("Spider"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    myRandomGridNetworkLabel = new FXLabel(horizontalFrame, TL("Random grid"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    myRandomNetworkLabel = new FXLabel(horizontalFrame, TL("Random"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    // build buttons
    horizontalFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    myGridNetworkButton = new MFXCheckableButton(false, horizontalFrame, applicationWindow->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_GRID), this, MID_GNE_NETGENERATE_GRID, GUIDesignMFXCheckableButtonBig);
    mySpiderNetworkButton = new MFXCheckableButton(false, horizontalFrame, applicationWindow->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_SPIDER), this, MID_GNE_NETGENERATE_SPIDER, GUIDesignMFXCheckableButtonBig);
    myRandomGridNetworkButton = new MFXCheckableButton(false, horizontalFrame, applicationWindow->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_RANDOMGRID), this, MID_GNE_NETGENERATE_RANDOMGRID, GUIDesignMFXCheckableButtonBig);
    myRandomNetworkButton = new MFXCheckableButton(false, horizontalFrame, applicationWindow->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_RANDOM), this, MID_GNE_NETGENERATE_RANDOM, GUIDesignMFXCheckableButtonBig);
    // add invisible separator
    new FXSeparator(myContentFrame, SEPARATOR_NONE);
    // build output file elements
    horizontalFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrame, "output-file", nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    GUIDesigns::buildFXButton(horizontalFrame, "", "", TL("Select filename"),
                              GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GNE_OPEN, GUIDesignButtonIcon);
    myOutputTextField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // open dialog
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    // reset buttons
    if (generateOptions.getBool("grid")) {
        if (generateOptions.getBool("rand.grid")) {
            myRandomGridNetworkButton->setChecked(true);
            onCmdSetRandomNetworkGridNetwork(nullptr, 0, nullptr);
        } else {
            myGridNetworkButton->setChecked(true);
            onCmdSetGridNetwork(nullptr, 0, nullptr);
        }
    } else if (generateOptions.getBool("spider")) {
        mySpiderNetworkButton->setChecked(true);
        onCmdSetSpiderNetwork(nullptr, 0, nullptr);
    } else if (generateOptions.getBool("random")) {
        myRandomNetworkButton->setChecked(true);
        onCmdSetRandomNetwork(nullptr, 0, nullptr);
    }
    // set output
    myOutputTextField->setText(generateOptions.getValueString("output-file").c_str());
    // disable run and advanced
    updateRunButtons();
    // open dialog
    openDialog();
}


GNENetgenerateDialog::~GNENetgenerateDialog() {}


void
GNENetgenerateDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNENetgenerateDialog::onCmdOpenOutputFile(FXObject*, FXSelector, void*) {
    // get output file
    const auto networkFileDialog = GNEFileDialog(myApplicationWindow, TL("network file"),
                                   SUMOXMLDefinitions::NetFileExtensions.getStrings(),
                                   GNEFileDialog::OpenMode::SAVE,
                                   GNEFileDialog::ConfigType::NETEDIT);
    // check file
    if (networkFileDialog.getResult() == GNEDialog::Result::ACCEPT) {
        myOutputTextField->setText(networkFileDialog.getFilename().c_str(), TRUE);
    }
    return 1;
}


long
GNENetgenerateDialog::onCmdSetOutput(FXObject*, FXSelector, void*) {
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    generateOptions.resetWritable();
    // check if filename is valid
    if (SUMOXMLDefinitions::isValidFilename(myOutputTextField->getText().text()) == false) {
        myOutputTextField->setTextColor(GUIDesignTextColorRed);
    } else {
        generateOptions.set("output-file", myOutputTextField->getText().text());
        myOutputTextField->setTextColor(GUIDesignTextColorBlack);
    }
    updateRunButtons();
    return 1;
}


long
GNENetgenerateDialog::onCmdSetGridNetwork(FXObject*, FXSelector, void*) {
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    // reset all flags
    generateOptions.resetWritable();
    generateOptions.set("grid", "true");
    generateOptions.set("spider", "false");
    generateOptions.set("rand", "false");
    generateOptions.set("rand.grid", "false");
    // set buttons
    myGridNetworkButton->setChecked(true, true);
    mySpiderNetworkButton->setChecked(false, true);
    myRandomGridNetworkButton->setChecked(false, true);
    myRandomNetworkButton->setChecked(false, true);
    // set labels color
    myGridNetworkLabel->setTextColor(GUIDesignTextColorBlue);
    mySpiderNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    // enable flag
    mySelectedNetworktypeFlag = true;
    updateRunButtons();
    return 1;
}


long
GNENetgenerateDialog::onCmdSetSpiderNetwork(FXObject*, FXSelector, void*) {
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    // reset all flags
    generateOptions.resetWritable();
    generateOptions.set("grid", "false");
    generateOptions.set("spider", "true");
    generateOptions.set("spider.omit-center", "true");
    generateOptions.set("rand", "false");
    generateOptions.set("rand.grid", "false");
    // set buttons
    myGridNetworkButton->setChecked(false, true);
    mySpiderNetworkButton->setChecked(true, true);
    myRandomGridNetworkButton->setChecked(false, true);
    myRandomNetworkButton->setChecked(false, true);
    // set labels color
    myGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    mySpiderNetworkLabel->setTextColor(GUIDesignTextColorBlue);
    myRandomGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    // enable flag
    mySelectedNetworktypeFlag = true;
    updateRunButtons();
    return 1;
}


long
GNENetgenerateDialog::onCmdSetRandomNetworkGridNetwork(FXObject*, FXSelector, void*) {
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    // reset all flags
    generateOptions.resetWritable();
    generateOptions.set("grid", "false");
    generateOptions.set("spider", "false");
    generateOptions.set("rand", "true");
    generateOptions.set("rand.grid", "true");
    // set buttons
    myGridNetworkButton->setChecked(false, true);
    mySpiderNetworkButton->setChecked(false, true);
    myRandomGridNetworkButton->setChecked(true, true);
    myRandomNetworkButton->setChecked(false, true);
    // set labels color
    myGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    mySpiderNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomGridNetworkLabel->setTextColor(GUIDesignTextColorBlue);
    myRandomNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    // enable flag
    mySelectedNetworktypeFlag = true;
    updateRunButtons();
    return 1;
}


long
GNENetgenerateDialog::onCmdSetRandomNetwork(FXObject*, FXSelector, void*) {
    auto& generateOptions = myApplicationWindow->getNetgenerateOptions();
    // reset all flags
    generateOptions.resetWritable();
    generateOptions.set("grid", "false");
    generateOptions.set("spider", "false");
    generateOptions.set("rand", "true");
    generateOptions.set("rand.grid", "false");
    // set buttons
    myGridNetworkButton->setChecked(false, true);
    mySpiderNetworkButton->setChecked(false, true);
    myRandomGridNetworkButton->setChecked(false, true);
    myRandomNetworkButton->setChecked(true, true);
    // set labels color
    myGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    mySpiderNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomGridNetworkLabel->setTextColor(GUIDesignTextColorBlack);
    myRandomNetworkLabel->setTextColor(GUIDesignTextColorBlue);
    // enable flag
    mySelectedNetworktypeFlag = true;
    updateRunButtons();
    return 1;
}


long
GNENetgenerateDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // close dialog
    closeDialogCanceling();
    // run netgenerate
    return myApplicationWindow->tryHandle(this, FXSEL(SEL_COMMAND, MID_GNE_RUNNETGENERATE), nullptr);
}


long
GNENetgenerateDialog::onCmdAdvanced(FXObject*, FXSelector, void*) {
    // close dialog
    closeDialogCanceling();
    // open netgenerate option dialog
    return myApplicationWindow->tryHandle(this, FXSEL(SEL_COMMAND, MID_GNE_NETGENERATEOPTIONS), nullptr);
}


void
GNENetgenerateDialog::updateRunButtons() {
    // enable or disable run and advanced buttons depending of flags
    if ((myOutputTextField->getText().length() > 0) && mySelectedNetworktypeFlag) {
        myRunButton->enable();
        myAdvancedButton->enable();
    } else {
        myRunButton->disable();
        myAdvancedButton->disable();
    }
}

/****************************************************************************/
