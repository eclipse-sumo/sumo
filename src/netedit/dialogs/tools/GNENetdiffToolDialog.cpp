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
/// @file    GNENetdiffToolDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Special dialog for netdiff
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetdiffToolDialog.h"
#include "GNEPythonTool.h"


// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetdiffToolDialog::GNENetdiffToolDialog(GNEApplicationWindow* GNEApp) :
    GNEPythonToolDialog(GNEApp) {
    // build options for netdiff
    myNetdiffOptions.addOptionSubTopic("Configuration");
    myNetdiffOptions.doRegister("original-net", new Option_FileName());
    myNetdiffOptions.addDescription("original-net", "Configuration", TL("Original network"));

    myNetdiffOptions.doRegister("modified-net", new Option_FileName());
    myNetdiffOptions.addDescription("modified-net", "Configuration", TL("Modified network"));

    myNetdiffOptions.doRegister("select-modified", new Option_Bool(false));
    myNetdiffOptions.addDescription("select-modified", "Configuration", TL("Select modified elements"));
    
    myNetdiffOptions.doRegister("select-added", new Option_Bool(false));
    myNetdiffOptions.addDescription("select-added", "Configuration", TL("Select added elements"));
    
    myNetdiffOptions.doRegister("select-deleted", new Option_Bool(false));
    myNetdiffOptions.addDescription("select-deleted", "Configuration", TL("Select deleted elements"));
    
    myNetdiffOptions.doRegister("load-shapes-modified", new Option_Bool(false));
    myNetdiffOptions.addDescription("load-shapes-modified", "Configuration", TL("Load shapes for elements"));

    myNetdiffOptions.doRegister("load-shapes-added", new Option_Bool(false));
    myNetdiffOptions.addDescription("load-shapes-added", "Configuration", TL("Load shapes for added"));

    myNetdiffOptions.doRegister("load-shapes-deleted", new Option_Bool(false));
    myNetdiffOptions.addDescription("load-shapes-deleted", "Configuration", TL("Load shapes for deleted elements"));
}


GNENetdiffToolDialog::~GNENetdiffToolDialog() {}


void
GNENetdiffToolDialog::buildArguments() {
    // first clear arguments
    for (const auto& argument : myArguments) {
        delete argument;
    }
    myArguments.clear();
    // create specific netdiff options
    for (const auto &option : myNetdiffOptions) {
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
    // adjust parameter column (call always after create elements)
    adjustParameterColumn();
}

/****************************************************************************/
