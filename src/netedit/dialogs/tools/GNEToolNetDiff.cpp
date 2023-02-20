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
/// @file    GNEToolNetDiff.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Dialog for GNEToolNetDiff
/****************************************************************************/


#include "GNEToolNetDiff.h"


// ============================================-===============================
// member method definitions
// ===========================================================================

GNEToolNetDiff::GNEToolNetDiff(GNEApplicationWindow* GNEApp) :
    GNEToolDialog(GNEApp, "NetDiff", 300, 300) {
    // build elements
    new GNEToolDialogElements::Separator(myContentFrame, "Input");
    new GNEToolDialogElements::FileNameArgument(myContentFrame, this, "source", "");
    new GNEToolDialogElements::FileNameArgument(myContentFrame, this, "destiny", "");
    new GNEToolDialogElements::Separator(myContentFrame, "Output");
    new GNEToolDialogElements::FileNameArgument(myContentFrame, this, "output", "");
    // open as modal dialog
    openAsModalDialog();
}


GNEToolNetDiff::~GNEToolNetDiff() {}

/****************************************************************************/
