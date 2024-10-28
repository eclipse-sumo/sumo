/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEPythonToolDialogElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEPythonToolDialog
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>
#include <netedit/tools/GNEPythonTool.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPythonToolDialogElements.h"
#include "GNEPythonToolDialog.h"

// ===========================================================================
// static members
// ===========================================================================

const std::string GNEPythonToolDialogElements::IntArgument::INVALID_INT_STR = toString(INVALID_INT);
const std::string GNEPythonToolDialogElements::FloatArgument::INVALID_DOUBLE_STR = toString(INVALID_DOUBLE);

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEPythonToolDialogElements::Argument) ArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEPythonToolDialogElements::Argument::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_RESET,          GNEPythonToolDialogElements::Argument::onCmdResetValue),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_RESET,          GNEPythonToolDialogElements::Argument::onUpdResetValue)
};

FXDEFMAP(GNEPythonToolDialogElements::FileNameArgument) FileNameArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,  GNEPythonToolDialogElements::FileNameArgument::onCmdOpenFilename)
};

FXDEFMAP(GNEPythonToolDialogElements::EdgeVectorArgument) EdgeVectorArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::EdgeVectorArgument::onCmdUseCurrent),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::EdgeVectorArgument::onUpdUseCurrent)
};

FXDEFMAP(GNEPythonToolDialogElements::NetworkArgument) NetworkArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,         GNEPythonToolDialogElements::NetworkArgument::onCmdOpenFilename),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::NetworkArgument::onCmdUseCurrentNetworkFile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::NetworkArgument::onUpdUseCurrentNetworkFile)
};

FXDEFMAP(GNEPythonToolDialogElements::AdditionalArgument) AdditionalArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,         GNEPythonToolDialogElements::AdditionalArgument::onCmdOpenFilename),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::AdditionalArgument::onCmdUseCurrentAdditionalFile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::AdditionalArgument::onUpdUseCurrentAdditionalFile)
};

FXDEFMAP(GNEPythonToolDialogElements::RouteArgument) RouteArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,         GNEPythonToolDialogElements::RouteArgument::onCmdOpenFilename),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::RouteArgument::onCmdUseCurrentRouteFile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::RouteArgument::onUpdUseCurrentRouteFile)
};

FXDEFMAP(GNEPythonToolDialogElements::DataArgument) DataArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,         GNEPythonToolDialogElements::DataArgument::onCmdOpenFilename),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::DataArgument::onCmdUseCurrentDataFile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::DataArgument::onUpdUseCurrentDataFile)
};

FXDEFMAP(GNEPythonToolDialogElements::SumoConfigArgument) SumoConfigArgumentMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT,         GNEPythonToolDialogElements::SumoConfigArgument::onCmdOpenFilename),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::SumoConfigArgument::onCmdUseCurrentSumoConfigFile),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_USE_CURRENT,    GNEPythonToolDialogElements::SumoConfigArgument::onUpdUseCurrentSumoConfigFile)
};

