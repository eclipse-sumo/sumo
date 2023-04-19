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
/// @file    GNENetgenerateDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// Dialog for netgenerate
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetgenerateDialog.h"

#define MARGING 4
#define MAXNUMCOLUMNS 4
#define NUMROWSBYCOLUMN 20

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNENetgenerateDialog) GNENetgenerateDialogMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0,                              GNENetgenerateDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN,                   GNENetgenerateDialog::onCmdOpenOutputFile),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNENetgenerateDialog::onCmdSetOutput),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_GRID,       GNENetgenerateDialog::onCmdSetGrid),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_SPIDER,     GNENetgenerateDialog::onCmdSetSpider),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_RANDOMGRID, GNENetgenerateDialog::onCmdSetRandomGrid),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_NETGENERATE_RANDOM,     GNENetgenerateDialog::onCmdSetRandom),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_RUN,             GNENetgenerateDialog::onCmdRun),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_BUTTON_RUN,             GNENetgenerateDialog::onUpdSettingsConfigured),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADVANCED,        GNENetgenerateDialog::onCmdAdvanced),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_BUTTON_ADVANCED,        GNENetgenerateDialog::onUpdSettingsConfigured),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_CANCEL,          GNENetgenerateDialog::onCmdCancel),
};

// Object implementation
FXIMPLEMENT(GNENetgenerateDialog, FXDialogBox, GNENetgenerateDialogMap, ARRAYNUMBER(GNENetgenerateDialogMap))

// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetgenerateDialog::GNENetgenerateDialog(GNEApplicationWindow* GNEApp) :
    FXDialogBox(GNEApp->getApp(), "Netgenerate", GUIDesignDialogBox),
    myGNEApp(GNEApp) {
    // set icon
    setIcon(GUIIconSubSys::getIcon(GUIIcon::NETGENERATE));
    // build labels
    auto horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGridNetworkLabel = new FXLabel(horizontalFrame, TL("Grid"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    mySpiderNetworkLabel = new FXLabel(horizontalFrame, TL("Spider"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    myRandomGridNetworkLabel = new FXLabel(horizontalFrame, TL("Random grid"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    myRandomNetworkLabel = new FXLabel(horizontalFrame, TL("Random"), nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    // build buttons
    horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGridNetworkButton = new MFXCheckableButton(false, horizontalFrame, GNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_GRID), this, MID_GNE_NETGENERATE_GRID, GUIDesignMFXCheckableButtonBig);
    mySpiderNetworkButton = new MFXCheckableButton(false, horizontalFrame, GNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_SPIDER), this, MID_GNE_NETGENERATE_SPIDER, GUIDesignMFXCheckableButtonBig);
    myRandomGridNetworkButton = new MFXCheckableButton(false, horizontalFrame, GNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_RANDOMGRID), this, MID_GNE_NETGENERATE_RANDOMGRID, GUIDesignMFXCheckableButtonBig);
    myRandomNetworkButton = new MFXCheckableButton(false, horizontalFrame, GNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::NETGENERATE_RANDOM), this, MID_GNE_NETGENERATE_RANDOM, GUIDesignMFXCheckableButtonBig);
    // add invisible separator
    new FXSeparator(this, SEPARATOR_NONE);
    // build output file elements
    horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrame, "output-file", nullptr, GUIDesignLabelThickedFixed(GUIDesignBigSizeElement));
    new FXButton(horizontalFrame, (std::string("\t\t") + TL("Select filename")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, MID_GNE_OPEN, GUIDesignButtonIcon);
    myOutputTextField = new FXTextField(horizontalFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // add separator
    new FXSeparator(this);
    // create buttons centered
    horizontalFrame = new FXHorizontalFrame(this, GUIDesignHorizontalFrame);
    new FXHorizontalFrame(horizontalFrame, GUIDesignAuxiliarHorizontalFrame);
    myRunButton = new FXButton(horizontalFrame, (TL("Run") + std::string("\t\t") + TL("close accepting changes")).c_str(),
                               GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, MID_GNE_BUTTON_RUN, GUIDesignButtonAccept);
    myAdvancedButton = new FXButton(horizontalFrame, (TL("Advanced") + std::string("\t\t") + TL("open advance netgenerate dialog")).c_str(),
                                    GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), this, MID_GNE_BUTTON_ADVANCED, GUIDesignButtonAdvanced);
    new FXButton(horizontalFrame, (TL("Cancel") + std::string("\t\t") + TL("Close dialog")).c_str(),
                 GUIIconSubSys::getIcon(GUIIcon::CANCEL),  this, MID_GNE_BUTTON_CANCEL,  GUIDesignButtonReset);
    new FXHorizontalFrame(horizontalFrame, GUIDesignAuxiliarHorizontalFrame);
}


GNENetgenerateDialog::~GNENetgenerateDialog() {}


void
GNENetgenerateDialog::openDialog() {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
    // reset buttons
    if (generateOptions.getBool("grid")) {
        if (generateOptions.getBool("rand.grid")) {
            myRandomGridNetworkButton->setChecked(true);
            onCmdSetRandomGrid(nullptr, 0, nullptr);
        } else {
            myGridNetworkButton->setChecked(true);
            onCmdSetGrid(nullptr, 0, nullptr);
        }
    } else if (generateOptions.getBool("spider")) {
        mySpiderNetworkButton->setChecked(true);
        onCmdSetSpider(nullptr, 0, nullptr);
    } else if (generateOptions.getBool("random")) {
        myRandomNetworkButton->setChecked(true);
        onCmdSetRandom(nullptr, 0, nullptr);
    }
    // set output
    myOutputTextField->setText(generateOptions.getValueString("output-file").c_str());
    // show dialog
    FXDialogBox::show(PLACEMENT_SCREEN);
    // refresh APP
    getApp()->refresh();
}


long
GNENetgenerateDialog::onCmdOpenOutputFile(FXObject*, FXSelector, void*) {
    // get output file
    const auto outputFile = GNEApplicationWindowHelper::openNetworkFileDialog(this, true);
    // check file
    if (!outputFile.empty()) {
        myOutputTextField->setText(outputFile.c_str(), TRUE);
    }
    return 1;
}


long
GNENetgenerateDialog::onCmdSetOutput(FXObject*, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
    generateOptions.resetWritable();
    // check if filename is valid
    if (SUMOXMLDefinitions::isValidFilename(myOutputTextField->getText().text()) == false) {
        myOutputTextField->setTextColor(FXRGB(255, 0, 0));
    } else {
        generateOptions.set("output-file", myOutputTextField->getText().text());
        myOutputTextField->setTextColor(FXRGB(0, 0, 0));
    }
    return 1;
}


long
GNENetgenerateDialog::onCmdSetGrid(FXObject*, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
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
    myGridNetworkLabel->setTextColor(FXRGB(0, 0, 255));
    mySpiderNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    return 1;
}


long
GNENetgenerateDialog::onCmdSetSpider(FXObject*, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
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
    myGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    mySpiderNetworkLabel->setTextColor(FXRGB(0, 0, 255));
    myRandomGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    return 1;
}


long
GNENetgenerateDialog::onCmdSetRandomGrid(FXObject*, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
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
    myGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    mySpiderNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomGridNetworkLabel->setTextColor(FXRGB(0, 0, 255));
    myRandomNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    return 1;
}


long
GNENetgenerateDialog::onCmdSetRandom(FXObject*, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
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
    myGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    mySpiderNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomGridNetworkLabel->setTextColor(FXRGB(0, 0, 0));
    myRandomNetworkLabel->setTextColor(FXRGB(0, 0, 255));
    return 1;
}


long
GNENetgenerateDialog::onCmdRun(FXObject*, FXSelector, void*) {
    // hide dialog
    hide();
    // run netgenerate
    return myGNEApp->tryHandle(this, FXSEL(SEL_COMMAND, MID_GNE_RUNNETGENERATE), nullptr);
}


long
GNENetgenerateDialog::onCmdAdvanced(FXObject*, FXSelector, void*) {
    // hide dialog
    hide();
    // open netgenerate option dialog
    return myGNEApp->tryHandle(this, FXSEL(SEL_COMMAND, MID_GNE_NETGENERATEOPTIONS), nullptr);
}


long
GNENetgenerateDialog::onUpdSettingsConfigured(FXObject* sender, FXSelector, void*) {
    auto& generateOptions = myGNEApp->getNetgenerateOptions();
    // check conditions
    if ((generateOptions.getBool("grid") == false) &&
            (generateOptions.getBool("spider") == false) &&
            (generateOptions.getBool("rand") == false)) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (generateOptions.getValueString("output-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNENetgenerateDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // hide dialog
    hide();
    return 1;
}


GNENetgenerateDialog::GNENetgenerateDialog() :
    myGNEApp(nullptr) {
}

/****************************************************************************/
