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
/// @file    GNENetdiffPythonToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Special dialog for netdiff
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetdiffPythonToolDialog.h"
#include "GNEPythonTool.h"


// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetdiffPythonToolDialog::GNENetdiffPythonToolDialog(GNEApplicationWindow* GNEApp) :
    GNEPythonToolDialog(GNEApp) {
}


GNENetdiffPythonToolDialog::~GNENetdiffPythonToolDialog() {}


void
GNENetdiffPythonToolDialog::buildArguments() {
/*
    // first clear arguments
    for (const auto& argument : myArguments) {
        delete argument;
    }
    myArguments.clear();
    // iterate over options
    for (const auto &option : myPythonTool->getToolsOptions()) {
        if (option.second->isInteger()) {
            myArguments.push_back(new GNENetdiffPythonToolDialogElements::IntArgument(this, option.first, option.second));
        } else if (option.second->isFloat()) {
            myArguments.push_back(new GNENetdiffPythonToolDialogElements::FloatArgument(this, option.first, option.second));
        } else if (option.second->isBool()) {
            myArguments.push_back(new GNENetdiffPythonToolDialogElements::BoolArgument(this, option.first, option.second));
        } else if (option.second->isFileName()) {
            myArguments.push_back(new GNENetdiffPythonToolDialogElements::FileNameArgument(this, option.first, option.second));       
        } else {
            myArguments.push_back(new GNENetdiffPythonToolDialogElements::StringArgument(this, option.first, option.second));
        }
    }
    // adjust parameter column (call always after create elements)
    adjustParameterColumn();
*/
}

/****************************************************************************/
