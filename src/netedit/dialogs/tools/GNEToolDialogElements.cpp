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
/// @file    GNEToolDialogElements.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEToolDialog
/****************************************************************************/

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/Translation.h>

#include "GNEToolDialogElements.h"
#include "GNEToolDialog.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEToolDialogElements::Argument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::Argument::Argument(const std::string &name, const Option* option) :
    myName(name),
    myOption(option) {
}


GNEToolDialogElements::Argument::~Argument() {}


const std::string
GNEToolDialogElements::Argument::getName() const {
    return myName;
}


const Option*
GNEToolDialogElements::Argument::getOption() const {
    return myOption;
}


GNEToolDialogElements::Argument::Argument():
    myOption(nullptr) {}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::FileNameArgument::FileNameArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    FXHorizontalFrame(toolDialogParent->getContentFrame(), GUIDesignAuxiliarHorizontalFrame),
    Argument(name, option) {
    // create label with name
    myNameLabel = new FXLabel(this, name.c_str(), nullptr, GUIDesignLabelThick150);
    // Create Open button
    myFilenameButton = new FXButton(this, (std::string("\t\t") + TL("Select filename")).c_str(), 
        GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonIcon);
    // create text field for filename
    myFilenameTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth50);
    // create label with name
    new FXLabel(this, option->getDescription().c_str(), nullptr, GUIDesignLabelThick150);
}


FXLabel*
GNEToolDialogElements::FileNameArgument::getNameLabel() {
    return myNameLabel;
}


void
GNEToolDialogElements::FileNameArgument::reset() {
    myFilenameTextField->setText("");
}


std::string
GNEToolDialogElements::FileNameArgument::getArgument() const {
    return "";
}

// ---------------------------------------------------------------------------
// GNEToolDialogElements::FileNameArgument - methods
// ---------------------------------------------------------------------------

GNEToolDialogElements::StringArgument::StringArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option) :
    FXHorizontalFrame(toolDialogParent->getContentFrame(), GUIDesignAuxiliarHorizontalFrame),
    Argument(name, option) {
    // create label with name
    myNameLabel = new FXLabel(this, name.c_str(), nullptr, GUIDesignLabelThick150);
    // create text field for filename
    myStringTextField = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFielWidth50);
    // create label with name
    new FXLabel(this, option->getDescription().c_str(), nullptr, GUIDesignLabelThick150);
}


FXLabel*
GNEToolDialogElements::StringArgument::getNameLabel() {
    return myNameLabel;
}


void
GNEToolDialogElements::StringArgument::reset() {
    myStringTextField->setText("");
}


std::string
GNEToolDialogElements::StringArgument::getArgument() const {
    return "";
}

/****************************************************************************/
