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
#include <netedit/tools/GNEPythonTool.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENetdiffToolDialog.h"

// ============================================-===============================
// member method definitions
// ===========================================================================

GNENetdiffToolDialog::GNENetdiffToolDialog(GNEApplicationWindow* GNEApp) :
    GNEPythonToolDialog(GNEApp) {
    // build custom options for netdiff
    myCustomToolsOptions.addOptionSubTopic("Configuration");
    myCustomToolsOptions.doRegister("original-net", new Option_Network());
    myCustomToolsOptions.addDescription("original-net", "Configuration", TL("Original network"));

    myCustomToolsOptions.doRegister("modified-net", new Option_Network());
    myCustomToolsOptions.addDescription("modified-net", "Configuration", TL("Modified network"));

    myCustomToolsOptions.doRegister("select-modified", new Option_Bool(false));
    myCustomToolsOptions.addDescription("select-modified", "Configuration", TL("Select modified elements"));
    
    myCustomToolsOptions.doRegister("select-added", new Option_Bool(false));
    myCustomToolsOptions.addDescription("select-added", "Configuration", TL("Select added elements"));
    
    myCustomToolsOptions.doRegister("select-deleted", new Option_Bool(false));
    myCustomToolsOptions.addDescription("select-deleted", "Configuration", TL("Select deleted elements"));
    
    myCustomToolsOptions.doRegister("load-shapes-modified", new Option_Bool(false));
    myCustomToolsOptions.addDescription("load-shapes-modified", "Configuration", TL("Load shapes for elements"));

    myCustomToolsOptions.doRegister("load-shapes-added", new Option_Bool(false));
    myCustomToolsOptions.addDescription("load-shapes-added", "Configuration", TL("Load shapes for added"));

    myCustomToolsOptions.doRegister("load-shapes-deleted", new Option_Bool(false));
    myCustomToolsOptions.addDescription("load-shapes-deleted", "Configuration", TL("Load shapes for deleted elements"));
}


GNENetdiffToolDialog::~GNENetdiffToolDialog() {}

/****************************************************************************/
