/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
// Elements used in GNEToolDialog
/****************************************************************************/
#include <config.h>

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "GNEToolDialogElements.h"
#include "GNEToolDialog.h"


// ============================================-===============================
// member method definitions
// ===========================================================================

GNEToolDialogElements::Argument::Argument(GNEToolDialog *_toolDialogParent, const std::string name, const std::string parameter_) :
    toolDialogParent(_toolDialogParent),
    argumentName(name),
    parameter(parameter_) {
    // add argument in GNEToolDialog parent
    _toolDialogParent->addArgument(this);
}


GNEToolDialogElements::Argument::~Argument() {}


GNEToolDialogElements::Argument::Argument() {}


GNEToolDialogElements::FileNameArgument::FileNameArgument(FXComposite *parent, GNEToolDialog *toolDialogParent, const std::string name, const std::string parameter) :
    FXVerticalFrame(parent, GUIDesignAuxiliarVerticalFrame),
    Argument(toolDialogParent, name, parameter) {
    // Create Button Close (And two more horizontal frames to center it)
    auto horizontalFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(horizontalFrame, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), this, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
}


std::string 
GNEToolDialogElements::FileNameArgument::getArgument() const {
    return "";
}

/****************************************************************************/