// Object implementation
FXIMPLEMENT_ABSTRACT(GNEPythonToolDialogElements::Argument,     FXHorizontalFrame,                              ArgumentMap,            ARRAYNUMBER(ArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::EdgeVectorArgument,    GNEPythonToolDialogElements::Argument,          EdgeVectorArgumentMap,  ARRAYNUMBER(EdgeVectorArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::FileNameArgument,      GNEPythonToolDialogElements::Argument,          FileNameArgumentMap,    ARRAYNUMBER(FileNameArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::NetworkArgument,       GNEPythonToolDialogElements::FileNameArgument,  NetworkArgumentMap,     ARRAYNUMBER(NetworkArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::AdditionalArgument,    GNEPythonToolDialogElements::FileNameArgument,  AdditionalArgumentMap,  ARRAYNUMBER(AdditionalArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::RouteArgument,         GNEPythonToolDialogElements::FileNameArgument,  RouteArgumentMap,       ARRAYNUMBER(RouteArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::DataArgument,          GNEPythonToolDialogElements::FileNameArgument,  DataArgumentMap,        ARRAYNUMBER(DataArgumentMap))
FXIMPLEMENT(GNEPythonToolDialogElements::SumoConfigArgument,    GNEPythonToolDialogElements::FileNameArgument,  SumoConfigArgumentMap,  ARRAYNUMBER(SumoConfigArgumentMap))


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::Category - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::Category::Category(FXVerticalFrame* argumentFrame, const std::string& category) :
    FXHorizontalFrame(argumentFrame, GUIDesignAuxiliarHorizontalFrame) {
    // create category label
    new FXLabel(this, category.c_str(), nullptr, GUIDesignLabel(JUSTIFY_NORMAL));
    // create category
    create();
}


GNEPythonToolDialogElements::Category::~Category() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::Argument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::Argument::Argument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string& parameter, Option* option) :
    FXHorizontalFrame(argumentFrame, GUIDesignAuxiliarHorizontalFrame),
    myToolDialogParent(toolDialogParent),
    myOption(option),
    myDefaultValue(toolDialogParent->getPythonTool()->getDefaultValue(parameter)) {
    // create parameter label
    myParameterLabel = new MFXLabelTooltip(this, toolDialogParent->myGNEApp->getStaticTooltipMenu(), parameter.c_str(), nullptr, GUIDesignLabelThickedFixed(0));
    myParameterLabel->setTipText((option->getTypeName() + ": " + option->getDescription()).c_str());
    // set color if is required
    if (option->isRequired()) {
        myParameterLabel->setTextColor(FXRGB(0, 0, 255));
    }
    // create horizontal frame for textField
    myElementsFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarFrame);
    // Create reset button
    myResetButton = GUIDesigns::buildFXButton(this, "", "", TL("Reset value"), GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_RESET, GUIDesignButtonIcon);
    // create argument
    create();
}


GNEPythonToolDialogElements::Argument::~Argument() {}


MFXLabelTooltip*
GNEPythonToolDialogElements::Argument::getParameterLabel() const {
    return myParameterLabel;
}


const std::string
GNEPythonToolDialogElements::Argument::getArgument() const {
    if (getValue() != myDefaultValue) {
        return ("-" + std::string(myParameterLabel->getText().text()) + " " + getValue() + " ");
    } else {
        return "";
    }
}


bool
GNEPythonToolDialogElements::Argument::requiredAttributeSet() const {
    if (myOption->isRequired()) {
        return getValue() != myDefaultValue;
    } else {
        return true;
    }
}


long
GNEPythonToolDialogElements::Argument::onCmdResetValue(FXObject*, FXSelector, void*) {
    // just reset value
    reset();
    return 1;
}


long
GNEPythonToolDialogElements::Argument::onUpdResetValue(FXObject*, FXSelector, void*) {
    if (getValue() == myDefaultValue) {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myResetButton->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::Argument::Argument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::FileNameArgument::FileNameArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, "") {
}


void
GNEPythonToolDialogElements::FileNameArgument::reset() {
    myFilenameTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::FileNameArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get file
    const auto file = GNEApplicationWindowHelper::openFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (file.size() > 0) {
        myFilenameTextField->setText(file.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::FileNameArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myFilenameTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myFilenameTextField->getText().text(), myFilenameTextField->getText().text(), false);
    }
    return 1;
}


GNEPythonToolDialogElements::FileNameArgument::FileNameArgument() {}


GNEPythonToolDialogElements::FileNameArgument::FileNameArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option, const std::string& useCurrent) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // check if create current button
    if (useCurrent.size() > 0) {
        myCurrentButton = new MFXButtonTooltip(myElementsFrame, toolDialogParent->myGNEApp->getStaticTooltipMenu(), "",
                                               GUIIconSubSys::getIcon(GUIIcon::CURRENT), this, MID_GNE_USE_CURRENT, GUIDesignButtonIcon);
        myCurrentButton->setTipText(TLF("Use current % file", useCurrent).c_str());
        myCurrentButton->create();
    }
    // Create Open button
    myOpenFilenameButton = new MFXButtonTooltip(myElementsFrame, toolDialogParent->myGNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::OPEN), this, MID_GNE_SELECT, GUIDesignButtonIcon);
    myOpenFilenameButton->setTipText(TLF("Select % file", useCurrent).c_str());
    myOpenFilenameButton->create();
    // create text field for filename
    myFilenameTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myFilenameTextField->create();
    // set value
    myFilenameTextField->setText(option->getValueString().c_str());
}

const std::string
GNEPythonToolDialogElements::FileNameArgument::getValue() const {
    return myFilenameTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::EdgeVectorArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::EdgeVectorArgument::EdgeVectorArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    myCurrentEdgesButton = new MFXButtonTooltip(myElementsFrame, toolDialogParent->myGNEApp->getStaticTooltipMenu(), "",
            GUIIconSubSys::getIcon(GUIIcon::EDGE), this, MID_GNE_USE_CURRENT, GUIDesignButtonIcon);
    myCurrentEdgesButton->setTipText(TL("Use current selected edges"));
    myCurrentEdgesButton->create();
    // create text field for string
    myEdgeVectorTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEdgeVectorTextField->create();
    // set value
    myEdgeVectorTextField->setText(option->getValueString().c_str());
}


void
GNEPythonToolDialogElements::EdgeVectorArgument::reset() {
    myEdgeVectorTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::EdgeVectorArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myEdgeVectorTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myEdgeVectorTextField->getText().text(), myEdgeVectorTextField->getText().text(), false);
    }
    return 1;
}


long
GNEPythonToolDialogElements::EdgeVectorArgument::onCmdUseCurrent(FXObject*, FXSelector, void*) {
    // obtain list of selected edges
    const auto selectedEdges = myToolDialogParent->getGNEApplicationWindow()->getViewNet()->getNet()->getAttributeCarriers()->getSelectedEdges();
    // convert list to string
    std::string selectedEdgesStr;
    for (const auto& edge : selectedEdges) {
        selectedEdgesStr.append(edge->getID());
        if (edge != selectedEdges.back()) {
            selectedEdgesStr.append(" ");
        }
    }
    myEdgeVectorTextField->setText(selectedEdgesStr.c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::EdgeVectorArgument::onUpdUseCurrent(FXObject* sender, FXSelector, void*) {
    // get view net
    const auto viewNet = myToolDialogParent->getGNEApplicationWindow()->getViewNet();
    if (viewNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (viewNet->getNet()->getAttributeCarriers()->getNumberOfSelectedEdges() == 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::EdgeVectorArgument::EdgeVectorArgument() {}


const std::string
GNEPythonToolDialogElements::EdgeVectorArgument::getValue() const {
    return myEdgeVectorTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::NetworkArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::NetworkArgument::NetworkArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, TL("network")) {
}


long
GNEPythonToolDialogElements::NetworkArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get network file
    const auto networkFile = GNEApplicationWindowHelper::openNetworkFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (networkFile.size() > 0) {
        myFilenameTextField->setText(networkFile.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::NetworkArgument::onCmdUseCurrentNetworkFile(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(OptionsCont::getOptions().getString("sumo-net-file").c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::NetworkArgument::onUpdUseCurrentNetworkFile(FXObject* sender, FXSelector, void*) {
    if (myToolDialogParent->getGNEApplicationWindow()->getViewNet() == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("sumo-net-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::NetworkArgument::NetworkArgument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::AdditionalArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::AdditionalArgument::AdditionalArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, TL("additional")) {
}


long
GNEPythonToolDialogElements::AdditionalArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get additional file
    const auto additionalFile = GNEApplicationWindowHelper::openAdditionalFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (additionalFile.size() > 0) {
        myFilenameTextField->setText(additionalFile.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::AdditionalArgument::onCmdUseCurrentAdditionalFile(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(OptionsCont::getOptions().getString("additional-files").c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::AdditionalArgument::onUpdUseCurrentAdditionalFile(FXObject* sender, FXSelector, void*) {
    if (myToolDialogParent->getGNEApplicationWindow()->getViewNet() == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("additional-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::AdditionalArgument::AdditionalArgument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::RouteArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::RouteArgument::RouteArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, TL("route")) {
}


long
GNEPythonToolDialogElements::RouteArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get route file
    const auto routeFile = GNEApplicationWindowHelper::openRouteFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (routeFile.size() > 0) {
        myFilenameTextField->setText(routeFile.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::RouteArgument::onCmdUseCurrentRouteFile(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(OptionsCont::getOptions().getString("route-files").c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::RouteArgument::onUpdUseCurrentRouteFile(FXObject* sender, FXSelector, void*) {
    if (myToolDialogParent->getGNEApplicationWindow()->getViewNet() == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("route-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::RouteArgument::RouteArgument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::DataArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::DataArgument::DataArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, TL("data")) {
}


long
GNEPythonToolDialogElements::DataArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get data file
    const auto dataFile = GNEApplicationWindowHelper::openDataFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (dataFile.size() > 0) {
        myFilenameTextField->setText(dataFile.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::DataArgument::onCmdUseCurrentDataFile(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(OptionsCont::getOptions().getString("data-files").c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::DataArgument::onUpdUseCurrentDataFile(FXObject* sender, FXSelector, void*) {
    if (myToolDialogParent->getGNEApplicationWindow()->getViewNet() == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("data-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::DataArgument::DataArgument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::SumoConfigArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::SumoConfigArgument::SumoConfigArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
        const std::string name, Option* option) :
    FileNameArgument(toolDialogParent, argumentFrame, name, option, TL("sumo config")) {
}


long
GNEPythonToolDialogElements::SumoConfigArgument::onCmdOpenFilename(FXObject*, FXSelector, void*) {
    // get sumoConfig file
    const auto sumoConfigFile = GNEApplicationWindowHelper::openSumoConfigFileDialog(this, (myOption->getSubTopic() == "output"), myOption->getListSeparator() != "");
    // check that file is valid
    if (sumoConfigFile.size() > 0) {
        myFilenameTextField->setText(sumoConfigFile.c_str(), TRUE);
    }
    return 1;
}


long
GNEPythonToolDialogElements::SumoConfigArgument::onCmdUseCurrentSumoConfigFile(FXObject*, FXSelector, void*) {
    myFilenameTextField->setText(OptionsCont::getOptions().getString("sumocfg-file").c_str(), TRUE);
    return 1;
}


long
GNEPythonToolDialogElements::SumoConfigArgument::onUpdUseCurrentSumoConfigFile(FXObject* sender, FXSelector, void*) {
    if (myToolDialogParent->getGNEApplicationWindow()->getViewNet() == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("sumocfg-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


GNEPythonToolDialogElements::SumoConfigArgument::SumoConfigArgument() {}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::EdgeArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::EdgeArgument::EdgeArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // create text field for int
    myEdgeTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myEdgeTextField->create();
    // set value
    myEdgeTextField->setText(option->getValueString().c_str());
}


void
GNEPythonToolDialogElements::EdgeArgument::reset() {
    myEdgeTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::EdgeArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myEdgeTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myEdgeTextField->getText().text(), myEdgeTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::EdgeArgument::getValue() const {
    return myEdgeTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::StringArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::StringArgument::StringArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // create text field for string
    myStringTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myStringTextField->create();
    // set value
    myStringTextField->setText(option->getValueString().c_str());
}


void
GNEPythonToolDialogElements::StringArgument::reset() {
    myStringTextField->setText(myDefaultValue.c_str());
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::StringArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myStringTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myStringTextField->getText().text(), myStringTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::StringArgument::getValue() const {
    return myStringTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::IntArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::IntArgument::IntArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // create text field for int
    myIntTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_INTEGER));
    myIntTextField->create();
    // set value
    myIntTextField->setText(option->getValueString().c_str());
}


void
GNEPythonToolDialogElements::IntArgument::reset() {
    myIntTextField->setText(myDefaultValue.c_str());
    if (myDefaultValue.empty()) {
        myOption->set(INVALID_INT_STR, "", false);
    } else {
        myOption->set(myDefaultValue, myDefaultValue, false);
    }
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::IntArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myIntTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myIntTextField->getText().text(), myIntTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::IntArgument::getValue() const {
    return myIntTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::FloatArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::FloatArgument::FloatArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // create text field for float
    myFloatTextField = new FXTextField(myElementsFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    myFloatTextField->create();
    // set value
    myFloatTextField->setText(option->getValueString().c_str());
}


void
GNEPythonToolDialogElements::FloatArgument::reset() {
    myFloatTextField->setText(myDefaultValue.c_str());
    if (myDefaultValue.empty()) {
        myOption->set(INVALID_DOUBLE_STR, "", false);
    } else {
        myOption->set(myDefaultValue, myDefaultValue, false);
    }
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::FloatArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myFloatTextField->getText().empty()) {
        reset();
    } else {
        myOption->set(myFloatTextField->getText().text(), myFloatTextField->getText().text(), false);
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::FloatArgument::getValue() const {
    return myFloatTextField->getText().text();
}

// ---------------------------------------------------------------------------
// GNEPythonToolDialogElements::BoolArgument - methods
// ---------------------------------------------------------------------------

GNEPythonToolDialogElements::BoolArgument::BoolArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option) :
    Argument(toolDialogParent, argumentFrame, name, option) {
    // create check button
    myCheckButton = new FXCheckButton(myElementsFrame, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myCheckButton->create();
    // set value
    if (option->getBool()) {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
}


void
GNEPythonToolDialogElements::BoolArgument::reset() {
    if (myDefaultValue == "True") {
        myCheckButton->setCheck(TRUE);
        myCheckButton->setText(TL("true"));
    } else {
        myCheckButton->setCheck(FALSE);
        myCheckButton->setText(TL("false"));
    }
    myOption->set(myDefaultValue, myDefaultValue, false);
    myOption->resetDefault();
}


long
GNEPythonToolDialogElements::BoolArgument::onCmdSetValue(FXObject*, FXSelector, void*) {
    myOption->resetWritable();
    if (myCheckButton->getCheck() == TRUE) {
        myCheckButton->setText(TL("true"));
        myOption->set("True", "True", false);
        if (myDefaultValue == "True") {
            myOption->resetDefault();
        }
    } else {
        myCheckButton->setText(TL("false"));
        myOption->set("False", "False", false);
        if (myDefaultValue == "False") {
            myOption->resetDefault();
        }
    }
    return 1;
}


const std::string
GNEPythonToolDialogElements::BoolArgument::getValue() const {
    if (myCheckButton->getCheck() == TRUE) {
        return "True";
    } else {
        return "False";
    }
}

/****************************************************************************/
